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

#include <cstddef>

#include "platform-hacksaw/Device.h"
#include "platform-hacksaw/ValueFunction.h"
#include "platform-hacksaw/Platform.h"

using namespace ahaplat;

/*
Device::Device(const char* name, const char* description)
    : Device(name, description, ::master_serial++)
{
}
*/

Device::Device(const char* name, const char* description, uint64_t serial)
    : m_description(description), m_name(name), m_serial(serial)
{
    ::platform->notify(this, true);
}

Device::~Device()
{
}

const char* Device::getDescription() const
{
    return this->m_description;
}

const char* Device::getName() const
{
    return this->m_name;
}

IPlatform* Device::getPlatform() const
{
    return ::platform;
}

uint64_t Device::getSerial() const
{
    return this->m_serial;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
