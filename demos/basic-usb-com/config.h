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

#define SPI_A_T volatile unsigned char

#define RFM70_A_IRQ P1IN
#define RFM70_B_IRQ BIT5
#define SPI_A_MISO P1IN
#define SPI_B_MISO BIT4
#define SPI_A_MOSI P2OUT
#define SPI_B_MOSI BIT0
#define SPI_A_CLOCK P1OUT
#define SPI_B_CLOCK BIT0
#define RFM70_SPI_A_ENABLE &P1OUT
#define RFM70_SPI_B_ENABLE 1
#define RFM70_A_ENABLE P1OUT
#define RFM70_B_ENABLE BIT3
#define FT220X_SPI_A_ENABLE &P1OUT
#define FT220X_SPI_B_ENABLE 2
#define FT220X_WIDTH 4
#define FT220X_A_MOSI_0 &P2OUT
#define FT220X_B_MOSI_0 BIT0
#define FT220X_A_MOSI_1 &P2OUT
#define FT220X_B_MOSI_1 BIT1
#define FT220X_A_MOSI_2 &P2OUT
#define FT220X_B_MOSI_2 BIT2
#define FT220X_A_MOSI_3 &P2OUT
#define FT220X_B_MOSI_3 BIT3
#define FT220X_A_MISO_0 &P2IN
#define FT220X_B_MISO_0 FT220X_B_MOSI_0
#define FT220X_A_MISO_1 &P2IN
#define FT220X_B_MISO_1 FT220X_B_MOSI_1
#define FT220X_A_MISO_2 &P2IN
#define FT220X_B_MISO_2 FT220X_B_MOSI_2
#define FT220X_A_MISO_3 &P2IN
#define FT220X_B_MISO_3 FT220X_B_MOSI_3
#define FT220X_TURN_MIOSIO_0 &P2DIR
#define FT220X_TURN_MIOSIO_1 &P2DIR
#define FT220X_TURN_MIOSIO_2 &P2DIR
#define FT220X_TURN_MIOSIO_3 &P2DIR

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
