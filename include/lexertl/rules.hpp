// rules.hpp
// Copyright (c) 2005-2023 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_RULES_HPP
#define LEXERTL_RULES_HPP

#include "abstemious.hpp"
#include "compile_assert.hpp"
#include "enums.hpp"
#include "narrow.hpp"
#include "parser/tokeniser/re_token.hpp"
#include "parser/tokeniser/re_tokeniser.hpp"
#include "parser/tokeniser/re_tokeniser_state.hpp"
#include "runtime_error.hpp"
#include "string_token.hpp"

#include <deque>
#include <locale>
#include <map>
#include <set>
#include "size_t.hpp"
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace lexertl
{
    template<typename r_ch_type, typename ch_type, typename id_ty = std::size_t>
    class basic_rules
    {
    public:
        typedef std::vector<bool> bool_vector;
        typedef std::deque<bool_vector> bool_vector_deque;
        typedef ch_type char_type;
        typedef r_ch_type rules_char_type;
        typedef id_ty id_type;
        typedef std::vector<id_type> id_vector;
        typedef std::deque<id_vector> id_vector_deque;
        typedef detail::basic_re_tokeniser_state<rules_char_type, id_type>
            re_state;
        typedef std::basic_string<rules_char_type> string;
        typedef basic_string_token<char_type> string_token;
        typedef std::deque<string> string_deque;
        typedef std::set<string> string_set;
        typedef std::pair<string, string> string_pair;
        typedef std::map<string, id_type> string_id_type_map;
        typedef std::pair<string, id_type> string_id_type_pair;
        typedef detail::basic_re_token<rules_char_type, char_type> token;
        typedef std::deque<token> token_deque;
        typedef std::deque<token_deque> token_deque_deque;
        typedef std::deque<token_deque_deque> token_deque_deque_deque;
        typedef std::map<string, token_deque> macro_map;
        typedef std::pair<string, token_deque> macro_pair;
        typedef std::deque<std::string> std_string_deque;
        typedef std::deque<std_string_deque> std_string_deque_deque;
        typedef detail::basic_re_tokeniser
            <rules_char_type, char_type, id_type> tokeniser;

        // If you get a compile error here you have
        // failed to define an unsigned id type.
        compile_assert <(static_cast<id_type>(~0) > 0)> _valid_id_type;

#ifdef _WIN32
        basic_rules(const std::size_t flags_ = dot_not_cr_lf) :
#else
        basic_rules(const std::size_t flags_ = dot_not_newline) :
#endif
            _valid_id_type(),
            _statemap(),
            _macro_map(),
            _regexes(),
            _regex_strings(),
            _features(),
            _ids(),
            _user_ids(),
            _next_dfas(),
            _pushes(),
            _pops(),
            _flags(flags_),
            _locale(),
            _lexer_state_names()
        {
            push_state(initial());
        }

        // Added to support python bindings
        virtual ~basic_rules()
        {
        }

        void clear()
        {
            _statemap.clear();
            _macro_map.clear();
            _regexes.clear();
            _regex_strings.clear();
            _features.clear();
            _ids.clear();
            _user_ids.clear();
            _next_dfas.clear();
            _pushes.clear();
            _pops.clear();
#ifdef _WIN32
            _flags = dot_not_cr_lf;
#else
            _flags = dot_not_newline;
#endif
            _locale = std::locale();
            _lexer_state_names.clear();
            push_state(initial());
        }

        void clear(const id_type dfa_)
        {
            if (_regexes.size() > dfa_)
            {
                _regexes[dfa_].clear();
                _regex_strings[dfa_].clear();
                _features[dfa_] = 0;
                _ids[dfa_].clear();
                _user_ids[dfa_].clear();
                _next_dfas[dfa_].clear();
                _pushes[dfa_].clear();
                _pops[dfa_].clear();
            }
        }

        void flags(const std::size_t flags_)
        {
            _flags = flags_;
        }

        std::size_t flags() const
        {
            return _flags;
        }

        static id_type skip()
        {
            return static_cast<id_type>(~1);
        }

        static id_type eoi()
        {
            return 0;
        }

        static id_type npos()
        {
            return static_cast<id_type>(~0);
        }

        std::locale imbue(const std::locale& locale_)
        {
            std::locale loc_ = _locale;

            _locale = locale_;
            return loc_;
        }

        const std::locale& locale() const
        {
            return _locale;
        }

        const rules_char_type* state(const id_type index_) const
        {
            if (index_ == 0)
            {
                return initial();
            }
            else
            {
                const id_type i_ = index_ - 1;

                if (_lexer_state_names.size() > i_)
                {
                    return _lexer_state_names[i_].c_str();
                }
                else
                {
                    return 0;
                }
            }
        }

        id_type state(const rules_char_type* name_) const
        {
            typename string_id_type_map::const_iterator iter_ =
                _statemap.find(name_);

            if (iter_ == _statemap.end())
            {
                return npos();
            }
            else
            {
                return iter_->second;
            }
        }

        id_type push_state(const rules_char_type* name_)
        {
            validate(name_);

            if (_statemap.insert(string_id_type_pair(name_,
                static_cast<id_type>(_statemap.size()))).second)
            {
                _regexes.push_back(token_deque_deque());
                _regex_strings.push_back(std_string_deque());
                _features.push_back(0);
                _ids.push_back(id_vector());
                _user_ids.push_back(id_vector());
                _next_dfas.push_back(id_vector());
                _pushes.push_back(id_vector());
                _pops.push_back(bool_vector());

                if (string(name_) != initial())
                {
                    _lexer_state_names.push_back(name_);
                }
            }
            else
            {
                return _statemap.find(name_)->second;
            }

            if (_next_dfas.size() > npos())
            {
                // Overflow
                throw runtime_error("The id_type you have chosen cannot hold "
                    "this many lexer start states.");
            }

            // Initial is not stored, so no need to - 1.
            return static_cast<id_type>(_lexer_state_names.size());
        }

        void insert_macro(const rules_char_type* name_,
            const rules_char_type* regex_)
        {
            insert_macro(name_, string(regex_));
        }

        void insert_macro(const rules_char_type* name_,
            const rules_char_type* regex_start_,
            const rules_char_type* regex_end_)
        {
            insert_macro(name_, string(regex_start_, regex_end_));
        }

        void insert_macro(const rules_char_type* name_, const string& regex_)
        {
            validate(name_);

            typename macro_map::const_iterator iter_ = _macro_map.find(name_);

            if (iter_ == _macro_map.end())
            {
                std::pair<typename macro_map::iterator, bool> pair_ =
                    _macro_map.insert(macro_pair(name_, token_deque()));
                token_deque_deque tokens_deque_ =
                    tokenise(regex_, npos(), name_);

                // There can only be one token_deque for a macro
                pair_.first->second = tokens_deque_.back();
            }
            else
            {
                std::ostringstream ss_;

                ss_ << "Attempt to redefine MACRO '";
                narrow(name_, ss_);
                ss_ << "'.";
                throw runtime_error(ss_.str());
            }
        }

        // Add rule to INITIAL
        // Separate overloads to support python bindings
        void push(const rules_char_type* regex_, const id_type id_)
        {
            push(string(regex_), id_, npos());
        }

        void push(const rules_char_type* regex_, const id_type id_,
            const id_type user_id_)
        {
            push(string(regex_), id_, user_id_);
        }

        void push(const rules_char_type* regex_start_,
            const rules_char_type* regex_end_,
            const id_type id_, const id_type user_id_ = npos())
        {
            push(string(regex_start_, regex_end_), id_, user_id_);
        }

        void push(const string& regex_, const id_type id_,
            const id_type user_id_ = npos())
        {
            std::ostringstream ss_;

            check_for_invalid_id(id_);
            narrow(regex_.c_str(), ss_);

            token_deque_deque tokens_deque_ = tokenise(regex_, id_, 0);

            if (tokens_deque_.front()[1]._type == detail::BOL)
            {
                _features.front() |= bol_bit;
            }

            if (tokens_deque_.back()[tokens_deque_.back().size() - 2].
                _type == detail::EOL)
            {
                _features.front() |= eol_bit;
            }

            if (id_ == skip())
            {
                _features.front() |= skip_bit;
            }
            else if (id_ == eoi())
            {
                _features.front() |= again_bit;
            }

            for (typename token_deque_deque::const_iterator iter_ =
                tokens_deque_.begin(), end_ = tokens_deque_.end();
                iter_ != end_; ++iter_)
            {
                const token_deque& tokens_ = *iter_;

                _regex_strings.front().push_back(ss_.str());
                _regexes.front().push_back(tokens_);
                _ids.front().push_back(id_);
                _user_ids.front().push_back(user_id_);
                _next_dfas.front().push_back(0);
                _pushes.front().push_back(npos());
                _pops.front().push_back(false);
            }
        }

        // Add rule with no id
        void push(const rules_char_type* curr_dfa_,
            const rules_char_type* regex_, const rules_char_type* new_dfa_)
        {
            push(curr_dfa_, string(regex_), new_dfa_);
        }

        void push(const rules_char_type* curr_dfa_,
            const rules_char_type* regex_start_,
            const rules_char_type* regex_end_, const rules_char_type* new_dfa_)
        {
            push(curr_dfa_, string(regex_start_, regex_end_), new_dfa_);
        }

        void push(const rules_char_type* curr_dfa_, const string& regex_,
            const rules_char_type* new_dfa_)
        {
            push(curr_dfa_, regex_, eoi(), new_dfa_, false);
        }

        // Add rule with id
        // Separate overloads to support python bindings
        void push(const rules_char_type* curr_dfa_,
            const rules_char_type* regex_, const id_type id_,
            const rules_char_type* new_dfa_)
        {
            push(curr_dfa_, string(regex_), id_, new_dfa_, npos());
        }

        void push(const rules_char_type* curr_dfa_,
            const rules_char_type* regex_, const id_type id_,
            const rules_char_type* new_dfa_, const id_type user_id_)
        {
            push(curr_dfa_, string(regex_), id_, new_dfa_, user_id_);
        }

        void push(const rules_char_type* curr_dfa_,
            const rules_char_type* regex_start_,
            const rules_char_type* regex_end_, const id_type id_,
            const rules_char_type* new_dfa_, const id_type user_id_ = npos())
        {
            push(curr_dfa_, string(regex_start_, regex_end_),
                id_, new_dfa_, user_id_);
        }

        void push(const rules_char_type* curr_dfa_, const string& regex_,
            const id_type id_, const rules_char_type* new_dfa_,
            const id_type user_id_ = npos())
        {
            push(curr_dfa_, regex_, id_, new_dfa_, true, user_id_);
        }

        void reverse()
        {
            typename token_deque_deque_deque::iterator state_iter_ =
                _regexes.begin();
            typename token_deque_deque_deque::iterator state_end_ =
                _regexes.end();
            typename macro_map::iterator macro_iter_ = _macro_map.begin();
            typename macro_map::iterator macro_end_ = _macro_map.end();

            for (; state_iter_ != state_end_; ++state_iter_)
            {
                typename token_deque_deque::iterator regex_iter_ =
                    state_iter_->begin();
                typename token_deque_deque::iterator regex_end_ =
                    state_iter_->end();

                for (; regex_iter_ != regex_end_; ++regex_iter_)
                {
                    reverse(*regex_iter_);
                }
            }

            for (; macro_iter_ != macro_end_; ++macro_iter_)
            {
                reverse(macro_iter_->second);
            }
        }

        const string_id_type_map& statemap() const
        {
            return _statemap;
        }

        const macro_map& macros() const
        {
            return _macro_map;
        }

        const token_deque_deque_deque& regexes() const
        {
            return _regexes;
        }

        const std_string_deque_deque& regex_strings() const
        {
            return _regex_strings;
        }

        const id_vector& features() const
        {
            return _features;
        }

        const id_vector_deque& ids() const
        {
            return _ids;
        }

        const id_vector_deque& user_ids() const
        {
            return _user_ids;
        }

        const id_vector_deque& next_dfas() const
        {
            return _next_dfas;
        }

        const id_vector_deque& pushes() const
        {
            return _pushes;
        }

        const bool_vector_deque& pops() const
        {
            return _pops;
        }

        bool empty() const
        {
            typename token_deque_deque_deque::const_iterator iter_ =
                _regexes.begin();
            typename token_deque_deque_deque::const_iterator end_ =
                _regexes.end();
            bool empty_ = true;

            for (; iter_ != end_; ++iter_)
            {
                if (!iter_->empty())
                {
                    empty_ = false;
                    break;
                }
            }

            return empty_;
        }

        static const rules_char_type* initial()
        {
            static const rules_char_type initial_[] =
            { 'I', 'N', 'I', 'T', 'I', 'A', 'L', 0 };

            return initial_;
        }

        static const rules_char_type* dot()
        {
            static const rules_char_type dot_[] = { '.', 0 };

            return dot_;
        }

        static const rules_char_type* all_states()
        {
            static const rules_char_type star_[] = { '*', 0 };

            return star_;
        }

    private:
        string_id_type_map _statemap;
        macro_map _macro_map;
        token_deque_deque_deque _regexes;
        std_string_deque_deque _regex_strings;
        id_vector _features;
        id_vector_deque _ids;
        id_vector_deque _user_ids;
        id_vector_deque _next_dfas;
        id_vector_deque _pushes;
        bool_vector_deque _pops;
        std::size_t _flags;
        std::locale _locale;
        string_deque _lexer_state_names;

        token_deque_deque tokenise(const string& regex_,
            const id_type id_, const rules_char_type* name_)
        {
            token_deque_deque ret_;
            token_deque tokens_;
            re_state state_(regex_.c_str(), regex_.c_str() + regex_.size(), id_,
                _flags, _locale, name_);
            string macro_;
            rules_char_type diff_ = 0;
            std::vector<std::size_t> ab_indexes_;

            tokens_.push_back(token());

            do
            {
                token* lhs_ = &tokens_.back();
                token rhs_;

                tokeniser::next(*lhs_, state_, rhs_);
                check_diff_error(name_, lhs_, rhs_, state_);

                if (rhs_._type == detail::MACRO)
                {
                    process_macro(name_, lhs_, rhs_, macro_, tokens_, state_,
                        regex_, diff_);
                }
                else if (rhs_._type == detail::DIFF)
                {
                    process_diff(name_, rhs_, state_, macro_, diff_);
                    continue;
                }
                else if (!diff_)
                {
                    tokens_.push_back(rhs_);
                    lhs_ = &tokens_.back();
                    macro_.clear();
                }

                // diff_ may have been set by previous conditional.
                if (diff_)
                {
                    check_diff(name_, lhs_, rhs_, state_, diff_);
                    diff_ = 0;
                }
            } while (tokens_.back()._type != detail::END);

            strip_parens(tokens_);
            record_abstemious(tokens_, ab_indexes_);

            if (!name_)
            {
                abstemious<rules_char_type, char_type>::
                    prune(tokens_, ab_indexes_);
                // Need to fill ab_indexes_ again following prune
                record_abstemious(tokens_, ab_indexes_);
            }

            check_empty(name_, tokens_, state_);

            if (ab_indexes_.empty())
                ret_.push_back(tokens_);
            else
                ret_ = split(tokens_);

            return ret_;
        }

        void strip_parens(token_deque& tokens_) const
        {
            for (;;)
            {
                const std::size_t lhs_ = 1;
                const std::size_t rhs_ = tokens_.size() - 2;

                if (lhs_ < rhs_ &&
                    tokens_[lhs_]._type == detail::OPENPAREN &&
                    tokens_[rhs_]._type == detail::CLOSEPAREN &&
                    balanced_parens(tokens_.begin() + lhs_ + 1,
                        tokens_.begin() + rhs_))
                {
                    tokens_.erase(tokens_.begin() + rhs_,
                        tokens_.end() - 1);
                    tokens_.erase(tokens_.begin() + lhs_,
                        tokens_.begin() + lhs_ + 1);
                }
                else
                    break;
            }
        }

        bool balanced_parens(typename token_deque::const_iterator iter_,
            typename token_deque::const_iterator end_) const
        {
            std::size_t parens_ = 0;

            for (; iter_ != end_; ++iter_)
            {
                switch (iter_->_type)
                {
                case detail::OPENPAREN:
                    ++parens_;
                    break;
                case detail::CLOSEPAREN:
                    if (parens_ == 0)
                        return false;

                    --parens_;
                    break;
                default:
                    break;
                }
            }

            return parens_ == 0;
        }

        void record_abstemious(const token_deque& tokens_,
            std::vector<std::size_t>& ab_indexes_) const
        {
            for (std::size_t idx_ = 0, size_ = tokens_.size();
                idx_ < size_; ++idx_)
            {
                switch (tokens_[idx_]._type)
                {
                case detail::AOPT:
                case detail::AZEROORMORE:
                case detail::AONEORMORE:
                case detail::AREPEATN:
                    ab_indexes_.push_back(idx_);
                    break;
                default:
                    break;
                }
            }
        }

        void check_diff_error(const rules_char_type* name_,
            const token* lhs_, const token& rhs_, const re_state& state_) const
        {
            if (rhs_._type != detail::DIFF &&
                lhs_->precedence(rhs_._type) == ' ')
            {
                std::ostringstream ss_;

                ss_ << "A syntax error occurred: '" <<
                    lhs_->precedence_string() <<
                    "' against '" << rhs_.precedence_string() <<
                    "' preceding index " << state_.index() <<
                    " in ";

                if (name_ != 0)
                {
                    ss_ << "macro ";
                    narrow(name_, ss_);
                }
                else
                {
                    ss_ << "rule id " << state_._id;
                }

                ss_ << '.';
                throw runtime_error(ss_.str());
            }
        }

        void process_macro(const rules_char_type* name_,
            token* lhs_, token& rhs_, string& macro_,
            token_deque& tokens_, const re_state& state_,
            const string& regex_, const rules_char_type diff_)
        {
            typename macro_map::const_iterator iter_ =
                _macro_map.find(rhs_._extra);

            macro_ = rhs_._extra;

            if (iter_ == _macro_map.end())
            {
                const rules_char_type* rhs_name_ = rhs_._extra.c_str();
                std::ostringstream ss_;

                ss_ << "Unknown MACRO name '";
                narrow(rhs_name_, ss_);
                ss_ << "'.";
                throw runtime_error(ss_.str());
            }
            else
            {
                const bool multiple_ = iter_->second.size() > 3;
                const token& first_ = iter_->second[1];
                const token& second_ =
                    iter_->second[iter_->second.size() - 2];
                const bool bol_ = tokens_.size() == 1 &&
                    first_._type == detail::BOL;
                const bool caret_ =
                    !bol_ && first_._type == detail::BOL;
                const bool eol_ =
                    state_._curr == regex_.c_str() + regex_.size() &&
                    second_._type == detail::EOL;
                const bool dollar_ =
                    !eol_ && second_._type == detail::EOL;

                if (diff_)
                {
                    if (multiple_)
                    {
                        std::ostringstream ss_;

                        ss_ << "Single CHARSET must follow {-} or "
                            "{+} at index " << state_.index() - 1 <<
                            " in ";

                        if (name_ != 0)
                        {
                            ss_ << "macro ";
                            narrow(name_, ss_);
                        }
                        else
                        {
                            ss_ << "rule id " << state_._id;
                        }

                        ss_ << '.';
                        throw runtime_error(ss_.str());
                    }
                    else
                    {
                        rhs_ = iter_->second[1];
                    }
                }

                // Any macro with more than one charset (or quantifiers)
                // requires bracketing.
                if (multiple_ && !(bol_ || eol_))
                {
                    token open_;

                    open_._type = detail::OPENPAREN;
                    open_._str.insert('(');
                    tokens_.push_back(open_);
                }

                // Don't need to store token if it is diff.
                if (!diff_)
                {
                    std::size_t start_offset_ = 1;
                    std::size_t end_offset_ = 1;

                    if (caret_)
                    {
                        token token_;

                        token_._type = detail::CHARSET;
                        token_._str.insert('^');
                        tokens_.push_back(token_);
                        ++start_offset_;
                    }

                    if (dollar_)
                    {
                        ++end_offset_;
                    }

                    // Don't insert BEGIN or END tokens
                    tokens_.insert(tokens_.end(),
                        iter_->second.begin() + start_offset_,
                        iter_->second.end() - end_offset_);

                    if (dollar_)
                    {
                        token token_;

                        token_._type = detail::CHARSET;
                        token_._str.insert('$');
                        tokens_.push_back(token_);
                    }

                    lhs_ = &tokens_.back();
                }

                if (multiple_ && !(bol_ || eol_))
                {
                    token close_;

                    close_._type = detail::CLOSEPAREN;
                    close_._str.insert(')');
                    tokens_.push_back(close_);
                }
            }
        }

        void process_diff(const rules_char_type* name_,
            const token& rhs_, const re_state& state_, string& macro_,
            rules_char_type& diff_)
        {
            if (!macro_.empty())
            {
                typename macro_map::const_iterator iter_ =
                    _macro_map.find(macro_);

                if (iter_->second.size() > 3)
                {
                    std::ostringstream ss_;

                    ss_ << "Single CHARSET must precede {-} or {+} at "
                        "index " << state_.index() - 1 << " in ";

                    if (name_ != 0)
                    {
                        ss_ << "macro ";
                        narrow(name_, ss_);
                    }
                    else
                    {
                        ss_ << "rule id " << state_._id;
                    }

                    ss_ << '.';
                    throw runtime_error(ss_.str());
                }
            }

            diff_ = rhs_._extra[0];
            macro_.clear();
        }

        void check_diff(const rules_char_type* name_,
            token* lhs_, const token& rhs_, const re_state& state_,
            const rules_char_type diff_) const
        {
            if (rhs_._type != detail::CHARSET)
            {
                std::ostringstream ss_;

                ss_ << "CHARSET must follow {-} or {+} at index " <<
                    state_.index() - 1 << " in ";

                if (name_ != 0)
                {
                    ss_ << "macro ";
                    narrow(name_, ss_);
                }
                else
                {
                    ss_ << "rule id " << state_._id;
                }

                ss_ << '.';
                throw runtime_error(ss_.str());
            }

            switch (diff_)
            {
            case '-':
                lhs_->_str.remove(rhs_._str);

                if (lhs_->_str.empty())
                {
                    std::ostringstream ss_;

                    ss_ << "Empty charset created by {-} at index " <<
                        state_.index() - 1 << " in ";

                    if (name_ != 0)
                    {
                        ss_ << "macro ";
                        narrow(name_, ss_);
                    }
                    else
                    {
                        ss_ << "rule id " << state_._id;
                    }

                    ss_ << '.';
                    throw runtime_error(ss_.str());
                }

                break;
            case '+':
                lhs_->_str.insert(rhs_._str);
                break;
            default:
                break;
            }
        }

        void check_empty(const rules_char_type* name_, token_deque& tokens_,
            const re_state& state_) const
        {
            if (tokens_.size() == 2 && !(_flags & match_zero_len))
            {
                std::ostringstream ss_;

                ss_ << "Empty regex in ";

                if (name_ != 0)
                {
                    ss_ << "macro ";
                    narrow(name_, ss_);
                }
                else
                {
                    ss_ << "rule id " << state_._id;
                }

                ss_ << " is not allowed.";
                throw runtime_error(ss_.str());
            }
        }

        token_deque_deque split(const token_deque& tokens_)
        {
            token_deque_deque ret_;
            std::size_t parens_ = 0;
            std::size_t start_ = 1;
            std::size_t end_ = 0;

            for (typename token_deque::const_iterator iter_ = tokens_.begin(),
                e_ = tokens_.end(); iter_ != e_; ++iter_)
            {
                const token& token_ = *iter_;

                switch (token_._type)
                {
                case detail::OPENPAREN:
                    ++parens_;
                    break;
                case detail::CLOSEPAREN:
                    --parens_;
                    break;
                case detail::OR:
                    if (!parens_)
                    {
                        ret_.push_back(token_deque());

                        token_deque& new_tokens_ = ret_.back();
                        typename token_deque::const_iterator start_iter_ =
                            tokens_.begin();
                        typename token_deque::const_iterator end_iter_ =
                            tokens_.begin();

                        std::advance(start_iter_, start_);
                        std::advance(end_iter_, end_);
                        new_tokens_.push_back(token());
                        new_tokens_.insert(new_tokens_.end(),
                            start_iter_,
                            end_iter_);
                        new_tokens_.push_back(token(detail::END));
                        start_ = end_ + 1;
                    }

                    break;
                case detail::END:
                {
                    ret_.push_back(token_deque());

                    token_deque& new_tokens_ = ret_.back();

                    new_tokens_.push_back(token());
                    new_tokens_.insert(new_tokens_.end(),
                        tokens_.begin() + start_,
                        tokens_.begin() + end_);
                    new_tokens_.push_back(token(detail::END));
                    break;
                }
                default:
                    break;
                }

                ++end_;
            }

            return ret_;
        }

        void reverse(token_deque& deque_)
        {
            token_deque new_deque_(deque_.size(), token());
            typename token_deque::reverse_iterator iter_ =
                deque_.rbegin();
            typename token_deque::reverse_iterator end_ =
                deque_.rend();
            typename token_deque::iterator dest_ = new_deque_.begin();
            std::stack<typename token_deque::reverse_iterator> stack_;

            for (; iter_ != end_; ++iter_, ++dest_)
            {
                switch (iter_->_type)
                {
                case detail::BEGIN:
                    iter_->swap(*dest_);
                    dest_->_type = detail::END;
                    break;
                case detail::BOL:
                    iter_->swap(*dest_);
                    dest_->_type = detail::EOL;
                    break;
                case detail::EOL:
                    iter_->swap(*dest_);
                    dest_->_type = detail::BOL;
                    break;
                case detail::OPENPAREN:
                    iter_->swap(*dest_);
                    dest_->_type = detail::CLOSEPAREN;

                    if (stack_.top() != end_)
                    {
                        ++dest_;
                        dest_->swap(*stack_.top());
                    }

                    stack_.pop();
                    break;
                case detail::CLOSEPAREN:
                    iter_->swap(*dest_);
                    dest_->_type = detail::OPENPAREN;
                    stack_.push(end_);
                    break;
                case detail::OPT:
                case detail::AOPT:
                case detail::ZEROORMORE:
                case detail::AZEROORMORE:
                case detail::ONEORMORE:
                case detail::AONEORMORE:
                case detail::REPEATN:
                case detail::AREPEATN:
                {
                    typename token_deque::reverse_iterator temp_ = iter_ + 1;

                    if (temp_->_type == detail::CLOSEPAREN)
                    {
                        stack_.push(iter_);
                        ++iter_;
                        iter_->swap(*dest_);
                        dest_->_type = detail::OPENPAREN;
                    }
                    else
                    {
                        dest_->swap(*temp_);
                        ++dest_;
                        dest_->swap(*iter_);
                        ++iter_;
                    }

                    break;
                }
                case detail::END:
                    iter_->swap(*dest_);
                    dest_->_type = detail::BEGIN;
                    break;
                default:
                    // detail::OR
                    // detail::CHARSET
                    iter_->swap(*dest_);
                    break;
                }
            }

            new_deque_.swap(deque_);
        }

        void push(const rules_char_type* curr_dfa_, const string& regex_,
            const id_type id_, const rules_char_type* new_dfa_,
            const bool check_, const id_type user_id_ = npos())
        {
            const bool star_ = *curr_dfa_ == '*' && *(curr_dfa_ + 1) == 0;
            const bool dot_ = *new_dfa_ == '.' && *(new_dfa_ + 1) == 0;
            const bool push_ = *new_dfa_ == '>';
            const rules_char_type* push_dfa_ = 0;
            const bool pop_ = *new_dfa_ == '<';

            if (push_ || pop_)
            {
                ++new_dfa_;
            }

            if (check_)
            {
                check_for_invalid_id(id_);
            }

            if (!dot_ && !pop_)
            {
                const rules_char_type* temp_ = new_dfa_;

                while (*temp_ && *temp_ != ':')
                {
                    ++temp_;
                }

                if (*temp_)
                    push_dfa_ = temp_ + 1;

                validate(new_dfa_, *temp_ ? temp_ : 0);

                if (!push_ && push_dfa_)
                {
                    std::ostringstream ss_;

                    ss_ << "Exit state is missing '>' for rule id " <<
                        id_ << '.';
                    throw runtime_error(ss_.str());
                }

                if (push_dfa_)
                {
                    validate(push_dfa_);
                }
            }

            // npos means pop here
            id_type new_dfa_id_ = npos();
            id_type push_dfa_id_ = npos();
            typename string_id_type_map::const_iterator iter_;
            typename string_id_type_map::const_iterator end_ = _statemap.end();
            id_vector next_dfas_;

            if (!dot_ && !pop_)
            {
                if (push_dfa_)
                {
                    iter_ = _statemap.find(string(new_dfa_, push_dfa_ - 1));
                }
                else
                {
                    iter_ = _statemap.find(new_dfa_);
                }

                if (iter_ == end_)
                {
                    std::ostringstream ss_;

                    ss_ << "Unknown state name '";
                    narrow(new_dfa_, ss_);
                    ss_ << "'.";
                    throw runtime_error(ss_.str());
                }

                new_dfa_id_ = iter_->second;

                if (push_dfa_)
                {
                    iter_ = _statemap.find(push_dfa_);

                    if (iter_ == end_)
                    {
                        std::ostringstream ss_;

                        ss_ << "Unknown state name '";
                        narrow(push_dfa_, ss_);
                        ss_ << "'.";
                        throw runtime_error(ss_.str());
                    }

                    push_dfa_id_ = iter_->second;
                }
            }

            if (star_)
            {
                const id_type size_ = static_cast<id_type>(_statemap.size());

                for (id_type i_ = 0; i_ < size_; ++i_)
                {
                    next_dfas_.push_back(i_);
                }
            }
            else
            {
                const rules_char_type* start_ = curr_dfa_;
                string next_dfa_;

                while (*curr_dfa_)
                {
                    while (*curr_dfa_ && *curr_dfa_ != ',')
                    {
                        ++curr_dfa_;
                    }

                    next_dfa_.assign(start_, curr_dfa_);

                    if (*curr_dfa_)
                    {
                        ++curr_dfa_;
                        start_ = curr_dfa_;
                    }

                    validate(next_dfa_.c_str());
                    iter_ = _statemap.find(next_dfa_.c_str());

                    if (iter_ == end_)
                    {
                        std::ostringstream ss_;

                        ss_ << "Unknown state name '";
                        curr_dfa_ = next_dfa_.c_str();
                        narrow(curr_dfa_, ss_);
                        ss_ << "'.";
                        throw runtime_error(ss_.str());
                    }

                    next_dfas_.push_back(iter_->second);
                }
            }

            for (std::size_t i_ = 0, size_ = next_dfas_.size();
                i_ < size_; ++i_)
            {
                const id_type curr_ = next_dfas_[i_];
                token_deque_deque tokens_deque_ = tokenise(regex_, id_, 0);
                std::ostringstream ss_;

                narrow(regex_.c_str(), ss_);

                if (tokens_deque_.back()[1]._type == detail::BOL)
                {
                    _features[curr_] |= bol_bit;
                }

                if (tokens_deque_.back()[tokens_deque_.back().size() - 2].
                    _type == detail::EOL)
                {
                    _features[curr_] |= eol_bit;
                }

                if (id_ == skip())
                {
                    _features[curr_] |= skip_bit;
                }
                else if (id_ == eoi())
                {
                    _features[curr_] |= again_bit;
                }

                if (push_ || pop_)
                {
                    _features[curr_] |= recursive_bit;
                }

                for (typename token_deque_deque::const_iterator tokens_iter_ =
                    tokens_deque_.begin(), tokens_end_ = tokens_deque_.end();
                    tokens_iter_ != tokens_end_; ++tokens_iter_)
                {
                    const token_deque& tokens_ = *tokens_iter_;

                    _regex_strings[curr_].push_back(ss_.str());
                    _regexes[curr_].push_back(tokens_);
                    _ids[curr_].push_back(id_);
                    _user_ids[curr_].push_back(user_id_);
                    _next_dfas[curr_].push_back(dot_ ? curr_ : new_dfa_id_);

                    id_ty push_id_ = npos();

                    if (push_)
                    {
                        if (push_dfa_)
                            push_id_ = push_dfa_id_;
                        else
                            push_id_ = curr_;
                    }

                    _pushes[curr_].push_back(push_id_);
                    _pops[curr_].push_back(pop_);
                }
            }
        }

        void validate(const rules_char_type* name_,
            const rules_char_type* end_ = 0) const
        {
            const rules_char_type* start_ = name_;

            if (*name_ != '_' && !(*name_ >= 'A' && *name_ <= 'Z') &&
                !(*name_ >= 'a' && *name_ <= 'z'))
            {
                std::ostringstream ss_;

                ss_ << "Invalid name '";
                narrow(name_, ss_);
                ss_ << "'.";
                throw runtime_error(ss_.str());
            }
            else if (*name_)
            {
                ++name_;
            }

            while (*name_ && name_ != end_)
            {
                if (*name_ != '_' && *name_ != '-' &&
                    !(*name_ >= 'A' && *name_ <= 'Z') &&
                    !(*name_ >= 'a' && *name_ <= 'z') &&
                    !(*name_ >= '0' && *name_ <= '9'))
                {
                    std::ostringstream ss_;

                    ss_ << "Invalid name '";
                    name_ = start_;
                    narrow(name_, ss_);
                    ss_ << "'.";
                    throw runtime_error(ss_.str());
                }

                ++name_;
            }
        }

        void check_for_invalid_id(const id_type id_) const
        {
            if (id_ == eoi())
            {
                throw runtime_error("Cannot re-use the id for eoi.");
            }

            if (id_ == npos())
            {
                throw runtime_error("The id npos is reserved for the "
                    "UNKNOWN token.");
            }
        }
    };

    typedef basic_rules<char, char> rules;
    typedef basic_rules<wchar_t, wchar_t> wrules;
}

#endif
