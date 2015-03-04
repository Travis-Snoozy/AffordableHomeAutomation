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

#include "Device.h"
#include "ValueFunction.h"

namespace ahaplat
{
namespace devices
{

class Switch : public ahaplat::Device
{
public:
    static  bool                in_bounds(int switch_state);

    Switch(int switch_state);
    Switch(int switch_state, uint64_t serial);

    parser::device_type         getType()                   const;
    void                        print(std::ostream& output) const;
    bool                        setValue(int value);
private:
    ahaplat::ValueFunction<int> m_switch;
};

class DeviceFactory
{
public:
    template<typename T, typename... Args>
    static T* makeDevice(uint64_t* serial, Args& ... args) {
        if(!T::in_bounds(args...)) {
            return nullptr;
        }

        if(serial == nullptr) {
            return new T(args...);
        } else {
            return new T(args..., *serial);
        }
    }
};

}
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
