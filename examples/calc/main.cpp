#include <iostream>
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include <string>

enum calc_id {eEOF, eNegate, eValue, eOperator, eOpen, eClose};

void gen_calc(lexertl::state_machine &sm_)
{
    lexertl::rules rules_;

    rules_.add_state("VALUE");
    rules_.add_state("OPERATOR");
    rules_.add_state("SUB_INITIAL");
    rules_.add_state("SUB_VALUE");
    rules_.add_state("SUB_OPERATOR");

//    rules_.add("INITIAL", "[a-z]", "INITIAL");
    rules_.add("INITIAL", "-", eNegate, "VALUE", 100);
    rules_.add("INITIAL,VALUE", "\\d+", eValue, "OPERATOR", 101);
    rules_.add("OPERATOR", "[-+*/%]", eOperator, "INITIAL", 102);

    rules_.add("INITIAL,VALUE", "\\(", eOpen, ">SUB_INITIAL:OPERATOR");
    rules_.add("SUB_INITIAL,SUB_VALUE", "\\(", eOpen,
        ">SUB_INITIAL:SUB_OPERATOR", 103);

    rules_.add("SUB_INITIAL", "-", eNegate, "SUB_VALUE", 104);
    rules_.add("SUB_INITIAL,SUB_VALUE", "\\d+", eValue, "SUB_OPERATOR", 105);
    rules_.add("SUB_OPERATOR", "[-+*/%]", eOperator, "SUB_INITIAL", 106);
    rules_.add("SUB_OPERATOR", "\\)", eClose, "<", 107);

    rules_.add("*", "\\s+", sm_.skip(), ".", 108);

    lexertl::generator::build(rules_, sm_);
//    lexertl::debug::dump(sm_, std::cout);
//    lexertl::table_based_cpp::generate_cpp("lookup", sm_, false, std::cout);
}

void reduce (std::stack<int> &operands_,
    std::stack<char> &ops_)
{
    if (operands_.empty() || ops_.empty()) return;

    char op_ = ops_.top();
    int rhs_ = 0;

    switch (op_)
    {
    case '~':
        ops_.pop();
        operands_.top() *= -1;
        break;
    case '+':
        ops_.pop();
        rhs_ = operands_.top();
        operands_.pop();
        operands_.top () += rhs_;
        break;
    case '-':
        ops_.pop();
        rhs_ = operands_.top();
        operands_.pop();
        operands_.top () -= rhs_;
        break;
    case '*':
        ops_.pop();
        rhs_ = operands_.top();
        operands_.pop();
        operands_.top () *= rhs_;
        break;
    case '/':
        ops_.pop();
        rhs_ = operands_.top();
        operands_.pop();
        operands_.top () /= rhs_;
        break;
    case '%':
        ops_.pop();
        rhs_ = operands_.top();
        operands_.pop();
        operands_.top () %= rhs_;
        break;
    case '(':
        break;
    case ')':
        ops_.pop();

        while (!ops_.empty() && op_ != '(')
        {
            op_ = ops_.top ();
            reduce (operands_, ops_);
        }

        if (op_ == '(') ops_.pop();

        break;
    }
}

void calc(const std::string &input_,
    const lexertl::state_machine &sm_, int &output_)
{
    lexertl::recursive_match_results<const char *> results_
        (input_.c_str(), input_.c_str() + input_.size());
    int value_ = 0;
    std::stack<int> operands_;
    std::stack<char> ops_;

    do
    {
        lexertl::lookup (sm_, results_);
//        lookup (results_);

        if (results_.id == results_.npos())
        {
            throw std::runtime_error("Syntax error");
        }

        if (results_.id == eOperator)
        {
            switch (*results_.start)
            {
            case '~':
            case '(':
                break;
            case '+':
            case '-':
                reduce(operands_, ops_);
                break;
            default:
                if (!ops_.empty())
                {
                    const char op_ = ops_.top();

                    if (op_ != '~' && op_ != '+' &&
                        op_ != '-' && op_ != '(')
                    {
                        reduce(operands_, ops_);
                    }
                }

                break;
            }

            ops_.push(*results_.start);
        }
        else if (results_.id == eNegate)
        {
            ops_.push('~');
        }
        else if (results_.id == eValue)
        {
            std::string val_(results_.start, results_.end);

            value_ = atoi(val_.c_str());
            operands_.push(value_);
        }
        else if (results_.id == eOpen)
        {
            ops_.push('(');
        }
        else if (results_.id == eClose)
        {
            ops_.push(')');
            reduce(operands_, ops_);
/*
            if (results_.state == 0 || results_.state == 1)
            {
                results_.state = 2;
            }
            else
            {
                results_.state = 5;
            }
*/
        }
    } while (results_.id != 0);

    if (results_.state != 2)
    {
        throw std::runtime_error("Unterminated expression!");
    }

    while (!ops_.empty())
    {
        reduce(operands_, ops_);
    }

    if (operands_.size() != 1)
    {
        throw std::runtime_error("internal error: operands_.size() != 1");
    }

    output_ = operands_.top();
}

int main(int /*argc*/, char** /*argv*/)
{
    lexertl::state_machine sm_;
    std::string input_;
    int output_ = 0;

    gen_calc(sm_);

    do
    {
        try
        {
            std::cout << "Input: " << std::flush;
            std::getline(std::cin, input_);

            if (input_ == "q" || input_ == "Q")
                break;

            std::cout << input_ << " = ";
            calc(input_, sm_, output_);
            std::cout << output_ << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    } while (input_ != "q" && input_ != "Q");

    return 0;
}
