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

#include "stddef.h"
#include "rfm70.h"

//#define RFM70_BUILD_RX

#define P1READ (P1IN & 0x60)
#define P2READ (P2IN & 0xC0)

// Read the config register
const uint8_t packet_config[] = {
    RFM70_CMD_R_REG_ + RFM70_REG0A_CONFIG,
    RFM70_CMD_NOP };

#ifdef RFM70_BUILD_RX
// Set pipe 0's RX length
const uint8_t packet_config_rx_length[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_RXLENP0,
    0x01 };

// Receive a payload
const uint8_t packet_receive[] = {
    RFM70_CMD_R_PAYLOAD,
    RFM70_CMD_NOP};

#else
#endif

void firmware_main()
{
    uint8_t scratch[2];

    spi_init();
    rfm70_init();

#ifdef RFM70_BUILD_RX
    // Configure pipe 0 to receive packets of length 1.
    RFM70_WRITE_PACKET(packet_config_rx_length, scratch);
#endif
    // Retrieve the current chip configuration
    RFM70_WRITE_PACKET(packet_config, scratch);

    // Set up the chip's config flags
#ifdef RFM70_BUILD_RX
    scratch[1] |= 0x01; /* Primary RX (RX) */
#else
    scratch[1] &= ~0x01; /* Primary RX (TX) */
#endif

    scratch[1] |= 0x02; /* Power up */

    // Convert the scratch space into a config-write command, and write
    // the updated chip config back to the chip.
    scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_CONFIG;
    RFM70_WRITE_PACKET(scratch, scratch);

    rfm70_enable();

    // Turn on the initialization complete light, indicating which
    // firmware (TX or RX) is being run.
#ifdef RFM70_BUILD_RX
    P2OUT |= 0x08;
#else
    P2OUT |= 0x20;
#endif
}

void firmware_io_isr()
{
    uint8_t i;
    uint8_t scratch[2];
    uint8_t lights = 0;
    uint8_t p1 = P1READ;
    uint8_t p2 = P2READ;
    uint8_t temp1;
    uint8_t temp2;

#ifndef RFM70_BUILD_RX
    // Debounce
    for(i = 0; i < 255; i++)
    {
        temp1 = P1READ;
        temp2 = P2READ;
        if((p1 != temp1) || (p2 != temp2))
        {
            p1 = P1READ;
            p2 = P2READ;
            i = 0;
        }
    }
#endif

    temp1 = P1IFG;
    temp2 = P2IFG;

    // Handle wireless IRQ
    if(temp1 & 0x20)
    {
#ifdef RFM70_BUILD_RX
        // Transmission received; read the data
        RFM70_WRITE_PACKET(packet_receive, scratch);
        // Decode it later.
        lights = scratch[1];
#else
        // Transmission complete, or retransmit attempts exceeded.
        // Turn off the radio.
        rfm70_disable();
#endif

        // Clear the interrupt on the wireless chip.
        RFM70_WRITE_PACKET(rfm70_packet_clear_interrupts, scratch);
    }

#ifndef RFM70_BUILD_RX
    // Handle flipped switch
    if((temp1 & 0x40) || (temp2 & 0xC0))
    {
        // Get a template of the edge selects so we can (re)set them
        // appropriately later on.
        temp1 = P1IES & ~0x40;
        temp2 = P2IES & ~0xC0;

        // Build the transmission packet.
        scratch[0] = RFM70_CMD_W_PAYLOAD;
        if(p1 & 0x40) { lights |= 0x01; temp1 |= 0x40; }
        if(p2 & 0x40) { lights |= 0x02; temp2 |= 0x40; }
        if(p2 & 0x80) { lights |= 0x04; temp2 |= 0x80; }
        lights |= 0x10;
        scratch[1] = lights;

        // Queue the packet
        RFM70_WRITE_PACKET(scratch, scratch);

        // Reset the interrupt edge selects for the switches
        P1IES = temp1;
        P2IES = temp2;

        // Turn on the radio
        // Note: because the IFGs are cleared on exit of this function,
        // this could result in a dangling/never-handled interrupt from
        // the wireless chip. The result is that we wouldn't turn the
        // radio off after our TX was complete.
        rfm70_enable();
    }
#endif

    // Set lights
    if(lights & 0x10)
    {
        // Grab the current P2OUT status, masking out the LED bits
        temp1 = P2OUT & ~0x38;

        // Set the LED bits on our copy
        if(lights & 0x01) { temp1 |= 0x08; }
        if(lights & 0x02) { temp1 |= 0x10; }
        if(lights & 0x04) { temp1 |= 0x20; }

        // Write-back to P2OUT.
        P2OUT = temp1;
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
