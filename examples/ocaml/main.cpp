#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"

int main(int /*argc*/, char ** /*argv*/)
{
    lexertl::rules rules_;
    lexertl::state_machine sm_;
    enum {eIdentifier = 1, eInteger, eFloat, eChar, eString,
          eLabel, eOptLabel, eInfixSymbol, ePrefixSymbol,
          eOperatorChar, eLineNumDir, eComment};
    const char *Tokens[] = {"EOF", "Identifier", "Integer",
        "Float", "Char", "String", "Label", "Optional Label",
        "Infix Symbol", "Prefix Symbol", "Operator Char",
        "Line Number Directive", "Comment"};
    std::string input_("(*** Get the time since(*nested*) the program started. ***)\n"
        "let time =\n"
        "let t = Unix.gettimeofday() in\n"
        "fun() -> Unix.gettimeofday() -");
    lexertl::srmatch results_(input_.begin(), input_.end());

    rules_.push_state("COMMENT");
    rules_.insert_macro("ESCAPE_SEQUENCE",
        "([\\\"'ntbr]|[0-9]{3}|x[0-9A-Fa-f]{2})");
    rules_.insert_macro("OPERATOR_CHAR",
        "[-!$%&*+./:<=>?@^|~]");
    rules_.push("[A-Za-z_][A-Za-z_0-9']*", eIdentifier);
    rules_.push("-?([0-9][0-9+]*|"
        "0([xX][0-9A-Fa-f][0-9A-Fa-f_]*|"
        "[oO][0-7][0-7_]*|"
        "[bB][01][01_]*))", eInteger);
    rules_.push("-?[0-9][0-9_]*(\\.[0-9-])?([eE][-+]?[0-9][0-9_]*)?", eFloat);
    rules_.push("'[^\\']|{ESCAPE_SEQUENCE}'", eChar);
    rules_.push("\"([^\\\"]|{ESCAPE_SEQUENCE})*\"", eString);
    rules_.push("[-=<>@^|&+*/$%]{OPERATOR_CHAR}*", eInfixSymbol);
    rules_.push("[!?~]{OPERATOR_CHAR}*", ePrefixSymbol);
    rules_.push("{OPERATOR_CHAR}", eOperatorChar);
    rules_.push("#\\d+(\"([^\\\"]|{ESCAPE_SEQUENCE})*\")?", eLineNumDir);

    rules_.push("INITIAL,COMMENT", "\\(\\*", ">COMMENT");
    rules_.push("COMMENT", "\\*\\)", eComment, "<");
    rules_.push("COMMENT", "[^(*]+|\\(|\\*", "COMMENT");
    rules_.push("[ \t\n]+", rules_.skip());
    lexertl::generator::build(rules_, sm_);

    do
    {
       lexertl::lookup(sm_, results_);
       std::cout << "Id: ";

       if (results_.id != results_.npos())
       {
           std::cout << Tokens[results_.id];
       }
       else
       {
           std::cout << "Unknown token";
       }

       std::cout << ", Token: '" << results_.str() << '\'' << std::endl;
    } while (results_.id != 0);

    results_.clear();
    return 0;
}
