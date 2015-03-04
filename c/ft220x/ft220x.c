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

#include "ft220x.h"
#include "spi.h"

#if FT220X_WIDTH != 1 && FT220X_WIDTH != 2 && FT220X_WIDTH != 4
  #error "You must define FT220X_WIDTH as 1, 2, or 4."
#endif

#ifndef FT220X_A_MISO_0
  #error "You must always define at least one bus input address."
#endif

#ifndef FT220X_B_MISO_0
  #error "You must always define at least one bus input address mask."
#endif

#ifndef FT220X_A_MOSI_0
  #error "You must always define at least one bus output address."
#endif

#ifndef FT220X_B_MOSI_0
  #error "You must always define at least one bus output address mask."
#endif

#if FT220X_WIDTH > 1
  #ifndef FT220X_A_MISO_1
    #error "You must define FT220X_A_MISO_1 (bus width 2+)"
  #endif
  #ifndef FT220X_B_MISO_1
    #error "You must define FT220X_B_MISO_1 (bus width 2+)"
  #endif
  #ifndef FT220X_A_MOSI_1
    #error "You must define FT220X_A_MOSI_1 (bus width 2+)"
  #endif
  #ifndef FT220X_B_MOSI_1
    #error "You must define FT220X_B_MOSI_1 (bus width 2+)"
  #endif
#endif

#if FT220X_WIDTH > 2
  #ifndef FT220X_A_MISO_2
    #error "You must define FT220X_A_MISO_2 (bus width 4)"
  #endif
  #ifndef FT220X_B_MISO_2
    #error "You must define FT220X_B_MISO_2 (bus width 4)"
  #endif
  #ifndef FT220X_A_MOSI_2
    #error "You must define FT220X_A_MOSI_2 (bus width 4)"
  #endif
  #ifndef FT220X_B_MOSI_2
    #error "You must define FT220X_B_MOSI_2 (bus width 4)"
  #endif
  #ifndef FT220X_A_MISO_3
    #error "You must define FT220X_A_MISO_3 (bus width 4)"
  #endif
  #ifndef FT220X_B_MISO_3
    #error "You must define FT220X_B_MISO_3 (bus width 4)"
  #endif
  #ifndef FT220X_A_MOSI_3
    #error "You must define FT220X_A_MOSI_3 (bus width 4)"
  #endif
  #ifndef FT220X_B_MOSI_3
    #error "You must define FT220X_B_MOSI_3 (bus width 4)"
  #endif
#endif

typedef struct {
    const SPI_A_T* address;
    SPI_A_T mask;
} ft220x_iline_t;

typedef struct {
    SPI_A_T* address;
    SPI_A_T mask;
} ft220x_oline_t;

static const ft220x_iline_t ilines[] = {
    { FT220X_A_MISO_0, FT220X_B_MISO_0 },
#if FT220X_WIDTH > 1
    { FT220X_A_MISO_1, FT220X_B_MISO_1 },
  #if FT220X_WIDTH > 2
    { FT220X_A_MISO_2, FT220X_B_MISO_2 },
    { FT220X_A_MISO_3, FT220X_B_MISO_3 },
  #endif
#endif
};

static const ft220x_oline_t olines[] = {
    { FT220X_A_MOSI_0, FT220X_B_MOSI_0 },
#if FT220X_WIDTH > 1
    { FT220X_A_MOSI_1, FT220X_B_MOSI_1 },
  #if FT220X_WIDTH > 2
    { FT220X_A_MOSI_2, FT220X_B_MOSI_2 },
    { FT220X_A_MOSI_3, FT220X_B_MOSI_3 },
  #endif
#endif
};

uint8_t ft220x_write(int16_t command, uint8_t* data)
{
    spi_start(1);
    uint8_t i;
    uint8_t retval = 0;
    uint8_t line = 0;

    // We only clock out 7 for the command.
    for(i = 7; i > 0; i--)
    {
        // Set/clear the write line
        if ( command & 0x01 )
        {
            *olines[line].address |= olines[line].mask;
        } else {
            *olines[line].address &= ~olines[line].mask;
        }

        // Shift down the write
        command >>= 1;

        // Determine which line we're operating on next
        line++;
        line %= FT220X_WIDTH;

        // Only clock if we've written all the lines.
        if(line == 0 || i == 1)
        {
            // Clock
            SPI_A_CLOCK ^= SPI_B_CLOCK;
            // Clock
            SPI_A_CLOCK ^= SPI_B_CLOCK;
        }
    }

    // Shift out the unused bit 8
    command >>= 1;

    // Reset the line pointer
    line = 0;

    // Turn around the bus
    // This goes here, instead of in the switch(), because the order
    // of operations (bus turnaround, clock, read status, clock) is
    // critical.
    if(command == FT220X_M_READ)
    {
        ft220x_hook__bus_turnaround(0);
    }

    SPI_A_CLOCK ^= SPI_B_CLOCK;
    
    // Capture the buffer status
    retval |= (SPI_A_MISO & SPI_B_MISO) ? 0x01 : 0x00;

    SPI_A_CLOCK ^= SPI_B_CLOCK;

    switch(command)
    {
        case FT220X_M_READ:
            if(!retval) {
                retval |= 0x04;
                *data = 0;
                for(i = 8; i > 0; i--)
                {
                    // Shift up the read
                    *data <<= 1;

                    // Read the current bit
                    *data += (*ilines[line].address & ilines[line].mask) ? 1 : 0;

                    // Determine which line we're operating on next
                    line++;
                    line %= FT220X_WIDTH;

                    // Only clock if we've read all the lines.
                    if(line == 0 || i == 1)
                    {
                        // Clock
                        SPI_A_CLOCK ^= SPI_B_CLOCK;

                        // If this is the first chunk we clocked in, read the ACK.
                        if(retval & 0x04)
                        {
                            retval ^= 0x04;
                            // Low: Acknowledged. Continue. (return error zero/false)
                            // High: Negative. Abort. (return error nonzero/true)
                            retval |= (SPI_A_MISO & SPI_B_MISO) ? 0x02 : 0x00;

                            if(retval)
                            {
                                break;
                            }
                        }

                        // Clock
                        SPI_A_CLOCK ^= SPI_B_CLOCK;
                    }
                }
            }
            
            // Clock, turnaround, chip deselect
            SPI_A_CLOCK ^= SPI_B_CLOCK;

            // Reset the bus to its default/idle state (write)
            ft220x_hook__bus_turnaround(1);

            break;
        case FT220X_M_FLUSH:
            break;
        case FT220X_M_ADDR:
            // TODO
            break;
        case FT220X_M_WRITE:
            i = 8;
            command = data[0];
            command <<= 8;
            if(!retval) {
                retval |= 0x04;
                for(; i > 0; i--)
                {
                    // Set/clear the write line
                    if ( command < 0 )
                    {
                        *olines[line].address |= olines[line].mask;
                    } else {
                        *olines[line].address &= ~olines[line].mask;
                    }

                    // Shift up the write (clocking out in MSB order)
                    command <<= 1;

                    // Determine which line we're operating on next
                    line++;
                    line %= FT220X_WIDTH;

                    // Only clock if we've written all the lines.
                    if(line == 0 || i == 1)
                    {
                        // Clock
                        SPI_A_CLOCK ^= SPI_B_CLOCK;

                        // If this is the first chunk we clocked out, read the ACK.
                        if(retval & 0x04)
                        {
                            retval ^= 0x04;
                            // Low: Acknowledged. Continue. (return error zero/false)
                            // High: Negative. Abort. (return error nonzero/true)
                            retval |= (SPI_A_MISO & SPI_B_MISO) ? 0x02 : 0x00;

                            if(retval)
                            {
                                break;
                            }
                        }

                        // Clock
                        SPI_A_CLOCK ^= SPI_B_CLOCK;
                    }
                }
            }

            // Final clock before deselect
            SPI_A_CLOCK  ^= SPI_B_CLOCK;

            // The bus is already in its default/idle state (write),
            // so there's no need to do a turnaround here.

            break;
    }

    spi_stop();
    return retval;

}

void ft220x_init()
{
    ft220x_hook__bus_turnaround(1);
    spi_select(FT220X_SPI_A_ENABLE, FT220X_SPI_B_ENABLE);
}

uint8_t ft220x_com_write(uint8_t* buffer, unsigned int count)
{
    for(; count > 0; count--)
    {
        while(ft220x_write(FT220X_CMD_WRITE, buffer));
        buffer++;
    }

    return 0;
}

uint8_t ft220x_com_read(uint8_t* buffer, unsigned int count)
{
    for(; count > 0; count--)
    {
        while(ft220x_write(FT220X_CMD_READ, buffer));
        buffer++;
    }

    return 0;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
