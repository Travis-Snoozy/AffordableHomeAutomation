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

void ft220x_hook__bus_turnaround(int write)
{
    if(write)
    {
        // Set the pin(s) to the output direction
        // (Write 1 to the correspending PxDIR regs)
       *FT220X_TURN_MIOSIO_0 |= FT220X_B_MOSI_0;
       *FT220X_A_MOSI_0 &= ~FT220X_B_MOSI_0;
#if FT220X_WIDTH > 1
       *FT220X_TURN_MIOSIO_1 |= FT220X_B_MOSI_1;
       *FT220X_A_MOSI_1 &= ~FT220X_B_MOSI_1;
#endif
#if FT220X_WIDTH > 2
       *FT220X_TURN_MIOSIO_2 |= FT220X_B_MOSI_2;
       *FT220X_A_MOSI_2 &= ~FT220X_B_MOSI_2;
       *FT220X_TURN_MIOSIO_3 |= FT220X_B_MOSI_3;
       *FT220X_A_MOSI_3 &= ~FT220X_B_MOSI_3;
#endif
    }
    else
    {
        // Set the pin(s) to the input direction
        // (write 0 to the corresponding PxDIR regs)
       *FT220X_TURN_MIOSIO_0 &= ~FT220X_B_MOSI_0;
#if FT220X_WIDTH > 1
       *FT220X_TURN_MIOSIO_1 &= ~FT220X_B_MOSI_1;
#endif
#if FT220X_WIDTH > 2
       *FT220X_TURN_MIOSIO_2 &= ~FT220X_B_MOSI_2;
       *FT220X_TURN_MIOSIO_3 &= ~FT220X_B_MOSI_3;
#endif
    }
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
