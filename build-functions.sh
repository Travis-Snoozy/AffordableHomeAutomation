#!/bin/bash
# Copyright © 2013, 2014, Travis Snoozy
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

MMCU=${MMCU:-msp430g2412}
# -dp, -dap, and -fdump-tree-all for extra GCC debug output
#CFLAGS="-mmcu=${MMCU} -nostartfiles -dp -Os -I/usr/msp430-elf/include/"
CFLAGS="-mmcu=${MMCU} -nostartfiles -dp -Os -I/usr/msp430-elf/include/ -include${MMCU}.h"
ASFLAGS="--warn"
LSFLAGS="--sort-section=alignment"
CPPFLAGS=""
C="msp430-elf-gcc"
OBJCOPY="msp430-elf-objcopy"
NM="msp430-elf-nm"

function compile {
    INPUT="$1"
    OUT_EXT="$2"
    TARGET="$3"
    ${C} ${CFLAGS} ${CPPFLAGS:+-Wp,}${CPPFLAGS} -Wa,-acdglmns="${TARGET}.lst"${ASFLAGS:+,}${ASFLAGS} -Wl,-M="${TARGET}.map"${LSFLAGS:+,}${LSFLAGS} -o "${TARGET}.${OUT_EXT}" ${INPUT}
}

function build {
    OUTPUT="$1"
    if [ -d "$OUTPUT" ]; then
        rm -rf "${OUTPUT}"
    fi
    mkdir "${OUTPUT}"
    for FILE in *.c; do
        compile "-S ${FILE}" "s" "${OUTPUT}/${FILE}"
    done
    for FILE in *.S; do
        compile "-E ${FILE}" "s" "${OUTPUT}/${FILE}"
    done

    pushd "${OUTPUT}"

    for FILE in *.s; do
        compile "-c ${FILE}" "o" "${FILE}"
    done

    compile '*.o' "elf" "${OUTPUT}"

    "${OBJCOPY}" -O ihex "${OUTPUT}.elf" "${OUTPUT}.txt"
    "${NM}" "${OUTPUT}.elf" > "${OUTPUT}.sym"

    popd
}

# vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
