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

#include "stddef.h"
#include "string.h"
#include "rfm70.h"
#include "settings.h"
#include "aes_init.h"
#include "iv_init.h"

//#define RFM70_BUILD_RX

#define P1READ (P1IN & 0x60)
#define P2READ (P2IN & 0xC0)

// Enable advanced features
const uint8_t packet_activate[] = {
    RFM70_CMD_ACTIVATE_,
    RFM70_CMD_ACTIVATE__ADVANCED };

// Enable dynamic payloads and ACK payloads
const uint8_t packet_config_features[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_FEATURE,
    0x06 };

// Enable dynamic payloads, pipe 0
const uint8_t packet_config_dynpd[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_DYNAMICPAYLOAD,
    0x01 };

// Disable retry
const uint8_t packet_config_retransmit[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_RETRANSMIT,
    0x00 };

// Address widths = 3 bytes
const uint8_t packet_config_addrlen[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_ADDRLEN,
    0x01 };

// Enable RX pipes (enable pipe 0 only)
const uint8_t packet_config_rxpipe[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_RXPIPE,
    0x01 };

// Read the config register
const uint8_t packet_config[] = {
    RFM70_CMD_R_REG_ + RFM70_REG0A_CONFIG,
    RFM70_CMD_NOP };

// Receive a payload
const uint8_t packet_receive[] = {
    RFM70_CMD_R_PAYLOAD,
    RFM70_CMD_NOP};

// Get the length of the next packet in the RX buffer
const uint8_t packet_rx_len[] = {
    RFM70_CMD_R_PAYLOAD_LEN,
    RFM70_CMD_NOP };

static uint8_t iv_tx[4];
static uint8_t iv_rx[4];

void isr_por()
{
    uint8_t scratch[2];
    info_t* settings = get_settings();

    spi_init();
    rfm70_init();

    RFM70_WRITE_PACKET(packet_activate, scratch);
    RFM70_WRITE_PACKET(packet_config_features, scratch);
    RFM70_WRITE_PACKET(packet_config_dynpd, scratch);
    RFM70_WRITE_PACKET(packet_config_retransmit, scratch);
    RFM70_WRITE_PACKET(packet_config_addrlen, scratch);
    RFM70_WRITE_PACKET(packet_config_rxpipe, scratch);

    // Set the radio channel
    scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_RFCHANNEL;
    scratch[1] = settings->radio_channel;
    RFM70_WRITE_PACKET(scratch, scratch);

    // Set the RF config
    scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_RFSETUP;
    scratch[1] = settings->radio_config;
    RFM70_WRITE_PACKET(scratch, scratch);

    {
        uint8_t addr[4];
        addr[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_RXADDRP0;
        addr[1] = settings->address[0];
        addr[2] = settings->address[1];
        addr[3] = settings->address[2];
        RFM70_WRITE_PACKET(addr, addr);

        addr[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_TXADDR;
        addr[1] = settings->address[0];
        addr[2] = settings->address[1];
        addr[3] = settings->address[2];
        RFM70_WRITE_PACKET(addr, addr);
    }

    // Retrieve the current chip configuration
    RFM70_WRITE_PACKET(packet_config, scratch);

    // Set up the chip's config flags
    scratch[1] |= 0x30; /* Mask retry and transmit interrupts. */
    scratch[1] |= 0x01; /* Primary RX (RX) */
    scratch[1] |= 0x02; /* Power up */

    // Convert the scratch space into a config-write command, and write
    // the updated chip config back to the chip.
    scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_CONFIG;
    RFM70_WRITE_PACKET(scratch, scratch);

    // Busy-wait for ~1s (try to shake out a low battery before we
    // start programming flash)
    uint16_t delay = 0;
    for(scratch[0] = 0; scratch[0] < 3; scratch[0]++)
    {
        for(delay = 0; delay < 65535; delay++)
        {
            scratch[1]++;
        }
    }

#ifdef PAWN_TEST_ENABLE_ENCRYPTION
    aes_init();
#ifdef PAWN_TEST_ENABLE_TX_ENCRYPTION
    iv_init(safe_iv_tx, iv_tx);
#endif /* PAWN_TEST_ENABLE_TX_ENCRYPTION */
#ifdef PAWN_TEST_ENABLE_RX_ENCRYPTION
    iv_init(safe_iv_rx, iv_rx);
#endif /* PAWN_TEST_ENABLE_RX_ENCRYPTION */
#endif /* PAWN_TEST_ENABLE_ENCRYPTION */
}

void isr_p2()
{
    uint8_t ifg = P1IFG;
    if(RFM70_B_IRQ & ifg)
    {
        uint8_t payload[33];
        uint8_t size;

        memset(payload, RFM70_CMD_NOP, sizeof(payload));

        // If we receive a packet, read it out
        RFM70_WRITE_PACKET(packet_rx_len, payload);
        size = payload[1];
        payload[0] = RFM70_CMD_R_PAYLOAD;
        payload[1] = RFM70_CMD_NOP;
        rfm70_spi_write(payload, payload, size*8);

        // Then write it back to the ACK buffer.
        payload[0] = RFM70_CMD_W_ACK_PAYLOAD_P0;
        rfm70_spi_write(payload, payload, size*8);

        // Clear any interrupt flags on the chip
        RFM70_WRITE_PACKET(rfm70_packet_clear_interrupts, payload);
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
