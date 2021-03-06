// Copyright © 2014, Travis Snoozy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdint.h"
#include "flash.h"

void iv_init(uint8_t* storage, uint8_t* out_iv)
{
    // Clear out the destination IV
    out_iv[0] = 0;
    out_iv[1] = 0;
    out_iv[2] = 0;
    out_iv[3] = 0;

    // Calculate how many slices have been used
    uint16_t count = 0;
    while(*storage != 0xFF && count < 511)
    {
        count++;
        storage++;
    }

    // Mark another slice as used (we're going to consume it right now).
    flash_unlock();
    *storage = 0;
    flash_lock();

    // Determine the next safe IV value based on
    // how many slices have been used.
    if(count & 0x0001)
    {
        out_iv[2] = 0x80;
    }
    out_iv[3] = (uint8_t)(count >> 1);
}
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
