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

#include "platform-hacksaw/Platform.h"
#include "platform-hacksaw/Device.h"
#include "platform-hacksaw/ValueFunction.h"

AHA_PRIVATE ahaplat::Platform* platform;
AHA_PRIVATE std::atomic<uint64_t> master_serial(0);

using namespace ahaplat;

Platform::Platform(PPlatformChange platformChange, PDeviceChange deviceChange)
    : m_platformChange(platformChange), m_deviceChange(deviceChange), m_devices(), m_server()
{
}

const char* Platform::getDescription() const
{
    return "Allows client applications to create and control virtual devices.";
}

IDevice* Platform::getDevice(uint64_t device) const
{
    auto it = this->m_devices.find(device);
    if(it == this->m_devices.end()) {
        return nullptr;
    }

    return ((*it).second.first);
}

const char* Platform::getName() const
{
    return "HACKSAW";
}

void Platform::iterateDevices(boost::function<void(IDevice*)> dev) const
{
    std::for_each(this->m_devices.begin(), this->m_devices.end(), [&](std::pair<uint64_t, std::pair<IDevice*, std::map<uint32_t, IFunction*>>> dev_data) {
        dev(dev_data.second.first);
    });
}

void Platform::notify(IFunction* func)
{
    if(this->m_devices.find(func->getDevice()) == this->m_devices.end()) {
        return;
    }

    this->m_devices[func->getDevice()].second[func->getId()] = func;

    this->m_deviceChange(func);
}

bool Platform::notify(IDevice* device, bool add)
{
    bool exists = (this->m_devices.find(device->getSerial()) != this->m_devices.end());

    if(add && !exists) {
        this->m_devices[device->getSerial()].first = device;
    } else if(!add && exists) {
        this->m_devices.erase(device->getSerial());
    } else {
        return false;
    }

    this->m_platformChange(device);
    return true;
}

void Platform::start()
{
    this->m_server.start();
}

AHA_PLATFORM_INITIALIZE {
    if(::platform == nullptr) {
        ::platform = new Platform(platformChange, deviceChange);
    }

    return ::platform;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
