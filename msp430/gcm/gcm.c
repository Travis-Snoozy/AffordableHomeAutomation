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

#include <stdint.h>
#include <string.h>

#include "gcm.h"
#include "aes.h"

void c_gcm_galois_mult(uint8_t* output, uint8_t* hash_constant);
void c_gcm_iv_incr(uint8_t* iv);


void c_gcm_aes_encrypt( uint8_t* iv,
                        uint8_t* ekey,
                        uint8_t* hash_const,
                        uint8_t* tag,
                        uint16_t ptlen,
                        uint8_t* pt
#ifdef GCM_ENABLE_AAD
                        ,
                        uint16_t aadlen,
                        uint8_t* aad
#endif // GCM_ENABLE_AAD
                        ) {
    uint8_t i;
    uint8_t offset;
    uint8_t limit;
    uint8_t end;
    uint8_t iv_const[16];
    uint8_t crypt_work[16];

    // Calculate E(K, Ysub0)
    // (And zero out the tag in passing...)
    for(i = 0; i < 8; i++)
    {
        ((uint16_t*)iv_const)[i] = ((uint16_t*)iv)[i];
        ((uint16_t*)tag)[i] = 0;
    }
    c_aes_encrypt(ekey, iv_const);

#ifdef GCM_ENABLE_AAD
    // Calculate Csub1
    end = aadlen >> 3;

    for(offset = 0; offset < end; offset += 16)
    {
        limit = end - offset;
        limit = limit > 16 ? 16 : limit;
        for(i = 0; i < limit; i++)
        {
            tag[i] ^= aad[offset+i];
        }
        c_gcm_galois_mult(tag, hash_const);
    }
#endif // GCM_ENABLE_AAD

    end = ptlen >> 3;

    for(offset = 0; offset < end; offset += 16)
    {
        limit = end - offset;
        limit = limit > 16 ? 16 : limit;
        c_gcm_iv_incr(iv);
        for(i = 0; i < 8; i++)
        {
            ((uint16_t*)crypt_work)[i] = ((uint16_t*)iv)[i];
        }
        c_aes_encrypt(ekey, crypt_work);

        for(i = 0; i < limit; i++)
        {
            pt[offset+i] ^= crypt_work[i];
            tag[i] ^= pt[offset+i];
        }
        c_gcm_galois_mult(tag, hash_const);
    }

    // Calculate tag
#ifdef GCM_ENABLE_AAD
    tag[6] ^= aadlen >> 8;
    tag[7] ^= aadlen & 0x00FF;
#endif // GCM_ENABLE_AAD
    tag[14] ^= ptlen >> 8;
    tag[15] ^= ptlen & 0x00FF;
    c_gcm_galois_mult(tag, hash_const);
    for(i = 0; i < 8; i++)
    {
        ((uint16_t*)tag)[i] ^= ((uint16_t*)iv_const)[i];
    }
}

uint8_t c_gcm_aes_decrypt(  uint8_t* iv,
                            uint8_t* ekey,
                            uint8_t* hash_const,
                            uint8_t taglen,
                            uint8_t* tag,
                            uint16_t ctlen,
                            uint8_t* ct
#ifdef GCM_ENABLE_AAD
                            ,
                            uint16_t aadlen,
                            uint8_t* aad
#endif // GCM_ENABLE_AAD
                        ) {
    uint8_t i;
    uint8_t offset;
    uint8_t limit;
    uint8_t end;
    int8_t match = 1;
    uint8_t iv_const[16];
    uint8_t crypt_work[16];
    uint8_t tag_work[16];

    // Calculate E(K, Ysub0)
    for(i = 0; i < 8; i++)
    {
        ((uint16_t*)iv_const)[i] = ((uint16_t*)iv)[i];
        ((uint16_t*)tag_work)[i] = 0;
    }
    c_aes_encrypt(ekey, iv_const);

#ifdef GCM_ENABLE_AAD
    // Calculate Csub1
    end = aadlen >> 3;

    for(offset = 0; offset < end; offset += 16)
    {
        limit = end - offset;
        limit = limit > 16 ? 16 : limit;
        for(i = 0; i < limit; i++)
        {
            tag_work[i] ^= aad[offset+i];
        }
        c_gcm_galois_mult(tag_work, hash_const);
    }
#endif // GCM_ENABLE_AAD

    end = ctlen >> 3;

    for(offset = 0; offset < end; offset += 16)
    {
        limit = end - offset;
        limit = limit > 16 ? 16 : limit;
        c_gcm_iv_incr(iv);
        for(i = 0; i < 8; i++)
        {
            ((uint16_t*)crypt_work)[i] = ((uint16_t*)iv)[i];
        }
        c_aes_encrypt(ekey, crypt_work);

        for(i = 0; i < limit; i++)
        {
            tag_work[i] ^= ct[offset+i];
            ct[offset+i] ^= crypt_work[i];
        }
        c_gcm_galois_mult(tag_work, hash_const);
    }

    // Calculate tag
#ifdef GCM_ENABLE_AAD
    tag_work[6] ^= aadlen >> 8;
    tag_work[7] ^= aadlen & 0x00FF;
#endif // GCM_ENABLE_AAD
    tag_work[14] ^= ctlen >> 8;
    tag_work[15] ^= ctlen & 0x00FF;
    c_gcm_galois_mult(tag_work, hash_const);

    // Verify the tags match
    taglen >>= 3;
    for(i = 0; i < taglen; i++)
    {
        tag_work[i] ^= iv_const[i];
        if(tag_work[i] != tag[i]) {
            match = 0;
        }
    }

    return match;
}
// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
