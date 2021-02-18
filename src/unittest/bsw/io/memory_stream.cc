//
// Created by igor on 17/02/2021.
//

#include <doctest.h>
#include <bsw/io/memory_stream_buf.hh>
#include <vector>
#include <sstream>

using namespace bsw::io;

TEST_SUITE ("memory stream")
{
    TEST_CASE ("input")
    {
        const char* data = "This is a test";
        memory_input_stream istr1(data, 14);

        int c = istr1.get();
        REQUIRE (c == 'T');
        c = istr1.get();
        REQUIRE (c == 'h');

        std::string str;
        istr1 >> str;
        REQUIRE (str == "is");

        char buffer[32];
        istr1.read(buffer, sizeof(buffer));
        REQUIRE (istr1.gcount() == 10);
        buffer[istr1.gcount()] = 0;
        REQUIRE (std::string(" is a test") == buffer);

        const char* data2 = "123";
        memory_input_stream istr2(data2, 3);
        c = istr2.get();
        REQUIRE (c == '1');
        REQUIRE (istr2.good());
        c = istr2.get();
        REQUIRE (c == '2');
        istr2.unget();
        c = istr2.get();
        REQUIRE (c == '2');
        REQUIRE (istr2.good());
        c = istr2.get();
        REQUIRE (c == '3');
        REQUIRE (istr2.good());
        c = istr2.get();
        REQUIRE (c == -1);
        REQUIRE (istr2.eof());
    }

    TEST_CASE ("output")
    {
        char output[64];
        memory_output_stream ostr1(output, 64);
        ostr1 << "This is a test " << 42 << std::ends;
        REQUIRE (ostr1.charsWritten() == 18);
        REQUIRE (std::string("This is a test 42") == output);

        char output2[4];
        memory_output_stream ostr2(output2, 4);
        ostr2 << "test";
        REQUIRE (ostr2.good());
        ostr2 << 'x';
        REQUIRE (ostr2.fail());
    }

    TEST_CASE ("tell")
    {
        std::vector<char> buffer(1024);
        memory_output_stream ostr(buffer.data(), buffer.size());
        ostr << 'H' << 'e' << 'l' << 'l' << 'o' << '\0';
        std::streamoff np = ostr.tellp();
        REQUIRE (np == 6);

        memory_input_stream istr(buffer.data(), buffer.size());

        char c;
        istr >> c;
        REQUIRE (c == 'H');

        istr >> c;
        REQUIRE (c == 'e');

        istr >> c;
        REQUIRE (c == 'l');

        std::streamoff ng = istr.tellg();
        REQUIRE (ng == 3);
    }


    TEST_CASE ("input seek")
    {
        std::vector<char> buffer(9);
        memory_output_stream ostr(buffer.data(), buffer.size());
        ostr << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        memory_input_stream istr(buffer.data(), buffer.size());
        char c;

        istr >> c;
        REQUIRE (c == '1');

        istr.seekg(3, std::ios_base::beg);    // 3 from beginning
        istr >> c;                            // now that makes 4
        REQUIRE (4 == istr.tellg());
        REQUIRE (c == '4');

        istr.seekg(2, std::ios_base::cur);    // now that makes 6
        istr >> c;                            // now that makes 7
        REQUIRE (7 == istr.tellg());
        REQUIRE (c == '7');

        istr.seekg(-7, std::ios_base::end);    // so that puts us at 9-7=2
        istr >> c;                            // now 3
        REQUIRE (3 == istr.tellg());
        REQUIRE (c == '3');


        istr.seekg(9, std::ios_base::beg);
        REQUIRE (istr.good());
        REQUIRE (9 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(10, std::ios_base::beg);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(0));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(-9, std::ios_base::end);
        REQUIRE (istr.good());
        REQUIRE (0 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(-10, std::ios_base::end);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(0));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(0, std::ios_base::beg);
        REQUIRE (istr.good());
        REQUIRE (0 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(-1, std::ios_base::beg);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(0));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(0, std::ios_base::end);
        REQUIRE (istr.good());
        REQUIRE (9 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(1, std::ios_base::end);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(0));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(3, std::ios_base::beg);
        REQUIRE (istr.good());
        REQUIRE (3 == istr.tellg());
        istr.seekg(6, std::ios_base::cur);
        REQUIRE (istr.good());
        REQUIRE (9 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(4, std::ios_base::beg);
            istr2.seekg(6, std::ios_base::cur);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(4));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(-4, std::ios_base::end);
        REQUIRE (istr.good());
        REQUIRE (5 == istr.tellg());
        istr.seekg(4, std::ios_base::cur);
        REQUIRE (istr.good());
        REQUIRE (9 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(-4, std::ios_base::end);
            istr2.seekg(5, std::ios_base::cur);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(5));
#else
        REQUIRE (istr2.fail());
#endif
        }


        istr.seekg(4, std::ios_base::beg);
        REQUIRE (istr.good());
        REQUIRE (4 == istr.tellg());
        istr.seekg(-4, std::ios_base::cur);
        REQUIRE (istr.good());
        REQUIRE (0 == istr.tellg());

        {
            memory_input_stream istr2(buffer.data(), buffer.size());
            istr2.seekg(4, std::ios_base::beg);
            istr2.seekg(-5, std::ios_base::cur);
#ifdef __APPLE__
            // workaround for clang libstdc++, which does not
        // set failbit if seek returns -1
        REQUIRE (istr2.fail() || istr2.tellg() == std::streampos(4));
#else
        REQUIRE (istr2.fail());
#endif
        }
    }

    TEST_CASE ("input seek vs stream")
    {
        std::vector<char> buffer(9);
        memory_output_stream ostr(buffer.data(), buffer.size());
        ostr << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        std::stringstream sss;
        sss << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        memory_input_stream mis(buffer.data(), buffer.size());

        char x, y;

        sss >> x;
        mis >> y;
        REQUIRE (x == y);

        sss.seekg(3, std::ios_base::beg);
        mis.seekg(3, std::ios_base::beg);
        sss >> x;
        mis >> y;
        REQUIRE (x == y);
        REQUIRE (sss.tellg() == mis.tellg());

        sss.seekg(2, std::ios_base::cur);
        mis.seekg(2, std::ios_base::cur);
        sss >> x;
        mis >> y;
        REQUIRE (x == y);
        REQUIRE (sss.tellg() == mis.tellg());

        sss.seekg(-7, std::ios_base::end);
        mis.seekg(-7, std::ios_base::end);
        sss >> x;
        mis >> y;
        REQUIRE (x == y);
        REQUIRE (sss.tellg() == mis.tellg());

    }

    TEST_CASE ("output seek")
    {
        std::vector<char> buffer(9);
        memory_output_stream ostr(buffer.data(), buffer.size());
        ostr << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        ostr.seekp(4, std::ios_base::beg);    // 4 from beginning
        ostr << 'a';                        // and that makes 5 (zero index 4)
        REQUIRE (5 == ostr.tellp());
        REQUIRE (buffer[4] == 'a');

        ostr.seekp(2, std::ios_base::cur);    // and this makes 7
        ostr << 'b';                        // and this makes 8 (zero index 7)
        REQUIRE (8 == ostr.tellp());
        REQUIRE (buffer[7] == 'b');

        ostr.seekp(-3, std::ios_base::end);    // 9-3=6 from the beginning
        ostr << 'c';                        // and this makes 7 (zero index 6)
        REQUIRE (7 == ostr.tellp());
        REQUIRE (buffer[6] == 'c');


        ostr.seekp(9, std::ios_base::beg);
        REQUIRE (ostr.good());
        REQUIRE (9 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(10, std::ios_base::beg);
                    REQUIRE (ostr2.fail());
        }


        ostr.seekp(-9, std::ios_base::end);
        REQUIRE (ostr.good());
        REQUIRE (0 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(-10, std::ios_base::end);
            REQUIRE (ostr2.fail());
        }


        ostr.seekp(0, std::ios_base::beg);
        REQUIRE (ostr.good());
        REQUIRE (0 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(-1, std::ios_base::beg);
            REQUIRE (ostr2.fail());
        }


        ostr.seekp(0, std::ios_base::end);
        REQUIRE (ostr.good());
        REQUIRE (9 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(1, std::ios_base::end);
            REQUIRE (ostr2.fail());
        }


        ostr.seekp(3, std::ios_base::beg);
        REQUIRE (ostr.good());
        REQUIRE (3 == ostr.tellp());
        ostr.seekp(6, std::ios_base::cur);
        REQUIRE (ostr.good());
        REQUIRE (9 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(4, std::ios_base::beg);
            ostr2.seekp(6, std::ios_base::cur);
            REQUIRE (ostr2.fail());
        }


        ostr.seekp(-4, std::ios_base::end);
        REQUIRE (ostr.good());
        REQUIRE (5 == ostr.tellp());
        ostr.seekp(4, std::ios_base::cur);
        REQUIRE (ostr.good());
        REQUIRE (9 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(-4, std::ios_base::end);
            ostr2.seekp(5, std::ios_base::cur);
            REQUIRE (ostr2.fail());
        }


        ostr.seekp(4, std::ios_base::beg);
        REQUIRE (ostr.good());
        REQUIRE (4 == ostr.tellp());
        ostr.seekp(-4, std::ios_base::cur);
        REQUIRE (ostr.good());
        REQUIRE (0 == ostr.tellp());

        {
            memory_output_stream ostr2(buffer.data(), buffer.size());
            ostr2.seekp(4, std::ios_base::beg);
            ostr2.seekp(-5, std::ios_base::cur);
            REQUIRE (ostr2.fail());
        }

    }

    TEST_CASE ("output seek vs stream")
    {
        std::vector<char> buffer(9);
        memory_output_stream mos(buffer.data(), buffer.size());
        mos << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        std::ostringstream oss;
        oss << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';

        mos.seekp(4, std::ios_base::beg);
        oss.seekp(4, std::ios_base::beg);
        mos << 'a';
        oss << 'a';
        REQUIRE (oss.str()[4] == 'a');
        REQUIRE (buffer[4] == oss.str()[4]);
        REQUIRE (oss.tellp() == mos.tellp());

        mos.seekp(2, std::ios_base::cur);
        oss.seekp(2, std::ios_base::cur);
        mos << 'b';
        oss << 'b';
        REQUIRE (oss.str()[7] == 'b');
        REQUIRE (buffer[7] == oss.str()[7]);
        REQUIRE (oss.tellp() == mos.tellp());

        mos.seekp(-3, std::ios_base::end);
        oss.seekp(-3, std::ios_base::end);
        mos << 'c';
        oss << 'c';
        REQUIRE (oss.str()[6] == 'c');
        REQUIRE (buffer[6] == oss.str()[6]);
        REQUIRE (oss.tellp() == mos.tellp());

        mos.seekp(-2, std::ios_base::cur);
        oss.seekp(-2, std::ios_base::cur);
        mos << 'd';
        oss << 'd';
        REQUIRE (oss.str()[5] == 'd');
        REQUIRE (buffer[5] == oss.str()[5]);
        REQUIRE (oss.tellp() == mos.tellp());

    }
}
