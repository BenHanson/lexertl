#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"

int main (int /*argc*/, char ** /*argv*/)
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
    std::string input_("(*** Get the time since (*nested*) the program started. ***)\n"
        "let time =\n"
        "let t = Unix.gettimeofday() in\n"
        "fun () -> Unix.gettimeofday() -");
    lexertl::srmatch results_(input_.begin(), input_.end());

    rules_.add_state("COMMENT");
    rules_.add_macro("ESCAPE_SEQUENCE",
        "([\\\"'ntbr]|[0-9]{3}|x[0-9A-Fa-f]{2})");
    rules_.add_macro("OPERATOR_CHAR",
        "[-!$%&*+./:<=>?@^|~]");
    rules_.add("[A-Za-z_][A-Za-z_0-9']*", eIdentifier);
    rules_.add("-?([0-9][0-9+]*|"
        "0([xX][0-9A-Fa-f][0-9A-Fa-f_]*|"
        "[oO][0-7][0-7_]*|"
        "[bB][01][01_]*))", eInteger);
    rules_.add("-?[0-9][0-9_]*(\\.[0-9-])?([eE][-+]?[0-9][0-9_]*)?", eFloat);
    rules_.add("'[^\\']|{ESCAPE_SEQUENCE}'", eChar);
    rules_.add("\"([^\\\"]|{ESCAPE_SEQUENCE})*\"", eString);
    rules_.add("[-=<>@^|&+*/$%]{OPERATOR_CHAR}*", eInfixSymbol);
    rules_.add("[!?~]{OPERATOR_CHAR}*", ePrefixSymbol);
    rules_.add("{OPERATOR_CHAR}", eOperatorChar);
    rules_.add("#\\d+(\"([^\\\"]|{ESCAPE_SEQUENCE})*\")?", eLineNumDir);

    rules_.add("INITIAL,COMMENT", "\\(\\*", ">COMMENT");
    rules_.add("COMMENT", "\\*\\)", eComment, "<");
    rules_.add("COMMENT", "[^(*]+|\\(|\\*", "COMMENT");
    rules_.add("[ \t\n]+", rules_.skip());
    lexertl::generator::build(rules_, sm_);

    do
    {
       lexertl::lookup (sm_, results_);
       std::cout << "Id: ";

       if (results_.id != results_.npos())
       {
           std::cout << Tokens[results_.id];
       }
       else
       {
           std::cout << "Unknown token";
       }

       std::cout << ", Token: '" <<
            std::string (results_.start, results_.end) <<
            '\'' << std::endl;
    } while (results_.id != 0);

    results_.clear();
    return 0;
}
