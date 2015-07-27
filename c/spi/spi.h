// Copyright © 2013-2015, Travis Snoozy
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

/*! \file
 * API for low-level data transfer using Serial Peripheral Interface.
 *
 * In order to compile the corresponding implementation of this API, your build
 * will need to provide the following macro definiotns:
 *
 * - SPI_A_CLOCK, a reference to the clocking pin
 * - SPI_B_CLOCK, the OR mask for SPI_A_CLOCK that enables the clocking pin.
 * - SPI_A_MISO, a reference to the Master In/Slave Out pin.
 * - SPI_B_MISO, the AND mask for SPI_A_MISO that enables reading the MISO pin.
 * - SPI_A_MOSI, a reference to the Master Out/Slave In pin.
 * - SPI_B_MOSI, the OR mask for SPI_A_MOSI that enables the MOSI pin.
 * - SPI_A_T, the bare data type that the above references refer to.
 *
 * Note that this API supports only one SPI bus.
 */

#include "stdint.h"

/*!
 * \pre spi_init() has not previously been called.
 * \post spi_select() may be called.
 * \post spi_stop() may be called.
 *
 * This function must be called once, and only once, after a device is reset,
 * loses power, or otherwise has its memory contents lost or scrambled.
 */
void spi_init();

/*!
 * \pre spi_init() must have been called.
 * \pre address is non-null.
 * \pre bit < sizeof(SPI_A_T) * 8
 * \pre *address |= bit causes the chosen SPI device's CE line to be pulled
 * high (disabled).
 * \post the previously selected SPI device has its CE line pulled high.
 * \post the chosen SPI device is selected for current use with the SPI API.
 * \post spi_start(int) may be called.
 * \param[out] address The address that controls an SPI device's CE pin.
 * \param[in] bit Which bit (LSB = 0) in \p address controls the device's CE
 * pin.
 * \returns 0 on success, nonzero on error.
 *
 * Selects a new SPI device for use with the SPI API. If a device was previously
 * selected, that device is stopped before the new device is set as the current
 * device.
 *
 * Note that the \p bit parameter is _not_ an OR mask -- it indicates which bit
 * is the CE pin control bit via its value. For example, if SPI_A_T was an 8-bit
 * value, \p bit could legitimately be 0-7. 0 represents the LSB, whereas 7 (in
 * this example) would represent the MSB.
 */
int spi_select(volatile unsigned char* address, uint8_t bit);

/*!
 * \pre spi_select() must have been called (without error).
 * \pre Bit 0 of \p mode is the desired SPI phase.
 * \pre Bit 1 of \p mode is the desired SPI polarity.
 * \pre The SPI device must be configured to correctly operate in the mode
 * indicated by \p mode.
 * \post spi_write(const uint8_t*,uint8_t*,int) may be called.
 * \post spi_start(uint8_t) must not be called (until another postcondition
 * allows it).
 * \param[in] mode The SPI mode (phase, polarity) that the currently-selected
 * chip uses.
 *
 * Sets up the clock and enables the currently-selected SPI chip for use.
 */
void spi_start(uint8_t mode);

/*!
 * \pre Current postconditions must allow calling this function.
 * \pre \p out must be non-null.
 * \pre \p in must be non-null.
 * \pre \p count <= the storage capacity of \p out
 * \pre \p count <= the storage capacity of \p in
 * \post The original contents of the first \p count bytes of \p out are written
 * to the active SPI chip.
 * \post The \p count bytes of pending data read from the SPI chip are stored
 * in the first \p count bytes of \p in.
 * \param[in] out The buffer with the bytes to write to the SPI chip.
 * \param[out] in The buffer to store the bytes read back from the SPI chip.
 * \param[in] count The number of bytes to clock out/in.
 *
 * Perform a read/write cycle on the SPI bus. If the buffer is writable, then
 * \p out and \p in may safely refer to the same buffer (the bytes will be
 * written out before the results overwrite them).
 *
 * This function reads/writes byte-by-byte, starting from index 0 of the arrays.
 * Keep this in mind if your chip works with values larger than a byte, as there
 * may be endian considerations you need to take into account.
 */
void spi_write(const uint8_t* out, uint8_t* in, int count);

/*!
 * \pre spi_init() must have been called.
 * \post spi_write() must not be called (until another postcondition allows it).
 * \post spi_start() may be called.
 *
 * Disables the currently-selected SPI chip (if applicable).
 */
void spi_stop();

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
