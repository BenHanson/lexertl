// abstemious.hpp
// Copyright (c) 2005-2023 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_ABSTEMIOUS_HPP
#define LEXERTL_ABSTEMIOUS_HPP

#include "parser/tokeniser/re_token.hpp"
#include "parser/tokeniser/re_tokeniser.hpp"

#include <algorithm>
#include <functional>
#include <vector>

namespace lexertl
{
    template<typename rules_char_type, typename char_type>
    class abstemious
    {
    public:
        typedef detail::basic_re_token<rules_char_type, char_type> token;
        typedef std::deque<token> token_deque;
        typedef std::vector<std::size_t> index_vector;

        static void prune(token_deque& tokens_, index_vector& indexes_)
        {
            while (!indexes_.empty())
            {
                std::size_t start_ = indexes_.back();
                std::size_t idx_ = start_;

                if (is_end(tokens_, idx_))
                {
                    token& op_ = tokens_[idx_];

                    switch (op_._type)
                    {
                    case detail::AOPT:
                    case detail::AZEROORMORE:
                        remove_sequence(tokens_, start_, idx_);
                        break;
                    case detail::AONEORMORE:
                        tokens_.erase(tokens_.begin() + idx_);
                        break;
                    case detail::AREPEATN:
                        op_._type = detail::REPEATN;
                        op_._extra = op_._extra.substr(0, op_._extra.find(','));

                        if (std::adjacent_find(op_._extra.begin(),
                            op_._extra.end(), std::not_equal_to<char_type>()) ==
                            op_._extra.end() && op_._extra[0] == '0')
                        {
                            remove_sequence(tokens_, start_, idx_);
                        }

                        break;
                    default:
                        break;
                    }
                }

                indexes_.pop_back();

                while (!indexes_.empty())
                {
                    const std::size_t back_ = indexes_.back();

                    if (back_ >= start_ && back_ <= idx_)
                        indexes_.pop_back();
                    else
                        break;
                }
            }
        }

    protected:
        static bool is_end(const token_deque& tokens_, const std::size_t start_)
        {
            bool ret_ = true;

            for (std::size_t idx_ = start_ + 1, size_ = tokens_.size(); idx_ < size_;)
            {
                const token& token_ = tokens_[idx_];

                switch (token_._type)
                {
                case detail::OR:
                    idx_ = end_block(tokens_, idx_);
                    break;
                case detail::CLOSEPAREN:
                    ++idx_;

                    // An optional group still ends the match, e.g. the
                    // "(...)?" left behind by drop_empty_alternative().
                    if (idx_ < size_ &&
                        tokens_[idx_]._type == detail::OPT)
                    {
                        ++idx_;
                    }

                    break;
                case detail::END:
                    return true;
                default:
                    return false;
                }
            }

            return ret_;
        }

        static std::size_t end_block(const token_deque& tokens_,
            const std::size_t start_)
        {
            std::size_t idx_ = start_ + 1;
            std::size_t parens_ = 0;

            for (std::size_t size_ = tokens_.size(); idx_ < size_; ++idx_)
            {
                const token& token_ = tokens_[idx_];

                switch (token_._type)
                {
                case detail::OR:
                    if (parens_ == 0)
                        return idx_;

                    break;
                case detail::OPENPAREN:
                    ++parens_;
                    break;
                case detail::CLOSEPAREN:
                    if (parens_ == 0)
                        return idx_;

                    --parens_;
                    break;
                case detail::END:
                    return idx_;
                default:
                    break;
                }
            }

            return idx_;
        }

        static void remove_sequence(token_deque& tokens_, std::size_t& start_,
            std::size_t& idx_)
        {
            typename token_deque::iterator iter_ = tokens_.begin() + idx_ - 1;

            if (iter_->_type ==
                detail::CLOSEPAREN)
            {
                // Find beginning of block
                std::size_t parens_ = 1;

                do
                {
                    --iter_;

                    switch (iter_->_type)
                    {
                    case detail::OPENPAREN:
                        --parens_;
                        break;
                    case detail::CLOSEPAREN:
                        ++parens_;
                        break;
                    default:
                        break;
                    }
                } while (parens_);
            }

            start_ = iter_ - tokens_.begin();

            // This simplistic approach works, because we know there can't be
            // a trailing greedy operator following a close paren as we
            // wouldn't even be in this function in that case.
            // A trailing abstemious operator would have already had its block
            // removed.
            while (tokens_[start_ - 1]._type ==
                detail::OPENPAREN &&
                tokens_[idx_ + 1]._type == detail::CLOSEPAREN)
            {
                --start_;
                --iter_;
                ++idx_;

                // Remove the group's optional marker too (if any)
                if (tokens_[idx_ + 1]._type == detail::OPT)
                    ++idx_;
            }

            iter_ = tokens_.erase(iter_, tokens_.begin() + idx_ + 1);
            drop_empty_alternative(tokens_, iter_ - tokens_.begin());
        }

        // pos_ is the index of the first token after the sequence that was
        // just erased.
        // If the erased sequence was a whole alternative, remove the
        // alternative (and its '|'). Inside a group the lazy path matched
        // nothing, which is still a valid match, so the group becomes
        // optional: "x(a*?|b)" -> "x(b)?".
        static void drop_empty_alternative(token_deque& tokens_,
            std::size_t pos_)
        {
            const detail::token_type prev_ = tokens_[pos_ - 1]._type;
            const detail::token_type next_ = tokens_[pos_]._type;
            const bool starts_ = prev_ == detail::BEGIN ||
                prev_ == detail::OPENPAREN ||
                prev_ == detail::OR;
            const bool ends_ = next_ == detail::OR ||
                next_ == detail::CLOSEPAREN ||
                next_ == detail::END;

            // The alternative still has content, e.g. "xa*?|b" -> "x|b"
            if (!starts_ || !ends_)
                return;

            if (next_ == detail::OR)
                tokens_.erase(tokens_.begin() + pos_);
            else if (prev_ == detail::OR)
                tokens_.erase(tokens_.begin() + --pos_);
            else
                return; // Nothing but BEGIN/END left

            // pos_ is now the first token after the dropped alternative.
            // Find the close paren of the enclosing group (if any).
            std::size_t parens_ = 0;

            for (std::size_t size_ = tokens_.size(); pos_ < size_; ++pos_)
            {
                switch (tokens_[pos_]._type)
                {
                case detail::OPENPAREN:
                    ++parens_;
                    break;
                case detail::CLOSEPAREN:
                    if (parens_ == 0)
                    {
                        // A trailing group has no quantifier, so any '?'
                        // here was added by an earlier call.
                        if (tokens_[pos_ + 1]._type != detail::OPT)
                        {
                            tokens_.insert(tokens_.begin() + pos_ + 1,
                                token(detail::OPT));
                        }

                        return;
                    }

                    --parens_;
                    break;
                default:
                    break;
                }
            }

            // Top level: an empty alternative is simply dropped.
        }
    };
}

#endif
