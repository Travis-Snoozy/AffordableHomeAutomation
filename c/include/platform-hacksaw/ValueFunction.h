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

#include "aha-platform/IValueFunction.h"

#include "Function.h"

namespace ahaplat
{
template <class T>
class ValueFunction : public Function, virtual public IValueFunction<T>
{
public:
    ValueFunction(const Device& device, uint32_t id, bool readOnly, const char* name, T minimum, T maximum, T value);
    ValueFunction(const Device& device, uint32_t id, bool readOnly, const char* name, const char* description, T minimum, T maximum, T value);

    virtual T       getMax()                        const;
    virtual T       getMin()                        const;
    virtual T       getValue()                      const;
    virtual bool    platformSetValue(T newValue);
    virtual void    print(std::ostream& output)     const;
    virtual bool    setValue(T newValue);
private:
    bool            m_isReadOnly;
    T               m_max;
    T               m_min;
    T               m_value;
};

template <class T>
ValueFunction<T>::ValueFunction(const Device& device, uint32_t id, bool readOnly, const char* name, T minimum, T maximum, T value)
    : ValueFunction(device, id, readOnly, name, nullptr, minimum, maximum, value)
{
}

template <class T>
ValueFunction<T>::ValueFunction(const Device& device, uint32_t id, bool readOnly, const char* name, const char* description, T minimum, T maximum, T value)
    : Function(device, id, readOnly, name, description), m_max(maximum), m_min(minimum)
{
    this->platformSetValue(value);
}

template<class T>
T ValueFunction<T>::getMax() const
{
    return this->m_max;
}

template<class T>
T ValueFunction<T>::getMin() const
{
    return this->m_min;
}

template<class T>
T ValueFunction<T>::getValue() const
{
    return this->m_value;
}

template<class T>
bool ValueFunction<T>::platformSetValue(T value)
{
    if(value > this->m_max || value < this->m_min) {
        return false;
    }

    this->m_value = value;

    ::platform->notify((IValueFunction<T>*)(this));

    return true;
}

template<class T>
void ValueFunction<T>::print(std::ostream& output) const
{
    output << this->m_value;
}

template<class T>
bool ValueFunction<T>::setValue(T value)
{
    if(this->isReadOnly()) {
        return false;
    }

    return this->platformSetValue(value);
}
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
