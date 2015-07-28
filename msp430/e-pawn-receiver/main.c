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

// Misc notes:
// We have to wrap the SPI selection code in critical sections, so that we can
// safely restore SPI state (and not be left with half-changed SPI state) after
// (during) an interrupt. Since we don't allow nested interrupts, we don't have
// to critical section this type of code in the interrupt.
//
// We also have to wrap the timer-check code in a critical section. Either the
// interrupt, or the COM-reader function needs to schedule the next deadline.
//
// TODO: need to set up the I/O with appropriate interrupt dis/enables.
//
// Truth table driving the receiver's behavior:
//
// Statuses:
// (S)end deadline expired
// (N)ew packet on COM
// (B)uffers full (3 in flight, combined TX+RX)
// (R)X Queue has one item at least/service packet reception
//
// Actions:
// (S)end packet
// (R)eceive packet (and write it to COM)
// (Q)ueue packet (from COM to wireless)
// (_) No action to take
//
// SBRN
// 1111: S
// 1110: S
// 1101: S
// 1100: S
// 1011: S
// 1010: S
// 1001: S
// 1000: S
// 0111: R
// 0110: R
// 0101: _
// 0100: _
// 0011: Q
// 0010: R
// 0001: Q
// 0000: _
//
// S: -S, ~+R
// R: ?-R, -B
// Q: ?+B, ?-N

#include "string.h"
#include "stddef.h"
#include "spi.h"
#include "rfm70.h"
#include "ft220x.h"
#include "timer.h"
#include "delay.h"
#include "pawn.h"

#define QUEUE_MAX 3
#define QUEUE_NEXT(X) (X = (X + 1) % QUEUE_MAX)

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

// Get the length of the next packet in the RX buffer
const uint8_t packet_rx_len[] = {
    RFM70_CMD_R_PAYLOAD_LEN,
    RFM70_CMD_NOP };

// Get the status of the TX and RX buffers
const uint8_t packet_fifo_status[] = {
    RFM70_CMD_R_REG_ + RFM70_REG0A_FIFOSTATUS,
    RFM70_CMD_NOP };

// Read the config register
const uint8_t packet_config[] = {
    RFM70_CMD_R_REG_ + RFM70_REG0A_CONFIG,
    RFM70_CMD_NOP };

uint8_t load_txdata();
void process_rx();

// Data queued for transmission
static passthrough_t queue[QUEUE_MAX];

// The next queue entry to queue to (0-2)
static uint8_t queue_next;

// The next queue entry to dequeue from (0-2)
static uint8_t dequeue_next;

// The number of queue entries in use
static uint8_t queue_use;

// The next queue entry to transmit
static uint8_t queue_tx;

// Keep track of SPI state so the ISR can restore it
static volatile uint8_t* spi_address;
static uint8_t spi_bit;

// Initialization
void isr_por()
{
    queue_next = 0;
    dequeue_next = 0;
    queue_use = 0;
    queue_tx = 0;
    {
        uint8_t scratch[2];

        spi_init();
        ft220x_init();
        rfm70_init();

        RFM70_WRITE_PACKET(packet_activate, scratch);
        RFM70_WRITE_PACKET(packet_config_features, scratch);
        RFM70_WRITE_PACKET(packet_config_dynpd, scratch);
        RFM70_WRITE_PACKET(packet_config_retransmit, scratch);
        RFM70_WRITE_PACKET(packet_config_addrlen, scratch);
        RFM70_WRITE_PACKET(packet_config_rxpipe, scratch);

        // Retrieve the current chip configuration
        RFM70_WRITE_PACKET(packet_config, scratch);

        // Set up the chip's config flags
        scratch[1] &= ~0x01; /* Primary RX (TX) */
        scratch[1] |= 0x02; /* Power up */

        // Convert the scratch space into a config-write command, and write
        // the updated chip config back to the chip.
        scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_CONFIG;
        RFM70_WRITE_PACKET(scratch, scratch);
    }
}

// Main loop for reading COM commands
// Must be interruptable so that transmission timers can be serviced.
void firmware_main()
{
    while(1) {
        if(queue_use >= QUEUE_MAX || load_txdata()) {
            // Nothing to do, OR we're blocked because our queue is
            // full. Either way, we should try to pull something
            // off the queue!
            process_rx();
        }
    }
}

// Call this when the COM chip has data on the wire to read out for a
// pending packet.
// Returns zero if a passthrough packet was read/loaded.
// Returns non-zero if no data is available on the COM.
uint8_t load_txdata()
{
    uint8_t payload[33];

    // Set FT220X as active.
    disable_interrupts();
    spi_address = FT220X_SPI_A_ENABLE;
    spi_bit = FT220X_SPI_B_ENABLE;
    spi_select(spi_address, spi_bit);
    enable_interrupts();

    payload[0] = RFM70_CMD_W_PAYLOAD;

    // Read data off the COM
    // Read the command byte
    do {
        // Use a straight read instead of a com read, because we don't
        // actually want to block in the case where there is nothing
        // to pull off the chip.
        // FIXME this needs to be made public, or hacked in as a local
        // declaration or something.
        if(ft220x_write(FT220X_CMD_READ, &queue[queue_next].size)) {
            return 1;
        }
        // Only passthrough is supported, currently. If we did not get
        // a valid passthrough byte, move on.
    } while(queue[queue_next].size > 32);

    // We got a passthrough byte. Now read the rest of the
    // passthrough_t structure, followed by the raw payload
    ft220x_com_read(&queue[queue_next].channel, sizeof(passthrough_t)-1);
    ft220x_com_read(&payload[1], queue[queue_next].size);

    // Queue raw payload onto the wireless chip
    // Set RFM70 as active
    disable_interrupts();
    spi_address = RFM70_SPI_A_ENABLE;
    spi_bit = RFM70_SPI_B_ENABLE;
    spi_select(spi_address, spi_bit);
    enable_interrupts();

    rfm70_spi_write(payload, payload, 8+(8*queue[queue_next].size));

    // We modify the queue BEFORE entering the critical section to set
    // up the timer. Why? ...
    QUEUE_NEXT(queue_next);
    // Because if the interrupt triggers here, then the ISR will
    // either set up the next timer interrupt, or have cleared the
    // timer. In the latter case, if this had been placed after the
    // critical section, we would "hang" due to the timer having been
    // set at the time we checked it, but unset after the check by the
    // ISR.
    queue_use++;

    // Critical section
    // If there is no timer presently set, set a timer based on this
    // packet's deadline.
    // Ordering of the code is super-important. We must do this AFTER
    // updating the queue data.
    disable_interrupts();
    if(!timer_is_set()) {
       timer_set(queue[queue_next].timeslot); 
    }
    enable_interrupts();

    return 0;
}

// Call this when a TX packet is scheduled to be transmitted.
void process_tx()
{
    // Set RFM70 as active
    spi_select(RFM70_SPI_A_ENABLE, RFM70_SPI_B_ENABLE);

    passthrough_t* data = &queue[queue_tx];

    {
        uint8_t scratch[2];

        // Set the radio channel
        scratch[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_RFCHANNEL;
        scratch[1] = data->channel;
        RFM70_WRITE_PACKET(scratch, scratch);
    }
    {
        uint8_t addr[4];
        addr[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_RXADDRP0;
        addr[1] = data->address[0];
        addr[2] = data->address[1];
        addr[3] = data->address[2];
        RFM70_WRITE_PACKET(addr, addr);

        addr[0] = RFM70_CMD_W_REG_ + RFM70_REG0A_TXADDR;
        addr[1] = data->address[0];
        addr[2] = data->address[1];
        addr[3] = data->address[2];
        RFM70_WRITE_PACKET(addr, addr);
    }

    // Payload should already be in the chip, at the head of the queue.
    rfm70_pulse();

    // Restore the prior SPI selection
    spi_select(spi_address, spi_bit);
    
    // Advance the TX queue slot and, if another packet has already
    // been queued, schedule its deadline with the timer.
    QUEUE_NEXT(queue_tx);
    if(queue_tx != queue_next) {
        timer_set(queue[queue_tx].timeslot);
    } else {
        timer_clear();
    }
}

// Call this when the buffers are full, and there's at least one RX
// outstanding, or if there is an RX outstanding and nothing else to
// do.
void process_rx()
{
    uint8_t payload[33];

    memset(payload, RFM70_CMD_NOP, sizeof(payload));

    // Set RFM70 as active
    disable_interrupts();
    spi_address = RFM70_SPI_A_ENABLE;
    spi_bit = RFM70_SPI_B_ENABLE;
    spi_select(spi_address, spi_bit);
    enable_interrupts();

    RFM70_WRITE_PACKET(packet_fifo_status, payload);
    // See if the RX FIFO empty flag (0x01) is set.
    if(payload[1] & 0x01) {
        // Bail out -- the RX FIFO is empty.
        return;
    }

    // Extract response payload from chip
    // Get the payload size
    RFM70_WRITE_PACKET(packet_rx_len, payload);
    queue[dequeue_next].size = payload[1];

    // Read the payload out
    payload[0] = RFM70_CMD_R_PAYLOAD;
    payload[1] = RFM70_CMD_NOP;
    rfm70_spi_write(payload, payload, queue[dequeue_next].size*8);


    // Spit out the relevant information over the COM 
    // Set FT220X as active
    disable_interrupts();
    spi_address = FT220X_SPI_A_ENABLE;
    spi_bit = FT220X_SPI_B_ENABLE;
    spi_select(spi_address, spi_bit);
    enable_interrupts();

    ft220x_com_write((uint8_t*)(&queue[dequeue_next]), 5);
    ft220x_com_write(&payload[1], queue[dequeue_next].size);


    // Update the queue bookkeeping
    QUEUE_NEXT(dequeue_next);
    queue_use--;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
