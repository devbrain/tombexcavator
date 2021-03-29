//
// Created by igor on 29/03/2021.
//

#include <doctest.h>
#include <tomb-excavator/bsw/string_utils.hh>
#include <map>
#include <set>

using bsw::trim_left;
using bsw::trim_left_in_place;
using bsw::trim_right;
using bsw::trim_right_in_place;
using bsw::trim;
using bsw::trim_in_place;
using bsw::to_upper;
using bsw::to_upper_in_place;
using bsw::to_lower;
using bsw::to_lower_in_place;
using bsw::icompare;
using bsw::istring;
using bsw::isubstr;
using bsw::translate;
using bsw::translate_in_place;
using bsw::replace;
using bsw::replace_in_place;
using bsw::remove;
using bsw::remove_in_place;
using bsw::cat;
using bsw::starts_with;
using bsw::ends_with;

TEST_SUITE("test trim")
{
    TEST_CASE("testTrimLeft()")
    {
        {
            std::string s = "abc";
            REQUIRE(trim_left(s) == "abc");
        }
        std::string s = " abc ";
        REQUIRE(trim_left(s) == "abc ");
        {
            std::string s = "  ab c ";
            REQUIRE(trim_left(s) == "ab c ");
        }
    }

    TEST_CASE("testTrimLeftInPlace")
    {
        {
            std::string s = "abc";
            REQUIRE(trim_left_in_place(s) == "abc");
        }
        {
            std::string s = " abc ";
            REQUIRE(trim_left_in_place(s) == "abc ");
        }
        {
            std::string s = "  ab c ";
            REQUIRE(trim_left_in_place(s) == "ab c ");
        }
    }

    TEST_CASE("testTrimRight")
    {
        {
            std::string s = "abc";
            REQUIRE(trim_right(s) == "abc");
        }
        {
            std::string s = " abc ";
            REQUIRE(trim_right(s) == " abc");
        }
        {
            std::string s = "  ab c  ";
            REQUIRE(trim_right(s) == "  ab c");
        }
    }

    TEST_CASE("testTrimRightInPlace")
    {
        {
            std::string s = "abc";
            REQUIRE(trim_right_in_place(s) == "abc");
        }
        {
            std::string s = " abc ";
            REQUIRE(trim_right_in_place(s) == " abc");
        }
        {
            std::string s = "  ab c  ";
            REQUIRE(trim_right_in_place(s) == "  ab c");
        }
    }

    TEST_CASE("testTrim")
    {
        {
            std::string s = "abc";
            REQUIRE(trim(s) == "abc");
        }
        {
            std::string s = "abc ";
            REQUIRE(trim(s) == "abc");
        }
        {
            std::string s = "  ab c  ";
            REQUIRE(trim(s) == "ab c");
        }
    }

    TEST_CASE("testTrimInPlace")
    {
        {
            std::string s = "abc";
            REQUIRE(trim_in_place(s) == "abc");
        }
        {
            std::string s = " abc ";
            REQUIRE(trim_in_place(s) == "abc");
        }
        {
            std::string s = "  ab c  ";
            REQUIRE(trim_in_place(s) == "ab c");
        }
    }
}

TEST_CASE("testToUpper")
{
    {
        std::string s = "abc";
        REQUIRE (to_upper(s) == "ABC");
    }
    {
        std::string s = "Abc";
        REQUIRE (to_upper(s) == "ABC");
    }
    {
        std::string s = "abc";
        REQUIRE (to_upper_in_place(s) == "ABC");
    }
    {
        std::string s = "Abc";
        REQUIRE (to_upper_in_place(s) == "ABC");
    }
}


TEST_CASE("testToLower")
{
    {
        std::string s = "ABC";
        REQUIRE (to_lower(s) == "abc");
    }
    {
        std::string s = "aBC";
        REQUIRE (to_lower(s) == "abc");
    }
    {
        std::string s = "ABC";
        REQUIRE (to_lower_in_place(s) == "abc");
    }
    {
        std::string s = "aBC";
        REQUIRE (to_lower_in_place(s) == "abc");
    }
}


TEST_CASE("testIstring")
{
    istring is1 = "AbC";
    istring is2 = "aBc";
    REQUIRE (is1 == is2);

    const char c1[] = { 'G', 0, (char) 0xFC, 'n', 't', 'e', 'r', '\0' };
    const char c2[] = { 'g', 0, (char) 0xDC, 'N', 'T', 'E', 'R', '\0' };
    is1 = c1;
    is2 = c2;
    REQUIRE (is1 == is2);
    is1[0] = 'f';
    REQUIRE (is1 < is2);
    is1[0] = 'F';
    REQUIRE (is1 < is2);
    is1[0] = 'H';
    REQUIRE (is1 > is2);
    is1[0] = 'h';
    REQUIRE (is1 > is2);

    is1 = "aAaBbBcCc";
    is2 = "bbb";
    REQUIRE (isubstr(is1, is2) == 3);
    is2 = "bC";
    REQUIRE (isubstr(is1, is2) == 5);
    is2 = "xxx";
    REQUIRE (isubstr(is1, is2) == istring::npos);
}


TEST_CASE("testIcompare")
{
    std::string s1 = "AAA";
    std::string s2 = "aaa";
    std::string s3 = "bbb";
    std::string s4 = "cCcCc";
    std::string s5;
    REQUIRE (icompare(s1, s2) == 0);
    REQUIRE (icompare(s1, s3) < 0);
    REQUIRE (icompare(s1, s4) < 0);
    REQUIRE (icompare(s3, s1) > 0);
    REQUIRE (icompare(s4, s2) > 0);
    REQUIRE (icompare(s2, s4) < 0);
    REQUIRE (icompare(s1, s5) > 0);
    REQUIRE (icompare(s5, s4) < 0);

    std::string ss1 = "xxAAAzz";
    std::string ss2 = "YaaaX";
    std::string ss3 = "YbbbX";
    REQUIRE (icompare(ss1, 2, 3, ss2, 1, 3) == 0);
    REQUIRE (icompare(ss1, 2, 3, ss3, 1, 3) < 0);
    REQUIRE (icompare(ss1, 2, 3, ss2, 1) == 0);
    REQUIRE (icompare(ss1, 2, 3, ss3, 1) < 0);
    REQUIRE (icompare(ss1, 2, 2, ss2, 1, 3) < 0);
    REQUIRE (icompare(ss1, 2, 2, ss2, 1, 2) == 0);
    REQUIRE (icompare(ss3, 1, 3, ss1, 2, 3) > 0);

    REQUIRE (icompare(s1, s2.c_str()) == 0);
    REQUIRE (icompare(s1, s3.c_str()) < 0);
    REQUIRE (icompare(s1, s4.c_str()) < 0);
    REQUIRE (icompare(s3, s1.c_str()) > 0);
    REQUIRE (icompare(s4, s2.c_str()) > 0);
    REQUIRE (icompare(s2, s4.c_str()) < 0);
    REQUIRE (icompare(s1, s5.c_str()) > 0);
    REQUIRE (icompare(s5, s4.c_str()) < 0);

    REQUIRE (icompare(ss1, 2, 3, "aaa") == 0);
    REQUIRE (icompare(ss1, 2, 2, "aaa") < 0);
    REQUIRE (icompare(ss1, 2, 3, "AAA") == 0);
    REQUIRE (icompare(ss1, 2, 2, "bb") < 0);

    REQUIRE (icompare(ss1, 2, "aaa") > 0);
}


TEST_CASE("testCILessThan")
{
    typedef std::map<std::string, int, bsw::case_insensitive_less_than> CIMapType;
    CIMapType ciMap;

    ciMap["z"] = 1;
    ciMap["b"] = 2;
    ciMap["A"] = 3;
    ciMap["Z"] = 4;

    REQUIRE (ciMap.size() == 3);
    CIMapType::iterator it = ciMap.begin();
    REQUIRE (it->first == "A"); ++it;
    REQUIRE (it->first == "b"); ++it;
    REQUIRE (it->first == "z");
    REQUIRE (it->second == 4);

    typedef std::set<std::string, bsw::case_insensitive_less_than> CISetType;

    CISetType ciSet;
    ciSet.insert("z");
    ciSet.insert("b");
    ciSet.insert("A");
    ciSet.insert("Z");

    REQUIRE (ciSet.size() == 3);
    CISetType::iterator sIt = ciSet.begin();
    REQUIRE (*sIt == "A"); ++sIt;
    REQUIRE (*sIt == "b"); ++sIt;
    REQUIRE (*sIt == "z");
}


TEST_CASE("testTranslate")
{
    std::string s = "aabbccdd";
    REQUIRE (translate(s, "abc", "ABC") == "AABBCCdd");
    REQUIRE (translate(s, "abc", "AB") == "AABBdd");
    REQUIRE (translate(s, "abc", "") == "dd");
    REQUIRE (translate(s, "cba", "CB") == "BBCCdd");
    REQUIRE (translate(s, "", "CB") == "aabbccdd");
}


TEST_CASE("testTranslateInPlace")
{
    std::string s = "aabbccdd";
    translate_in_place(s, "abc", "ABC");
    REQUIRE (s == "AABBCCdd");
}


TEST_CASE("testReplace")
{
    std::string s("aabbccdd");

    REQUIRE (replace(s, std::string("aa"), std::string("xx")) == "xxbbccdd");
    REQUIRE (replace(s, std::string("bb"), std::string("xx")) == "aaxxccdd");
    REQUIRE (replace(s, std::string("dd"), std::string("xx")) == "aabbccxx");
    REQUIRE (replace(s, std::string("bbcc"), std::string("xx")) == "aaxxdd");
    REQUIRE (replace(s, std::string("b"), std::string("xx")) == "aaxxxxccdd");
    REQUIRE (replace(s, std::string("bb"), std::string("")) == "aaccdd");
    REQUIRE (replace(s, std::string("b"), std::string("")) == "aaccdd");
    REQUIRE (replace(s, std::string("ee"), std::string("xx")) == "aabbccdd");
    REQUIRE (replace(s, std::string("dd"), std::string("")) == "aabbcc");

    REQUIRE (replace(s, "aa", "xx") == "xxbbccdd");
    REQUIRE (replace(s, "bb", "xx") == "aaxxccdd");
    REQUIRE (replace(s, "dd", "xx") == "aabbccxx");
    REQUIRE (replace(s, "bbcc", "xx") == "aaxxdd");
    REQUIRE (replace(s, "bb", "") == "aaccdd");
    REQUIRE (replace(s, "b", "") == "aaccdd");
    REQUIRE (replace(s, "ee", "xx") == "aabbccdd");
    REQUIRE (replace(s, "dd", "") == "aabbcc");

    s = "aabbaabb";
    REQUIRE (replace(s, std::string("aa"), std::string("")) == "bbbb");
    REQUIRE (replace(s, std::string("a"), std::string("")) == "bbbb");
    REQUIRE (replace(s, std::string("a"), std::string("x")) == "xxbbxxbb");
    REQUIRE (replace(s, std::string("a"), std::string("xx")) == "xxxxbbxxxxbb");
    REQUIRE (replace(s, std::string("aa"), std::string("xxx")) == "xxxbbxxxbb");

    REQUIRE (replace(s, std::string("aa"), std::string("xx"), 2) == "aabbxxbb");

    REQUIRE (replace(s, "aa", "") == "bbbb");
    REQUIRE (replace(s, "a", "") == "bbbb");
    REQUIRE (replace(s, "a", "x") == "xxbbxxbb");
    REQUIRE (replace(s, "a", "xx") == "xxxxbbxxxxbb");
    REQUIRE (replace(s, "aa", "xxx") == "xxxbbxxxbb");

    REQUIRE (replace(s, "aa", "xx", 2) == "aabbxxbb");
    REQUIRE (replace(s, 'a', 'x', 2) == "aabbxxbb");
    REQUIRE (remove(s, 'a', 2) == "aabbbb");
    REQUIRE (remove(s, 'a') == "bbbb");
    REQUIRE (remove(s, 'b', 2) == "aaaa");
}


TEST_CASE("testReplaceInPlace")
{
    std::string s("aabbccdd");

    replace_in_place(s, std::string("aa"), std::string("xx"));
    REQUIRE (s == "xxbbccdd");

    s = "aabbccdd";
    replace_in_place(s, 'a', 'x');
    REQUIRE (s == "xxbbccdd");
    replace_in_place(s, 'x');
    REQUIRE (s == "bbccdd");
    remove_in_place(s, 'b', 1);
    REQUIRE (s == "bccdd");
    remove_in_place(s, 'd');
    REQUIRE (s == "bcc");
}


TEST_CASE("testCat")
{
    std::string s1("one");
    std::string s2("two");
    std::string s3("three");
    std::string s4("four");
    std::string s5("five");
    std::string s6("six");

    REQUIRE (cat(s1, s2) == "onetwo");
    REQUIRE (cat(s1, s2, s3) == "onetwothree");
    REQUIRE (cat(s1, s2, s3, s4) == "onetwothreefour");
    REQUIRE (cat(s1, s2, s3, s4, s5) == "onetwothreefourfive");
    REQUIRE (cat(s1, s2, s3, s4, s5, s6) == "onetwothreefourfivesix");

    std::vector<std::string> vec;
    REQUIRE (cat(std::string(), vec.begin(), vec.end()) == "");
    REQUIRE (cat(std::string(","), vec.begin(), vec.end()) == "");
    vec.push_back(s1);
    REQUIRE (cat(std::string(","), vec.begin(), vec.end()) == "one");
    vec.push_back(s2);
    REQUIRE (cat(std::string(","), vec.begin(), vec.end()) == "one,two");
    vec.push_back(s3);
    REQUIRE (cat(std::string(","), vec.begin(), vec.end()) == "one,two,three");
}


TEST_CASE("testStartsWith")
{
    std::string s1("o");

    REQUIRE (starts_with(s1, std::string("o")));
    REQUIRE (starts_with(s1, std::string("")));

    REQUIRE (!starts_with(s1, std::string("O")));
    REQUIRE (!starts_with(s1, std::string("1")));

    std::string s2("");

    REQUIRE (starts_with(s2, std::string("")));

    REQUIRE (!starts_with(s2, std::string("o")));

    std::string s3("oO");

    REQUIRE (starts_with(s3, std::string("o")));

    REQUIRE (!starts_with(s3, std::string(" o")));
}


TEST_CASE("testEndsWith")
{
    std::string s1("o");

    REQUIRE (ends_with(s1, std::string("o")));
    REQUIRE (ends_with(s1, std::string("")));

    REQUIRE (!ends_with(s1, std::string("O")));
    REQUIRE (!ends_with(s1, std::string("1")));


    std::string s2("");

    REQUIRE (ends_with(s2, std::string("")));

    REQUIRE (!ends_with(s2, std::string("o")));

    std::string s3("Oo");

    REQUIRE (ends_with(s3, std::string("o")));

    REQUIRE (!ends_with(s3, std::string("o ")));
}


