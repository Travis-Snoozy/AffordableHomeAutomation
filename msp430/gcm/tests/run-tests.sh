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

rm -rf {en,de}crypt/{gcmtest,temp.mem} output-*.txt
rm ~/.mspdebug
ln -s "`pwd`/gcm_tests.mspd" ~/.mspdebug

pushd encrypt
./build.sh
echo -n | mspdebug sim > ../output-enc.txt
popd

pushd decrypt
./build.sh
echo -n | mspdebug sim > ../output-dec.txt
popd

# vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
