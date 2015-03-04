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

#include "stdint.h"

// Register addresses
// CONFIG
#define RFM70_REG0A_CONFIG           0x00

// EN_AA
#define RFM70_REG0A_AUTOACK          0x01

// EN_RXADDR
#define RFM70_REG0A_RXPIPE           0x02

// SETUP_AW
#define RFM70_REG0A_ADDRLEN          0x03

// SETUP_RETR
#define RFM70_REG0A_RETRANSMIT       0x04

// RF_CH
#define RFM70_REG0A_RFCHANNEL        0x05

// RF_SETUP
#define RFM70_REG0A_RFSETUP          0x06

// STATUS
#define RFM70_REG0A_STATUS           0x07

// OBSERVE_TX
#define RFM70_REG0A_TXSTATS          0x08

// CD
#define RFM70_REG0A_CARRIERDETECT    0x09

// RX_ADDR_P0
#define RFM70_REG0A_RXADDRP0         0x0A + 0x00

// RX_ADDR_P1
#define RFM70_REG0A_RXADDRP1         0x0A + 0x01

// RX_ADDR_P2
#define RFM70_REG0A_RXADDRP2         0x0A + 0x02

// RX_ADDR_P3
#define RFM70_REG0A_RXADDRP3         0x0A + 0x03

// RX_ADDR_P4
#define RFM70_REG0A_RXADDRP4         0x0A + 0x04

// RX_ADDR_P5
#define RFM70_REG0A_RXADDRP5         0x0A + 0x05

// TX_ADDR
#define RFM70_REG0A_TXADDR           0x10

// RX_PW_P0
#define RFM70_REG0A_RXLENP0          0x11 + 0x00

// RX_PW_P1
#define RFM70_REG0A_RXLENP1          0x11 + 0x01

// RX_PW_P2
#define RFM70_REG0A_RXLENP2          0x11 + 0x02

// RX_PW_P3
#define RFM70_REG0A_RXLENP3          0x11 + 0x03

// RX_PW_P4
#define RFM70_REG0A_RXLENP4          0x11 + 0x04

// RX_PW_P5
#define RFM70_REG0A_RXLENP5          0x11 + 0x05

// FIFO_STATUS
#define RFM70_REG0A_FIFOSTATUS       0x17

// DYNPD
#define RFM70_REG0A_DYNAMICPAYLOAD   0x1C

// FEATURE
#define RFM70_REG0A_FEATURE          0x1D

// N/A
#define RFM70_REG1A_CONST0           0x00

// N/A
#define RFM70_REG1A_CONST1           0x01

// N/A
#define RFM70_REG1A_CONST2           0x02

// N/A
#define RFM70_REG1A_CONST3           0x03

// N/A
#define RFM70_REG1A_RFSETUP          0x04

// N/A
#define RFM70_REG1A_RSSI             0x05

// Chip ID
#define RFM70_REG1A_CHIPID           0x08

// N/A
#define RFM70_REG1A_CONST4           0x0C

// NEW_FEATURE
#define RFM70_REG1A_CONST5           0x0D

// RAMP
#define RFM70_REG1A_CONST6           0x0E


// Constants for use with bank 1 register initialization
// The values have already been put in the correct (little/big endian) order

// Little endian
#define RFM70_REG1C_CONST6_0         0x41
#define RFM70_REG1C_CONST6_1         0x20
#define RFM70_REG1C_CONST6_2         0x08
#define RFM70_REG1C_CONST6_3         0x04
#define RFM70_REG1C_CONST6_4         0x81
#define RFM70_REG1C_CONST6_5         0x20
#define RFM70_REG1C_CONST6_6         0xCF
#define RFM70_REG1C_CONST6_7         0xF7
#define RFM70_REG1C_CONST6_8         0xFE
#define RFM70_REG1C_CONST6_9         0xFF
#define RFM70_REG1C_CONST6_10        0xFF

// Little endian
#define RFM70_REG1C_CONST5_0         0x36
#define RFM70_REG1C_CONST5_1         0xB4
#define RFM70_REG1C_CONST5_2         0x80
#define RFM70_REG1C_CONST5_3         0x00

// Little endian
#define RFM70_REG1C_CONST4_0         0x00
#define RFM70_REG1C_CONST4_1         0x12
#define RFM70_REG1C_CONST4_2         0x73
#define RFM70_REG1C_CONST4_3         0x00

// Big endian
#define RFM70_REG1C_RSSI_0           0x24
#define RFM70_REG1C_RSSI_1           0x06
#define RFM70_REG1C_RSSI_2           0x7F
#define RFM70_REG1C_RSSI_3           0xA6

// Big endian
#define RFM70_REG1C_RFSETUP_0        0xD9
#define RFM70_REG1C_RFSETUP_1        0x9E
#define RFM70_REG1C_RFSETUP_2        0x86
#define RFM70_REG1C_RFSETUP_3        0x0B

// Big endian
#define RFM70_REG1C_CONST3_0         0x99
#define RFM70_REG1C_CONST3_1         0x00
#define RFM70_REG1C_CONST3_2         0x39
#define RFM70_REG1C_CONST3_3         0x41

// Big endian
#define RFM70_REG1C_CONST2_0         0xD0
#define RFM70_REG1C_CONST2_1         0xFC
#define RFM70_REG1C_CONST2_2         0x8C
#define RFM70_REG1C_CONST2_3         0x02

// Big endian
#define RFM70_REG1C_CONST1_0         0xC0
#define RFM70_REG1C_CONST1_1         0x4B
#define RFM70_REG1C_CONST1_2         0x00
#define RFM70_REG1C_CONST1_3         0x00

// Big endian
#define RFM70_REG1C_CONST0_0         0x40
#define RFM70_REG1C_CONST0_1         0x4B
#define RFM70_REG1C_CONST0_2         0x01
#define RFM70_REG1C_CONST0_3         0xE2


// Register contents
typedef struct {
    uint8_t : 1;                                          // Reserved
    uint8_t MASK_INT_RX_COMPLETE : 1;                     // MASK_RX_DR
    uint8_t MASK_INT_TX_COMPLETE : 1;                     // MASK_TX_DS
    uint8_t MASK_INT_TX_RETRY_EXCEEDED : 1;               // MASK_MAX_RT
    uint8_t ENABLE_CRC : 1;                               // EN_CRC
    uint8_t CRC_ENCODING : 1;                             // CRCO
    uint8_t POWER_UP : 1;                                 // PWR_UP
    uint8_t PRIMARY_RX : 1;                               // PRIM_RX
} RFM70_REG0S_CONFIG;

typedef struct {
    uint8_t : 2;
    uint8_t P5 : 1;
    uint8_t P4 : 1;
    uint8_t P3 : 1;
    uint8_t P2 : 1;
    uint8_t P1 : 1;
    uint8_t P0 : 1;
} RFM70_REG0S_PIPES;

typedef RFM70_REG0S_PIPES RFM70_REG0S_AUTOACK;
typedef RFM70_REG0S_PIPES RFM70_REG0S_RXPIPE;
typedef RFM70_REG0S_PIPES RFM70_REG0S_DYNAMICPAYLOAD;

typedef struct {
    uint8_t : 6;
    uint8_t LENGTH;
} RFM70_REG0S_ADDRLEN;

typedef struct {
    uint8_t DELAY : 4;
    uint8_t COUNT : 4;
} RFM70_REG0S_RETRANSMIT;

typedef struct {
    uint8_t : 1;
    uint8_t CHANNEL : 7;
} RFM70_REG0S_RFCHANNEL;

typedef struct {
    uint8_t : 4;
    uint8_t DATA_RATE : 1;
    uint8_t GAIN : 2;
    uint8_t LOW_NOISE_AMP_GAIN : 1;
} RFM70_REG0S_RFSETUP;

typedef struct {
    uint8_t BANK : 1;
    uint8_t FLAG_RX_COMPLETE : 1;
    uint8_t FLAG_TX_COMPLETE : 1;
    uint8_t FLAG_TX_RETRY_EXCEEDED : 1;
    uint8_t RX_PIPE : 3;
    uint8_t TX_FULL : 1;
} RFM70_REG0S_STATUS;

typedef struct {
    uint8_t LOST : 4;
    uint8_t RETRANSMITTED : 4;
} RFM70_REG0S_TXSTATS;

typedef struct {
    uint8_t : 6;
    uint8_t CARRIER : 1;
} RFM70_REG0S_CARRIERDETECT;

typedef struct {
    uint8_t SUFFIX : 8;
    uint32_t PREFIX : 32;
} RFM70_REG0S_LONGADDR;

typedef struct {
    uint8_t SUFFIX : 8;
} RFM70_REG0S_SHORTADDR;

typedef RFM70_REG0S_LONGADDR  RFM70_REG0S_RXADDRP0;
typedef RFM70_REG0S_LONGADDR  RFM70_REG0S_RXADDRP1;
typedef RFM70_REG0S_SHORTADDR RFM70_REG0S_RXADDRP2;
typedef RFM70_REG0S_SHORTADDR RFM70_REG0S_RXADDRP3;
typedef RFM70_REG0S_SHORTADDR RFM70_REG0S_RXADDRP4;
typedef RFM70_REG0S_SHORTADDR RFM70_REG0S_RXADDRP5;

typedef struct {
    uint8_t : 2;
    uint8_t PAYLOAD_BYTES : 6;
} RFM70_REG0S_RXLENP;

typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP0;
typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP1;
typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP2;
typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP3;
typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP4;
typedef RFM70_REG0S_RXLENP RFM70_REG0S_RXLENP5;

typedef struct {
    uint8_t : 1;
    uint8_t TX_REUSE : 1;
    uint8_t TX_FULL : 1;
    uint8_t TX_EMPTY : 1;
    uint8_t : 2;
    uint8_t RX_FULL : 1;
    uint8_t RX_EMPTY : 1;
} RFM70_REG0S_FIFOSTATUS;

typedef struct {
    uint8_t : 5;
    uint8_t DYNAMIC_PAYLOAD : 1;
    uint8_t ACK_PAYLOAD : 1;
    uint8_t DYNAMIC_ACK : 1;
} RFM70_REG0S_FEATURE;

typedef struct {
    uint32_t CHIP_ID : 32;
} RFM70_REG1S_CHIPID;


// SPI commands
// R_REGISTER
#define RFM70_CMD_R_REG_             0x00

// W_REGISTER
#define RFM70_CMD_W_REG_             0x20

// R_RX_PAYLOAD
#define RFM70_CMD_R_PAYLOAD          0x61

// W_TX_PAYLOAD
#define RFM70_CMD_W_PAYLOAD          0xA0

// FLUSH_TX
#define RFM70_CMD_FLUSH_TX           0xE1

// FLUSH_RX
#define RFM70_CMD_FLUSH_RX           0xE2

// REUSE_TX_PL
#define RFM70_CMD_REUSE_TX           0xE3

// ACTIVATE
#define RFM70_CMD_ACTIVATE_          0x50

// R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK
#define RFM70_CMD_ACTIVATE__ADVANCED 0x73

// N/A
#define RFM70_CMD_ACTIVATE__BANK     0x53

// R_RX_PL_WID
#define RFM70_CMD_R_PAYLOAD_LEN      0x60

// W_ACK_PAYLOAD (pipe 0)
#define RFM70_CMD_W_ACK_PAYLOAD_P0   0xA8 + 0x00

//               (pipe 1)
#define RFM70_CMD_W_ACK_PAYLOAD_P1   0xA8 + 0x01

//               (pipe 2)
#define RFM70_CMD_W_ACK_PAYLOAD_P2   0xA8 + 0x02

//               (pipe 3)
#define RFM70_CMD_W_ACK_PAYLOAD_P3   0xA8 + 0x03

//               (pipe 4)
#define RFM70_CMD_W_ACK_PAYLOAD_P4   0xA8 + 0x04

//               (pipe 5)
#define RFM70_CMD_W_ACK_PAYLOAD_P5   0xA8 + 0x05

// W_TX_PAYLOAD_NOACK
#define RFM70_CMD_W_NOACK_PAYLOAD    0XB0

// NOP
#define RFM70_CMD_NOP                0xFF

// Syntactic sugar; prevent the 1st and 3rd fields from mis-matching
#define RFM70_WRITE_PACKET(X, Y)     rfm70_spi_write(X, Y, (sizeof(X) * 8))

extern const uint8_t rfm70_packet_noop[1];
extern const uint8_t rfm70_packet_clear_interrupts[2];

void rfm70_spi_write(const uint8_t* out, uint8_t* in, int count);
void rfm70_init();
void rfm70_enable();
void rfm70_disable();

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
