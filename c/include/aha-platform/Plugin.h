// Copyright © 2015, Travis Snoozy
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

#include "AhaMacroGlue.h"
#include "IDevice.h"
#include "IFunction.h"

namespace ahaplat
{

/*!
 * Function pointer for sending IFunction update notifications.
 *
 * This occurs when an IFunction first shows up (i.e., newly-registered device),
 * and when an IFunction changes value.
 */
typedef void            (*PDeviceChange)(IFunction*);

/*!
 * Function pointer for sending IDevice update notifications.
 *
 * This occurs when an IDevice is newly registered, and also when its deleted.
 */
typedef void            (*PPlatformChange)(IDevice*);

/*!
 * \def AHA_PLATFORM_INITIALIZE
 * Clients should use this macro to declare the function signature for their
 * plugin's entry point. The plugin will need to declare the body of this
 * function immediately after this macro.
 */
#define AHA_PLATFORM_INITIALIZE AHA_RUNTIME_EXPORT(ahaplat::IPlatform*, GetPlatform, (ahaplat::PPlatformChange platformChange, ahaplat::PDeviceChange deviceChange))

AHA_PLATFORM_INITIALIZE;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
