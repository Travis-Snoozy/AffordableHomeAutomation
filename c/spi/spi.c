// Copyright © 2013, Travis Snoozy
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  - Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  - Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

// Generic SPI bit-bang code
// Version: 0.1.0

#include "spi.h"

// SPI_A_CLOCK -- Address for the clocking pin
// SPI_B_CLOCK -- Bit for the clocking pin
// SPI_A_MISO  -- Address for the MISO pin
// SPI_B_MISO  -- Bit for the MISO pin
// SPI_A_MOSI  -- Address for the MOSI pin
// SPI_B_MOSI  -- Bit for the MOSI pin
// SPI_A_T     -- The type of address the target arch uses for its I/O

// SPI_A_ENABLE-- Address for the CS/enable pin
static SPI_A_T* SPI_A_ENABLE;

// SPI_B_ENABLE-- Bit for the CS/enable pin
static SPI_A_T SPI_B_ENABLE;

void spi_init()
{
    // We must set these values here. Setting them as part of their declaration
    // merely causes them to be programmed into RAM with a value of 0 -- it does
    // not cause them to be reset to this value if the chip loses power or is
    // otherwise reset. spi_init() can (and should) be called as part of POR,
    // causing these values to be defined in the case of resets, power losses,
    // etc.
    //
    // Furthermore, we do need to initialize these to 0, as spi_stop will be
    // called as part of spi_select() -- and spi_stop() needs to know if these
    // values are uninitialized (e.g., 0) so that it can no-op in that case.
    SPI_A_ENABLE = 0;
    SPI_B_ENABLE = 0;
}

int spi_select(SPI_A_T* address, uint8_t bit)
{
    if(bit >= (sizeof(SPI_A_T) * 8) || address == 0)
    {
        return -1;
    }

    spi_stop();

    SPI_A_ENABLE = address;
    SPI_B_ENABLE = (1 << bit);

    return 0;
}

void spi_start(uint8_t mode)
{
    ////
    // Set the polarity
    ////

    // Clock line needs to be set per the mode,
    // prior to enabling the chip.
    if ( mode & 0x02 )
    {
        SPI_A_CLOCK |= SPI_B_CLOCK;
    } else {
        SPI_A_CLOCK &= ~SPI_B_CLOCK;
    }

    // Enable the chip
    *SPI_A_ENABLE &= ~(SPI_B_ENABLE);

    ////
    // Set the phase
    ////

    // Clock-write-clock-read-...
    if ( mode & 0x01 )
    {
        SPI_A_CLOCK ^= SPI_B_CLOCK;
    }
    // else...
    // Write-clock-read-clock-...
}

void spi_write(const uint8_t* out, uint8_t* in, int count)
{
    unsigned int i = 0;
    uint8_t w = 0;
    uint8_t r = 0;

    while(count > 0)
    {
        w = *out;
        r = 0;
        for(i = 0; i < 8; i++)
        {
            // Shift up the read (doesn't matter for the first round)
            r <<= 1;

            // Set/clear the write line
            if ( w & 0x80 )
            {
                SPI_A_MOSI |= SPI_B_MOSI;
            } else {
                SPI_A_MOSI &= ~SPI_B_MOSI;
            }

            // Shift up the write
            w <<= 1;

            // Clock
            SPI_A_CLOCK ^= SPI_B_CLOCK;

            // Set/clear the read-in bit
            if ( SPI_A_MISO & SPI_B_MISO )
            {
                r += 1;
            }

            // Clock
            SPI_A_CLOCK ^= SPI_B_CLOCK;
            count--;
            if(count == 0)
            {
                break;
            }
        }
        *in = r;

        // Move to the next buffer position
        in++;
        out++;
    }
}

void spi_stop()
{
    // Bail if SPI_A_ENABLE is 0.
    if(!SPI_A_ENABLE)
    {
        return;
    }
    // Disable the chip
    *SPI_A_ENABLE |= SPI_B_ENABLE;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
