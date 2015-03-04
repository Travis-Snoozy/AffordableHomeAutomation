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

// Generic RFM70 driver

#include "rfm70.h"
#include "spi.h"

const uint8_t rfm70_packet_bankswitch[] = {
    RFM70_CMD_ACTIVATE_,
    RFM70_CMD_ACTIVATE__BANK };

const uint8_t rfm70_packet_const0[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST0,
    RFM70_REG1C_CONST0_0,
    RFM70_REG1C_CONST0_1,
    RFM70_REG1C_CONST0_2,
    RFM70_REG1C_CONST0_3 };

const uint8_t rfm70_packet_const1[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST1,
    RFM70_REG1C_CONST1_0,
    RFM70_REG1C_CONST1_1,
    RFM70_REG1C_CONST1_2,
    RFM70_REG1C_CONST1_3 };

const uint8_t rfm70_packet_const2[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST2,
    RFM70_REG1C_CONST2_0,
    RFM70_REG1C_CONST2_1,
    RFM70_REG1C_CONST2_2,
    RFM70_REG1C_CONST2_3 };

const uint8_t rfm70_packet_const3[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST3,
    RFM70_REG1C_CONST3_0,
    RFM70_REG1C_CONST3_1,
    RFM70_REG1C_CONST3_2,
    RFM70_REG1C_CONST3_3 };

const uint8_t rfm70_packet_rfsetup1[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_RFSETUP,
    RFM70_REG1C_RFSETUP_0,
    RFM70_REG1C_RFSETUP_1,
    RFM70_REG1C_RFSETUP_2,
    RFM70_REG1C_RFSETUP_3 };

const uint8_t rfm70_packet_rssi[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_RSSI,
    RFM70_REG1C_RSSI_0,
    RFM70_REG1C_RSSI_1,
    RFM70_REG1C_RSSI_2,
    RFM70_REG1C_RSSI_3 };

const uint8_t rfm70_packet_const4[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST4,
    RFM70_REG1C_CONST4_0,
    RFM70_REG1C_CONST4_1,
    RFM70_REG1C_CONST4_2,
    RFM70_REG1C_CONST4_3 };

const uint8_t rfm70_packet_const5[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST5,
    RFM70_REG1C_CONST5_0,
    RFM70_REG1C_CONST5_1,
    RFM70_REG1C_CONST5_2,
    RFM70_REG1C_CONST5_3 };

const uint8_t rfm70_packet_const6[] = {
    RFM70_CMD_W_REG_ + RFM70_REG1A_CONST6,
    RFM70_REG1C_CONST6_0,
    RFM70_REG1C_CONST6_1,
    RFM70_REG1C_CONST6_2,
    RFM70_REG1C_CONST6_3,
    RFM70_REG1C_CONST6_4,
    RFM70_REG1C_CONST6_5,
    RFM70_REG1C_CONST6_6,
    RFM70_REG1C_CONST6_7,
    RFM70_REG1C_CONST6_8,
    RFM70_REG1C_CONST6_9,
    RFM70_REG1C_CONST6_10 };

const uint8_t rfm70_packet_noop[] = {
    RFM70_CMD_NOP };

const uint8_t rfm70_packet_clear_interrupts[] = {
    RFM70_CMD_W_REG_ + RFM70_REG0A_STATUS,
    0x70 };

void rfm70_spi_write(const uint8_t* out, uint8_t* in, int count)
{
    spi_start(0);
    spi_write(out, in, count);
    spi_stop();
}

void rfm70_init()
{
    spi_select(RFM70_SPI_A_ENABLE, RFM70_SPI_B_ENABLE);
    spi_stop();

    uint8_t in[11];
    uint8_t* status = in;

    do
    {
        RFM70_WRITE_PACKET(rfm70_packet_bankswitch, in);
        RFM70_WRITE_PACKET(rfm70_packet_noop, in);
    } while(!(*status & 0x80 /*bank*/));

    RFM70_WRITE_PACKET(rfm70_packet_const0, in);
    RFM70_WRITE_PACKET(rfm70_packet_const1, in);
    RFM70_WRITE_PACKET(rfm70_packet_const2, in);
    RFM70_WRITE_PACKET(rfm70_packet_const3, in);
    RFM70_WRITE_PACKET(rfm70_packet_rfsetup1, in);
    RFM70_WRITE_PACKET(rfm70_packet_rssi, in);
    RFM70_WRITE_PACKET(rfm70_packet_const4, in);
    RFM70_WRITE_PACKET(rfm70_packet_const5, in);
    RFM70_WRITE_PACKET(rfm70_packet_const6, in);

    do
    {
        RFM70_WRITE_PACKET(rfm70_packet_bankswitch, in);
        RFM70_WRITE_PACKET(rfm70_packet_noop, in);
    } while((*status & 0x80 /*bank*/));

    RFM70_WRITE_PACKET(rfm70_packet_clear_interrupts, in);
}

#ifdef RFM70_B_ENABLE
#ifdef RFM70_A_ENABLE
void rfm70_enable()
{
    RFM70_A_ENABLE |= RFM70_B_ENABLE;
}

void rfm70_disable()
{
    RFM70_A_ENABLE &= ~RFM70_B_ENABLE;
}
#endif
#endif
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
