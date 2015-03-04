// Copyright © 2013, 2014, Travis Snoozy
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

void spi_init();
int spi_select(volatile unsigned char* address, uint8_t bit);
void spi_start(uint8_t mode);
void spi_write(const uint8_t* out, uint8_t* in, int count);
void spi_stop();

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8: