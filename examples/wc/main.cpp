//#include "../../lexertl/debug.hpp"
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/memory_file.hpp"

int main(int argc, char *argv[])
{
    enum {eoi, word, ws, newline};
    lexertl::rules rules_;
    lexertl::state_machine sm_;

    rules_.insert_macro("ws", "[ \t]");
    rules_.insert_macro("nonws", "[^ \t\n]");
    rules_.push("{nonws}+", word);
    rules_.push("{ws}+", ws);
    rules_.push("\r|\n|\r\n", newline);
    lexertl::generator::build(rules_, sm_);
//    lexertl::debug::dump(sm_, std::cout);

    lexertl::memory_file if_(argc == 2 ? argv[1] :
        "lexertl/licence_1_0.txt");
    const char *start_ = if_.data();
    const char *end_ = start_ + if_.size();
    lexertl::cmatch results_(start_, end_);
    int cc = 0, wc = 0, lc = 0;

    do
    {
        lexertl::lookup(sm_, results_);

        switch (results_.id)
        {
        case eoi:
            break;
        case word:
            cc += results_.second - results_.first;
            ++wc;
            break;
        case ws:
            cc += results_.second - results_.first;
            break;
        case newline:
            ++lc;
            cc += results_.second - results_.first;
            break;
        default:
            assert(0);
            break;
        }
    } while (results_.id != eoi);

    std::cout << "lines: " << lc << ", words: " << wc <<
        ", chars: " << cc << '\n';
    return 0;
}
