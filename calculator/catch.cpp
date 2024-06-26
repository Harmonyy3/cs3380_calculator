#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "calculator.cpp"

//update:
//either main () conflict with catch.cpp or 
//in catch.cpp putback() into full buffer

TEST_CASE("operation","calculator")
{

    SECTION("subtraction")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('-'));
        ts.putback(token(number, 1.7));
        ts.putback(token(number, 2.2));
        REQUIRE(expression() == -0.5);
    }
    SECTION("subtraction")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('-'));
        ts.putback(token(number, -1.7));
        ts.putback(token(number, 2.2));
        REQUIRE(expression() == -3.9);
    }
    SECTION("subtraction")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('-'));
        ts.putback(token(number, -1.7));
        ts.putback(token(number, -2.2));
        REQUIRE(expression() == -3.9);
    }
    SECTION("remiander")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('%'));
        ts.putback(token(number, 3));
        ts.putback(token(number, 1));
        REQUIRE(expression() == 0);
    }
    SECTION("remiander")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('%'));
        ts.putback(token(number, -4));
        ts.putback(token(number, 3));
        REQUIRE(expression() == -1);
    }
    SECTION("remiander")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('%'));
        ts.putback(token(number, 7));
        ts.putback(token(number, 0));
        REQUIRE_THROWS_AS(expression(), std::runtime_error);
    }
    SECTION("pre-defined value") 
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('e'));
        ts.putback(token(number, exp(3))); // e^3
        REQUIRE(expression() == 20.0855);
    }
    SECTION("pre-defined value")
    {
        token_stream ts;
        ts = token_stream();
        ts.putback(token('p'));
        ts.putback(token(number, 3));
        REQUIRE(expression() == 9.42478);
    }
}