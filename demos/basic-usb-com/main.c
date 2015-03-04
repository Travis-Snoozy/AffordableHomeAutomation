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

#include "stddef.h"
#include "ft220x.h"

void firmware_main()
{
    char* message="Hello, world!\r\n";
    int length = 15;
    char input;
    spi_init();
    ft220x_init();
    ft220x_com_read(&input, 1);
    ft220x_com_write(message, length);
    do
    {
        ft220x_com_write(&input, 1);
        if(input == '\r')
        {
            ft220x_com_write("\n", 1);
        }
        ft220x_com_read(&input, 1);
    } while(input != '~');

    /*
    do
    {
        ft220x_com_read(&input, 1);
    } while (input < 0x80 && input > 0x60);
    P2OUT |= 0x10; // Turn on the diagnostic LED
    // ft220x_com_write(message, length);
    */

    //ft220x_com_read(&input, 1);
    // if(input > 0x61 && input < 0xff) lit
    // if(input > 0xb0 && input < 0xff) unlit
    // if(input > 0x61 && input < 0x89) lit
    // if(input > 0x61 && input < 0x75) unlit
    // if(input > 0x7f && input < 0x89) unlit
    // if(input >= 0x75 && input <= 0x7f) lit
    // if(input >= 0x7A && input <= 0x7f) unlit 
    // Has to be 0x75, 0x76, 0x77, 0x78 or 0x79
    // if(input == 0x75) unlit
    // if(input == 0x76) unlit
    // if(input == 0x77) lit

/*
    while(1)
    {
        ft220x_com_write(message, length);
    }
*/

}

void firmware_io_isr()
{
    unsigned int i;

    for(i=0; i < 0x0FFF; i++)
    {
        if(P2IN & 0x20)
        {
            i = 0;
        }
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
