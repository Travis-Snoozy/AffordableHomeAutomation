// Copyright © 2013, 2015, Travis Snoozy
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

#pragma once

#include <cstdint>

#include "IPlatform.h"

namespace ahaplat
{
/*!
 * The interface that all devices must conform to.
 */
class IDevice
{
public:
    /*!
     * Gets a brief description of the device's functionality.
     */
    virtual const char* getDescription()    const = 0;

    /*!
     * Gets the name (e.g., one word description) of this device.
     */
    virtual const char* getName()           const = 0;

    /*!
     * Gets the platform plugin that this device is registered to.
     */
    virtual IPlatform*  getPlatform()       const = 0;

    /*!
     * Gets the unique serial number that identifies this device within its platform.
     */
    virtual uint64_t    getSerial(void)     const = 0;
protected:
private:
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
