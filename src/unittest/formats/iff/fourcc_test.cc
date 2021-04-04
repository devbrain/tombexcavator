//
// Created by igor on 04/04/2021.
//

#include <doctest.h>

#include <tomb-excavator/formats/iff/fourcc.hh>
#define FOURCC(A,B,C,D) ((A << 24) | (B << 16) | (C << 8) | D)

TEST_CASE("FourCC")
{
    using namespace formats::iff;
    constexpr chunk_type a = fourcc("FORM");
    static_assert(a.value() == FOURCC('F','O','R','M'), "");

    REQUIRE(a.to_string() == "FORM");
}