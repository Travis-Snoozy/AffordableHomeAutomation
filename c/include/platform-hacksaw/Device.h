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

#include <cstdint>
#include <memory>
#include <ostream>

#include "aha-platform/IDevice.h"
#include "aha-platform/IPlatform.h"

#include "Parser.h"

namespace ahaplat
{
class Device : virtual public IDevice
{
public:
    virtual const char*         getDescription()            const;
    virtual const char*         getName()                   const;
    virtual IPlatform*          getPlatform()               const;
    virtual uint64_t            getSerial(void)             const;
    virtual parser::device_type getType()                   const = 0;
    virtual void                print(std::ostream& output) const = 0;
protected:
    Device(const char* name, const char* description);
    Device(const char* name, const char* description, uint64_t serial);
private:
    const char*                 m_description;
    const char*                 m_name;
    uint64_t                    m_serial;
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
