//
// Created by igor on 18/02/2021.
//
#include <sstream>
#include <doctest.h>

#include "bsw/io/binary_reader.hh"
#include "bsw/io/binary_writer.hh"


using namespace bsw::io;

static void write(binary_writer& writer)
{
    writer << true;
    writer << false;
    writer << 'a';
    writer << (short) -100;
    writer << (unsigned short) 50000;
    writer << -123456;
    writer << (unsigned) 123456;
    writer << (long) -1234567890;
    writer << (unsigned long) 1234567890;


    writer << (int64_t) -1234567890;
	writer << (uint64_t) 1234567890;


    writer << (float) 1.5;
    writer << (double) -1.5;
}

static void read(binary_reader& reader)
{
    bool b;
    reader >> b;
    REQUIRE (b);
    reader >> b;
    REQUIRE (!b);

    char c;
    reader >> c;
    REQUIRE (c == 'a');

    short shortv;
    reader >> shortv;
    REQUIRE (shortv == -100);

    unsigned short ushortv;
    reader >> ushortv;
    REQUIRE (ushortv == 50000);

    int intv;
    reader >> intv;
    REQUIRE (intv == -123456);

    unsigned uintv;
    reader >> uintv;
    REQUIRE (uintv == 123456);

    long longv;
    reader >> longv;
    REQUIRE (longv == -1234567890);

    unsigned long ulongv;
    reader >> ulongv;
    REQUIRE (ulongv == 1234567890);


    int64_t int64v;
	reader >> int64v;
	REQUIRE (int64v == -1234567890);

	uint64_t uint64v;
	reader >> uint64v;
	REQUIRE (uint64v == 1234567890);


    float floatv;
    reader >> floatv;
    REQUIRE (floatv == 1.5);

    double doublev;
    reader >> doublev;
    REQUIRE (doublev == -1.5);
}

TEST_SUITE("binary reader writer")
{
    TEST_CASE("test native")
    {
        std::stringstream sstream;
        binary_writer writer(sstream);
        binary_reader reader(sstream);
        write(writer);
        read(reader);
    }

    TEST_CASE("test big endian")
    {
        std::stringstream sstream;
        binary_writer writer(sstream, binary_writer::BIG_ENDIAN_BYTE_ORDER);
        binary_reader reader(sstream, binary_reader::BIG_ENDIAN_BYTE_ORDER);
        write(writer);
        read(reader);
    }

    TEST_CASE("test little endian")
    {
        std::stringstream sstream;
        binary_writer writer(sstream, binary_writer::LITTLE_ENDIAN_BYTE_ORDER);
        binary_reader reader(sstream, binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
        write(writer);
        read(reader);
    }
}