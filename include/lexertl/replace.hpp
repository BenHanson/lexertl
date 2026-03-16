// replace.hpp
// Copyright (c) 2023 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_REPLACE_HPP
#define LEXERTL_REPLACE_HPP

#include "lookup.hpp"
#include "match_results.hpp"
#include "state_machine.hpp"

#include <algorithm>
#include <map>
#include <string>

namespace lexertl
{
    template<class char_type>
    const char_type* find_end(const char_type* ptr_)
    {
        for (; *ptr_; ++ptr_)
            ;

        return ptr_;
    }

    // Stream overloads
    template<class out_iter, class fwd_iter, class id_type, class char_type,
        class straits, class salloc>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::basic_string<char_type, straits, salloc>& fmt_)
    {
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            std::copy(last_, results_.first, out_);
            std::copy(fmt_.begin(), fmt_.end(), out_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    template<class out_iter, class fwd_iter, class id_type, class char_type,
        class straits, class salloc>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::map<id_type,
            std::basic_string<char_type, straits, salloc> >& fmt_)
    {
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, std::basic_string
                <char_type, straits, salloc> >::const_iterator
                iter_ = fmt_.find(results_.id);

            std::copy(last_, results_.first, out_);

            if (iter_ == fmt_.end())
                std::copy(results_.first, results_.second, out_);
            else
                std::copy(iter_->second.begin(), iter_->second.end(), out_);

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    template<class out_iter, class fwd_iter, class id_type, class char_type,
        class straits, class salloc>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::map<id_type,
            std::basic_string<char_type, straits, salloc>(*)
                (typename std::basic_string<char_type, straits,
                    salloc>::const_iterator&,
                typename std::basic_string<char_type, straits,
                    salloc>::const_iterator&)>& fmt_)
    {
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, std::basic_string
                <char_type, straits, salloc>(*)
                (typename std::basic_string<char_type, straits, salloc>::
                    const_iterator&,
                typename std::basic_string<char_type, straits, salloc>::
                    const_iterator&)>::const_iterator iter_ =
                    fmt_.find(results_.id);

            std::copy(last_, results_.first, out_);

            if (iter_ == fmt_.end())
                std::copy(results_.first, results_.second, out_);
            else
            {
                const std::basic_string<char_type, straits, salloc> str_ =
                    iter_->second(results_.first, results_.second);

                std::copy(str_.begin(), str_.end(), out_);
            }

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    template<class out_iter, class fwd_iter, class id_type, class char_type>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const char_type* fmt_)
    {
        fwd_iter last_ = first_;
        const char_type* end_fmt_ = find_end(fmt_);
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);

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

    template<class out_iter, class fwd_iter, class id_type, class char_type>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::map<id_type, const char_type*> fmt_)
    {
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);
        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type*>::
                const_iterator iter_ = fmt_.find(results_.id);

            std::copy(last_, results_.first, out_);

            if (iter_ == fmt_.end())
                std::copy(results_.first, results_.second, out_);
            else
            {
                const char_type* end_fmt_ = find_end(iter_->second);

                std::copy(iter_->second, end_fmt_, out_);
            }

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    template<class out_iter, class fwd_iter, class id_type, class char_type>
    out_iter replace(out_iter out_, fwd_iter first_, fwd_iter second_,
        const basic_state_machine<char_type, id_type>& sm_,
        const std::map<id_type,
        const char_type*(*)(const char_type*, const char_type*)> fmt_)
    {
        fwd_iter last_ = first_;
        lexertl::match_results<fwd_iter, id_type> results_(first_, second_);
        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type*(*)
                (const char_type*, const char_type*)>::const_iterator iter_ =
                fmt_.find(results_.id);

            std::copy(last_, results_.first, out_);

            if (iter_ == fmt_.end())
                std::copy(results_.first, results_.second, out_);
            else
            {
                const char_type* str_ =
                    iter_->second(results_.first, results_.second);
                const char_type* end_str_ = find_end(str_);

                std::copy(str_, end_str_, out_);
            }

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        std::copy(last_, results_.first, out_);
        return out_;
    }

    // String overloads
    template<class id_type, class char_type,
        class straits, class salloc, class ftraits, class falloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::basic_string<char_type, ftraits, falloc>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            ret_.append(last_, results_.first);
            ret_.append(fmt_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type,
        class straits, class salloc, class ftraits, class falloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type,
                std::basic_string<char_type, ftraits, falloc> >& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type,
                std::basic_string<char_type, ftraits, falloc> >::
                    const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second);

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type,
        class straits, class salloc, class ftraits, class falloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type,
                std::basic_string<char_type, ftraits, falloc>(*)
                    (typename std::basic_string<char_type, straits, salloc>::
                        const_iterator&,
                    typename std::basic_string<char_type, straits, salloc>::
                        const_iterator&)>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type,
                std::basic_string<char_type, ftraits, falloc>(*)
                    (typename std::basic_string<char_type, straits, salloc>::
                        const_iterator&,
                    typename std::basic_string<char_type, straits, salloc>::
                        const_iterator&)>::const_iterator iter_ =
                            fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second(results_.first, results_.second));

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const char_type* fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        const char_type* end_fmt_ = find_end(fmt_);
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            ret_.append(last_, results_.first);
            ret_.append(fmt_, end_fmt_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type, const char_type*>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type*>::
                const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second);

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const std::basic_string<char_type, straits, salloc>& s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type, const char_type*(*)
                (const char_type*, const char_type*)>& fmt_)
    {
        std::basic_string<char_type, straits, salloc> ret_;
        const char_type* last_ = s_.c_str();
        lexertl::match_results<const char_type*, id_type>
            results_(s_.c_str(), s_.c_str() + s_.size());

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type*(*)
                (const char_type*, const char_type*)>::
                    const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second(results_.first, results_.second));

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::basic_string<char_type, straits, salloc>& fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type> results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            ret_.append(last_, results_.first);
            ret_.append(fmt_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type,
                std::basic_string<char_type, straits, salloc> >& fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type> results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type,
                std::basic_string<char_type, straits, salloc> >::
                    const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second);

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type, class straits, class salloc>
    std::basic_string<char_type, straits, salloc>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type,
                std::basic_string<char_type, straits, salloc>(*)
            (const char_type*, const char_type*)>& fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type> results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type,
                std::basic_string<char_type, straits, salloc>(*)
                    (const char_type*, const char_type*)>::
                        const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second(results_.first, results_.second));

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type>
    std::basic_string<char_type>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const char_type* fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type> results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            ret_.append(last_, results_.first);
            ret_.append(fmt_);
            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type>
    std::basic_string<char_type>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type, const char_type*>& fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type>results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type*>::
                const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second);

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }

    template<class id_type, class char_type>
    std::basic_string<char_type>
        replace(const char_type* s_,
            const basic_state_machine<char_type, id_type>& sm_,
            const std::map<id_type,
                const char_type* (*)(const char_type*, const char_type*)>& fmt_)
    {
        std::basic_string<char_type> ret_;
        const char_type* end_s_ = find_end(s_);
        const char_type* last_ = s_;
        lexertl::match_results<const char_type*, id_type> results_(s_, end_s_);

        // Lookahead
        lexertl::lookup(sm_, results_);

        while (results_.id != 0)
        {
            typename std::map<id_type, const char_type* (*)
                (const char_type*, const char_type*)>::
                    const_iterator iter_ = fmt_.find(results_.id);

            ret_.append(last_, results_.first);

            if (iter_ == fmt_.end())
                ret_.append(results_.first, results_.second);
            else
                ret_.append(iter_->second(results_.first, results_.second));

            last_ = results_.second;
            lexertl::lookup(sm_, results_);
        }

        ret_.append(last_, results_.first);
        return ret_;
    }
}

#endif
