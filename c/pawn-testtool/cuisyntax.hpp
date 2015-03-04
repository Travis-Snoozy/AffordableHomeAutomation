// Copyright © 2014, Travis Snoozy
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

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/variant.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace aha {
namespace pawn {
namespace testtool {

    typedef struct option_display_command {
    } option_display_command_t;

    typedef struct hex128 {
        uint32_t byte1;
        uint32_t byte2;
        uint32_t byte3;
        uint32_t byte4;
    } hex128_t;

    typedef struct option_create_node_command {
        std::string node;
    } option_create_node_command_t;

    typedef struct option_display_node_command {
        std::string node;
    } option_display_node_command_t;

    typedef boost::variant<
            hex128_t,
            bool>
        command_data_t;

    enum class option_set_node_param {
        KEY,
        TXIV,
        RXIV,
        TIMESLOT,
        TXENCRYPT,
        RXDECRYPT
    };

    typedef struct option_set_node_command {
        std::string node;
        option_set_node_param param;
        command_data_t data;
    } option_set_node_command_t;

    typedef boost::variant<
            option_display_command_t,
            option_display_node_command_t,
            option_set_node_command_t,
            option_create_node_command_t>
        command_t;

    typedef struct node_data {
        hex128_t    key;
        hex128_t    txiv;
        hex128_t    rxiv;
        uint16_t    timeslot;
        bool        enable_tx_encryption;
        bool        enable_rx_decryption;
        bool        enable_timeslot;
    } node_data_t;
}}}

BOOST_FUSION_ADAPT_STRUCT(
    aha::pawn::testtool::hex128_t,
    (uint32_t, byte1)
    (uint32_t, byte2)
    (uint32_t, byte3)
    (uint32_t, byte4)
)

BOOST_FUSION_ADAPT_STRUCT(
    aha::pawn::testtool::option_create_node_command_t,
    (std::string, node)
)

BOOST_FUSION_ADAPT_STRUCT(
    aha::pawn::testtool::option_display_node_command_t,
    (std::string, node)
)

BOOST_FUSION_ADAPT_STRUCT(
    aha::pawn::testtool::option_set_node_command_t,
    (std::string, node)
    (aha::pawn::testtool::option_set_node_param, param)
    (aha::pawn::testtool::command_data_t, data)
)

namespace aha {
namespace pawn {
namespace testtool {
namespace grammar {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    using qi::lit;
    using qi::lexeme;
    using qi::uint_parser;
    using qi::bool_;
    using qi::alpha;
    using qi::as_string;
    using qi::attr;
    using ascii::char_;
    using aha::pawn::testtool::hex128_t;
    using aha::pawn::testtool::option_set_node_param;
    using namespace qi::labels;

    template <typename Iterator>
    struct cui_grammar : qi::grammar<Iterator, command_t(), ascii::space_type>
    {
        cui_grammar() : cui_grammar::base_type(command)
        {
            option_set_node_symbols.add("key", option_set_node_param::KEY);
            option_set_node_symbols.add("txiv", option_set_node_param::TXIV);
            option_set_node_symbols.add("rxiv", option_set_node_param::RXIV);
            option_set_node_symbols.add("timeslot", option_set_node_param::TIMESLOT);
            option_set_node_symbols.add("rxdec", option_set_node_param::RXDECRYPT);
            option_set_node_symbols.add("txenc", option_set_node_param::TXENCRYPT);

            command =
                    option_display
                |   option_display_node
                |   option_create_node
                |   option_set_node;

            option_display =
                    lit("option display all")
                >>  attr(option_display_command_t());

            option_display_node =
                    lit("option display node")
                >>  lexeme[+alpha];

            option_create_node =
                    lit("option create node")
                >>  lexeme[+alpha];

            option_set_node =
                    lit("option set node")
                >>  lexeme[+alpha]
                >>  option_set_node_symbols
                >>  qi::lazy(boost::phoenix::bind(&aha::pawn::testtool::grammar::cui_grammar<Iterator>::set_data_mapper, this, _val));

            hex_128 =
                    uint_parser<uint32_t, 16, 8, 8>()
                >>  uint_parser<uint32_t, 16, 8, 8>()
                >>  uint_parser<uint32_t, 16, 8, 8>()
                >>  uint_parser<uint32_t, 16, 8, 8>();

            hex_96 =
                    uint_parser<uint32_t, 16, 8, 8>()
                >>  uint_parser<uint32_t, 16, 8, 8>()
                >>  uint_parser<uint32_t, 16, 8, 8>()
                >>  attr(0U);

            hex_16 =
                    attr(0U)
                >>  attr(0U)
                >>  attr(0U)
                >>  uint_parser<uint32_t, 16, 4, 4>();

            bool_data =
                    bool_;

            hex_128_data =
                    hex_128;

            hex_16_or_bool_data =
                    hex_16
                |   bool_;

            hex_96_or_128_data =
                    hex_128
                |   hex_96;
        }

        qi::rule<Iterator, command_data_t(), ascii::space_type>& set_data_mapper(option_set_node_command_t const& command)
        {
            switch(command.param)
            {
                case option_set_node_param::KEY:
                    return hex_128_data;
                case option_set_node_param::RXIV:
                case option_set_node_param::TXIV:
                    return hex_96_or_128_data;
                case option_set_node_param::RXDECRYPT:
                case option_set_node_param::TXENCRYPT:
                    return bool_data;
                case option_set_node_param::TIMESLOT:
                    return hex_16_or_bool_data;
                default:
                    return bool_data;
            }
        }

        qi::rule<Iterator, command_t(), ascii::space_type> command;
        qi::rule<Iterator, option_display_command_t(), ascii::space_type> option_display;
        qi::rule<Iterator, option_display_node_command_t(), ascii::space_type> option_display_node;
        qi::rule<Iterator, option_create_node_command_t(), ascii::space_type> option_create_node;
        qi::rule<Iterator, option_set_node_command_t(), ascii::space_type> option_set_node;
        qi::rule<Iterator, hex128_t(), ascii::space_type> hex_16;
        qi::rule<Iterator, hex128_t(), ascii::space_type> hex_128;
        qi::rule<Iterator, hex128_t(), ascii::space_type> hex_96;
        qi::rule<Iterator, command_data_t(), ascii::space_type> bool_data;
        qi::rule<Iterator, command_data_t(), ascii::space_type> hex_128_data;
        qi::rule<Iterator, command_data_t(), ascii::space_type> hex_16_or_bool_data;
        qi::rule<Iterator, command_data_t(), ascii::space_type> hex_96_or_128_data;

        qi::symbols<char, option_set_node_param> option_set_node_symbols;
    };

}}}}

// vim: set expandtab ts=4 sts=4 sw=4 fileencoding=utf-8:
