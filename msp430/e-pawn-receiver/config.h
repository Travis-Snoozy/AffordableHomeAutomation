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

#include "msp430g2412.h"

/*
MSP430G2412
Batt+   VCC _/| |\_GND  Batt-
SCK     P1.0_/| |\_P2.6 XIN
SSN1    P1.1_/| |\_P2.7 XOUT
SSN2    P1.2_/| |\_TEST N/C
CE1     P1.3_/| |\_RST  10k -> VCC
MISO    P1.4_/| |\_P1.7 N/C
IRQ     P1.5_/| |\_P1.6 N/C
MOSI0   P2.0_/| |\_P2.5 N/C
MOSI1   P2.1_/| |\_P2.4 N/C
MOSI2   P2.2_/| |\_P2.3 MOSI3

SS1     -> O: RFM70 (SPI slave select)
SS2     -> O: FT220X (SPI slave select)
MOSI0   -> I/O: SPI MOSI, MIOSIO[0] (FT220X)
MOSI1-3 -> I/O: MIOSIO[1-3] (FT220X)
SCK     -> O: SPI clock
MISO    -> I: SPI MISO
IRQ     -> I: RFM70 interrupt line
CE1     -> O: RFM70 chip enable

The FT220X must be configured to NOT drive its buffer status
when de-selected.

The MOSI0-3 pins must start in output mode.
*/


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

#define MCISR_POR       isr_por
#define MCISR_TA0_CC0   process_tx

// Clock control
// Set up the clock for 16MHz operation
#define MCBCSCTL    &CALBC1_16MHZ
#define MCDCOCTL    &CALDCO_16MHZ

// I/O setup

// I/O feature bits (not defined in chip header)
#define P2XIN       BIT6
#define P2XOUT      BIT7

// I/O direction (1 = out, 0 = in)
#define MCIODIR1    (0xFF - (RFM70_B_IRQ + SPI_B_MISO))
#define MCIODIR2    (0xFF)

// I/O pull or drive direction (1 = up, 0 = down)
#define MCRESDIR1    (RFM70_B_IRQ + (1 << FT220X_SPI_B_ENABLE) + (1 << RFM70_SPI_B_ENABLE))
#define MCRESDIR2   (0x00)

// I/O resistor enable (1 = enable, 0 = disable)
#define MCRESEN1   (RFM70_B_IRQ)
#define MCRESEN2    (0x00)

// I/O IRQ enable (1 = enable, 0 = disable)
#define MCIRQEN1    (RFM70_B_IRQ)
#define MCIRQEN2    (0x00)

// I/O IRQ edge select (1 = high -> low, 0 = low -> high)
#define MCIRQES1    (RFM70_B_IRQ)
#define MCIRQES2    (0x00)

// I/O selection
// -Pure I/O
#define MCP1F1      (0x00)
#define MCP1F2      (0x00)

// -P2.7, 2.6: LFXT1CLK (watch crystal)
#define MCP2F1      (P2XIN + P2XOUT)
#define MCP2F2      (0x00)

// Timer A setup
// Very basic -- just select ACLK as the source, and go into continuous mode.
#define FREQ        32768
#define MCTACTL     (TASSEL_1 + MC_2)
#define MCTACCR0    (0x00)
#define MCTACCTL0   (0x00)
#define MCTACCR1    (0x00)
#define MCTACCTL1   (0x00)
#define MCTACCR2    (0x00)
#define MCTACCTL2   (0x00)

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
