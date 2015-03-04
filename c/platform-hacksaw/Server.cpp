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

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <iterator>
#include <sstream>

#include "boost/bind.hpp"
#include "boost/asio.hpp"
#include "boost/thread.hpp"

#include "platform-hacksaw/Device.h"
#include "platform-hacksaw/Server.h"
#include "platform-hacksaw/ValueFunction.h"
#include "platform-hacksaw/Parser.h"
#include "platform-hacksaw/DeviceFactory.h"

using namespace ahaplat;

class client_command_visitor : public boost::static_visitor<std::string>
{
private:
    std::string buildError(const char* message) const {
        return std::string("error,") + message;
    }
public:
    std::string operator()(parser::client_command_new& command) const {
        std::string retval;
        Device* ptr = nullptr;
        switch(command.device) {
        case parser::device_type::switch_: {
            int* value = boost::get<int>(&command.value);
            if(!value) {
                retval = buildError("Invalid value type provided.");
                break;
            }

            ptr = devices::DeviceFactory::makeDevice<devices::Switch>(nullptr, *value);
            if(ptr == nullptr) {
                retval = buildError("Failed to create device.");
                break;
            }

            std::stringstream buffer;
            buffer << "new," << ptr->getSerial() << ",";
            ptr->print(buffer);
            retval = buffer.str();
        }
        break;
        default:
            retval = buildError("Unsupported device type.");
            break;
        }

        return retval;
    }

    std::string operator()(parser::client_command_delete& command) const {
        IDevice* device = ::platform->getDevice(command);

        if(device == nullptr) {
            return buildError("Failed to get device with requested ID.");
        }

        if(!::platform->notify(device, false)) {
            return buildError("Failed to delete device.");
        }

        std::stringstream buffer;
        buffer << "delete," << command;
        return buffer.str();
    }

    std::string operator()(parser::client_command_update& command) const {
        std::string retval;
        IDevice* exists = ::platform->getDevice(command.device_id);
        Device* device = dynamic_cast<Device*>(exists);

        if(exists) {
            switch(device->getType()) {
            case parser::device_type::switch_: {
                devices::Switch* downcast = dynamic_cast<devices::Switch*>(device);
                int* value = boost::get<int>(&command.value);

                if(downcast == nullptr) {
                    retval = buildError("Downcast failed; this device has a corrupt value.");
                    break;
                }
                if(value == nullptr) {
                    retval = buildError("Invalid value type provided.");
                    break;
                }

                if(!downcast->setValue(*value)) {
                    retval = buildError("Value provided was out of bounds.");
                    break;
                }

                std::stringstream buffer;
                buffer << "update," << command.device_id << ",";
                downcast->print(buffer);
                retval = buffer.str();
            }
            break;
            default:
                retval = buildError("Unsupported device type.");
                break;
            }
        } else {
            retval = buildError("Device does not exist.");
        }
        return retval;
    }
};

Server::Server()
    : m_service(), m_work(m_service), m_listener(m_service), m_threads(), m_strand_sockets(m_service)
{
    for(int i = 0; i < 4; i++)
    {
        m_threads.create_thread(boost::bind(&boost::asio::io_service::run, &(m_service)));
    }
}

void Server::start()
{
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 55455);
    this->m_listener.open(ep.protocol());
    this->m_listener.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    this->m_listener.bind(ep);
    this->m_listener.listen();
    this->nextConnect();
}

void Server::handleBye(const boost::system::error_code& error, std::size_t bytesWritten, io_type io, std::shared_ptr<boost::asio::streambuf> buffer)
{
    this->m_strand_sockets.post(boost::bind(&Server::socketTeardown, this, io));
}

void Server::handleConnect(const boost::system::error_code& error, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    this->nextConnect();
    this->m_strand_sockets.post(boost::bind(&Server::socketSetup, this, socket));
}

void Server::socketSetup(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    this->m_sockets[socket] = std::shared_ptr<boost::asio::strand>(new boost::asio::strand(this->m_service));
    std::shared_ptr<boost::asio::streambuf> buffer(new boost::asio::streambuf());
    nextRead(*(this->m_sockets.find(socket)), buffer);
}

void Server::socketTeardown(io_type io)
{
    io.first->close();
    this->m_sockets.erase(io.first);
}

void Server::handleRead(const boost::system::error_code& error, std::size_t bytesRead, io_type io, std::shared_ptr<boost::asio::streambuf> buffer)
{
    if(error) {
        this->m_strand_sockets.post(boost::bind(&Server::socketTeardown, this, io));
        return;
    }
    std::shared_ptr<char> localBuffer(new char[bytesRead+1]);
    buffer->sgetn(&(*localBuffer), bytesRead);
    (&(*localBuffer))[bytesRead] = '\0';
    this->m_service.post(boost::bind(&Server::parse, this, localBuffer, io, buffer));
}

void Server::handleSend(const boost::system::error_code& error, std::size_t bytesWritten, std::shared_ptr<boost::asio::streambuf> buffer)
{
}

void Server::handleStatus(io_type io, uint32_t sequence)
{
    std::shared_ptr<boost::asio::streambuf> output_backing(new boost::asio::streambuf);
    std::ostream output(&(*output_backing));
    auto devBuilder = [&output,sequence](IDevice* dev) -> void {
        Device* downcast = dynamic_cast<Device*>(dev);
        output << sequence << ",new," << dev->getSerial() << ",";
        downcast->print(output);
        output << std::endl;
    };
    ::platform->iterateDevices(devBuilder);
    output << sequence << ",status" << std::endl;

    boost::asio::async_write(*(io.first), *output_backing, boost::bind(&Server::handleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, output_backing));
}

void Server::nextConnect()
{
    std::shared_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(this->m_service));
    this->m_listener.async_accept(*socket, boost::bind(&Server::handleConnect, this, boost::asio::placeholders::error, socket));
}

void Server::nextRead(io_type io, std::shared_ptr<boost::asio::streambuf> buffer)
{
    boost::asio::async_read_until(*(io.first), *buffer, '\n', boost::bind(&Server::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, io, buffer));
}

void Server::parse(std::shared_ptr<char> parseChunk, io_type io, std::shared_ptr<boost::asio::streambuf> buffer)
{
    size_t len = strlen(&(*parseChunk));
    parser::client_command_parser<char*> p;
    parser::client_command result;
    char* start = parseChunk.get();
    char* end = parseChunk.get() + len;
    std::shared_ptr<boost::asio::streambuf> output_backing(new boost::asio::streambuf);
    std::ostream output(output_backing.get());
    bool handled = false;
    bool broadcast = false;

    if(boost::spirit::qi::parse(start, end, p, result)) {

        output << result.sequence << ",";
        switch(result.type) {
        case parser::command_type::status:
            this->m_service.post(boost::bind(&Server::handleStatus, this, io, result.sequence));
            handled = true;
            break;
        case parser::command_type::bye:
            output << "bye" << std::endl;
            boost::asio::async_write(*(io.first), *output_backing, boost::bind(&Server::handleBye, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, io, output_backing));
            handled = true;
            return;
            break;
        case parser::command_type::new_:
        case parser::command_type::delete_:
        case parser::command_type::update:
            output << boost::apply_visitor(client_command_visitor(), result.arguments);
            broadcast = true;
            break;
        default:
            output << "error,Unknown command.";
            break;
        }
    } else {
        output << result.sequence << ",error,Parsing error.";
    }

    if(!handled) {
        output << std::endl;

        if(broadcast) {
            this->m_strand_sockets.post(boost::bind(&Server::startBroadcast, this, output_backing));
        } else {
            io.second->post(boost::bind(&Server::broadcast, this, io, output_backing));
        }
    }

    nextRead(io, buffer);
}

void Server::startBroadcast(std::shared_ptr<boost::asio::streambuf> buffer)
{
    std::for_each(this->m_sockets.begin(), this->m_sockets.end(), [&buffer,this](io_type io){
        io.second->post(boost::bind(&Server::broadcast, this, io, buffer));
    });
}

void Server::broadcast(io_type io, std::shared_ptr<boost::asio::streambuf> buffer)
{
    boost::asio::async_write(*(io.first), *buffer, boost::bind(&Server::handleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, buffer));
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
