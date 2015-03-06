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
class IPlatform;
}

#include "AhaMacroGlue.h"
#include "IDevice.h"
#include "IFunction.h"

namespace ahaplat
{
/*!
 * Represents a handle to a platform (plugin).
 */
class IPlatform
{
public:
    /*!
     * Gets a brief description of the platform.
     */
    virtual const char* getDescription()    const = 0;

    /*!
     * Gets the name (e.g., one word description) of the platform.
     */
    virtual const char* getName()           const = 0;

    /*!
     * Inform the platform that it may start processing input/sending messages
     * to the host.
     */
    virtual void        start()                   = 0;
protected:
private:
};

typedef void            (*PDeviceChange)(IFunction*);
typedef void            (*PPlatformChange)(IDevice*);

// Platform plug-ins need to implement this function.
#define AHA_PLATFORM_INITIALIZE AHA_RUNTIME_EXPORT(ahaplat::IPlatform*, GetPlatform, (ahaplat::PPlatformChange platformChange, ahaplat::PDeviceChange deviceChange))
AHA_PLATFORM_INITIALIZE;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
