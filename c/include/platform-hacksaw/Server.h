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

#include <memory>

#include "boost/asio.hpp"
#include "boost/thread.hpp"

namespace ahaplat
{

typedef std::pair<std::shared_ptr<boost::asio::ip::tcp::socket>, std::shared_ptr<boost::asio::strand>> io_type;

class Server
{
public:
    Server();

    virtual void start();
private:
    void broadcast(io_type io, std::shared_ptr<boost::asio::streambuf> buffer);
    void handleBye(const boost::system::error_code& error, std::size_t bytesWritten, io_type io, std::shared_ptr<boost::asio::streambuf> buffer);
    void handleConnect(const boost::system::error_code& error, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void handleRead(const boost::system::error_code& error, std::size_t bytesRead, io_type io, std::shared_ptr<boost::asio::streambuf> buffer);
    void handleSend(const boost::system::error_code& error, std::size_t bytesWritten, std::shared_ptr<boost::asio::streambuf> buffer);
    void handleStatus(io_type io, uint32_t sequence);
    void nextConnect();
    void nextRead(io_type io, std::shared_ptr<boost::asio::streambuf> buffer);
    void parse(std::shared_ptr<char> parseChunk, io_type io, std::shared_ptr<boost::asio::streambuf> buffer);
    void printStuff(char stuff);
    void socketSetup(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void socketTeardown(io_type io);
    void startBroadcast(std::shared_ptr<boost::asio::streambuf> buffer);

    boost::asio::io_service         m_service;
    boost::asio::io_service::work   m_work;
    boost::asio::ip::tcp::acceptor  m_listener;
    boost::thread_group             m_threads;
    boost::asio::strand             m_strand_sockets;
    std::map<std::shared_ptr<boost::asio::ip::tcp::socket>, std::shared_ptr<boost::asio::strand>>  m_sockets;
};
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
