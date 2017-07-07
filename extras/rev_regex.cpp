#include "stdafx.h"

//#include "../lexertl/debug.hpp"
#include "../lexertl/generator.hpp"
#include <iostream>
#include "../lexertl/iterator.hpp"

enum eToken {eEOF, eCharset, eComment, eString, eQuantifier, eMacro,
    eDifference, eBOL, eEOL, eOr, eOpen, eClose};

typedef std::pair<eToken, std::string> token_pair;
typedef std::stack<token_pair> token_stack;

// lexertl syntax:
void build_rev_regex_lexer(lexertl::state_machine &sm_)
{
    lexertl::rules rules_;

    rules_.insert_macro("posix_name", "alnum|alpha|blank|cntrl|digit|graph|"
        "lower|print|punct|space|upper|xdigit");
    rules_.insert_macro("posix", "\\[:{posix_name}:\\]");
    rules_.insert_macro("escape", "\\\\([^xc]|x\\d+|\\d{3}|c[@a-zA-Z]|"
        "p[{](C[cfos]?|L[Clmotu]?|M[cen]?|N[dlo]?|P[cdefios]?|S[ckmo]?|"
        "Z[lps]?)[}])");

    rules_.push("{escape}|{posix}", eCharset);
    rules_.push("\\[^?({escape}|{posix}|[^\\\\\\]])*\\]", eCharset);
    rules_.push("[(][?]#[^)]*[)]", eComment);
    rules_.push("[\"]({escape}|[^\"])*[\"]", eString);
    rules_.push("\\^", eBOL);
    rules_.push("[$]", eEOL);
    rules_.push("[|]", eOr);
    rules_.push("[*+?][?]?", eQuantifier);
    rules_.push("[{][0-9]+(,([0-9]+)?)?[}][?]?", eQuantifier);
    rules_.push("[{][A-Za-z_][A-Za-z0-9_]*[}]", eMacro);
    rules_.push("[{][-+][}]", eDifference);
    rules_.push("[(]([?](-?[is])*:)?", eOpen);
    rules_.push("[)]", eClose);
    rules_.push(".{+}[\r\n]", eCharset);

    lexertl::generator::build(rules_, sm_);
    //    lexertl::debug::dump(g_sm, std::cout);
}

void reduce_stack(token_stack &stack_)
{
    std::string str_;

    for (; !stack_.empty() && (stack_.top().first == eCharset ||
        stack_.top().first == eString || stack_.top().first == eComment ||
        stack_.top().first == eMacro || stack_.top().first == eOr);
        stack_.pop())
    {
        str_ += stack_.top().second;
    }

    stack_.push(token_pair(eCharset, str_));
}

std::string rev_regex(const std::string &rx_, const lexertl::state_machine &sm_)
{
    std::string curr_;
    token_stack stack_;
    lexertl::citerator iter_(rx_.c_str(), rx_.c_str() + rx_.length(), sm_);
    lexertl::citerator end_;
    std::size_t parens_ = 0;

    for (; iter_ != end_; ++iter_)
    {
        switch (iter_->id)
        {
            case eCharset:
            case eMacro:
            case eComment:
                stack_.push(token_pair((eToken)iter_->id, iter_->str()));
                break;
            case eString:
            {
                lexertl::citerator i_(iter_->first + 1, iter_->second - 1, sm_);
                lexertl::citerator e_;
                std::string s_;

                for (; i_ != e_; ++i_)
                {
                    s_ = i_->str() + s_;
                }

                s_ = '"' + s_ + '"';
                stack_.push(token_pair((eToken)iter_->id, s_));
                break;
            }
            case eQuantifier:
                if (stack_.empty())
                {
                    throw std::runtime_error("Quantifier without regex.");
                }

                stack_.top().second += iter_->str();
                break;
            case eDifference:
                if (stack_.empty())
                {
                    throw std::runtime_error("Difference without regex.");
                }

                stack_.top().second += iter_->str();
                ++iter_;
                stack_.top().second += iter_->str();
                break;
            case eBOL:
                // Reverse to eEOL
                stack_.push(token_pair(eEOL, std::string(1, '$')));
                break;
            case eEOL:
                // Reverse to eBOL
                stack_.push(token_pair(eBOL, std::string(1, '^')));
                break;
            case eOr:
                reduce_stack(stack_);
                stack_.push(token_pair((eToken)iter_->id, iter_->str()));
                break;
            case eOpen:
                stack_.push(token_pair((eToken)iter_->id, iter_->str()));
                ++parens_;
                break;
            case eClose:
            {
                if (parens_ < 1)
                {
                    throw std::runtime_error("Unbalanced parens in regex.");
                }

                std::string s_;

                reduce_stack(stack_);
                s_ = stack_.top().second;
                s_ += iter_->str();
                stack_.pop();
                stack_.top().second += s_;
                stack_.top().first = eCharset;
                --parens_;
                break;
            }
            default:
                throw std::runtime_error("Unknown regex token.");
                break;
        }
    }

    std::string str_;

    for (; !stack_.empty(); stack_.pop())
    {
        str_ += stack_.top().second;
    }

    return str_;
}
