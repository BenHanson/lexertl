// rules.hpp
// Copyright (c) 2005-2012 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_RULES_HPP
#define LEXERTL_RULES_HPP

#include "compile_assert.hpp"
#include <deque>
#include "enums.hpp"
#include "internals.hpp"
#include <locale>
#include <map>
#include "runtime_error.hpp"
#include <set>
#include "size_t.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace lexertl
{
template<typename ch_type, typename id_ty = std::size_t>
class basic_rules
{
public:
    typedef std::vector<bool> bool_vector;
    typedef std::deque<bool_vector> bool_vector_deque;
    typedef ch_type char_type;
    typedef id_ty id_type;
    typedef std::vector<id_type> id_vector;
    typedef std::deque<id_vector> id_vector_deque;
    typedef std::basic_string<char_type> string;
    typedef std::deque<string> string_deque;
    typedef std::deque<string_deque> string_deque_deque;
    typedef std::set<string> string_set;
    typedef std::pair<string, string> string_pair;
    typedef std::deque<string_pair> string_pair_deque;
    typedef std::map<string, id_type> string_id_type_map;
    typedef std::pair<string, id_type> string_id_type_pair;

    // If you get a compile error here you have
    // failed to define an unsigned id type.
    compile_assert<(static_cast<id_type>(~0) > 0)>
        _valid_id_type;

    basic_rules (const std::size_t flags_ = dot_not_newline) :
        _valid_id_type (),
        _statemap (),
        _macrodeque (),
        _macroset (),
        _regexes (),
        _features (),
        _ids (),
        _user_ids (),
        _next_dfas (),
        _pushes (),
        _pops (),
        _flags (flags_),
        _locale (),
        _lexer_state_names (),
        _eoi (0)
    {
        add_state (initial ());
    }

    void clear ()
    {
        _statemap.clear ();
        _macrodeque.clear ();
        _macroset.clear ();
        _regexes.clear ();
        _features.clear ();
        _ids.clear ();
        _user_ids.clear ();
        _next_dfas.clear ();
        _pushes.clear ();
        _pops.clear ();
        _flags = dot_not_newline;
        _locale = std::locale ();
        _lexer_state_names.clear ();
        _eoi = 0;
        add_state (initial ());
    }

    void clear (const id_type dfa_)
    {
        if (_regexes.size () > dfa_)
        {
            _regexes[dfa_].clear ();
            _features[dfa_] = 0;
            _ids[dfa_].clear ();
            _user_ids[dfa_].clear ();
            _next_dfas[dfa_].clear ();
            _pushes[dfa_].clear ();
            _pops[dfa_].clear ();
        }
    }

    void flags (const std::size_t flags_)
    {
        _flags = flags_;
    }

    std::size_t flags () const
    {
        return _flags;
    }

    static id_type skip ()
    {
        return static_cast<id_type>(~1);
    }

    void eoi (const id_type eoi_)
    {
        _eoi = eoi_;
    }

    id_type eoi () const
    {
        return _eoi;
    }

    std::locale imbue (const std::locale &locale_)
    {
        std::locale loc_ = _locale;

        _locale = locale_;
        return loc_;
    }

    const std::locale &locale () const
    {
        return _locale;
    }

    const char_type *state (const id_type index_) const
    {
        if (index_ == 0)
        {
            return initial ();
        }
        else
        {
            const id_type i_ = index_ - 1;

            if (_lexer_state_names.size () > i_)
            {
                return _lexer_state_names[i_].c_str ();
            }
            else
            {
                return 0;
            }
        }
    }

    id_type state (const char_type *name_) const
    {
        typename string_id_type_map::const_iterator iter_ =
            _statemap.find (name_);

        if (iter_ == _statemap.end ())
        {
            return npos ();
        }
        else
        {
            return iter_->second;
        }
    }

    id_type add_state (const char_type *name_)
    {
        validate (name_);

        if (_statemap.insert (string_id_type_pair (name_,
            _statemap.size ())).second)
        {
            _regexes.push_back (string_deque ());
            _features.push_back (0);
            _ids.push_back (id_vector ());
            _user_ids.push_back (id_vector ());
            _next_dfas.push_back (id_vector ());
            _pushes.push_back (id_vector ());
            _pops.push_back (bool_vector ());

            if (string (name_) != initial ())
            {
                _lexer_state_names.push_back (name_);
            }
        }
        else
        {
            return _statemap.find (name_)->second;
        }

        if (_next_dfas.size () > npos ())
        {
            // Overflow
            throw runtime_error ("The data type you have chosen cannot hold "
                "this many lexer start states.");
        }

        // Initial is not stored, so no need to - 1.
        return static_cast<id_type>(_lexer_state_names.size ());
    }

    void add_macro (const char_type *name_, const char_type *regex_)
    {
        add_macro (name_, string (regex_));
    }

    void add_macro (const char_type *name_, const char_type *regex_start_,
        const char_type *regex_end_)
    {
        add_macro (name_, string (regex_start_, regex_end_));
    }

    void add_macro (const char_type *name_, const string &regex_)
    {
        validate (name_);

        typename string_set::const_iterator iter_ = _macroset.find (name_);

        if (iter_ == _macroset.end ())
        {
            _macrodeque.push_back (string_pair (name_, regex_));
            _macroset.insert (name_);
        }
        else
        {
            std::basic_stringstream<char_type> ss_;
            std::ostringstream os_;

            os_ << "Attempt to redefine MACRO '";

            while (*name_)
            {
                os_ << ss_.narrow (*name_++, static_cast<char_type> (' '));
            }

            os_ << "'.";
            throw runtime_error (os_.str ());
        }
    }

    void add_macros (const basic_rules &rules_)
    {
        const string_pair_deque &macros_ = rules_.macrodeque ();
        typename string_pair_deque::const_iterator macro_iter_ =
            macros_.begin ();
        typename string_pair_deque::const_iterator macro_end_ =
            macros_.end ();

        for (; macro_iter_ != macro_end_; ++macro_iter_)
        {
            add_macro (macro_iter_->first.c_str (),
                macro_iter_->second.c_str ());
        }
    }

    void merge_macros (const basic_rules &rules_)
    {
        const string_pair_deque &macros_ = rules_.macrodeque ();
        typename string_pair_deque::const_iterator macro_iter_ =
            macros_.begin ();
        typename string_pair_deque::const_iterator macro_end_ =
            macros_.end ();
        typename string_set::const_iterator macro_dest_iter_;
        typename string_set::const_iterator macro_dest_end_ = _macroset.end ();

        for (; macro_iter_ != macro_end_; ++macro_iter_)
        {
            macro_dest_iter_ = _macroset.find (macro_iter_->first);

            if (macro_dest_iter_ == macro_dest_end_)
            {
                add_macro (macro_iter_->first.c_str (),
                    macro_iter_->second.c_str ());
            }
        }
    }

    // Add rule to INITIAL
    void add (const char_type *regex_, const id_type id_,
        const id_type user_id_ = npos ())
    {
        add (string (regex_), id_, user_id_);
    }

    void add (const char_type *regex_start_, const char_type *regex_end_,
        const id_type id_, const id_type user_id_ = npos ())
    {
        add (string (regex_start_, regex_end_), id_, user_id_);
    }

    void add (const string &regex_, const id_type id_,
        const id_type user_id_ = npos ())
    {
        check_for_invalid_id (id_);
        _regexes.front ().push_back (regex_);

        if (regex_[0] == '^')
        {
            _features.front () |= bol_bit;
        }

        if (regex_.size () > 0 && regex_[regex_.size () - 1] == '$')
        {
            _features.front () |= eol_bit;
        }

        if (id_ == skip ())
        {
            _features.front () |= skip_bit;
        }
        else if (id_ == eoi ())
        {
            _features.front () |= again_bit;
        }

        _ids.front ().push_back (id_);
        _user_ids.front ().push_back (user_id_);
        _next_dfas.front ().push_back (0);
        _pushes.front ().push_back (npos ());
        _pops.front ().push_back (false);
    }

    // Add rule with no id
    void add (const char_type *curr_dfa_,
        const char_type *regex_, const char_type *new_dfa_)
    {
        add (curr_dfa_, string (regex_), new_dfa_);
    }

    void add (const char_type *curr_dfa_,
        const char_type *regex_start_, const char_type *regex_end_,
        const char_type *new_dfa_)
    {
        add (curr_dfa_, string (regex_start_, regex_end_), new_dfa_);
    }

    void add (const char_type *curr_dfa_, const string &regex_,
        const char_type *new_dfa_)
    {
        add (curr_dfa_, regex_, _eoi, new_dfa_, false);
    }

    // Add rule with id
    void add (const char_type *curr_dfa_,
        const char_type *regex_, const id_type id_,
        const char_type *new_dfa_, const id_type user_id_ = npos ())
    {
        add (curr_dfa_, string (regex_), id_, new_dfa_, user_id_);
    }

    void add (const char_type *curr_dfa_, const char_type *regex_start_,
        const char_type *regex_end_, const id_type id_,
        const char_type *new_dfa_, const id_type user_id_ = npos ())
    {
        add (curr_dfa_, string (regex_start_, regex_end_),
            id_, new_dfa_, user_id_);
    }

    void add (const char_type *curr_dfa_, const string &regex_,
        const id_type id_, const char_type *new_dfa_,
        const id_type user_id_ = npos ())
    {
        add (curr_dfa_, regex_, id_, new_dfa_, true, user_id_);
    }

    const string_id_type_map &statemap () const
    {
        return _statemap;
    }

    const string_pair_deque &macrodeque () const
    {
        return _macrodeque;
    }

    const string_deque_deque &regexes () const
    {
        return _regexes;
    }

    const id_vector &features () const
    {
        return _features;
    }

    const id_vector_deque &ids () const
    {
        return _ids;
    }

    const id_vector_deque &user_ids () const
    {
        return _user_ids;
    }

    const id_vector_deque &next_dfas () const
    {
        return _next_dfas;
    }

    const id_vector_deque &pushes () const
    {
        return _pushes;
    }

    const bool_vector_deque &pops () const
    {
        return _pops;
    }

    bool empty () const
    {
        typename string_deque_deque::const_iterator iter_ = _regexes.begin ();
        typename string_deque_deque::const_iterator end_ = _regexes.end ();
        bool empty_ = true;

        for (; iter_ != end_; ++iter_)
        {
            if (!iter_->empty ())
            {
                empty_ = false;
                break;
            }
        }

        return empty_;
    }

    static const char_type *initial ()
    {
        static const char_type initial_[] =
            {'I', 'N', 'I', 'T', 'I', 'A', 'L', 0};

        return initial_;
    }

    static const char_type *dot ()
    {
        static const char_type dot_[] = {'.', 0};

        return dot_;
    }

    static const char_type *all_states ()
    {
        static const char_type star_[] = {'*', 0};

        return star_;
    }

    static id_type npos ()
    {
        return static_cast<id_type>(~0);
    }

private:
    string_id_type_map _statemap;
    string_pair_deque _macrodeque;
    string_set _macroset;
    string_deque_deque _regexes;
    id_vector _features;
    id_vector_deque _ids;
    id_vector_deque _user_ids;
    id_vector_deque _next_dfas;
    id_vector_deque _pushes;
    bool_vector_deque _pops;
    std::size_t _flags;
    std::locale _locale;
    string_deque _lexer_state_names;
    id_type _eoi;

    void add (const char_type *curr_dfa_, const string &regex_,
        const id_type id_, const char_type *new_dfa_,
        const bool check_, const id_type user_id_ = npos ())
    {
        const bool star_ = *curr_dfa_ == '*' && *(curr_dfa_ + 1) == 0;
        const bool dot_ = *new_dfa_ == '.' && *(new_dfa_ + 1) == 0;
        const bool push_ = *new_dfa_ == '>';
        const char_type *push_dfa_ = 0;
        const bool pop_ = *new_dfa_ == '<';

        if (push_ || pop_)
        {
            ++new_dfa_;
        }

        if (check_)
        {
            check_for_invalid_id (id_);
        }

        if (!dot_ && !pop_)
        {
            const char_type *temp_ = new_dfa_;

            while (*temp_ && *temp_ != ':')
            {
                ++temp_;
            }

            if (*temp_) push_dfa_ = temp_ + 1;

            validate (new_dfa_, *temp_ ? temp_ : 0);

            if (push_dfa_)
            {
                validate (push_dfa_);
            }
        }

        // npos means pop here
        id_type new_dfa_id_ = npos ();
        id_type push_dfa_id_ = npos ();
        typename string_id_type_map::const_iterator iter_;
        typename string_id_type_map::const_iterator end_ = _statemap.end ();
        id_vector next_dfas_;

        if (!dot_ && !pop_)
        {
            if (push_dfa_)
            {
                iter_ = _statemap.find (string (new_dfa_, push_dfa_ - 1));
            }
            else
            {
                iter_ = _statemap.find (new_dfa_);
            }

            if (iter_ == end_)
            {
                std::basic_stringstream<char_type> ss_;
                std::ostringstream os_;

                os_ << "Unknown state name '";

                while (*new_dfa_)
                {
                    os_ << ss_.narrow (*new_dfa_++, ' ');
                }

                os_ << "'.";
                throw runtime_error (os_.str ());
            }

            new_dfa_id_ = iter_->second;

            if (push_dfa_)
            {
                iter_ = _statemap.find (push_dfa_);

                if (iter_ == end_)
                {
                    std::basic_stringstream<char_type> ss_;
                    std::ostringstream os_;

                    os_ << "Unknown state name '";

                    while (*push_dfa_)
                    {
                        os_ << ss_.narrow (*push_dfa_++, ' ');
                    }

                    os_ << "'.";
                    throw runtime_error (os_.str ());
                }

                push_dfa_id_ = iter_->second;
            }
        }

        if (star_)
        {
            const std::size_t size_ = _statemap.size ();

            for (id_type i_ = 0; i_ < size_; ++i_)
            {
                next_dfas_.push_back (i_);
            }
        }
        else
        {
            const char_type *start_ = curr_dfa_;
            string next_dfa_;

            while (*curr_dfa_)
            {
                while (*curr_dfa_ && *curr_dfa_ != ',')
                {
                    ++curr_dfa_;
                }

                next_dfa_.assign (start_, curr_dfa_);

                if (*curr_dfa_)
                {
                    ++curr_dfa_;
                    start_ = curr_dfa_;
                }

                validate (next_dfa_.c_str ());
                iter_ = _statemap.find (next_dfa_.c_str ());

                if (iter_ == end_)
                {
                    std::basic_stringstream<char_type> ss_;
                    std::ostringstream os_;

                    os_ << "Unknown state name '";
                    curr_dfa_ = next_dfa_.c_str ();

                    while (*curr_dfa_)
                    {
                        os_ << ss_.narrow (*curr_dfa_++, ' ');
                    }

                    os_ << "'.";
                    throw runtime_error (os_.str ());
                }

                next_dfas_.push_back (iter_->second);
            }
        }

        for (std::size_t i_ = 0, size_ = next_dfas_.size ();
            i_ < size_; ++i_)
        {
            const id_type curr_ = next_dfas_[i_];

            _regexes[curr_].push_back (regex_);

            if (regex_[0] == '^')
            {
                _features[curr_] |= bol_bit;
            }

            if (regex_[regex_.size () - 1] == '$')
            {
                _features[curr_] |= eol_bit;
            }

            if (id_ == skip ())
            {
                _features[curr_] |= skip_bit;
            }
            else if (id_ == eoi ())
            {
                _features[curr_] |= again_bit;
            }

            if (push_ || pop_)
            {
                _features[curr_] |= recursive_bit;
            }

            _ids[curr_].push_back (id_);
            _user_ids[curr_].push_back (user_id_);
            _next_dfas[curr_].push_back (dot_ ? curr_ : new_dfa_id_);
            _pushes[curr_].push_back (push_ ? (push_dfa_ ?
                push_dfa_id_ : curr_) : npos ());
            _pops[curr_].push_back (pop_);
        }
    }

    void validate (const char_type *name_, const char_type *end_ = 0) const
    {
        const char_type *start_ = name_;

        if (*name_ != '_' && !(*name_ >= 'A' && *name_ <= 'Z') &&
            !(*name_ >= 'a' && *name_ <= 'z'))
        {
            std::basic_stringstream<char_type> ss_;
            std::ostringstream os_;

            os_ << "Invalid name '";

            while (*name_)
            {
                os_ << ss_.narrow (*name_++, ' ');
            }

            os_ << "'.";
            throw runtime_error (os_.str ());
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
                std::basic_stringstream<char_type> ss_;
                std::ostringstream os_;

                os_ << "Invalid name '";
                name_ = start_;

                while (*name_)
                {
                    os_ << ss_.narrow (*name_++, ' ');
                }

                os_ << "'.";
                throw runtime_error (os_.str ());
            }

            ++name_;
        }
    }

    void check_for_invalid_id (const id_type id_) const
    {
        if (id_ == _eoi)
        {
            throw runtime_error ("Cannot resuse the id for eoi.");
        }

        if (id_ == npos ())
        {
            throw runtime_error ("id npos is reserved for the "
                "UNKNOWN token.");
        }
    }
};

typedef basic_rules<char> rules;
typedef basic_rules<wchar_t> wrules;
}

#endif
