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
#include "platform-hacksaw/Function.h"
#include "platform-hacksaw/Platform.h"

using namespace ahaplat;

/*
Function::Function(const Device& device, uint32_t id, bool readOnly, const char* name)
    : Function(device, id, readOnly, name, nullptr)
{
}
*/

Function::Function(const Device& device, uint32_t id, bool readOnly, const char* name, const char* description)
    : m_readOnly(readOnly), m_description(description), m_name(name), m_device(device.getSerial()), m_id(id)
{
}

const char* Function::getDescription() const
{
    return this->m_description;
}

uint64_t Function::getDevice() const
{
    return this->m_device;
}

uint32_t Function::getId() const
{
    return this->m_id;
}

const char* Function::getName() const
{
    return this->m_name;
}

IPlatform* Function::getPlatform() const
{
    return ::platform;
}

bool Function::isReadOnly() const
{
    return this->m_readOnly;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
