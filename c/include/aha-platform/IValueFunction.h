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

#include "IFunction.h"

namespace ahaplat
{
template <class T>
/*!
 * \invariant getMax() > getMin()
 * \tparam T a comparable value type.
 *
 * Represents a device function that returns a bounded value.
 */
class IValueFunction : virtual public IFunction
{
public:
    /*!
     * Gets the largest value this Function supports.
     *
     * This function returns an immutable value for its Function object.
     * getValue() will never return a larger value, and setValue(T) will not
     * accept a larger value, than this function returns.
     */
    virtual T       getMax()                const = 0;

    /*!
     * Gets the smallest value this Function supports.
     *
     * This function returns an immutable value for its Function object.
     * getValue() will never return a smaller value, and setValue(T) will not
     * accept a smaller value, than this function returns.
     */
    virtual T       getMin()                const = 0;

    /*!
     * Gets the current value of this Function.
     */
    virtual T       getValue()              const = 0;

    /*!
     * \pre \p newValue <= getMax()
     * \pre \p newValue >= getMin()
     * \pre !isReadOnly()
     * \post getValue() == newValue
     *
     * Sets the current value of this Function.
     */
    virtual bool    setValue(T newValue)          = 0;
protected:
private:
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
