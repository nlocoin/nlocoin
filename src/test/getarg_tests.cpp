#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-NLO");
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", false));
    BOOST_CHECK(GetBoolArg("-NLO", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-NLOo"));
    BOOST_CHECK(!GetBoolArg("-NLOo", false));
    BOOST_CHECK(GetBoolArg("-NLOo", true));

    ResetArgs("-NLO=0");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", false));
    BOOST_CHECK(!GetBoolArg("-NLO", true));

    ResetArgs("-NLO=1");
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", false));
    BOOST_CHECK(GetBoolArg("-NLO", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noNLO");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", false));
    BOOST_CHECK(!GetBoolArg("-NLO", true));

    ResetArgs("-noNLO=1");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", false));
    BOOST_CHECK(!GetBoolArg("-NLO", true));

    ResetArgs("-NLO -noNLO");  // -NLO should win
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", false));
    BOOST_CHECK(GetBoolArg("-NLO", true));

    ResetArgs("-NLO=1 -noNLO=1");  // -NLO should win
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", false));
    BOOST_CHECK(GetBoolArg("-NLO", true));

    ResetArgs("-NLO=0 -noNLO=0");  // -NLO should win
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", false));
    BOOST_CHECK(!GetBoolArg("-NLO", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--NLO=1");
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", false));
    BOOST_CHECK(GetBoolArg("-NLO", true));

    ResetArgs("--noNLO=1");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", false));
    BOOST_CHECK(!GetBoolArg("-NLO", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NLO", "eleven"), "eleven");

    ResetArgs("-NLO -bar");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NLO", "eleven"), "");

    ResetArgs("-NLO=");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NLO", "eleven"), "");

    ResetArgs("-NLO=11");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-NLO", "eleven"), "11");

    ResetArgs("-NLO=eleven");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-NLO", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NLO", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-NLO", 0), 0);

    ResetArgs("-NLO -bar");
    BOOST_CHECK_EQUAL(GetArg("-NLO", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-NLO=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-NLO", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-NLO=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-NLO", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--NLO");
    BOOST_CHECK_EQUAL(GetBoolArg("-NLO"), true);

    ResetArgs("--NLO=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-NLO", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noNLO");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", true));
    BOOST_CHECK(!GetBoolArg("-NLO", false));

    ResetArgs("-noNLO=1");
    BOOST_CHECK(!GetBoolArg("-NLO"));
    BOOST_CHECK(!GetBoolArg("-NLO", true));
    BOOST_CHECK(!GetBoolArg("-NLO", false));

    ResetArgs("-noNLO=0");
    BOOST_CHECK(GetBoolArg("-NLO"));
    BOOST_CHECK(GetBoolArg("-NLO", true));
    BOOST_CHECK(GetBoolArg("-NLO", false));

    ResetArgs("-NLO --noNLO");
    BOOST_CHECK(GetBoolArg("-NLO"));

    ResetArgs("-noNLO -NLO"); // NLO always wins:
    BOOST_CHECK(GetBoolArg("-NLO"));
}

BOOST_AUTO_TEST_SUITE_END()
