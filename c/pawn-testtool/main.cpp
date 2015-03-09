// Copyright © 2014, 2015, Travis Snoozy
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

#include "cuisyntax.hpp"
#include <boost/spirit/include/qi_parse.hpp>

#include <cstdlib>
#include <iostream>
#include <iomanip>

// Boost modules
// Asio
// Program Options

namespace aha {
namespace pawn {
namespace testtool {
std::ostream& operator<<(std::ostream& os, const aha::pawn::testtool::hex128_t& hex)
{
    std::ios save(NULL);
    save.copyfmt(os);
    os << std::hex << std::setfill('0') << std::setw(8) << hex.byte1
       << std::setw(8) << hex.byte2
       << std::setw(8) << hex.byte3
       << std::setw(8) << hex.byte4;
    os.copyfmt(save);
    return os;
}

std::ostream& operator<<(std::ostream& os, const aha::pawn::testtool::node_data_t& data)
{
    std::ios save(NULL);
    save.copyfmt(os);
    os << std::setfill(' ') << std::left
    << std::setw(22) << "Key" << data.key << "\n"
    << std::setw(22) << "TX IV" << data.txiv << "\n"
    << std::setw(22) << "RX IV" << data.rxiv << "\n";

    std::ios save2(NULL);
    save2.copyfmt(os);
    os << std::setw(22) << "Timeslot"
    << std::hex << std::setfill('0') << std::setw(4) << data.timeslot << "\n";
    os.copyfmt(save2);

    os << std::setw(22) << "Timeslot enable" << data.enable_timeslot << "\n"
    << std::setw(22) << "TX encryption enable" << data.enable_tx_encryption << "\n"
    << std::setw(22) << "RX decryption enable" << data.enable_rx_decryption << "\n";
    os.copyfmt(save);
    return os;
}

enum class option_visitor_result {
    SUCCESS,
    DOES_NOT_EXIST,
    ALREADY_EXISTS,
    INVALID_COMMAND,
    UNEXPECTED_DATA
};

class option_visitor
    : public boost::static_visitor<option_visitor_result>
{
private:
    boost::spirit::qi::symbols<char, node_data_t> nodes;

public:
    option_visitor_result operator()(const option_display_command_t& cmd)
    {
        nodes.for_each([](std::string& key, node_data_t& value){
            std::cout << "Node " << key << "\n" << value << std::endl;
        });
        return option_visitor_result::SUCCESS;
    }

    option_visitor_result operator()(const option_display_node_command_t& cmd)
    {
        node_data_t* node = nodes.find(cmd.node);

        if(node == NULL)
        {
            return option_visitor_result::DOES_NOT_EXIST;
        }

        std::cout << "Node " << cmd.node << "\n" << *node << std::endl;

        return option_visitor_result::SUCCESS;
    }

    option_visitor_result operator()(const option_set_node_command_t& cmd)
    {
        node_data_t* node = nodes.find(cmd.node);

        if(node == NULL)
        {
            return option_visitor_result::DOES_NOT_EXIST;
        }

        switch(cmd.param)
        {
            case option_set_node_param::KEY:
                node->key = boost::get<hex128_t>(cmd.data);
                break;
            case option_set_node_param::RXDECRYPT:
                node->enable_rx_decryption = boost::get<bool>(cmd.data);
                break;
            case option_set_node_param::RXIV:
                node->rxiv = boost::get<hex128_t>(cmd.data);
                break;
            case option_set_node_param::TIMESLOT:
                if(cmd.data.type() == typeid(bool)) {
                    node->enable_timeslot = boost::get<bool>(cmd.data);
                }
                else if(cmd.data.type() == typeid(hex128_t)) {
                    node->timeslot = (uint16_t)boost::get<hex128_t>(cmd.data).byte4;
                }
                else {
                    return option_visitor_result::UNEXPECTED_DATA;
                }
                break;
            case option_set_node_param::TXENCRYPT:
                node->enable_tx_encryption = boost::get<bool>(cmd.data);
                break;
            case option_set_node_param::TXIV:
                node->txiv = boost::get<hex128_t>(cmd.data);
                break;
            default:
                return option_visitor_result::INVALID_COMMAND;
        }

        return option_visitor_result::SUCCESS;
    }

    option_visitor_result operator()(const option_create_node_command_t& cmd)
    {
        if(nodes.find(cmd.node) != NULL)
        {
            return option_visitor_result::ALREADY_EXISTS;
        }

        nodes.at(cmd.node);

        return option_visitor_result::SUCCESS;
    }

    option_visitor_result operator()(const transmit_command_t& cmd)
    {
        node_data_t* node = nodes.find(cmd.node);
        if(node == NULL)
        {
            return option_visitor_result::DOES_NOT_EXIST;
        }

        // TODO: Write this out to the designated serial port.

        return option_visitor_result::SUCCESS;
    }
};
}}}

using aha::pawn::testtool::option_visitor_result;

int main(int argc, char** argv)
{
    std::string input;
    std::string message;
    aha::pawn::testtool::command_t* command;
    std::string::const_iterator begin, end;
    aha::pawn::testtool::grammar::cui_grammar<std::string::const_iterator> grammar;
    std::getline(std::cin, input);
    aha::pawn::testtool::option_visitor option_visitor;

    while(input != "quit")
    {
        command = new aha::pawn::testtool::command_t();
        begin = input.begin();
        end = input.end();
        bool result = boost::spirit::qi::phrase_parse(
            begin,
            end,
            grammar,
            boost::spirit::ascii::space,
            (*command));
        if(result && begin == end)
        {
            switch(boost::apply_visitor(option_visitor, *command))
            {
                case option_visitor_result::SUCCESS:
                    std::cout << "OK.";
                    break;
                case option_visitor_result::DOES_NOT_EXIST:
                    std::cout << "Node does not exist.";
                    break;
                case option_visitor_result::ALREADY_EXISTS:
                    std::cout << "Node already exists.";
                    break;
                case option_visitor_result::INVALID_COMMAND:
                case option_visitor_result::UNEXPECTED_DATA:
                    std::cout << "An unexpected error occurred.";
                    break;
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Could not parse that." << std::endl;
        }
        delete command;
        std::getline(std::cin, input);
    }
    return EXIT_SUCCESS;
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
