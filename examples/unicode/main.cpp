#include <fstream>
#include "../../lexertl/generator.hpp"
#include <iomanip>
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"
#include "../../lexertl/stream_shared_iterator.hpp"
#include "../../lexertl/utf_iterators.hpp"

#ifdef WIN32
#include <windows.h>
#endif

void lex_unicode_data()
{
    lexertl::rules rules_;
    lexertl::state_machine state_machine_;
    lexertl::memory_file if_("examples/unicode/UnicodeData.txt");
    const char *start_ = if_.data();
    const char *end_ = start_ + if_.size();
    lexertl::cmatch results_(start_, end_);
    enum { eNumber = 1, eName };

    std::size_t num_ = 0;
    typedef lexertl::basic_string_token<std::size_t> string_token;
    typedef std::map<std::string, string_token> map;
    map map_;

    rules_.push_state("LONG_NAME");
    rules_.push_state("SHORT_NAME");
    rules_.push_state("FINISH");

    rules_.push("INITIAL", "^[A-F0-9]+", eNumber, "LONG_NAME");
    rules_.push("LONG_NAME", ";[^;]+;", rules_.skip(), "SHORT_NAME");
    rules_.push("SHORT_NAME", "[A-Z][a-z]?", eName, "FINISH");
    rules_.push("FINISH", ".*\n", rules_.skip(), "INITIAL");
    lexertl::generator::build(rules_, state_machine_);

    do
    {
        lexertl::lookup(state_machine_, results_);

        if (results_.id == eNumber)
        {
            std::stringstream ss_;
            
            ss_ << results_.str();
            ss_ >> std::hex >> num_;
        }
        else if (results_.id == eName)
        {
            const std::string name_ = results_.str();

            map_[name_].insert(lexertl::basic_string_token<std::size_t>::range
                (num_, num_));
        }
    } while (results_.id != 0);

    typedef std::map<std::string, std::string> string_map;
    string_map code_map;
    map::const_iterator i_ = map_.begin();
    map::const_iterator e_ = map_.end();
    std::string line_;

    code_map["Cc"] = "other_control";
    code_map["Cf"] = "other_format";
    code_map["Co"] = "other_private";
    code_map["Cs"] = "other_surrogate";
    code_map["Ll"] = "letter_lowercase";
    code_map["Lm"] = "letter_modifier";
    code_map["Lo"] = "letter_other";
    code_map["Lt"] = "letter_titlecase";
    code_map["Lu"] = "letter_uppercase";
    code_map["Mc"] = "mark_combining";
    code_map["Me"] = "mark_enclosing";
    code_map["Mn"] = "mark_nonspacing";
    code_map["Nd"] = "number_decimal";
    code_map["Nl"] = "number_letter";
    code_map["No"] = "number_other";
    code_map["Pc"] = "punctuation_connector";
    code_map["Pd"] = "punctuation_dash";
    code_map["Pe"] = "punctuation_close";
    code_map["Pf"] = "punctuation_final";
    code_map["Pi"] = "punctuation_initial";
    code_map["Po"] = "punctuation_other";
    code_map["Ps"] = "punctuation_open";
    code_map["Sc"] = "symbol_currency";
    code_map["Sk"] = "symbol_modifier";
    code_map["Sm"] = "symbol_math";
    code_map["So"] = "symbol_other";
    code_map["Zl"] = "separator_line";
    code_map["Zp"] = "separator_paragraph";
    code_map["Zs"] = "separator_space";

    for (; i_ != e_; ++i_)
    {
        std::cout << "    static const char *" << code_map[i_->first] <<
            "()\n" << "    {\n";
        line_ = "        return \"[";

        string_token::range_vector::const_iterator ri =
            i_->second._ranges.begin();
        string_token::range_vector::const_iterator re =
            i_->second._ranges.end();

        for (; ri != re; ++ri)
        {
            std::size_t max_chars = 79;
            std::ostringstream ss_;
            std::string range_;

            ss_ << "\\\\x" << std::hex << ri->first;

            if (ri->first != ri->second)
            {
                if (ri->second - ri->first > 1)
                {
                    --max_chars;
                    ss_ << '-';
                }

                ss_ << "\\\\x" << ri->second;
            }

            range_ = ss_.str();

            if (ri + 1 == re)
            {
                max_chars -= 3;
            }

            if (line_.size() + range_.size() > max_chars)
            {
                line_ += "\"\n";
                std::cout << line_;
                line_ = "            \"";
            }

            line_ += range_;
        }

        line_ += "]\";\n    }\n\n";
        std::cout << line_;
    }
}

void case_mapping()
{
    lexertl::rules rules_;
    lexertl::state_machine sm_;
    lexertl::memory_file mf_("UnicodeData.txt");
    lexertl::cmatch results_(mf_.data(), mf_.data() + mf_.size());
    enum e_Token { eEOF, eCodeValue, eName, eLl, eLu, eNeither, eMapping, eEmpty };
    e_Token eToken = eEOF;
    std::string code_;
    std::string mapping_;
    std::map<std::size_t, std::size_t> map;

    rules_.push_state("NAME");
    rules_.push_state("TYPE");
    rules_.push_state("Ll");
    rules_.push_state("Lu");
    rules_.push_state("MAPPING");
    rules_.push_state("END");

    rules_.push("INITIAL", "^[0-9A-F]{4,6};", eCodeValue, "NAME");
    rules_.push("NAME", "[^;]*;", sm_.skip(), "TYPE");
    rules_.push("TYPE", "Ll;", eLl, "Ll");
    rules_.push("Ll", "([^;]*;){9}", sm_.skip(), "MAPPING");
    rules_.push("TYPE", "Lu;", eLu, "Lu");
    rules_.push("Lu", "([^;]*;){10}", sm_.skip(), "MAPPING");
    rules_.push("TYPE", "[^;]*;", eNeither, "END");
    rules_.push("MAPPING", ";", eEmpty, "END");
    rules_.push("MAPPING", "[0-9A-F]{4,6};", eMapping, "END");
    rules_.push("END", "[^\n]*\n", sm_.skip(), "INITIAL");
    lexertl::generator::build(rules_, sm_);

    do
    {
        lexertl::lookup(sm_, results_);
        eToken = static_cast<e_Token>(results_.id);

        if (eToken == eEOF)
        {
            break;
        }
        else if (eToken != eCodeValue)
        {
            throw std::runtime_error("Syntax error");
        }

        code_ = results_.str();
        lexertl::lookup(sm_, results_);
        eToken = static_cast<e_Token>(results_.id);

        if (eToken != eLl && eToken != eLu && eToken != eNeither)
        {
            throw std::runtime_error("Syntax error");
        }

        if (eToken != eNeither)
        {
            lexertl::lookup(sm_, results_);
            eToken = static_cast<e_Token>(results_.id);

            if (eToken == eMapping)
            {
                std::stringstream ss_;
                std::size_t code_val_;
                std::size_t mapping_val_;

                mapping_ = results_.str();
                ss_ << code_;
                ss_ >> std::hex >> code_val_;
                ss_.str("");
                ss_ << mapping_;
                ss_ >> std::hex >> mapping_val_;
                map[code_val_] = mapping_val_;
                code_.clear();
                mapping_.clear();
            }
        }
    } while (results_.id != 0);

    std::pair<std::size_t, std::size_t> first_ = { 0, 0 };
    std::pair<std::size_t, std::size_t> second_ = { 0, 0 };

    for (auto &pair : map)
    {
        if (first_.first == 0)
        {
            first_.first = first_.second = pair.first;
            second_.first = second_.second = pair.second;
        }
        else if (pair.first == first_.second + 1 &&
            (pair.second == second_.second + 1 ||
            pair.second <= pair.first &&
            pair.second == second_.second - 1))
        {
            first_.second = pair.first;
            second_.second = pair.second;
        }
        else
        {
            std::cout << "            {{0x" << std::hex << std::setfill('0') <<
                std::setw(4) << first_.first << ", 0x" << std::setfill('0') <<
                std::setw(4) << first_.second << "}, {0x" <<
                std::setfill('0') << std::setw(4) << second_.first << ", 0x" <<
                std::setfill('0') << std::setw(4) << second_.second << "}},\n";
            first_.first = first_.second = pair.first;
            second_.first = second_.second = pair.second;
        }
    }

    std::cout << "            {{0x" << std::hex << std::setfill('0') <<
        std::setw(4) << first_.first << ", 0x" << std::setfill('0') <<
        std::setw(4) << first_.second << "}, {0x" << std::setfill('0') <<
        std::setw(4) << second_.first << ", 0x" << std::setfill('0') <<
        std::setw(4) << second_.second << "}},\n";
}

void test_unicode()
{
    lexertl::cmatch r_;
    const char *i_ = "";

    r_.clear();
    r_.reset(i_, i_);

    const char utf8_[] = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88\x7f";
    lexertl::basic_utf8_in_iterator<const char *, int> u8iter_(utf8_,
        utf8_ + sizeof(utf8_));
    int i = *u8iter_; // 0x10346

    i = *++u8iter_; // 0x65e5
    i = *u8iter_++; // 0x65e5
    i = *u8iter_; // 0x0448
    i = *++u8iter_; // 0x7f

    const wchar_t utf16_[] = L"\xdbff\xdfff\xd801\xdc01\xd800\xdc00\xd7ff";
    lexertl::basic_utf16_in_iterator<const wchar_t *, int> u16iter_(utf16_,
        utf16_ + sizeof(utf16_) / sizeof(wchar_t));

    i = *u16iter_; // 0x10ffff
    i = *++u16iter_; // 0x10401
    i = *u16iter_++; // 0x10401
    i = *u16iter_; // 0x10000
    i = *++u16iter_; // 0xd7ff

    // Not all compilers have char32_t, so use int for now
    lexertl::basic_rules<char, int> rules_(lexertl::icase);
    lexertl::basic_state_machine<int> sm_;
    const int in_[] = {0x393, ' ', 0x393, 0x398, ' ', 0x398,
        '1', ' ', 'i', 'd', 0x41f, 0};
    std::basic_string<int> input_(in_);
    const int *iter_ = input_.c_str();
    const int *end_ = iter_ + input_.size();
    lexertl::match_results<const int *> results_(iter_, end_);

    rules_.push("\\p{LC}[\\p{LC}0-9]*", 1);
    lexertl::basic_generator<lexertl::basic_rules<char, int>,
        lexertl::basic_state_machine<int> >::build(rules_, sm_);

#ifdef WIN32
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwBytesWritten = 0;
#endif

    do
    {
#ifdef WIN32
        std::wstring str_;
#else
        std::string str_;
#endif

        lexertl::lookup(sm_, results_);

#ifdef WIN32
        str_.assign(lexertl::basic_utf16_out_iterator<const int *>
            (results_.first, results_.second),
            lexertl::basic_utf16_out_iterator<const int *>
            (results_.second, results_.second));
        std::wcout << L"Id: " << results_.id << L", Token: '";
        ::WriteConsoleW(hStdOut, str_.c_str(), str_.size(), &dwBytesWritten, 0);
        std::wcout << '\'' << std::endl;
#else
        str_.assign(lexertl::basic_utf8_out_iterator<const int *>
            (results_.first, results_.second),
            lexertl::basic_utf8_out_iterator<const int *>
            (results_.second, results_.second));
        std::cout << "Id: " << results_.id << ", Token: '" <<
            str_ << '\'' << std::endl;
#endif
    } while (results_.id != 0);
}

int main(int /*argc*/, char ** /*argv*/)
{
    test_unicode();
    return 0;
}
