//
// Created by igor on 20/03/2021.
//

#include <doctest.h>
#include <tomb-excavator/bsw/string_tokenizer.hh>

using namespace bsw;

TEST_SUITE("StringTokenizer")
{
    TEST_CASE("Test string tokenizer")
    {
        {
            string_tokenizer st("", "");
            REQUIRE (st.begin() == st.end());
        }
        {
            string_tokenizer st("", "", string_tokenizer::TOK_IGNORE_EMPTY);
            REQUIRE (st.begin() == st.end());
        }
        {
            string_tokenizer st("", "", string_tokenizer::TOK_TRIM);
            REQUIRE (st.begin() == st.end());
        }
        {
            string_tokenizer st("", "", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            REQUIRE (st.begin() == st.end());
        }
        {
            string_tokenizer st("abc", "");
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc ", "", string_tokenizer::TOK_TRIM);
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("  abc  ", "", string_tokenizer::TOK_TRIM);
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("  abc", "", string_tokenizer::TOK_TRIM);
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "b");
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (st.find("c") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "b", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (st.find("c") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "bc");
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (st.find("") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "bc", string_tokenizer::TOK_TRIM);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (st.find("") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "bc", string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "bc", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc", "bc", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("a a,c c", ",");
            auto it = st.begin();
            REQUIRE (st.find("a a") == 0);
            REQUIRE (st.find("c c") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("a a,c c", ",", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a a") == 0);
            REQUIRE (st.find("c c") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(" a a , , c c ", ",");
            auto it = st.begin();
            REQUIRE (st.find(" a a ") == 0);
            REQUIRE (st.find(" ") == 1);
            REQUIRE (st.find(" c c ") == 2);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == " a a ");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == " ");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == " c c ");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(" a a , , c c ", ",", string_tokenizer::TOK_TRIM);
            auto it = st.begin();
            REQUIRE (st.find("a a") == 0);
            REQUIRE (st.find("") == 1);
            REQUIRE (st.find("c c") == 2);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(" a a , , c c ", ",", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a a") == 0);
            REQUIRE (st.find("c c") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "c c");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc,def,,ghi , jk,  l ", ",", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (st.find("def") == 1);
            REQUIRE (st.find("ghi") == 2);
            REQUIRE (st.find("jk") == 3);
            REQUIRE (st.find("l") == 4);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "def");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ghi");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "jk");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "l");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("abc,def,,ghi // jk,  l ", ",/", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("abc") == 0);
            REQUIRE (st.find("def") == 1);
            REQUIRE (st.find("ghi") == 2);
            REQUIRE (st.find("jk") == 3);
            REQUIRE (st.find("l") == 4);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "abc");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "def");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ghi");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "jk");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "l");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("a/bc,def,,ghi // jk,  l ", ",/", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("a") == 0);
            REQUIRE (st.find("bc") == 1);
            REQUIRE (st.find("def") == 2);
            REQUIRE (st.find("ghi") == 3);
            REQUIRE (st.find("jk") == 4);
            REQUIRE (st.find("l") == 5);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "a");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "bc");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "def");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ghi");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "jk");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "l");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(",ab,cd,", ",");
            auto it = st.begin();
            REQUIRE (st.find("") == 0);
            REQUIRE (st.find("ab") == 1);
            REQUIRE (st.find("cd") == 2);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ab");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "cd");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(",ab,cd,", ",", string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("ab") == 0);
            REQUIRE (st.find("cd") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ab");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "cd");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st(" , ab , cd , ", ",", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            auto it = st.begin();
            REQUIRE (st.find("ab") == 0);
            REQUIRE (st.find("cd") == 1);
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "ab");
            REQUIRE (it != st.end());
            REQUIRE (*it++ == "cd");
            REQUIRE (it == st.end());
        }
        {
            string_tokenizer st("1 : 2 , : 3 ", ":,", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);
            REQUIRE (st.count() == 3);
            REQUIRE (st[0] == "1");
            REQUIRE (st[1] == "2");
            REQUIRE (st[2] == "3");
            REQUIRE (st.find("1") == 0);
            REQUIRE (st.find("2") == 1);
            REQUIRE (st.find("3") == 2);
        }

        {
            string_tokenizer st(" 2- ", "-", string_tokenizer::TOK_TRIM);
            REQUIRE (st.count() == 2);
            REQUIRE (st[0] == "2");
            REQUIRE (st[1] == "");
        }
    }

    TEST_CASE("Test find")
    {
        string_tokenizer st("0,1,2,3,3,2,1,0", ",", string_tokenizer::TOK_TRIM | string_tokenizer::TOK_IGNORE_EMPTY);

        REQUIRE (st.count() == 8);
        REQUIRE (2 == st.count("0"));
        REQUIRE (2 == st.count("1"));
        REQUIRE (2 == st.count("2"));
        REQUIRE (2 == st.count("3"));
        REQUIRE (0 == st.count("4"));
        REQUIRE (0 == st.count("5"));
        REQUIRE (0 == st.count("6"));
        REQUIRE (0 == st.count("7"));

        REQUIRE (st[0] == "0");
        REQUIRE (st[1] == "1");
        REQUIRE (st[2] == "2");
        REQUIRE (st[3] == "3");
        REQUIRE (st[4] == "3");
        REQUIRE (st[5] == "2");
        REQUIRE (st[6] == "1");
        REQUIRE (st[7] == "0");

        REQUIRE (st.has("0"));
        REQUIRE (st.has("1"));
        REQUIRE (st.has("2"));
        REQUIRE (st.has("3"));
        REQUIRE (!st.has("4"));
        REQUIRE (!st.has("5"));
        REQUIRE (!st.has("6"));
        REQUIRE (!st.has("7"));

        REQUIRE (st.find("0") == 0);
        REQUIRE (st.find("1") == 1);
        REQUIRE (st.find("2") == 2);
        REQUIRE (st.find("3") == 3);

        REQUIRE (st.find("0", 1) == 7);
        REQUIRE (st.find("1", 2) == 6);
        REQUIRE (st.find("2", 3) == 5);
        REQUIRE (st.find("3", 4) == 4);

        REQUIRE_THROWS_AS(st.find("4"), std::runtime_error);
        REQUIRE_THROWS_AS(st[8], std::runtime_error);


        st[0] = "1";
        st[7] = "1";
        REQUIRE (st[0] == "1");
        REQUIRE (st[7] == "1");
        REQUIRE (0 == st.count("0"));
        REQUIRE (4 == st.count("1"));

        st.replace("2", "5");
        REQUIRE (0 == st.count("2"));
        REQUIRE (2 == st.count("5"));

        st.replace("3", "6", 4);
        REQUIRE (1 == st.count("3"));
        REQUIRE (1 == st.count("6"));
    }
}
