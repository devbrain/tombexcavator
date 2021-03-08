//
// Created by igor on 06/03/2021.
//
#include <iostream>
#include <sstream>
#include <tomb-excavator/export/xml_writer.hh>
#include <doctest.h>
// ====================================================================================
TEST_CASE("Simple xml")
{
    std::ostringstream os;
    START_XML(os, false)
    {
        XML_NODE("a");
    }
    REQUIRE(os.str() == "<a />");
}
// -------------------------------------------------------------------------------------
TEST_CASE("Simple xml - flat nodes")
{
    std::ostringstream os;
    START_XML(os, false)
    {
        XML_NODE("a");
        XML_NODE("b");
    }
    REQUIRE(os.str() == "<a /><b />");
}
// -------------------------------------------------------------------------------------
TEST_CASE("Simple xml - flat and nesting nodes")
{
    std::ostringstream os;
    START_XML(os, false)
    {
        XML_NODE("a")
        {
            XML_NODE("b");
        }
        XML_NODE("c");
    }
    REQUIRE(os.str() == "<a><b /></a><c />");
}
// -------------------------------------------------------------------------------------
TEST_CASE("Simple xml - flat and nesting nodes 2")
{
    std::ostringstream os;
    START_XML(os, false)
    {
        XML_NODE("a")
        {
            XML_NODE("b")
            {
                XML_NODE("c");
                XML_NODE("d");
                XML_NODE("e")
                {
                    XML_NODE("f");
                }
                XML_NODE("g");
            }
        }
    }
    REQUIRE(os.str() == "<a><b><c /><d /><e><f /></e><g /></b></a>");
}
