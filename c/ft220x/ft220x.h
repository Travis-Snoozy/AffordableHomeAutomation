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

#include "stdint.h"

/*! \file
 * Constants and API for interacting with the FT220X.
 *
 * The FT220X_CMD_* macros are a bit weird. While each command is documented as
 * having a straightforward ID (0, 1, 2, etc.), the reality is that the values
 * being sent out bear little resemblance to these straightforward IDs.
 *
 * Specifically, they're changed to [3]00[2]0[1][0]b (yes, that's _seven_ bits),
 * and then shuffled out MSB first. The macros reflect this mangling directly;
 * for example, the flush command is 0x04 -> 0000100b -> 0001000b -> 0001000b ->
 * 0x08.
 *
 * The FT220X_CMD_* macros, in their high-order byte, contain information
 * required to turn the bus once the command has been issued. These values are
 * replicated by the FT220X_M_* macros.
 *
 * In order to compile the corresponding implementation of this API, your build
 * will need to provide the following macro definiotns:
 *
 * - FT220X_WIDTH set to 1, 2 or 4; how many I/O lines are in use for the FT220X.
 * - FT220X_A_MISO_0, a constant address that the MISO line data can be read from.
 * - FT220X_B_MISO_0, the AND mask for FT220X_A_MISO_0 that enables reading the MISO pin.
 * - FT220X_A_MOSI_0, a constant address that the MOSI line data can be written to.
 * - FT220X_B_MOSI_0, the OR mask for FT220X_A_MOSI_0 that enables the MOSI pin.
 *
 * The above numbered macros have variants from 0-3. If FT220X_WIDTH is set to 1,
 * only the 0-series macros need to be defined; 2 requires 0-1; 4 requires 0-3.
 * Failure to have a correct set of macros defined will lead to a compile-time
 * error.
 *
 */

/*!
 * \def FT220X_CMD_WRITE Write data to the virtual COM.
 * \def FT220X_CMD_READ Read data from the virtual COM.
 * \def FT220X_CMD_MODEM_READ Read the modem status bits from the virtual COM.
 * \def FT220X_CMD_MODEM_WRITE Set the modem status bits for the virtual COM.
 * \def FT220X_CMD_FLUSH Flush the write buffers.
 * \def FT220X_CMD_EEPROM_ADDRESS Set the EEPROM address for a subsequent read/write.
 * \def FT220X_CMD_EEPROM_WRITE Write a value to the EEPROM.
 * \def FT220X_CMD_EEPROM_READ Read a value from the EEPROM.
 * \def FT220X_CMD_USB_READ Read the current USB connection status.
 */
#define FT220X_CMD_WRITE            0x0000
#define FT220X_CMD_READ             0x0140
#define FT220X_CMD_MODEM_READ       0x0120
#define FT220X_CMD_MODEM_WRITE      0x0060
#define FT220X_CMD_FLUSH            0x0408
#define FT220X_CMD_EEPROM_ADDRESS   0x0248
#define FT220X_CMD_EEPROM_WRITE     0x0028
#define FT220X_CMD_EEPROM_READ      0x0168
#define FT220X_CMD_USB_READ         0x0101

/*!
 * \def FT220X_M_WRITE Indicates a command that writes to the bus.
 * \def FT220X_M_READ Indicates a command that reads from the bus.
 * \def FT220X_M_ADDR Indicates a 2-part command (address part).
 * \def FT220X_M_FLUSH Reserved.
 */
#define FT220X_M_WRITE 0x00
#define FT220X_M_READ  0x01
#define FT220X_M_ADDR  0x02
#define FT220X_M_FLUSH 0x04

/*!
 * \pre spi_init() must have been called.
 * \post the FT220X is selected as the current active SPI chip.
 * \post The FT220X API is ready for use.
 *
 * This function should be called prior to executing any of the other FT220X
 * API calls. It should be safe to call multiple times, e.g., if you are unsure
 * if another device has become active in the SPI driver, or if the
 * bus direction is uncertain.
 */
void ft220x_init();

/*!
 * \invariant The FT220X device is the current SPI device
 * \invariant The FT220X IO bus is set to write mode
 * \pre \p buffer is not null.
 * \pre \p buffer contains at least \p count elements.
 * \pre \p buffer contains the data to be written to the virtual COM device.
 * \post The first \p count bytes of \p buffer are written to the virtual COM device.
 * \return 0.
 *
 * Writes \p count bytes of data from \p buffer to the virtual COM device.
 *
 * TODO change return to void
 *
 * TODO change buffer to be const
 */
uint8_t ft220x_com_write(uint8_t* buffer, unsigned int count);

/*!
 * \invariant The FT220X device is the current SPI device
 * \invariant The FT220X IO bus is set to write mode
 * \pre \p buffer is not null.
 * \pre \p buffer contains at least \p count elements.
 * \post The first \p count bytes of \p buffer are set by reading from the virtual COM buffer.
 * \return 0.
 *
 * Reads \p count bytes of data from the virtual COM port into \p buffer.
 *
 * TODO change return to void
 */
uint8_t ft220x_com_read(uint8_t* buffer, unsigned int count);

/*!
 * Not implemented. Do not use.
 */
uint8_t ft220x_com_flush();

/*!
 * Not implemented. Do not use.
 */
uint8_t ft220x_rom_write(uint16_t address, uint8_t* data, unsigned int count);

/*!
 * Not implemented. Do not use.
 */
uint8_t ft220x_rom_read(uint16_t address, uint8_t* data, unsigned int count);

/*!
 * Not implemented. Do not use.
 */
uint8_t ft220x_usb_status();

/*!
 * \pre \p write is 0 or 1.
 * \post If \p write is 0, the bus is configured for reading.
 * \post If \p write is 1, the bus is configured for writing.
 *
 * This function must be implemented in your client's code, as the FT220X driver
 * itself won't know how to turn your busses. The driver will call this function
 * when it needs to change the direction of the busses. The driver may call this
 * function even if a bus is already in the desired state (e.g., calling with
 * write 1 when the bus is already configured for writing), and your
 * implementation must allow for this.
 */
extern void ft220x_hook__bus_turnaround(int write);

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
