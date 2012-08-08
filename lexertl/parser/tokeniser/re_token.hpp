// re_token.hpp
// Copyright (c) 2005-2012 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_RE_TOKEN_HPP
#define LEXERTL_RE_TOKEN_HPP

#include "../../string_token.hpp"

namespace lexertl
{
namespace detail
{
enum token_type {BEGIN, REGEX, OREXP, SEQUENCE, SUB, EXPRESSION, REPEAT,
    DUP, OR, CHARSET, BOL, EOL, MACRO, OPENPAREN, CLOSEPAREN, OPT, AOPT,
    ZEROORMORE, AZEROORMORE, ONEORMORE, AONEORMORE, REPEATN, AREPEATN,
    END};

template<typename input_char_type, typename char_type>
struct basic_re_token
{
    typedef basic_string_token<char_type> string_token;
    typedef std::basic_string<input_char_type> string;

    token_type _type;
    string _extra;
    string_token _str;
    static const char _precedence_table[END + 1][END + 1];
    static const char *_precedence_strings[END + 1];

    basic_re_token (const token_type type_ = BEGIN) :
        _type (type_)
    {
    }

    void clear ()
    {
        _type = BEGIN;
        _str.clear ();
    }

    basic_re_token &operator = (const basic_re_token &rhs_)
    {
        _type = rhs_._type;
        _str = rhs_._str;
        return *this;
    }

    char precedence (const token_type type_) const
    {
        return _precedence_table[_type][type_];
    }

    const char *precedence_string () const
    {
        return _precedence_strings[_type];
    }
};

template<typename input_char_type, typename char_type>
const char basic_re_token<input_char_type, char_type>::
    _precedence_table[END + 1][END + 1] = {
//        BEG, REG, ORE, SEQ, SUB, EXP, RPT, DUP,  | , CHR, BOL, EOL, MCR,  ( ,  ) ,  ? , ?? ,  * , *? ,  + , +?, {n}?, {n}, END
/*BEGIN*/{' ', '<', '<', '<', '<', '<', '<', ' ', ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*REGEX*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*OREXP*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', '>', '>', '>', '>', ' ', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* SEQ */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* SUB */{' ', ' ', ' ', ' ', ' ', '=', '<', ' ', '>', '<', '<', '<', '<', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*EXPRE*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* RPT */{' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '>'},
/*DUPLI*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*  |  */{' ', ' ', ' ', '=', '<', '<', '<', ' ', ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
/*CHARA*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/* BOL */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/* EOL */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*MACRO*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*  (  */{' ', '=', '<', '<', '<', '<', '<', ' ', ' ', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
/*  )  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
/*  ?  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* ??  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*  *  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* *?  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*  +  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* +?  */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*{n,m}*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/*{nm}?*/{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>'},
/* END */{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
};

template<typename input_char_type, typename char_type>
const char *basic_re_token<input_char_type,
    char_type>::_precedence_strings[END + 1] =
{"BEGIN", "REGEX", "OREXP", "SEQUENCE", "SUB", "EXPRESSION", "REPEAT",
    "DUPLICATE", "|", "CHARSET", "^", "$", "MACRO", "(", ")", "?", "??", "*",
    "*?", "+", "+?", "{n[,[m]]}", "{n[,[m]]}?", "END"};
}
}

#endif
