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

#include <ostream>

#include "aha-platform/IFunction.h"

#include "Device.h"

namespace ahaplat
{
class Function : virtual public IFunction
{
public:
    Function(const Device& device, uint32_t id, bool readOnly, const char* name);
    Function(const Device& device, uint32_t id, bool readOnly, const char* name, const char* description);

    virtual const char* getDescription()            const;
    virtual uint64_t    getDevice()                 const;
    virtual uint32_t    getId()                     const;
    virtual const char* getName()                   const;
    virtual IPlatform*  getPlatform()               const;
    virtual bool        isReadOnly()                const;
    virtual void        print(std::ostream& output) const = 0;
private:
    bool                m_readOnly;
    const char*         m_description;
    const char*         m_name;
    uint64_t            m_device;
    uint32_t            m_id;
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
