// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "fail_tests.h"
#include "../lexertl/parser/tokeniser/re_tokeniser.hpp"

void charsets()
{
    typedef lexertl::detail::basic_re_tokeniser_state<char, std::size_t> state;
    typedef lexertl::detail::basic_re_tokeniser
        <char, char, std::size_t> tokeniser;
    std::string str_("[[:xdigit:]]");
    state state_(str_.c_str(), str_.c_str() + str_.size(), 1, 0, std::locale(""), 0);
    lexertl::detail::basic_re_token<char, char> lhs_;
    lexertl::detail::basic_re_token<char, char> token_;

    tokeniser::next(lhs_, state_, token_);
}

int main(int argc, char *argv[])
{
    try
    {
        if (fail_tests() != 0)
        {
            throw std::runtime_error("One of the failure regexes did not fail.");
        }

    //    charsets();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
