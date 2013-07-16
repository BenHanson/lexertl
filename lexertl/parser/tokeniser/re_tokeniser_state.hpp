// tokeniser_state.hpp
// Copyright (c) 2005-2013 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_RE_TOKENISER_STATE_HPP
#define LEXERTL_RE_TOKENISER_STATE_HPP

#include "../../char_traits.hpp"
#include "../../enums.hpp"
#include <locale>
#include "../../size_t.hpp"
#include <stack>

namespace lexertl
{
namespace detail
{
template<typename ch_type, typename id_type>
struct basic_re_tokeniser_state
{
    typedef ch_type char_type;
    typedef typename basic_char_traits<char_type>::index_type index_type;

    const char_type * const _start;
    const char_type * const _end;
    const char_type *_curr;
    id_type _id;
    std::size_t _flags;
    std::stack<std::size_t> _flags_stack;
    std::locale _locale;
    bool _macro;
    long _paren_count;
    bool _in_string;
    id_type _nl_id;

    basic_re_tokeniser_state (const char_type *start_,
        const char_type * const end_, id_type id_, const std::size_t flags_,
        const std::locale locale_, const bool macro_) :
        _start (start_),
        _end (end_),
        _curr (start_),
        _id (id_),
        _flags (flags_),
        _flags_stack (),
        _locale (locale_),
        _macro (macro_),
        _paren_count (0),
        _in_string (false),
        _nl_id (static_cast<id_type>(~0))
    {
    }

    basic_re_tokeniser_state (const basic_re_tokeniser_state &rhs_)
    {
        assign (rhs_);
    }

    // prevent VC++ 7.1 warning:
    const basic_re_tokeniser_state &operator =
        (const basic_re_tokeniser_state &rhs_)
    {
        assign (rhs_);
    }

    void assign (const basic_re_tokeniser_state &rhs_)
    {
        _start = rhs_._start;
        _end = rhs_._end;
        _curr = rhs_._curr;
        _id = rhs_._id;
        _flags = rhs_._flags;
        _flags_stack = rhs_._flags_stack;
        _locale = rhs_._locale;
        _macro = rhs_._macro;
        _paren_count = rhs_._paren_count;
        _in_string = rhs_._in_string;
        _nl_id = rhs_._nl_id;
        return this;
    }

    inline bool next (char_type &ch_)
    {
        if (_curr >= _end)
        {
            ch_ = 0;
            return true;
        }
        else
        {
            ch_ = *_curr;
            increment ();
            return false;
        }
    }

    inline void increment ()
    {
        ++_curr;
    }

    inline std::size_t index ()
    {
        return _curr - _start;
    }

    inline bool eos ()
    {
        return _curr >= _end;
    }
};
}
}

#endif
