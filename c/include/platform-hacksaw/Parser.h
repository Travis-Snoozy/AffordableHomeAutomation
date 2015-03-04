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

#include <iostream>

#include "boost/spirit/include/qi.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include "boost/variant/variant.hpp"

namespace ahaplat
{
namespace parser
{

enum command_type {
    new_,
    delete_,
    update,
    status,
    bye
};

extern struct command_type_table_ : boost::spirit::qi::symbols<char, command_type> {
    command_type_table_() {
        add
        ("new",     command_type::new_)
        ("delete",  command_type::delete_)
        ("update",  command_type::update)
        ("status",  command_type::status)
        ("bye",     command_type::bye)
        ;
    }
} command_type_table;

enum device_type {
    switch_
};

extern struct device_type_table_ : boost::spirit::qi::symbols<char, device_type> {
    device_type_table_() {
        add
        ("switch",  device_type::switch_)
        ;
    }
} device_type_table;

typedef boost::variant<int, std::string> device_value;

struct client_command_new {
    device_type device;
    device_value value;
};

typedef unsigned int client_command_delete;

struct client_command_update {
    unsigned int device_id;
    device_value value;
};

typedef boost::variant<client_command_new, client_command_delete, client_command_update> client_command_variant;

struct client_command {
    unsigned int sequence;
    command_type type;
    client_command_variant arguments;
};
}
}

BOOST_FUSION_ADAPT_STRUCT(
    ahaplat::parser::client_command_new,
    (ahaplat::parser::device_type, device)
    (ahaplat::parser::device_value, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ahaplat::parser::client_command_update,
    (unsigned int, device_id)
    (ahaplat::parser::device_value, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ahaplat::parser::client_command,
    (unsigned int, sequence)
    (ahaplat::parser::command_type, type)
    (ahaplat::parser::client_command_variant, arguments)
)

namespace ahaplat
{
namespace parser
{
template <typename Iterator>
struct client_command_parser : boost::spirit::qi::grammar<Iterator, client_command(), boost::spirit::qi::locals<command_type>> {
client_command_parser() : client_command_parser::base_type(command) {
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::uint_;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::_val;
    using boost::spirit::qi::eps;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::locals;
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::labels::_1;
    using boost::spirit::qi::labels::_4;
    using boost::spirit::qi::labels::_a;
    using boost::spirit::qi::labels::_r1;

    parse_device_value %=
        (int_) |
        (+(char_))
        ;
    command_new     %=
        device_type_table > ',' >
        parse_device_value
        ;
    command_delete  %=
        uint_
        ;
    command_update  %=
        uint_ > ',' >
        parse_device_value
        ;
    command_variant %=
        (eps(_r1 == (int)command_type::new_) > ',' > command_new) |
        (eps(_r1 == (int)command_type::delete_) > ',' > command_delete) |
        (eps(_r1 == (int)command_type::update) > ',' > command_update) |
        (eps(_r1 == (int)command_type::status)) |
        (eps(_r1 == (int)command_type::bye))
        ;
    command         %=
        uint_ > ',' >
        command_type_table[_a = _1] >
        command_variant(_a)
        ;

    on_error<fail>(command, _1);
    ;
}

boost::spirit::qi::rule<Iterator, device_value()> parse_device_value;
boost::spirit::qi::rule<Iterator, client_command_new()> command_new;
boost::spirit::qi::rule<Iterator, client_command_delete()> command_delete;
boost::spirit::qi::rule<Iterator, client_command_update()> command_update;
boost::spirit::qi::rule<Iterator, client_command_variant(command_type)> command_variant;
boost::spirit::qi::rule<Iterator, client_command(), boost::spirit::qi::locals<command_type>> command;
        };
}
}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
