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

#include "msp430g2412.h"

/*
MSP430G2412
Batt+   VCC _/| |\_GND  Batt-
Free    P1.0_/| |\_P2.6 XIN
Free    P1.1_/| |\_P2.7 XOUT
WEN     P1.2_/| |\_TEST N/C
Free    P1.3_/| |\_RST  10k -> VCC
Free    P1.4_/| |\_P1.7 SDA (USI I2C)
Free    P1.5_/| |\_P1.6 SCL (USI I2C)
Free    P2.0_/| |\_P2.5 SCK (SW SPI)
WIRQ    P2.1_/| |\_P2.4 MISO (SW SPI)
WSPICS  P2.2_/| |\_P2.3 MOSI (SW SPI)
*/


#define SPI_A_T volatile unsigned char

//#define PAWN_TEST_ENABLE_INTERVAL       1
//#define PAWN_TEST_ENABLE_TX_ENCRYPTION  1
//#define PAWN_TEST_ENABLE_RX_ENCRYPTION  1

// If any of the specific PAWN encryption macros are defined, define
// the overall PAWN_TEST_ENABLE_ENCRYPTION macro.
#ifdef PAWN_TEST_ENABLE_TX_ENCRYPTION
#define PAWN_TEST_ENABLE_ENCRYPTION 1
#else /* PAWN_TEST_ENABLE_TX_ENCRYPTION */
#ifdef PAWN_TEST_ENABLE_RX_ENCRYPTION
#define PAWN_TEST_ENABLE_ENCRYPTION 1
#endif /* PAWN_TEST_ENABLE_RX_ENCRYPTION*/
#endif /* PAWN_TEST_ENABLE_TX_ENCRYPTION (else)*/

#define RFM70_A_IRQ P2IN
#define RFM70_B_IRQ BIT1
#define SPI_A_MISO P2IN
#define SPI_B_MISO BIT4
#define SPI_A_MOSI P2OUT
#define SPI_B_MOSI BIT3
#define SPI_A_CLOCK P2OUT
#define SPI_B_CLOCK BIT5
#define RFM70_SPI_A_ENABLE &P2OUT
#define RFM70_SPI_B_ENABLE 2

// We're using Timer_A to flip the enable bit, rather
// than controlling it through software directly.
//#define RFM70_A_ENABLE P1OUT
//#define RFM70_B_ENABLE BIT2

#define MCISR_POR   isr_por
//#define MCISR_TA0_CC2 isr_ta0cc2
#define MCISR_P2    isr_p2

// I/O setup

// I/O feature bits (not defined in chip header)
#define P2XIN       BIT6
#define P2XOUT      BIT7
#define P1USISCL    BIT6
#define P1USISDA    BIT7
#define P1TA0_1     BIT2

// I/O direction (1 = out, 0 = in)
#define MCIODIR1    (0xFF)
#define MCIODIR2    (0xFF - (RFM70_B_IRQ + SPI_B_MISO))

// I/O resistor direction (1 = up, 0 = down)
#define MCRESDIR1   (0x00)
#define MCRESDIR2   (RFM70_B_IRQ)

// I/O resistor enable (1 = enable, 0 = disable)
#define MCRESEN1    MCRESDIR1
#define MCRESEN2    MCRESDIR2

// I/O IRQ enable (1 = enable, 0 = disable)
#define MCIRQEN1    (0x00)
#define MCIRQEN2    (RFM70_B_IRQ)

// I/O IRQ edge select (1 = high->low, 0 = low->high)
#define MCIRQES1    (0x00)
#define MCIRQES2    (RFM70_B_IRQ)

// I/O selection
// -P1.7, 1.6: USI feature (I2C)
// -P1.2: TA0.1 output
#define MCP1F1      (P1TA0_1 + P1USISCL + P1USISDA)
#define MCP1F2      (0x00)

// -P2.7, 2.6: LFXT1CLK (watch crystal)
#define MCP2F1      (P2XIN + P2XOUT)
#define MCP2F2      (0x00)

// Configure USI for I2C
// - Enable P2.7 and P2.6 lines (data and clock)
// - Enable I2C mode
// - Positive clock polarity
// - Use SMCLK as clock source (nominal 1.1MHz)
// - Use a /16 divisor to bring the SMCLK <100KHz,
//   and to allow slaves to clock-stretch
#define MCUSI0      (USIPE7 + USIPE6 + USIMST)
#define MCUSI1      (USII2C)
#define MCUSICK     (USIDIV_4 + USISSEL_2 + USICKPL)

// Configure Timer_A for pulsing the wireless enable pin,
// and setting a deadline interrupt for packaging the next
// output packet.
// - ACLK source
// - UP mode
// - Counter 0:
//   - 0 to 1/5 of a second
//   - No other output or interrupts
// - Counter 1:
//   - 0 to .000451s (1/2217)
//   - Reset/set mode, but no interrupts
//     (The wireless chip will interrupt the MCU when needed)
// - Counter 2:
//   - 0 to .15s (3/20)
//   - Software will enable the interrupt on this channel if
//     a local update causes a state change.

#ifdef PAWN_TEST_ENABLE_INTERVAL
#define FREQ        32768
//#define FREQ        12000
#define MCTACTL     (TASSEL_1 + OUTMOD0 + TAIE)
#define MCTACCR0    ((FREQ/5) + 1)
#define MCTACCTL0   (0x00)
#define MCTACCR1    ((FREQ/2217) + 1)
#define MCTACCTL1   (OUTMOD_7)
#define MCTACCR2    ((FREQ*3)/20)
#define MCTACCTL2   (0x00)
#endif /* PAWN_TEST_ENABLE_INTERVAL */
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
