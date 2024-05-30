// replace.hpp
// Copyright (c) 2023 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_REPLACE_HPP
#define LEXERTL_REPLACE_HPP

#include "lookup.hpp"
#include "state_machine.hpp"
#include "match_results.hpp"

namespace lexertl
{
    template<class out_iter, class fwd_iter,
        class id_type, class char_type,
        class traits, class alloc>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::basic_string<char_type, traits, alloc>& fmt_)
    {
        return replace(out_, first_, second_, sm_, fmt_.c_str());
    }

    template<class out_iter, class fwd_iter,
        class id_type, class char_type>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const char_type* fmt_)
    {
        const char_type* end_fmt_ = fmt_;
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter> results_(first_, second_);

        while (*end_fmt_)
            ++end_fmt_;

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            std::copy(last_, results_.first, out_);
            std::copy(fmt_, end_fmt_, out_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    template<class id_type, class char_type,
        class straits, class salloc,
        class ftraits, class falloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::basic_string<char_type, ftraits, falloc>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;

        replace(std::back_inserter(ret_), s_.cbegin(), s_.cend(), sm_, fmt_);
        return ret_;
    }

    template<class id_type, class char_type,
        class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const char_type* fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;

        replace(std::back_inserter(ret_), s_.cbegin(), s_.cend(), sm_, fmt_);
        return ret_;
    }

    template<class id_type, class char_type,
        class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::basic_string<char_type, straits, salloc>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* end_s_ = s_;
        
        while (*end_s_)
            ++end_s_;

        replace(std::back_inserter(ret_), s_, end_s_, sm_, fmt_);
        return ret_;
    }

    template<class id_type, class char_type>
    std::basic_string<char_type> replace(const char_type* s_,
        const basic_state_machine<char_type, id_type>& sm_,
        const char_type* fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = s; while (*end_s_) ++end_s_;
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*> results(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            ret_.append(last_, results_.first);
            ret.append(fmt_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret.append(last_, results_.first);
        return ret_;
    }
}

#endif
