//
// Created by igor on 04/04/2021.
//

#include <doctest.h>
#include <tomb-excavator/bsw/string_constant.hh>

TEST_CASE("String constants test")
{
    using namespace bsw;

    constexpr auto foo = StringFactory( "foo" );
    constexpr auto bar = StringFactory( "bar" );

    constexpr auto foobar = foo + bar;

    // Test cases as static_assert statements, change
    // any of these if you'd like to see the compile fail

    // Equality, both directions
    static_assert( foobar == "foobar", "Failure, is unacceptable" );
    static_assert( "foobar" == foobar, "Failure, is unacceptable" );

    // On the fly concat, and equality, both directions
    static_assert( ( foo + "bar" ) == "foobar", "Failure, is unacceptable" );
    static_assert( "foobar" == ( "foo" + bar ), "Failure, is unacceptable" );

    // Odds and ends
    static_assert( (foo + bar)[3] == 'b', "Failure, is unacceptable" );
    static_assert( (foo + bar).Length( ) == 6, "Failure, is unacceptable" );
}