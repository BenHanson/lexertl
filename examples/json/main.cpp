#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"
#include "../../lexertl/utf_iterators.hpp"

int main()
{
    try
    {
        typedef lexertl::basic_rules<char, char32_t> urules;
        typedef lexertl::basic_state_machine<char32_t> usm;
        typedef lexertl::basic_utf8_in_iterator<const char *, char32_t>
            utf_in_iter;
        typedef lexertl::basic_utf8_out_iterator<utf_in_iter>
            utf_out_iter;
        urules rules;
        usm sm;
        lexertl::memory_file file("C:\\json.txt");
        const char *begin = file.data();
        const char *end = begin + file.size();
        lexertl::recursive_match_results<utf_in_iter> results(utf_in_iter(begin, end),
            utf_in_iter(end, end));
        enum {eEOF, eString, eNumber, eBoolean, eOpenOb, eName, eCloseOb,
            eOpenArr, eCloseArr, eNull};

        // http://code.google.com/p/bsn-goldparser/wiki/JsonGrammar
        rules.insert_macro("STRING", "[\"]([ -\\x10ffff]{-}[\"\\\\]|"
            "\\\\([\"\\\\/bfnrt]|u[0-9a-fA-F]{4}))*[\"]");
        rules.insert_macro("NUMBER", "-?(0|[1-9]\\d*)([.]\\d+)?([eE][-+]?\\d+)?");
        rules.insert_macro("BOOL", "true|false");
        rules.insert_macro("NULL", "null");

        rules.push_state("END");

        rules.push_state("OBJECT");
        rules.push_state("NAME");
        rules.push_state("COLON");
        rules.push_state("OB_VALUE");
        rules.push_state("OB_COMMA");

        rules.push_state("ARRAY");
        rules.push_state("ARR_COMMA");
        rules.push_state("ARR_VALUE");

        rules.push("INITIAL", "[{]", eOpenOb, ">OBJECT:END");
        rules.push("INITIAL", "[[]", eOpenArr, ">ARRAY:END");

        rules.push("OBJECT,OB_COMMA", "[}]", eCloseOb, "<");
        rules.push("OBJECT,NAME", "{STRING}", eName, "COLON");
        rules.push("COLON", ":", rules.skip(), "OB_VALUE");

        rules.push("OB_VALUE", "{STRING}", eString, "OB_COMMA");
        rules.push("OB_VALUE", "{NUMBER}", eNumber, "OB_COMMA");
        rules.push("OB_VALUE", "{BOOL}", eBoolean, "OB_COMMA");
        rules.push("OB_VALUE", "{NULL}", eNull, "OB_COMMA");
        rules.push("OB_VALUE", "[{]", eOpenOb, ">OBJECT:OB_COMMA");
        rules.push("OB_VALUE", "[[]", eOpenArr, ">ARRAY:OB_COMMA");

        rules.push("OB_COMMA", ",", rules.skip(), "NAME");

        rules.push("ARRAY,ARR_COMMA", "\\]", eCloseArr, "<");
        rules.push("ARRAY,ARR_VALUE", "{STRING}", eString, "ARR_COMMA");
        rules.push("ARRAY,ARR_VALUE", "{NUMBER}", eNumber, "ARR_COMMA");
        rules.push("ARRAY,ARR_VALUE", "{BOOL}", eBoolean, "ARR_COMMA");
        rules.push("ARRAY,ARR_VALUE", "{NULL}", eNull, "ARR_COMMA");
        rules.push("ARRAY,ARR_VALUE", "[{]", eOpenOb, ">OBJECT:ARR_COMMA");
        rules.push("ARRAY,ARR_VALUE", "[[]", eOpenArr, ">ARRAY:ARR_COMMA");

        rules.push("ARR_COMMA", ",", rules.skip(), "ARR_VALUE");

        rules.push("*", "[ \t\r\n]+", rules.skip(), ".");

        lexertl::basic_generator<urules, usm>::build(rules, sm);
        // Read-ahead
        lexertl::lookup(sm, results);

        while (results.id != eEOF)
        {
            std::cout << "Id: " << results.id << " token: ";

            if (results.id == eString)
            {
                std::cout << std::string(utf_out_iter(results.first + 1, results.second - 1),
                    utf_out_iter(results.second - 1, results.second - 1));
            }
            else
            {
                std::cout << std::string(utf_out_iter(results.first, results.second),
                    utf_out_iter(results.second, results.second));
            }
            
            std::cout << " state = " << results.state << '\n';
            lexertl::lookup(sm, results);
        }

        std::cout << "Stack has " << results.stack.size() << " values on it.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
