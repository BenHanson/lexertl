// replace.hpp
// Copyright (c) 2026 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_RANGE_HPP
#define LEXERTL_RANGE_HPP

#include <algorithm>
#include <utility>

namespace lexertl
{
template<typename iter>
struct range
{
    iter _first;
    iter _second;

    range(const iter& first, const iter& second) :
        _first(first),
        _second(second)
    {
    }

    iter& begin()
    {
        return _first;
    }

    iter& end()
    {
        return _second;
    }
};
}

#endif
