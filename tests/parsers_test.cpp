#include <catch2/catch_test_macros.hpp>
#include <compiler/parsers.h>

namespace xi {

TEST_CASE("Parse digit", "[string]") {
    auto [result1, result2] = s_digit("123").value();
    REQUIRE(result1 == '1');
    REQUIRE(result2 == "23");

    auto result3 = s_digit("abc");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse space", "[string]") {
    auto [result1, result2] = s_space(" 123").value();
    REQUIRE(result1 == ' ');
    REQUIRE(result2 == "123");

    auto result3 = s_space("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse letter", "[string]") {
    auto [result1, result2] = s_letter("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto result3 = s_letter("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse alphanum", "[string]") {
    auto [result1, result2] = s_alphanum("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto [result3, result4] = s_alphanum("123").value();
    REQUIRE(result3 == '1');
    REQUIRE(result4 == "23");
}

TEST_CASE("Parse symbol", "[string]") {
    auto [result1, result2] = symbol('a')("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");
}

TEST_CASE("Test token", "[string]") {
    auto parser             = token(str("foo"));
    auto [result1, result2] = parser("  foo bar").value();
    REQUIRE(result1 == "foo");
    REQUIRE(result2 == " bar");
}

TEST_CASE("Test natural", "[string]") {
    auto [result1, result2] = s_natural("123abc").value();
    REQUIRE(result1 == "123");
    REQUIRE(result2 == "abc");
}

TEST_CASE("Test integer", "[integer]") {
    auto [result1, result2] = Xi_integer("123abc").value();
    REQUIRE(result1 == Xi_Integer{"123", 123});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_integer("-123abc").value();
    REQUIRE(result3 == Xi_Integer{"-123", -123});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Test boolean", "[boolean]") {
    auto [result1, result2] = Xi_boolean("trueabc").value();
    REQUIRE(result1 == Xi_Boolean{"true", true});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_boolean("falseabc").value();
    REQUIRE(result3 == Xi_Boolean{"false", false});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Test real", "[real]") {
    auto [result1, result2] = Xi_real("123.456abc").value();
    REQUIRE(result1 == Xi_Real{"123.456", 123.456});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_real("-123.456abc").value();
    REQUIRE(result3 == Xi_Real{"-123.456", -123.456});
    REQUIRE(result4 == "abc");

    auto result5 = Xi_real("123abc");
    REQUIRE(result5 == std::nullopt);
}

} // namespace xi
