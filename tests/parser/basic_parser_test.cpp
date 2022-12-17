
#include "test_header.h"

#include <compiler/parser/basic_parsers.h>

namespace xi
{

TEST_CASE("Parse Xi_Op", "[Xi_Op]")
{
    auto [op5, op6] = Xi_mul(" *abc").value();
    REQUIRE(op5 == Xi_Op::Mul);
    REQUIRE(op6 == "abc");
}

TEST_CASE("Parse digit", "[string]")
{
    SECTION("number")
    {
        auto [number, left] = s_digit("123").value();
        REQUIRE(number == '1');
        REQUIRE(left == "23");
    }

    SECTION("number")
    {
        auto digit = s_digit("abc");
        REQUIRE(digit == std::nullopt);
    }

    SECTION("whitespace")
    {
        auto digit = s_digit(" abc");
        REQUIRE(digit == std::nullopt);
    }
}

TEST_CASE("Parse space", "[string]")
{
    auto [result1, result2] = s_space(" 123").value();
    REQUIRE(result1 == ' ');
    REQUIRE(result2 == "123");

    auto result3 = s_space("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse letter", "[string]")
{
    auto [result1, result2] = s_alpha("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto result3 = s_alpha("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse alphanum", "[string]")
{
    auto [result1, result2] = s_alphanum("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto [result3, result4] = s_alphanum("123").value();
    REQUIRE(result3 == '1');
    REQUIRE(result4 == "23");
}

TEST_CASE("Parse symbol", "[string]")
{
    auto [result1, result2] = symbol('a')("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");
}

TEST_CASE("Parse token", "[string]")
{
    auto parser             = token(str("foo"));
    auto [result1, result2] = parser("  foo bar").value();
    REQUIRE(result1 == "foo");
    REQUIRE(result2 == " bar");
}

TEST_CASE("Parse String", "[Xi_String]")
{
    auto [result1, result2] = Xi_string("\"abcccb\"").value();
    REQUIRE(result1 == Xi_String{"abcccb"});
    REQUIRE(result2.empty());
    REQUIRE_THAT(
        Xi_string("\"abcccb\""), AstNodeMatcher(Xi_String{"abcccb"}, "")
    );
    REQUIRE_THAT(
        Xi_string("\"Hello, World\""),
        AstNodeMatcher(Xi_String{"Hello, World"}, "")
    );
}

TEST_CASE("Parse Xi_Iden", "[Xi_Iden]")
{
    auto [iden1, iden2] = Xi_iden("abc").value();
    REQUIRE(iden1 == Xi_Iden{"abc"});
    REQUIRE(iden2.empty());

    auto [iden3, iden4] = Xi_iden("abc123").value();
    REQUIRE(iden3 == Xi_Iden{"abc123"});
    REQUIRE(iden4.empty());

    auto [iden5, iden6] = Xi_iden("abc_123").value();
    REQUIRE(iden5 == Xi_Iden{"abc_123"});
    REQUIRE(iden6.empty());

    auto [iden7, iden8] = Xi_iden("abc 123").value();
    REQUIRE(iden7 == Xi_Iden{"abc"});
    REQUIRE(iden8 == " 123");
}

} // namespace xi
