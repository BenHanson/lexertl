// http://stackoverflow.com/questions/524548/regular-expression-to-detect-semi-colon-terminated-c-for-while-loops/11725855#11725855

#include <algorithm>
#include "../../lexertl/generator.hpp"
#include <iostream>
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"

int main(int /*argc*/, char ** /*argv*/)
{
    lexertl::rules rules_;
    lexertl::state_machine sm_;

    rules_.push_state("FW");
    rules_.push_state("SEMI");
    rules_.push_state("NESTED");

    rules_.push("*", "[/][/].*|[/][*].{+}[\r\n]*?[*][/]|[\"](.|\\\")*[\"]",
        rules_.skip(), ".");
    rules_.push("INITIAL", "for\\s*\\([^;]*;[^;]*;|while\\s*\\(",
        rules_.skip(), "FW");
    rules_.push("FW", "\\)", rules_.skip(), "SEMI");
    rules_.push("FW,NESTED", "\\(", ">NESTED");
    rules_.push("NESTED", "\\)", rules_.skip(), "<");
    rules_.push("SEMI", "\\s*;", 1, "INITIAL");
    rules_.push("SEMI", ".{+}[\r\n]", rules_.skip(), "INITIAL");
    lexertl::generator::build(rules_, sm_);

    lexertl::memory_file buff_("main.cpp");
    const char *start_ = buff_.data();
    const char *end_ = start_ + buff_.size();
    lexertl::crmatch results_(start_, end_);

    do
    {
        lexertl::lookup(sm_, results_);

        if (results_.id == 1)
        {
            std::cout << "found on line " <<
                std::count(start_, results_.second, '\n') + 1 << '\n';
        }
    } while (results_.id != sm_.eoi());

    return 0;
}
