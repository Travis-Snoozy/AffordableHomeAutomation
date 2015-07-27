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

#include <stdint.h>
#include <string.h>

#include "aes.h"
#include "gcm.h"

#define NATURAL_IV_BYTES 12
#define NATURAL_IV_BITS (NATURAL_IV_BYTES * 8)
#define FAIL_TAG    1
#define FAIL_CRYPT  2

// FIXME
// Something else is defining FAIL as a macro... we are a bit married
// to using it as a symbol name at the moment, so hack it out. Change
// the symbol name to something non-conflicting in the future.
#undef FAIL

// -------------------------------------------------------------------
// Values set by debug script

uint16_t Keylen = 0;
uint16_t IVlen = 0;
uint16_t PTlen = 0;
uint16_t AADlen = 0;
uint16_t Taglen = 0;

uint8_t IV[128];

// TODO: is there a better way to keep alignment than padding these
// arrays with an extra character?
uint8_t PT[52]; // Actually 51 (408 bits)

uint8_t CT[52]; // Actually 51 (408 bits)

uint8_t AAD[90]; 

uint8_t Key[16];

uint8_t Tag[16];

uint16_t FAIL; // 16 for alignment...

// -------------------------------------------------------------------
// Calculated values

static uint8_t iv[16];
static uint8_t ekey[176];
static uint8_t hash_const[16];
static uint8_t tag[16];
static uint16_t pass;


void init();
void firmware_main();

void init() {
    // Expand the encryption key.
    c_aes_expand_key(Key, ekey);

    // Set up the hash constant (encrypt 0s)
    memset(hash_const, 0, sizeof(hash_const));
    c_aes_encrypt(ekey, hash_const);

    // Set up the IV
    if(IVlen == NATURAL_IV_BITS)
    {
        memcpy(iv, IV, NATURAL_IV_BYTES);
        memset(iv + NATURAL_IV_BYTES, 0, sizeof(iv) - NATURAL_IV_BYTES - 1);
        iv[sizeof(iv) - 1] = 1;
    }
    // TODO: non-natural IV sizes
    
    // Clear the tag
    memset(tag, 0, sizeof(tag));

    pass = 0;
}

void verify() {
    if(memcmp(tag, Tag, Taglen >> 3))
    {
        pass |= FAIL_TAG;
    }

    if(memcmp(PT, CT, PTlen >> 3))
    {
        pass |= FAIL_CRYPT;
    }
}

void firmware_main() {
    while(1)
    {
        init();
        c_gcm_aes_encrypt(iv, ekey, hash_const, tag, PTlen, PT, AADlen, AAD);
        verify();
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
