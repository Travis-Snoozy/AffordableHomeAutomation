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

#include <atomic>
#include <map>
#include <memory>

#include "aha-platform/AhaMacroGlue.h"
#include "aha-platform/IDevice.h"
#include "aha-platform/IFunction.h"

#include "Server.h"

namespace ahaplat
{
class Platform : virtual public IPlatform
{
public:
    Platform(PPlatformChange platformChange, PDeviceChange deviceChange);

    virtual const char* getDescription() const;
    IDevice*            getDevice(uint64_t device) const;
    virtual const char* getName() const;
    virtual void        iterateDevices(boost::function<void(IDevice*)> dev) const;
    virtual void        notify(IFunction* func);
    virtual bool        notify(IDevice* device, bool add);
    virtual void        start();
private:
    PPlatformChange     m_platformChange;
    PDeviceChange       m_deviceChange;
    std::map<uint64_t, std::pair<IDevice*, std::map<uint32_t, IFunction*>>> m_devices;
    Server              m_server;
};
}

extern AHA_PRIVATE ahaplat::Platform*       platform;
extern AHA_PRIVATE std::atomic<uint64_t>    master_serial;

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
