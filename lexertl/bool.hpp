// bool.hpp
// Copyright (c) 2010-2018 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LEXERTL_BOOL_HPP
#define LEXERTL_BOOL_HPP

namespace lexertl
{
// Named template param for compiler compatibility
template<bool b>
struct bool_
{
};

typedef bool_<true> true_;
typedef bool_<false> false_;
}

#endif
