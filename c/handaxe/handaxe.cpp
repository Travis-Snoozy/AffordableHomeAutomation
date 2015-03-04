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

#include <cstdlib>
#include <iostream>
#include <locale>
#include <utility>
#include <typeinfo>
#include <map>

#include "boost/filesystem.hpp"
#include "aha-platform/IPlatform.h"
#include "aha-platform/IValueFunction.h"

#include "handaxe/Device.h"

std::map<ahaplat::IPlatform*, std::map<uint64_t, std::pair<Device, std::map<uint32_t, ahaplat::IFunction*>>>> state;

void deviceAttachDetachHandler(ahaplat::IDevice* device)
{
    auto it = state[device->getPlatform()].find(device->getSerial());
    if(it == state[device->getPlatform()].end()) {
        Device& dev = state[device->getPlatform()][device->getSerial()].first;
        dev.Description = device->getDescription();
        dev.Name = device->getName();
        dev.Platform = device->getPlatform();
        dev.Serial = device->getSerial();
        std::cout << "Device added: " << device->getPlatform()->getName() << ":" << std::hex << device->getSerial() << "(" << device->getName() << ")" << std::endl;
    } else {
        state[device->getPlatform()].erase(device->getSerial());
        std::cout << "Device removed: " << device->getPlatform()->getName() << ":" << std::hex << device->getSerial() << "(" << device->getName() << ")" << std::endl;
    }
}

void deviceUpdateHandler(ahaplat::IFunction* newValue)
{
    std::cout << newValue->getPlatform()->getName() << ":" << std::hex << newValue->getDevice() << "(" << state[newValue->getPlatform()][newValue->getDevice()].first.Name << "):" << newValue->getName() << "(" << std::hex << newValue->getId() << "): ";

    if(ahaplat::IValueFunction<int>* downcast = dynamic_cast<ahaplat::IValueFunction<int>*>(newValue)) {
        state[newValue->getPlatform()][newValue->getDevice()].second[newValue->getId()] = newValue;
        std::cout << " <int> " << downcast->getValue();
    } else {
        std::cout << "Value changed for an unsupported type.";
    }

    std::cout << std::endl;
}

std::pair<ahaplat::IPlatform*, AHA_DLL>* load_platform_iterator(boost::filesystem::directory_entry& platformFile)
{
    std::pair<ahaplat::IPlatform*, AHA_DLL>* retval = nullptr;
    ahaplat::PGetPlatform getPlatform = nullptr;
    ahaplat::IPlatform* platform = nullptr;
    AHA_DLL dll = nullptr;

    if(platformFile.path().extension().compare(std::string(AHA_DLL_EXTENSION)) != 0) {
        std::cerr << "Skipping non-plug-in file: " << platformFile << std::endl;
        return retval;
    }

    dll = AHA_OPEN_DLL(platformFile.path().string().c_str());

    if(dll == nullptr) {
        std::cerr << "Failed to open plug-in: " << platformFile << std::endl;
    } else {
        getPlatform = (ahaplat::PGetPlatform)AHA_SYM_DLL(dll, "GetPlatform");
    }

    if(getPlatform != nullptr) {
        platform = (*getPlatform)(deviceAttachDetachHandler, deviceUpdateHandler);
    } else if(dll != nullptr) {
        std::cerr << "Failed to find GetPlatform symbol in plug-in: " << platformFile << std::endl;
    }

    if(getPlatform != nullptr && platform == nullptr) {
        std::cerr << "Failed to initialize platform: " << platformFile << std::endl;
    }

    if(dll != nullptr) {
        if(platform != nullptr) {
            retval = new std::pair<ahaplat::IPlatform*, AHA_DLL>(platform, dll);
            std::cout << "Loaded platform: " << platform->getName() << std::endl;
        } else {
            AHA_CLOSE_DLL(dll);
        }
    }

    return retval;
}

bool is_null(void* pointer)
{
    return pointer == nullptr;
}

std::vector<std::pair<ahaplat::IPlatform*, AHA_DLL>*>* LoadAllPlugins(const boost::filesystem::path* pluginPath)
{
    std::vector<std::pair<ahaplat::IPlatform*, AHA_DLL>*>* retval = nullptr;
    std::vector<std::pair<ahaplat::IPlatform*, AHA_DLL>*>::iterator end;

    if(pluginPath == nullptr) {
        return retval;
    }

    if(!boost::filesystem::is_directory(*pluginPath)) {
        std::cerr << "Platform plug-in directory does not exist: " << *pluginPath << std::endl;
        return retval;
    }

    retval = new std::vector<std::pair<ahaplat::IPlatform*, AHA_DLL>*>();

    std::transform(boost::filesystem::directory_iterator(*pluginPath), boost::filesystem::directory_iterator(), std::back_inserter(*retval), load_platform_iterator);
    end = std::remove_if(retval->begin(), retval->end(), is_null);
    retval->resize(end - retval->begin());

    if(retval->size() == 0) {
        std::cerr << "No plug-ins present." << std::endl;
    }

    return retval;
}

void cleanup_platform_iterator(std::pair<ahaplat::IPlatform*, AHA_DLL>* platform)
{
    std::cout << "Cleaned up platform: " << platform->first->getName() << std::endl;
    AHA_CLOSE_DLL(platform->second);
    delete platform;
    platform = nullptr;
}

void start_platform_iterator(std::pair<ahaplat::IPlatform*, AHA_DLL>* platform)
{
    platform->first->start();
}

int main(int argc, char** argv)
{
    int retval = EXIT_SUCCESS;
    std::vector<std::pair<ahaplat::IPlatform*, AHA_DLL>*>* platforms = nullptr;

    boost::filesystem::path pluginPath(argv[0]);

    pluginPath.remove_filename();
    pluginPath /= "platforms";
    pluginPath.normalize();

    platforms = LoadAllPlugins(&pluginPath);

    if(platforms == nullptr) {
        retval = EXIT_FAILURE;
    } else {
        int junk = 0;
        std::for_each(platforms->begin(), platforms->end(), start_platform_iterator);
        std::cin >> junk;
        std::for_each(platforms->begin(), platforms->end(), cleanup_platform_iterator);
        delete platforms;
        platforms = nullptr;
    }

    return retval;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
