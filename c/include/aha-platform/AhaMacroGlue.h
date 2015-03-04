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

#ifdef __cplusplus
#define AHA_EXTERN extern "C"
#else
#define AHA_EXTERN extern
#endif

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

#define AHA_LINK_EXPORT(x, y, z) AHA_EXTERN AHA_PUBLIC x AHA_CONVENTION y z

#ifdef BUILD_DLL
#define AHA_RUNTIME_EXPORT(x, y, z) AHA_LINK_EXPORT(x, y, z)
#else
#define AHA_RUNTIME_EXPORT(x, y, z) typedef x (*P##y)z
#endif

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
