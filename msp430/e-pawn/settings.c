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

#include "settings.h"

info_t* get_settings()
{
    // If infob is unwritten/erased, use_c will be 0xFF (meaning we
    // should use infoc instead).
    if(__infob.use_c == 0)
    {
        return &__infob;
    }

    return &__infoc;

    // The following procedure ensures we can safely switch between
    // the infob and infoc sections:
    // - If infob is written...
    //   1. Write infoc
    //   2. Erase infob
    //   - Infoc will be used when infob is cleared, because infob.usec
    //     will revert to 0xFF.
    // - If infob us unwritten...
    //   1. Write all parts of infob except usec
    //   2. Write 0 to usec
    //   3. Erase infoc
    //   - Infob will be used once usec is cleared.
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
