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

#include "stdint.h"
// Settings shall be stored in non-volatile memory, for access across
// device resets/power losses.

// Data required to bring us back onto the network
// after we've lost power.
typedef struct {
    uint8_t key[16];        // Unexpanded AES key
    uint8_t iv_tx[12];      // AES-GCM IV for transmissions
    uint8_t iv_rx[12];      // AES-GCM IV for reception
    uint8_t address[3];     // Radio address
    uint8_t radio_channel;  // Radio channel
    uint8_t radio_config;   // RFSETUP settings (data rate, TX gain, RX gain)
    uint8_t use_c;          // Internal use
} info_t;

// Derived key information, needs to be recalculated
// if your key changes.
extern uint8_t expanded_key[176];
extern uint8_t hash_key[16];
extern uint8_t key_segment;

// Track which IVs have not yet been used (so we
// never re-use one)
extern uint8_t safe_iv_tx[512];
extern uint8_t safe_iv_rx[512];

// Map our config information over the info
// section of ROM. We need two segments, so
// that we can safely cut over from one
// configuration to an updated configuration.
extern info_t infob;
extern info_t infoc;

// Acquire the currently active info segment.
info_t* get_settings();
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
