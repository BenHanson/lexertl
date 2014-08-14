#ifndef REV_REGEX_H
#define REV_REGEX_H

#include "../lexertl/state_machine.hpp"
#include <string>

void build_rev_regex_lexer(lexertl::state_machine &sm_);
std::string rev_regex(const std::string &rx_, const lexertl::state_machine &sm_);

#endif
