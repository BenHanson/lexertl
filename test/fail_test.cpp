#include "stdafx.h"

#include "../lexertl/generator.hpp"

const char *error_regexes_ [] =
{
	"",
	"{UNKNOWN}",
	"\"a",
	"(",
	")",
	"()",
	"(aa",
	"aa)",
	"\\()",
	"(\\)",
	"((a)))",
	"(*)",
	"a|*",
	"(+)",
	"+a",
	"a|+",
	"(?)",
	"?a",
	"a|?",
	"{",
	"a{3",
	"a{3,",
	"a{3,a",
	"a{3,5",
	"a{0}",
	"a{0,0}",
	"a{3,1}",
	"({1,2})",
	"a|{1,2}",
	"a{}",
	"a{",
	"a{1",
	"a{1,",
	"a{1,2",
	"a{ }",
	"a{1 , 2}",
	"{1}",
	"a{b}",
	"a{1b}",
	"a{1,b}",
	"a{1,2v}",
	"a{2,1}",
	"{UNKNOWN",
	"{@INVALID}",
	"{_@INVALID}",
	"{_12345678901234567890}",
	"\\",
	"[",
	"[^",
	"[a",
	"[^a",
	"[]",
	"a[b",
	"a[",
	"a[]",
	"a[1-",
	"a[3-1]c",
	"\\c",
	"\\c1",
	"\\x",
	"\\xz",
	"[\\d-a]",
	"[a-",
	"[!-\\d]",
	"[z-a]",
	"|",
	"|c",
	"c|",
	"(|)",
	"(a|)",
	"(|a)",
	"a|",
	"a(b|c){0,0}d",
	"\\x{",
	"\\x}",
	"\\x{}",
	"\\x{yy",
	"\\x{1b",
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
