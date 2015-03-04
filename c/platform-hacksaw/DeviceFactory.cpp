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

#include "platform-hacksaw/DeviceFactory.h"

namespace ahaplat
{
namespace devices
{

bool Switch::in_bounds(int switch_state)
{
    return switch_state >= 0 && switch_state <= 1;
}

Switch::Switch(int switch_state)
    : Switch(switch_state, ::master_serial++)
{
}

Switch::Switch(int switch_state, uint64_t serial)
    : Device("FlipSwitch", "A simple on/off switch device", serial), m_switch(*this, 1, false, "Switch", 0, 1, switch_state)
{
}

parser::device_type Switch::getType() const
{
    return parser::device_type::switch_;
}

void Switch::print(std::ostream& output) const
{
    output << "switch,";
    m_switch.print(output);
}

bool Switch::setValue(int value)
{
    return m_switch.setValue(value);
}

}
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
