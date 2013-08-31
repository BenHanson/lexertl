#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"
#include "../../lexertl/utf_iterators.hpp"

int main()
{
    try
    {
        typedef lexertl::basic_state_machine<char32_t> usm;
        typedef lexertl::basic_utf8_in_iterator<const char *, char32_t>
            utf_in_iter;
        typedef lexertl::basic_utf8_out_iterator<utf_in_iter>
            utf_out_iter;
        lexertl::rules rules;
        usm sm;
        lexertl::memory_file file("C:\\json.txt");
        const char *begin = file.data();
        const char *end = begin + file.size();
        lexertl::recursive_match_results<utf_in_iter> results(utf_in_iter(begin, end),
            utf_in_iter(end, end));
        enum {eEOF, eString, eNumber, eBoolean, eOpenOb, eName, eCloseOb,
            eOpenArr, eCloseArr, eNull};

        // http://code.google.com/p/bsn-goldparser/wiki/JsonGrammar
        rules.add_macro("STRING", "[\"]([ -\\x10ffff]{-}[\"\\\\]|"
            "\\\\([\"\\\\/bfnrt]|u[0-9a-fA-F]{4}))*[\"]");
        rules.add_macro("NUMBER", "-?(0|[1-9]\\d*)([.]\\d+)?([eE][-+]?\\d+)?");
        rules.add_macro("BOOL", "true|false");
        rules.add_macro("NULL", "null");

        rules.add_state("END");

        rules.add_state("OBJECT");
        rules.add_state("NAME");
        rules.add_state("COLON");
        rules.add_state("OB_VALUE");
        rules.add_state("OB_COMMA");

        rules.add_state("ARRAY");
        rules.add_state("ARR_COMMA");
        rules.add_state("ARR_VALUE");

        rules.add("INITIAL", "[{]", eOpenOb, ">OBJECT:END");
        rules.add("INITIAL", "[[]", eOpenArr, ">ARRAY:END");

        rules.add("OBJECT,OB_COMMA", "[}]", eCloseOb, "<");
        rules.add("OBJECT,NAME", "{STRING}", eName, "COLON");
        rules.add("COLON", ":", rules.skip(), "OB_VALUE");

        rules.add("OB_VALUE", "{STRING}", eString, "OB_COMMA");
        rules.add("OB_VALUE", "{NUMBER}", eNumber, "OB_COMMA");
        rules.add("OB_VALUE", "{BOOL}", eBoolean, "OB_COMMA");
        rules.add("OB_VALUE", "{NULL}", eNull, "OB_COMMA");
        rules.add("OB_VALUE", "[{]", eOpenOb, ">OBJECT:OB_COMMA");
        rules.add("OB_VALUE", "[[]", eOpenArr, ">ARRAY:OB_COMMA");

        rules.add("OB_COMMA", ",", rules.skip(), "NAME");

        rules.add("ARRAY,ARR_COMMA", "\\]", eCloseArr, "<");
        rules.add("ARRAY,ARR_VALUE", "{STRING}", eString, "ARR_COMMA");
        rules.add("ARRAY,ARR_VALUE", "{NUMBER}", eNumber, "ARR_COMMA");
        rules.add("ARRAY,ARR_VALUE", "{BOOL}", eBoolean, "ARR_COMMA");
        rules.add("ARRAY,ARR_VALUE", "{NULL}", eNull, "ARR_COMMA");
        rules.add("ARRAY,ARR_VALUE", "[{]", eOpenOb, ">OBJECT:ARR_COMMA");
        rules.add("ARRAY,ARR_VALUE", "[[]", eOpenArr, ">ARRAY:ARR_COMMA");

        rules.add("ARR_COMMA", ",", rules.skip(), "ARR_VALUE");

        rules.add("*", "[ \t\r\n]+", rules.skip(), ".");

        lexertl::basic_generator<lexertl::rules, usm>::build(rules, sm);
        // Read-ahead
        lexertl::lookup(sm, results);

        while (results.id != eEOF)
        {
            std::cout << "Id: " << results.id << " token: " <<
                std::string(utf_out_iter(results.start, results.end),
                utf_out_iter(results.end, results.end)) <<
                " state = " << results.state << '\n';
            lexertl::lookup(sm, results);
        }

        std::cout << "Stack has " << results.stack.size() << " values on it.\n";
    }
    catch (const std::exception &e)
    {
        std::cout << e.what () << std::endl;
    }

    return 0;
}
