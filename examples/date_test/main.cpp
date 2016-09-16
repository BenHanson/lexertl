#include <fstream>
#include "../../lexertl/generator.hpp"
#include "../../lexertl/lookup.hpp"
#include "../../lexertl/stream_shared_iterator.hpp"

int main(int /*argc*/, char** /*argv*/)
{
    lexertl::rules rules_(lexertl::icase | lexertl::dot_not_newline);
    lexertl::state_machine sm_;
    std::ifstream if_("datetest.txt");
    lexertl::stream_shared_iterator iter_(if_);
    lexertl::stream_shared_iterator end_;
    lexertl::match_results<lexertl::stream_shared_iterator>
        results_(iter_, end_);

    rules_.insert_macro("skip", "of|the|[ \t,]+");
    rules_.insert_macro("mon", "(mon(day)?)");
    rules_.insert_macro("tue", "(tue(sday)?)");
    rules_.insert_macro("wed", "(wed(nesday)?)");
    rules_.insert_macro("thu", "(thu(rsday)?)");
    rules_.insert_macro("fri", "(fri(day)?)");
    rules_.insert_macro("sat", "(sat(urday)?)");
    rules_.insert_macro("sun", "(sun(day)?)");
    rules_.insert_macro("day_of_the_week", "({mon}|{tue}|{wed}|{thu}|{fri}|{sat}|{sun})");

    rules_.insert_macro("jan", "(jan(uary)?)");
    rules_.insert_macro("feb", "(feb(ruary)?)");
    rules_.insert_macro("mar", "(mar(ch)?)");
    rules_.insert_macro("apr", "(apr(il)?)");
    rules_.insert_macro("may", "(may)");
    rules_.insert_macro("jun", "(jun(e)?)");
    rules_.insert_macro("jul", "(jul(y)?)");
    rules_.insert_macro("aug", "(aug(ust)?)");
    rules_.insert_macro("sep", "(sep(tember)?)");
    rules_.insert_macro("oct", "(oct(ober)?)");
    rules_.insert_macro("nov", "(nov(ember)?)");
    rules_.insert_macro("dec", "(dec(ember)?)");

    rules_.insert_macro("first_half", "({jan}|{feb}|{mar}|{apr}|{may}|{jun})");
    rules_.insert_macro("second_half", "({jul}|{aug}|{sep}|{oct}|{nov}|{dec})");
    rules_.insert_macro("month", "{first_half}|{second_half}");

    rules_.insert_macro("nday", "[1-9]|[1-2][0-9]|3[0-1]");
    rules_.insert_macro("nmonth", "[1-9]|1[0-2]");
    rules_.insert_macro("nyear", "[0-9]{1,4}");

    rules_.insert_macro("year_ext", "(ad|AD|bc|BC)?");
    rules_.insert_macro("day_ext", "(st|nd|rd|th)?");

    rules_.push_state("LONG");
    rules_.push_state("SHORT");
    rules_.push_state("DAY");
    rules_.push_state("DAY_FIRST");
    rules_.push_state("YEAR_FIRST");
    rules_.push_state("YEAR_LAST");
    rules_.push_state("YFMONTH");
    rules_.push_state("YLMONTH");

    enum {edow = 1, eday, emonth, eyear, elong, eshort};
    std::string dow;
    std::string day;
    std::string month;
    std::string year;

    rules_.push("LONG", "{day_of_the_week}", edow, ".");
    rules_.push("LONG", "{month}", emonth, "DAY");

    rules_.push("LONG", "{nday}{day_ext}", eday, "DAY_FIRST");
    rules_.push("DAY_FIRST", "{month}", emonth, "LONG");
    rules_.push("DAY", "{nday}{day_ext}", eday, "LONG");

    rules_.push("LONG", "{nyear}{year_ext}", elong, ".");

    rules_.push("SHORT", "{nday}", eday, "YEAR_LAST");
    rules_.push("YEAR_LAST", "{nmonth}", emonth, "YLMONTH");
    rules_.push("YLMONTH", "{nyear}", eyear, "SHORT");

    rules_.push("SHORT", "{nyear}", eyear, "YEAR_FIRST");
    rules_.push("YEAR_FIRST", "{nmonth}", emonth, "YFMONTH");
    rules_.push("YFMONTH", "{nday}", eday, "SHORT");

    rules_.push("SHORT", "\n", eshort, ".");

    rules_.push("INITIAL,SHORT,LONG", "long\n", results_.skip(), "LONG");
    rules_.push("INITIAL,SHORT,LONG", "short\n", results_.skip(), "SHORT");

    rules_.push("*", "{skip}+|\n|.", results_.skip(), ".");

    lexertl::generator::build(rules_, sm_);

    do
    {
        lexertl::lookup(sm_, results_);

        switch (results_.id)
        {
        case 0:
            break;
        case edow:
            dow += results_.str();
            break;
        case eday:
            day = results_.str();
            break;
        case emonth:
            month = results_.str();
            break;
        case eyear:
            year = results_.str();
            break;
        case elong:
            std::cout << "Long:\n" <<
                "  DOW   : " << dow << " \n" <<
                "  Day   : " << day << " \n" <<
                "  Month : " << month << " \n" <<
                "  Year  : " << results_.str() <<
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
