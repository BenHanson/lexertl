// serialise.hpp
// Copyright (c) 2007-2023 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_SERIALISE_HPP
#define LEXERTL_SERIALISE_HPP

#include "enums.hpp"
#include "internals.hpp"
#include "runtime_error.hpp"
#include "state_machine.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace lexertl
{
    namespace detail
    {
        const int serialise_version = 1;

        template<typename id_type>
        std::size_t decimal_digits(id_type value_)
        {
            std::size_t digits_ = 1;

            for (; value_ >= 10; value_ /= 10)
                ++digits_;

            return digits_;
        }

        // The numbers are written straight to the stream, so the stream's
        // character type is irrelevant (char_type is only kept for source
        // compatibility). The output format is unchanged.
        template<typename char_type, typename id_type, class stream>
        void output_vec(const std::vector<id_type>& vec_, stream& stream_)
        {
            std::size_t line_len_ = 0;

            stream_ << vec_.size() << '\n';

            for (typename std::vector<id_type>::const_iterator iter_ =
                vec_.begin(), end_ = vec_.end();
                iter_ != end_; ++iter_)
            {
                const std::size_t len_ = decimal_digits(*iter_);

                if (line_len_ + len_ + 1 > 80)
                {
                    stream_ << '\n';
                    line_len_ = 0;
                }

                stream_ << static_cast<std::size_t>(*iter_) << ' ';
                line_len_ += len_ + 1;
            }

            stream_ << '\n';
        }

        // Reads one non-negative number. Anything else (truncated stream,
        // garbage, a minus sign, a value above max_) is an error.
        // Note that extracting straight into an unsigned type would quietly
        // accept "-1" and wrap.
        template<class stream>
        std::size_t read_num(stream& stream_, const std::size_t max_,
            const char* what_)
        {
            long long num_ = -1;

            stream_ >> num_;

            if (stream_.fail() || num_ < 0 ||
                static_cast<unsigned long long>(num_) > max_)
            {
                throw runtime_error(std::string("Invalid or truncated ") +
                    what_ + " in lexertl::load().");
            }

            return static_cast<std::size_t>(num_);
        }

        template<typename char_type, class stream, typename id_type>
        void input_vec(stream& stream_, std::vector<id_type>& vec_)
        {
            const std::size_t num_ = read_num(stream_,
                std::numeric_limits<std::size_t>::max(), "vector size");

            vec_.clear();
            // Never size an allocation from an untrusted count. A bogus
            // count simply runs into the end of the stream.
            vec_.reserve(std::min<std::size_t>(num_, 1 << 16));

            for (std::size_t idx_ = 0; idx_ < num_; ++idx_)
            {
                vec_.push_back(static_cast<id_type>(read_num(stream_,
                    std::numeric_limits<id_type>::max(), "value")));
            }
        }

        static void error(const char* msg_)
        {
            throw runtime_error(std::string("Corrupt state machine in "
                "lexertl::load(): ") + msg_ + '.');
        }

        // Checks everything lookup() relies on, so that a state machine that
        // has been loaded cannot be walked out of bounds.
        template<typename id_type>
        void validate(const basic_internals<id_type>& internals_)
        {
            const std::size_t end_state_ =
                static_cast<std::size_t>(end_state_index);
            const std::size_t push_dfa_ =
                static_cast<std::size_t>(push_dfa_index);
            const std::size_t next_dfa_ =
                static_cast<std::size_t>(next_dfa_index);
            const std::size_t eol_ =
                static_cast<std::size_t>(eol_index);
            const std::size_t trans_ =
                static_cast<std::size_t>(transitions_index);
            const std::size_t pop_bit_ =
                static_cast<std::size_t>(pop_dfa_bit);
            const std::size_t npos_ = std::numeric_limits<id_type>::max();
            const std::size_t dfas_ = internals_._dfa->size();

            if (internals_._lookup->size() != dfas_ ||
                internals_._dfa_alphabet.size() != dfas_)
            {
                error("inconsistent number of states");
            }

            for (std::size_t i_ = 0; i_ < dfas_; ++i_)
            {
                typedef basic_internals<id_type> internals;
                const typename internals::id_type_vector& lookup_ =
                    *(*internals_._lookup)[i_];
                const typename internals::id_type_vector& dfa_ =
                    *(*internals_._dfa)[i_];
                const std::size_t alphabet_ = internals_._dfa_alphabet[i_];

                if (lookup_.size() != 256)
                    error("bad lookup table size");

                // The generator does not allow states without rules
                if (dfa_.empty())
                    error("state without rules");

                // jam state + at least one real state
                if (alphabet_ < trans_ || dfa_.size() % alphabet_ != 0 ||
                    dfa_.size() / alphabet_ < 2)
                {
                    error("bad dfa dimensions");
                }

                const std::size_t rows_ = dfa_.size() / alphabet_;

                for (typename internals::id_type_vector::const_iterator iter_ =
                    lookup_.begin(), end_ = lookup_.end();
                    iter_ != end_; ++iter_)
                {
                    if (*iter_ >= alphabet_)
                        error("lookup entry out of range");
                }

                // BOL start state is stored in the jam state
                if (dfa_[0] >= rows_)
                    error("bad BOL state");

                for (std::size_t row_ = 1; row_ < rows_; ++row_)
                {
                    const id_type* ptr_ = &dfa_[row_ * alphabet_];

                    if (ptr_[eol_] >= rows_)
                        error("bad EOL state");

                    for (std::size_t col_ = trans_; col_ < alphabet_; ++col_)
                    {
                        if (ptr_[col_] >= rows_)
                            error("bad transition");
                    }

                    // These are only ever read from end states
                    if (ptr_[end_state_] != 0)
                    {
                        // A pop takes the next state from the stack, so the
                        // generator stores npos as the next state of a pop.
                        const bool pop_ = (ptr_[end_state_] & pop_bit_) != 0;

                        if (!(pop_ && ptr_[next_dfa_] == npos_) &&
                            ptr_[next_dfa_] >= dfas_)
                        {
                            error("bad next state");
                        }

                        if (ptr_[push_dfa_] != npos_ &&
                            ptr_[push_dfa_] >= dfas_)
                        {
                            error("bad push state");
                        }
                    }
                }
            }
        }
    }

    template<typename char_type, typename id_type, class stream>
    void save(const basic_state_machine<char_type, id_type>& sm_,
        stream& stream_)
    {
        typedef detail::basic_internals<id_type> internals;
        const internals& internals_ = sm_.data();

        // Version number
        stream_ << detail::serialise_version << '\n';
        stream_ << sizeof(char_type) << '\n';
        stream_ << sizeof(id_type) << '\n';
        stream_ << static_cast<std::size_t>(internals_._eoi) << '\n';
        stream_ << internals_._lookup->size() << '\n';

        for (typename internals::id_type_vector_vector::
            vector::const_iterator iter_ = internals_._lookup->begin(),
            end_ = internals_._lookup->end(); iter_ != end_; ++iter_)
        {
            detail::output_vec<char_type>(**iter_, stream_);
        }

        detail::output_vec<char_type>(internals_._dfa_alphabet, stream_);
        stream_ << static_cast<std::size_t>(internals_._features) << '\n';
        stream_ << internals_._dfa->size() << '\n';

        for (typename internals::id_type_vector_vector::
            vector::const_iterator iter_ = internals_._dfa->begin(),
            end_ = internals_._dfa->end(); iter_ != end_; ++iter_)
        {
            detail::output_vec<char_type>(**iter_, stream_);
        }

        if (stream_.fail())
            throw runtime_error("Failed to write the stream in lexertl::save().");
    }

    // On failure an exception is thrown and sm_ is left unchanged.
    template<typename char_type, typename id_type, class stream>
    void load(stream& stream_, basic_state_machine<char_type, id_type>& sm_)
    {
        typedef detail::basic_internals<id_type> internals;
        const std::size_t max_id_ = std::numeric_limits<id_type>::max();
        internals internals_;

        if (detail::read_num(stream_, max_id_, "version") !=
            static_cast<std::size_t>(detail::serialise_version))
        {
            throw runtime_error("Unsupported version in lexertl::load().");
        }

        if (detail::read_num(stream_, max_id_, "char_type size") !=
            sizeof(char_type))
        {
            throw runtime_error("char_type mismatch in lexertl::load().");
        }

        if (detail::read_num(stream_, max_id_, "id_type size") !=
            sizeof(id_type))
        {
            throw runtime_error("id_type mismatch in lexertl::load().");
        }

        internals_._eoi = static_cast<id_type>
            (detail::read_num(stream_, max_id_, "eoi"));

        // Ids of states are id_type, so there cannot be more than max_id_
        std::size_t num_ = detail::read_num(stream_, max_id_, "state count");

        internals_._lookup->reserve(std::min<std::size_t>(num_, 1 << 12));

        for (std::size_t idx_ = 0; idx_ < num_; ++idx_)
        {
            internals_._lookup->
                push_back(static_cast<typename internals::id_type_vector*>(0));
            internals_._lookup->back() = new typename internals::id_type_vector;
            detail::input_vec<char_type>(stream_, *internals_._lookup->back());
        }

        detail::input_vec<char_type>(stream_, internals_._dfa_alphabet);
        internals_._features = static_cast<id_type>
            (detail::read_num(stream_, max_id_, "features"));
        num_ = detail::read_num(stream_, max_id_, "dfa count");
        internals_._dfa->reserve(std::min<std::size_t>(num_, 1 << 12));

        for (std::size_t idx_ = 0; idx_ < num_; ++idx_)
        {
            internals_._dfa->
                push_back(static_cast<typename internals::id_type_vector*>(0));
            internals_._dfa->back() = new typename internals::id_type_vector;
            detail::input_vec<char_type>(stream_, *internals_._dfa->back());
        }

        detail::validate(internals_);
        internals_.swap(sm_.data());
    }
}

#endif
