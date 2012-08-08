#include <fstream>
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/stream_shared_iterator.hpp"

int main(int /*argc*/, char** /*argv*/)
{
    lexertl::rules rules_(lexertl::icase | lexertl::dot_not_newline);
    lexertl::state_machine sm_;
    std::ifstream if_ ("datetest.txt");
    lexertl::stream_shared_iterator iter_ (if_);
    lexertl::stream_shared_iterator end_;
    lexertl::match_results<lexertl::stream_shared_iterator, std::size_t>
        results_(iter_, end_);

    rules_.add_macro("skip", "of|the|[ \t,]+");
    rules_.add_macro("mon", "(mon(day)?)");
    rules_.add_macro("tue", "(tue(sday)?)");
    rules_.add_macro("wed", "(wed(nesday)?)");
    rules_.add_macro("thu", "(thu(rsday)?)");
    rules_.add_macro("fri", "(fri(day)?)");
    rules_.add_macro("sat", "(sat(urday)?)");
    rules_.add_macro("sun", "(sun(day)?)");
    rules_.add_macro("day_of_the_week", "({mon}|{tue}|{wed}|{thu}|{fri}|{sat}|{sun})");

    rules_.add_macro("jan", "(jan(uary)?)");
    rules_.add_macro("feb", "(feb(ruary)?)");
    rules_.add_macro("mar", "(mar(ch)?)");
    rules_.add_macro("apr", "(apr(il)?)");
    rules_.add_macro("may", "(may)");
    rules_.add_macro("jun", "(jun(e)?)");
    rules_.add_macro("jul", "(jul(y)?)");
    rules_.add_macro("aug", "(aug(ust)?)");
    rules_.add_macro("sep", "(sep(tember)?)");
    rules_.add_macro("oct", "(oct(ober)?)");
    rules_.add_macro("nov", "(nov(ember)?)");
    rules_.add_macro("dec", "(dec(ember)?)");

    rules_.add_macro("first_half", "({jan}|{feb}|{mar}|{apr}|{may}|{jun})");
    rules_.add_macro("second_half", "({jul}|{aug}|{sep}|{oct}|{nov}|{dec})");
    rules_.add_macro("month", "{first_half}|{second_half}");

    rules_.add_macro("nday", "[1-9]|[1-2][0-9]|3[0-1]");
    rules_.add_macro("nmonth", "[1-9]|1[0-2]");
    rules_.add_macro("nyear", "[0-9]{1,4}");

    rules_.add_macro("year_ext", "(ad|AD|bc|BC)?");
    rules_.add_macro("day_ext", "(st|nd|rd|th)?");

    rules_.add_state("LONG");
    rules_.add_state("SHORT");
    rules_.add_state("DAY");
    rules_.add_state("DAY_FIRST");
    rules_.add_state("YEAR_FIRST");
    rules_.add_state("YEAR_LAST");
    rules_.add_state("YFMONTH");
    rules_.add_state("YLMONTH");

    enum {edow = 1, eday, emonth, eyear, elong, eshort};
    std::string dow;
    std::string day;
    std::string month;
    std::string year;

    rules_.add("LONG", "{day_of_the_week}", edow, ".");
    rules_.add("LONG", "{month}", emonth, "DAY");

    rules_.add("LONG", "{nday}{day_ext}", eday, "DAY_FIRST");
    rules_.add("DAY_FIRST", "{month}", emonth, "LONG");
    rules_.add("DAY", "{nday}{day_ext}", eday, "LONG");

    rules_.add("LONG", "{nyear}{year_ext}", elong, ".");

    rules_.add("SHORT", "{nday}", eday, "YEAR_LAST");
    rules_.add("YEAR_LAST", "{nmonth}", emonth, "YLMONTH");
    rules_.add("YLMONTH", "{nyear}", eyear, "SHORT");

    rules_.add("SHORT", "{nyear}", eyear, "YEAR_FIRST");
    rules_.add("YEAR_FIRST", "{nmonth}", emonth, "YFMONTH");
    rules_.add("YFMONTH", "{nday}", eday, "SHORT");

    rules_.add("SHORT", "\n", eshort, ".");

    rules_.add("INITIAL,SHORT,LONG", "long\n", results_.skip(), "LONG");
    rules_.add("INITIAL,SHORT,LONG", "short\n", results_.skip(), "SHORT");

    rules_.add("*", "{skip}+|\n|.", results_.skip(), ".");

    lexertl::generator::build(rules_, sm_);

    do
    {
        lexertl::lookup (sm_, results_);

        switch (results_.id)
        {
        case 0:
            break;
        case edow:
            dow += std::string(results_.start, results_.end);
            break;
        case eday:
            day.assign(results_.start, results_.end);
            break;
        case emonth:
            month.assign(results_.start, results_.end);
            break;
        case eyear:
            year.assign(results_.start, results_.end);
            break;
        case elong:
            std::cout << "Long:\n" <<
                "  DOW   : " << dow << " \n" <<
                "  Day   : " << day << " \n" <<
                "  Month : " << month << " \n" <<
                "  Year  : " << std::string(results_.start, results_.end) <<
                "\n";
            dow.clear();
            day.clear();
            month.clear();
            break;
        case eshort:
            std::cout << "Short:\n" <<
                "  Day   : " << day << " \n" <<
                "  Month : " << month << " \n" <<
                "  Year  : " << year << "\n";
            year.clear();
            day.clear();
            month.clear();
            break;
        default:
            assert(0);
            break;
        }
    } while (results_.id != 0);
}
