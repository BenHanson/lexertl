// http://stackoverflow.com/questions/524548/regular-expression-to-detect-semi-colon-terminated-c-for-while-loops/11725855#11725855

#include <algorithm>
#include "../../lexertl/generator.hpp"
#include <iostream>
#include "../../lexertl/lookup.hpp"

int main (int /*argc*/, char ** /*argv*/)
{
    lexertl::rules rules_;
    lexertl::state_machine sm_;

    rules_.add_state ("FW");
    rules_.add_state ("SEMI");
    rules_.add_state ("NESTED");

    rules_.add ("*", "[/][/].*|[/][*](.|\n)*?[*][/]|[\"](.|\\\")*[\"]",
        rules_.skip (), ".");
    rules_.add ("INITIAL", "for\\s*\\([^;]*;[^;]*;|while\\s*\\(",
        rules_.skip (), "FW");
    rules_.add ("FW", "\\)", rules_.skip (), "SEMI");
    rules_.add ("FW,NESTED", "\\(", ">NESTED");
    rules_.add ("NESTED", "\\)", rules_.skip (), "<");
    rules_.add ("SEMI", "\\s*;", 1, "INITIAL");
    rules_.add ("SEMI", ".|\n", rules_.skip (), "INITIAL");
    lexertl::generator::build (rules_, sm_);

    lexertl::memory_file buff_ ("main.cpp");
    const char *start_ = buff_.data ();
    const char *end_ = start_ + buff_.size ();
    lexertl::crmatch results_ (start_, end_);

    do
    {
        lexertl::lookup (sm_, results_);

        if (results_.id == 1)
        {
            std::cout << "found on line " <<
                std::count (start_, results_.end, '\n') + 1 << '\n';
        }
    } while (results_.id != sm_.eoi ());

    return 0;
}
