// Copyright © 2013, Travis Snoozy
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

#include "msp430g2412.h"

#define SPI_A_T volatile unsigned char

#define RFM70_A_IRQ P1IN
#define RFM70_B_IRQ BIT5
#define SPI_A_MISO P1IN
#define SPI_B_MISO BIT4
#define SPI_A_MOSI P1OUT
#define SPI_B_MOSI BIT3
#define SPI_A_CLOCK P1OUT
#define SPI_B_CLOCK BIT2
#define RFM70_SPI_A_ENABLE &P1OUT
#define RFM70_SPI_B_ENABLE 1
#define RFM70_A_ENABLE P1OUT
#define RFM70_B_ENABLE BIT0

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
