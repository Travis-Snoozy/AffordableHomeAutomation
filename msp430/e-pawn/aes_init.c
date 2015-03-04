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
#include "string.h"
#include "settings.h"
#include "aes.h"

void aes_init_recalc(info_t* settings)
{
    uint8_t counter;

    // Expand the AES key in memory, and write the result to flash.
    {
        uint8_t ekey[176];
        c_aes_expand_key(settings->key, ekey);
        flash_erase(expanded_key);
        flash_unlock();
        for(counter = 0; counter < sizeof(ekey); counter++)
        {
            expanded_key[counter] = ekey[counter];
        }
    }

    // Calculate the hash key in memory, and write the result to flash.
    {
        uint8_t hk[16];
        memset(hk, 0, sizeof(hk));
        c_aes_encrypt(expanded_key, hk);
        for(counter = 0; counter < sizeof(hk); counter++)
        {
            hash_key[counter] = hk[counter];
        }
    }

    // Set the key segment to 0, so we know that the current key has been
    // expanded, the hash key calculated, and all the results successfully
    // stored.
    key_segment = 0;
    flash_lock();
}

void aes_init()
{
    info_t* settings = get_settings();

    if (key_segment != 0xFF)
    {
        // The key segment is set. Verify its correctness.

        // See if the expanded key matches up with the key we are currently
        // using.
        int i;
        for(i = 0; i < 16; i++)
        {
            if(settings->key[i] != expanded_key[i])
            {
                // The expanded key does not match the current key.
                // Recalculate the expanded key and hash key, based on
                // the actual current key.
                aes_init_recalc(settings);
                return;
            }
        }

        // The key and expanded key match up; we're ready to rock.
        // The hash key, by extension, must also be set and match the
        // key (otherwise, key_segment would have been 0xFF).
    }
    else
    {
        // key_segment is unset, or only partially set. Calculate it.
        aes_init_recalc(settings);
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
