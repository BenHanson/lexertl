#include "stdafx.h"

#include "../lexertl/generator.hpp"

const char *error_regexes_ [] =
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

int fail_tests()
{
	const char * *ptr_ = error_regexes_;
	lexertl::rules rules_;
	lexertl::state_machine sm_;

	while (*ptr_)
	{
		try
		{
			std::cout << "regex: " << *ptr_ << std::endl;
			rules_.push(*ptr_, 1);
			// Test should throw exception...
			return -1;
		}
		catch (const lexertl::runtime_error &e)
		{
			// success
			std::cout << e.what() << std::endl;
		}

		++ptr_;
		rules_.clear();
	}

	return 0;
}
