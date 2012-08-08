#include <fstream>
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"
#include "../../lexertl/stream_shared_iterator.hpp"
#include "../../lexertl/utf_iterators.hpp"

#ifdef WIN32
#include <windows.h>
#endif

void lex_prop_list()
{
    lexertl::rules rules_;
    lexertl::state_machine state_machine_;
    std::ifstream if_ ("PropList.txt");
    lexertl::stream_shared_iterator iter_ (if_);
    lexertl::stream_shared_iterator end_;
    lexertl::match_results<lexertl::stream_shared_iterator, std::size_t>
        results_(iter_, end_);
    enum {eRange = 1, eName, eShortName};

    rules_.add_state("RANGE");
    rules_.add_state("WS");
    rules_.add_state("NAME");
    rules_.add_state("WS2");
    rules_.add_state("SHORT_NAME");
    rules_.add_state("FINISH");
    rules_.add ("^#.*", rules_.skip ());
    rules_.add ("\n", rules_.skip ());
    rules_.add ("INITIAL", "^[0-9A-F]+(\\.\\.[0-9A-F]+)?", eRange, "RANGE");
    rules_.add ("RANGE", " *; ", rules_.skip (), "NAME");
    rules_.add ("NAME", "[A-Z][a-zA-Z_]+", eName, "WS2");
    rules_.add ("WS2", " # ", rules_.skip (), "SHORT_NAME");
    rules_.add ("SHORT_NAME", "[A-Z][a-z&]", eShortName, "FINISH");
    rules_.add ("FINISH", ".*\n", rules_.skip (), "INITIAL");
    lexertl::generator::build (rules_, state_machine_);

    do
    {
        lexertl::lookup (state_machine_, results_);
        std::cout << "Id: " << results_.id << ", Token: '" <<
            std::string (results_.start, results_.end) << "'\n";

        if (results_.id > eShortName)
        {
//            int i = 0;
        }
    } while (results_.id != 0);
}

void lex_unicode_data ()
{
    clock_t started_ = ::clock ();
    lexertl::rules rules_;
    lexertl::state_machine state_machine_;
    lexertl::memory_file if_ ("UnicodeData.txt");
    const char *start_ = if_.data ();
    const char *end_ = start_ + if_.size ();
    lexertl::cmatch results_(start_, end_);
    enum {eNumber = 1, eName};

    std::size_t num_ = 0;
    std::map<std::string, lexertl::basic_string_token<std::size_t> > map_;

    rules_.add_state("LONG_NAME");
    rules_.add_state("SHORT_NAME");
    rules_.add_state("FINISH");

    rules_.add("INITIAL", "^[A-F0-9]+", eNumber, "LONG_NAME");
    rules_.add("LONG_NAME", ";[^;]+;", rules_.skip (), "SHORT_NAME");
    rules_.add("SHORT_NAME", "[A-Z][a-z]?", eName, "FINISH");
    rules_.add("FINISH", ".*\n", rules_.skip (), "INITIAL");
    lexertl::generator::build (rules_, state_machine_);

    do
    {
        lexertl::lookup (state_machine_, results_);

        if (results_.id == eNumber)
        {
            num_ = 0;

            for (;;)
            {
                if (*results_.start >= '0' && *results_.start <= '9')
                {
                    num_ <<= 4;
                    num_ |= *results_.start++ - '0';
                }
                else if (*results_.start >= 'A' && *results_.start <= 'F')
                {
                    num_ <<= 4;
                    num_ |= *results_.start++ - 'A' + 10;
                }
                else
                {
                    break;
                }
            }

//            ::sscanf (&*results_.start, "%x", &num_); // Too slow!
        }
        else if (results_.id == eName)
        {
            const std::string name_ (results_.start, results_.end);

            map_[name_].insert (lexertl::basic_string_token<std::size_t>::range
                (num_, num_));
        }
    } while (results_.id != 0);

    clock_t finished_ = ::clock ();
    double seconds_ = static_cast<double>
        (finished_ - started_) / CLOCKS_PER_SEC;

    std::cout << seconds_ << "\n";
}

void case_mapping ()
{
    lexertl::rules rules_;
    lexertl::state_machine sm_;
    std::ifstream if_ ("UnicodeData.txt");
    lexertl::stream_shared_iterator iter_ (if_);
    lexertl::stream_shared_iterator end_;
    lexertl::match_results<lexertl::stream_shared_iterator, std::size_t>
        results_(iter_, end_);
    enum e_Token {eEOF, eCodeValue, eName, eLl, eLu, eNeither, eMapping, eEmpty};
    e_Token eToken = eEOF;
    std::string code_;
    std::string mapping_;
    int count_ = 0;

    rules_.add_state ("NAME");
    rules_.add_state ("TYPE");
    rules_.add_state ("Ll");
    rules_.add_state ("Lu");
    rules_.add_state ("MAPPING");
    rules_.add_state ("END");

    rules_.add("INITIAL", "^[0-9A-F]{4,6};", eCodeValue, "NAME");
    rules_.add("NAME", "[^;]*;", sm_.skip (), "TYPE");
    rules_.add("TYPE", "Ll;", eLl, "Ll");
    rules_.add("Ll", "([^;]*;){9}", sm_.skip (), "MAPPING");
    rules_.add("TYPE", "Lu;", eLu, "Lu");
    rules_.add("Lu", "([^;]*;){10}", sm_.skip (), "MAPPING");
    rules_.add("TYPE", "[^;]*;", eNeither, "END");
    rules_.add("MAPPING", ";", eEmpty, "END");
    rules_.add("MAPPING", "[0-9A-F]{4,6};", eMapping, "END");
    rules_.add("END", "[^\n]*\n", sm_.skip (), "INITIAL");
    lexertl::generator::build (rules_, sm_);

    do
    {
        lexertl::lookup (sm_, results_);
        eToken = static_cast<e_Token>(results_.id);

        if (eToken == eEOF)
        {
            break;
        }
        else if (eToken != eCodeValue)
        {
            throw std::runtime_error ("Syntax error");
        }

        code_.assign(results_.start, results_.end);
        lexertl::lookup (sm_, results_);
        eToken = static_cast<e_Token>(results_.id);

        if (eToken != eLl && eToken != eLu && eToken != eNeither)
        {
            throw std::runtime_error ("Syntax error");
        }

        if (eToken != eNeither)
        {
            lexertl::lookup (sm_, results_);
            eToken = static_cast<e_Token>(results_.id);

            if (eToken == eMapping)
            {
                mapping_.assign(results_.start, results_.end);
                std::cout << "(0x" << code_.substr(0, code_.size() - 1) << ", " <<
                    "0x" << mapping_.substr(0, mapping_.size() - 1) << "), ";
                code_.clear();
                mapping_.clear();
                ++count_;

                if (count_ > 2)
                {
                    count_ = 0;
                    std::cout << '\n';
                }
            }
        }
    } while (results_.id != 0);
}

void test_unicode ()
{
    lexertl::cmatch r_;
    const char *i_ = "";

    r_.clear ();
    r_.reset (i_, i_);

    const char *utf8_ = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88\x7f";
    lexertl::basic_utf8_in_iterator<const char *, int> u8iter_(utf8_);
    int i = *u8iter_; // 0x10346

    i = *++u8iter_; // 0x65e5
    i = *u8iter_++; // 0x65e5
    i = *u8iter_; // 0x0448
    i = *++u8iter_; // 0x7f

    const wchar_t utf16_[] = L"\xdbff\xdfff\xd801\xdc01\xd800\xdc00\xd7ff";
    lexertl::basic_utf16_in_iterator<const wchar_t *, int> u16iter_(utf16_);

    i = *u16iter_; // 0x10ffff
    i = *++u16iter_; // 0x10401
    i = *u16iter_++; // 0x10401
    i = *u16iter_; // 0x10000
    i = *++u16iter_; // 0xd7ff

    // Not all compilers have char32_t, so use int for now
    lexertl::rules rules_(lexertl::icase);
    lexertl::basic_state_machine<int> sm_;
    const int in_[] = {0x393, ' ', 0x393, 0x398, ' ', 0x398,
        '1', ' ', 'i', 'd', 0x41f, 0};
    std::basic_string<int> input_ (in_);
    const int *iter_ = input_.c_str();
    const int *end_ = iter_ + input_.size();
    lexertl::match_results<const int *> results_ (iter_, end_);

    rules_.add("\\p{LC}[\\p{LC}0-9]*", 1);
    lexertl::basic_generator<lexertl::rules,
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

        lexertl::lookup (sm_, results_);

#ifdef WIN32
        str_.assign(lexertl::basic_utf16_out_iterator<const int *>
            (results_.start),
            lexertl::basic_utf16_out_iterator<const int *>
            (results_.end));
        std::wcout << L"Id: " << results_.id << L", Token: '";
        ::WriteConsoleW(hStdOut, str_.c_str(), str_.size(), &dwBytesWritten, 0);
        std::wcout << '\'' << std::endl;
#else
        str_.assign(lexertl::basic_utf8_out_iterator<const int *>
            (results_.start),
            lexertl::basic_utf8_out_iterator<const int *>
            (results_.end));
        std::cout << "Id: " << results_.id << ", Token: '" <<
            str_ << '\'' << std::endl;
#endif
    } while (results_.id != 0);
}

int main (int /*argc*/, char ** /*argv*/)
{
    lex_prop_list();
    return 0;
}
