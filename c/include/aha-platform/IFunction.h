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

#pragma once

namespace ahaplat
{
class IFunction;
}

#include "IPlatform.h"

namespace ahaplat
{
/*!
 * The base class for device functions, declaring all the data-independent
 * metadata about the function's purpose.
 *
 * You probably don't want to work with this class (including deriving from
 * it) -- look at the child classes that derive from this class instead.
 */
class IFunction
{
public:
    /*!
     * Gets a brief text description of this function's functionality.
     */
    virtual const char* getDescription()    const = 0;

    /*!
     * Gets the unique serial number of the device this function is a part of.
     */
    virtual uint64_t    getDevice()         const = 0;

    /*!
     * Gets the function ID of this function within the given device.
     */
    virtual uint32_t    getId()             const = 0;

    /*!
     * Gets the name (e.g., one word description) of this function within the device.
     */
    virtual const char* getName()           const = 0;

    /*!
     * Gets the platform that this function's device belongs to.
     */
    virtual IPlatform*  getPlatform()       const = 0;

    /*!
     * \return true if this function is enabled for "set" commands; false otherwise.
     *
     * Gets the read-only status of this function.
     */
    virtual bool        isReadOnly()        const = 0;
protected:
private:
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
