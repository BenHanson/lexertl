#include "stdafx.h"

#include "../lexertl/generator.hpp"

const char *error_regexes_[] =
{
    // re_tokeniser.hpp
    "\"",
    "(a",
    "(a))",
    "(?#",
    "(?x:/*",
    "(?x: ",
    "(?z:",
    "{",
    "{-",
    "{+",
    "a{-",
    "a{-a",
    "a{1",
    "a{1,",
    "a{1,a",
    "a{1,1",
    "a{1,1a",
    "a{0}",
    "a{1,0}",
    "{@",
    "{a",
    "{a@",
    // re_tokeniser_helper.hpp
    "\\",
    "[",
    "[^",
    "[a",
    "[]",
    "[[:z",
    "[[:",
    "[[:a",
    "[[:alpha",
    "[[:b",
    "[[:blank",
    "[[:c",
    "[[:cntrl",
    "[[:d",
    "[[:digit",
    "[[:g",
    "[[:graph",
    "[[:l",
    "[[:lower",
    "[[:p",
    "[[:print",
    "[[:s",
    "[[:space",
    "[[:u",
    "[[:upper",
    "[[:x",
    "[[:xdigit",
    "[[:xdigita",
    "[[:xdigit:",
    "[[:xdigit:a",
    "\\p",
    "\\pa",
    "\\p{",
    "\\p{C",
    "\\p{Ca",
    "\\p{L",
    "\\p{La",
    "\\p{M",
    "\\p{Ma",
    "\\p{N",
    "\\p{Na",
    "\\p{P",
    "\\p{Pa",
    "\\p{S",
    "\\p{Sa",
    "\\p{Z",
    "\\p{Za",
    "\\p{x",
    "\\p{Zs",
    "\\777",
    "\\c",
    "\\c:",
    "\\x",
    "\\xG",
    "\\xfff",
    "[\\d-z]",
    "[a-",
    "[a-\\d]",
    "[a-[:xdigit:]]",
    "[a-d",
    "[d-a]",
    "/",
    0
};

const char *error_msgs_ [] =
{
    "Unexpected end of regex (missing '\"') in rule id 1.",
    "Unexpected end of regex (missing ')') in rule id 1.",
    "Number of open parenthesis < 0 at index 3 in rule id 1.",
    "Unexpected end of regex (unterminated comment) in rule id 1.",
    "Unexpected end of regex (unterminated C style comment) in rule id 1.",
    "Unexpected end of regex (missing ')') in rule id 1.",
    "Unknown option at index 2 in rule id 1.",
    "Unexpected end of regex (missing '}') in rule id 1.",
    "CHARSET must precede {-} at index 0 in rule id 1.",
    "CHARSET must precede {+} at index 0 in rule id 1.",
    "Unexpected end of regex (missing '}') in rule id 1.",
    "Missing '}' at index 3 in rule id 1.",
    "Unexpected end of regex (missing repeat terminator '}') in rule id 1.",
    "Unexpected end of regex (missing repeat terminator '}') in rule id 1.",
    "Missing repeat terminator '}' at index 4 in rule id 1.",
    "Unexpected end of regex (missing repeat terminator '}') in rule id 1.",
    "Missing repeat terminator '}' at index 5 in rule id 1.",
    "Cannot have exactly zero repeats preceding index 4 in rule id 1.",
    "Max less than min preceding index 6 in rule id 1.",
    "Invalid MACRO name at index 1 in rule id 1.",
    "Unexpected end of regex (missing MACRO name terminator '}') in rule id 1.",
    "Missing MACRO name terminator '}' at index 2 in rule id 1.",
    "Unexpected end of regex following '\\' in rule id 1.",
    "Unexpected end of regex following '[' in rule id 1.",
    "Unexpected end of regex following '^' in rule id 1.",
    "Unexpected end of regex (missing ']') in rule id 1.",
    "Empty charset not allowed preceding index 2 in rule id 1.",
    "Unknown POSIX charset at index 3 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Unknown POSIX charset at index 4 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Missing ':' at index 9 in rule id 1.",
    "Unexpected end of regex (unterminated POSIX charset) in rule id 1.",
    "Missing ']' at index 10 in rule id 1.",
    "Unexpected end of regex following \\p in rule id 1.",
    "Missing '{' following \\p at index 2 in rule id 1.",
    "Unexpected end of regex following \\p{ in rule id 1.",
    "Unexpected end of regex following \\p{C in rule id 1.",
    "Syntax error following \\p{C at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{L in rule id 1.",
    "Syntax error following \\p{L at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{M in rule id 1.",
    "Syntax error following \\p{M at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{N in rule id 1.",
    "Syntax error following \\p{N at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{P in rule id 1.",
    "Syntax error following \\p{P at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{S in rule id 1.",
    "Syntax error following \\p{S at index 4 in rule id 1.",
    "Unexpected end of regex following \\p{Z in rule id 1.",
    "Syntax error following \\p{Z at index 4 in rule id 1.",
    "Syntax error following \\p{ at index 3 in rule id 1.",
    "Missing '}' at index 5 in rule id 1.",
    "Escape \\777 is too big for the state machine char type preceding index "
        "4 in rule id 1.",
    "Unexpected end of regex following \\c in rule id 1.",
    "Invalid control char at index 2 in rule id 1.",
    "Unexpected end of regex following \\x in rule id 1.",
    "Illegal char following \\x at index 2 in rule id 1.",
    "Escape \\xfff is too big for the state machine char type preceding index "
        "5 in rule id 1.",
    "Charset cannot form start of range preceding index 3 in rule id 1.",
    "Unexpected end of regex following '-' in rule id 1.",
    "Charset cannot form end of range preceding index 5 in rule id 1.",
    "POSIX char class cannot form end of range at index 3 in rule id 1.",
    "Unexpected end of regex (missing ']') in rule id 1.",
    "Max less than Min in charset range preceding index 4 in rule id 1.",
    "Lookahead ('/') is not supported yet in rule id 1.",
    ""
};

int fail_tests()
{
    int ret = 0;
    const char * *regex_ptr_ = error_regexes_;
    const char * *error_ptr_ = error_msgs_;
    lexertl::rules rules_;
    lexertl::state_machine sm_;

    while (*regex_ptr_)
    {
        try
        {
            std::cout << "regex: " << *regex_ptr_ << std::endl;
            rules_.push(*regex_ptr_, 1);
            // Test should throw exception...
            std::cout << "MISSING ERROR MSG!" << std::endl;
            ret = -1;
        }
        catch (const lexertl::runtime_error &e)
        {
            // success
            if (strcmp(e.what(), *error_ptr_) != 0)
            {
                std::cout << "INVALID ERROR MSG!" << std::endl;
            }
            else
            {
                std::cout << "Valid error msg." << std::endl;
            }
        }

        ++regex_ptr_;
        ++error_ptr_;
        rules_.clear();
    }

    return ret;
}
