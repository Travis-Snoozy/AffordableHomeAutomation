#!/bin/bash
# Copyright © 2013, Travis Snoozy
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

# Change the MCU so that there's enough memory to run this hog of a test. :)
MMCU=msp430g2955

source build-functions.sh

#CPPFLAGS="${CPPFLAGS}${CPPFLAGS:+,}-includeconfig.h"
CFLAGS="${CFLAGS}${CFLAGS:+ }-fno-zero-initialized-in-bss -DGCM_ENABLE_AAD=1"
build "gcmtest"

# vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
