#!/bin/bash
# Copyright © 2014, Travis Snoozy
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.                                        

if ! [ -f "$1" ]; then
  echo "You must specify a valid input file."
  exit
fi

# Strip off the file extension of the input file name
BASEFILE=${1%.*}

# Strip off any trailing digits of the input file name
until [ "$TEMP" == "$BASEFILE" ]; do
  TEMP="$BASEFILE"
  BASEFILE="${BASEFILE%[0-9]}"
done

unset TEMP

# - Convert the file's line endings
# - Strip out the RSP file header and leading blank lines
dos2unix < "$1" | tail -n +7 > temp.txt

# Convert the "len" values from decimal to hex
# Process the temp file line by line to do this
cat temp.txt | \
while read VARNAME EQUALS VALUE; do
  # If this line's variable name ends in "len"
  if [ "${VARNAME%%*len}" != "$VARNAME" ]; then
    # Output the line (removing the starting "[", up to the = sign
    echo -n ${VARNAME#[[]} $EQUALS" "
    # Then convert the value from decimal to hex, printing it at the very end of the line.
    # s/^(.?.)$/0x\1 0x0/ - If we have a 1 or 2 digit number, put it in the LSB position.
    # s/^(.?.)(..)$/0x\2 0x\1/ - If we have a 3 or 4 digit number, put it in little-endian order.
    echo "obase=16; ${VALUE%%[]]*}" | bc | sed -re 's/^(.?.)$/0x\1 0x0/' -e 's/^(.?.)(..)$/0x\2 0x\1/'
  else
    # This isn't a length variable? It's already hex, then; just print it straight out.
    echo $VARNAME $EQUALS $VALUE;
  fi
done > temp2.txt

mv temp2.txt temp.txt

# - Strip out the block-level variable's enclosing square brackets
# - Strip out the "Count" lines (we don't need them for anything).
# - Strip lines with no values
# - Strip trailing spaces
sed -ri -e 's/\[|\]//g' -e '/^Count .*/d' -e '/^[^ ]+ = $/d' -e 's/ +$//' temp.txt

# Convert the "var = value" format to "mw var value" used by MSPD
sed -rie 's/^([^ ]*) =/mw \1/' temp.txt

# Convert hex values to 0x## byte notation MSPD will understand.
# :loop                             - A label we'll need later
# ^(mw (Key|IV|PT|AAD|CT|Tag) )     - Match only these keys (and eat them and their trailing space)
# ((0x[0-9a-f]{2} )*)               - Eat up any parts of the hex string that have already been split into 0x-prefixed bytes
# ([0-9a-f]{2})                     - Capture the next un-processed byte's worth of digits, if they exist
# (.*)$                             - Capture the rest of the line.
# \1\30x\5 \6                       - Paste the key (and its space), the processed hex, the new 0x, hex byte, and space, and then any remainder.
# t loop                            - If we actually replaced something, run the replace again (go back to :loop)
sed -ri -e ':loop' -e 's/^(mw (Key|IV|PT|AAD|CT|Tag) )((0x[0-9a-f]{2} )*)([0-9a-f]{2})(.*)$/\1\30x\5 \6/' -e 't loop' temp.txt

# Split each test block into its own file.
# /^mw Keylen/                      - If this is a "Keylen" line, it's the start of a new test block
# {x="test-"++i;}                   - Increment our counter (start printing to a new file)
# {print > x;}                      - Append the current line in the buffer to the current file.
awk '/^mw Keylen/{x="test-"++i;}{print > x;}' temp.txt

# - Normalize each test so it has an appropriate FAIL line
# - Normalize each test so it reads the test round (after setting variables)
# - Rename the files to reflect the tests they contain.
for FILE in test-*; do
  # Make sure that there always exists a pass OR fail indicator for each test round.
  # --- This is incredibly slow :( ---
  # $!P - If this is not the last buffer, print it.
  # 7~1N - Process 2 lines, starting from the 7th line on
  # /FAIL\n$/! - If this is NOT a FAIL line followed by a blank line...
  # s/(.*)\n$/ - If this IS (then) a line followed by a blank line...
  # /\1\nmw FAIL 0\n/ - insert "md FAIL 0" as a line.
  # D - shift out the oldest (first) line, and jump back to the N
  sed -ri -e '$!P' -e '7~1N' -e '/FAIL\n$/!{s/(.*)\n$/\1\nmw FAIL 0x00 0x00\n/}' -e 'D' "$FILE"

  # Replace all the "FAIL" lines with appropriate memory sets.
  sed -rie 's/^FAIL$/mw FAIL 1/' "$FILE"

  # For each discrete set of test variables, read in the file responsible for actually running a single test round.
  # 7~1 - Skip the first 7 lines of the file
  # s/^$/ - If this is a blank line...
  # /read gcm_test_round.mspd\n/ - Insert the appropriate read line.
  sed -rie '7~1s/^$/read gcm_test_round.mspd\n/' "$FILE"

  # Rename the files to reflect the class of tests they contain.
  # head -n5 "$FILE" - Grab the first five lines of the file, which hold (in order) the values for key length, IV length, text length, AAD length, and tag length for all the test entries contained in that file.
  # ^.* 0x(.?.) 0x(.?.) - Match the two 1-2 digit hex numbers at the end of the lines
  # ibase=16; \2\1 - Put the bytes back into big-endian, and strip the 0x (prep for BC)
  # { while read; do echo $REPLY | bc; done; } - Pipe each line to BC one by one, converting the hex values back to decimal
  # :a - Label "a"
  # N - Append another line to the buffer
  # $!ba - If this is NOT the last line, branch to A
  # s/\n/-/g - Replace all the newlines in the processing space with dashes
  mv "$FILE" "$BASEFILE"`head -n5 "$FILE" | sed -re 's/^.* 0x(.?.) 0x(.?.)/ibase=16; \2\1/g' | { while read; do echo $REPLY | bc; done; } | sed -re ':a' -e 'N' -e '$!ba' -e 's/\n/-/g'`.mspd

  # The resulting renamed files are of the format:
  # [BASEFILE][Keylen]-[IVlen]-[PTlen]-[AADlen]-[Taglen].mspd

  # Get rid of the temporary file
  rm "${FILE}e"
done

# Get rid of temporary files
rm temp.txt{,e}

# vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
