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

#define FT220X_CMD_WRITE            0x0000
#define FT220X_CMD_READ             0x0140
#define FT220X_CMD_MODEM_READ       0x0120
#define FT220X_CMD_MODEM_WRITE      0x0060
#define FT220X_CMD_FLUSH            0x0408
#define FT220X_CMD_EEPROM_ADDRESS   0x0248
#define FT220X_CMD_EEPROM_WRITE     0x0028
#define FT220X_CMD_EEPROM_READ      0x0168
#define FT220X_CMD_USB_READ         0x0101

#define FT220X_M_WRITE 0x00
#define FT220X_M_READ  0x01
#define FT220X_M_ADDR  0x02
#define FT220X_M_FLUSH 0x04

void ft220x_init();
uint8_t ft220x_com_write(uint8_t* buffer, unsigned int count);
uint8_t ft220x_com_read(uint8_t* buffer, unsigned int count);
uint8_t ft220x_com_flush();
uint8_t ft220x_rom_write(uint16_t address, uint8_t* data, unsigned int count);
uint8_t ft220x_rom_read(uint16_t address, uint8_t* data, unsigned int count);
uint8_t ft220x_usb_status();

extern void ft220x_hook__bus_turnaround(int write);

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
