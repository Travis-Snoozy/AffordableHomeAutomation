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

/*!
 * \file
 * Contains macro glue for DLL/SO handling.
 *
 * This file is sensitive to the definition (on non-definition) of BUILD_DLL.
 * If you are building a plugin (a platform for Handaxe), then you should
 * ensure that BUILD_DLL is defined during that build (it doesn't have to
 * expand to anything, just be defined).
 */

#pragma once

/*!
 * \def AHA_EXTERN
 *
 * Marks a symbol as external (both C and C++).
 *
 * Primarily for use in header files.
 */
#ifdef __cplusplus
#define AHA_EXTERN extern "C"
#else
#define AHA_EXTERN extern
#endif

/*!
 * \def AHA_PUBLIC
 *
 * Marks a symbol as public (to be exported by a DLL).
 */

/*!
 * \def AHA_PRIVATE
 *
 * Marks a symbol as private (to not be exported by a DLL).
 */

/*!
 * \def AHA_CONVENTION
 *
 * Internal.
 *
 * You probably don't want to use this; you probably want AHA_RUNTIME_EXPORT(x,y,z) instead.
 */

/*!
 * \def AHA_DLL
 *
 * The DLL handle type returned by AHA_OPEN_DLL.
 */

/*!
 * \def AHA_DLL_EXTENSION
 *
 * The appropriate file extension to use when loading a DLL.
 */

/*!
 * \def AHA_OPEN_DLL(x)
 * \param[in] x the relative path to a DLL to open.
 * \return a handle of type AHA_DLL, or NULL if the DLL could not be opened.
 *
 * Opens a DLL for use.
 */

/*!
 * \def AHA_SYM_DLL(x, y)
 * \param[in] x the AHA_DLL handle to a DLL to search.
 * \param[in] y the name of a symbol to look up in the DLL.
 * \return a void* to the desired symbol, or NULL if the symbol could not be found.
 *
 * Finds a symbol in a DLL.
 */

/*!
 * \def AHA_CLOSE_DLL(x)
 * \param[in] x the AHA_DLL handle to close.
 *
 * Closes the provided AHA_DLL handle.
 */

#ifdef __MINGW32__
#include <windows.h>
#endif // __MINGW32__

#if defined __CYGWIN__ || _WIN32
#ifdef BUILD_DLL
#define AHA_PUBLIC __declspec(dllexport)
#else
#define AHA_PUBLIC __declspec(dllimport)
#endif
#define AHA_PRIVATE
#define AHA_CONVENTION __cdecl
#define AHA_DLL HMODULE
#define AHA_DLL_EXTENSION ".dll"
#define AHA_OPEN_DLL(x) LoadLibrary(x)
#define AHA_SYM_DLL(x, y) GetProcAddress(x, y)
#define AHA_CLOSE_DLL(x) FreeLibrary(x)
#else
#if defined __GNUC__ >= 4
#define AHA_PUBLIC __attribute__ ((visibility ("default")))
#define AHA_PRIVATE  __attribute__ ((visibility ("hidden")))
#else
#define AHA_PUBLIC
#define AHA_PRIVATE
#endif
#define AHA_CONVENTION
#include <dlfcn.h>
#define AHA_DLL void*
#define AHA_DLL_EXTENSION ".so"
#define AHA_OPEN_DLL(x) dlopen(x, RTLD_LAZY)
#define AHA_SYM_DLL(x, y) dlsym(x, y)
#define AHA_CLOSE_DLL(x) dlclose(x)
#endif

/*!
 * \def AHA_LINK_EXPORT(x,y,z)
 * \param[in] x the type that the exported function will return.
 * \param[in] y the name of the function to be exported.
 * \param[in] z the parameters list of the function to be exported.
 *
 * Internal.
 *
 * You probably want to use AHA_RUNTIME_EXPORT(x,y,z) instead.
 */
#define AHA_LINK_EXPORT(x, y, z) AHA_EXTERN AHA_PUBLIC x AHA_CONVENTION y z

/*!
 * \def AHA_RUNTIME_EXPORT(x, y, z)
 * \param[in] x the return type of a DLL-implemented function.
 * \param[in] y the name of the DLL-implemented function.
 * \param[in] z the parameters list of the DLL-implemented function.
 *
 * For use in headers that declare DLL-defined functions.
 *
 * When BUILD_DLL is set (building the DLL implementing the call), this macro
 * expands to an appropriate function signature. When consuming a DLL, it
 * instead defines a function pointer named P##y, allowing the consumer to
 * easily cast the void* that comes back from AHA_SYM_DLL to a usable type.
 */
#ifdef BUILD_DLL
#define AHA_RUNTIME_EXPORT(x, y, z) AHA_LINK_EXPORT(x, y, z)
#else
#define AHA_RUNTIME_EXPORT(x, y, z) typedef x (*P##y)z
#endif

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
