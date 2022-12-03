#include <catch2/catch_test_macros.hpp>
#include <compiler/parsers.h>

namespace xi {

TEST_CASE("Test item") {
    auto [result1, result2] = item("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");
}

TEST_CASE("Test str") {
    auto [result1, result2] = str("abc")("abcdef").value();
    REQUIRE(result1 == "abc");
    REQUIRE(result2 == "def");
}

TEST_CASE("Test ||") {
    auto parser             = str("abc") || str("def");
    auto [result1, result2] = parser("abcdef").value();
    REQUIRE(result1 == "abc");
    REQUIRE(result2 == "def");

    auto [result3, result4] = parser("defghi").value();
    REQUIRE(result3 == "def");
    REQUIRE(result4 == "ghi");
}

TEST_CASE("Test >>") {
    auto parser = str("abc") >> [](auto) {
        return str("def");
    };
    auto [result1, result2] = parser("abcdef").value();
    REQUIRE(result1 == "def");
    REQUIRE(result2.empty());
}

TEST_CASE("Test Satisfy") {
    auto parser             = satisfy([](char c) { return c == 'a'; });
    auto [result1, result2] = parser("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto result3 = parser("def");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse digit") {
    auto [result1, result2] = P_digit("123").value();
    REQUIRE(result1 == '1');
    REQUIRE(result2 == "23");

    auto result3 = P_digit("abc");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse space") {
    auto [result1, result2] = P_space(" 123").value();
    REQUIRE(result1 == ' ');
    REQUIRE(result2 == "123");

    auto result3 = P_space("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse letter") {
    auto [result1, result2] = P_letter("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto result3 = P_letter("123");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Parse alphanum") {
    auto [result1, result2] = P_alphanum("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");

    auto [result3, result4] = P_alphanum("123").value();
    REQUIRE(result3 == '1');
    REQUIRE(result4 == "23");
}

TEST_CASE("Parse symbol") {
    auto [result1, result2] = symbol('a')("abc").value();
    REQUIRE(result1 == 'a');
    REQUIRE(result2 == "bc");
}

TEST_CASE("Test many") {
    auto parser             = many(symbol('+'));
    auto [result1, result2] = parser("+++cabcabc").value();
    REQUIRE(result1 == "+++");
    REQUIRE(result2 == "cabcabc");

    auto [result3, result4] = parser("cabcabc").value();
    REQUIRE(result3.empty());
    REQUIRE(result4 == "cabcabc");
}

TEST_CASE("Test token") {
    auto parser             = token(str("foo"));
    auto [result1, result2] = parser("  foo bar").value();
    REQUIRE(result1 == "foo");
    REQUIRE(result2 == " bar");
}

TEST_CASE("Test some") {
    auto parser             = some(symbol('+'));
    auto [result1, result2] = parser("+++cabcabc").value();
    REQUIRE(result1 == "+++");
    REQUIRE(result2 == "cabcabc");

    auto result3 = parser("cabcabc");
    REQUIRE(result3 == std::nullopt);
}

TEST_CASE("Test natural") {
    auto [result1, result2] = natural("123abc").value();
    REQUIRE(result1 == 123);
    REQUIRE(result2 == "abc");
}

TEST_CASE("Test integer") {
    auto [result1, result2] = integer("123abc").value();
    REQUIRE(result1 == 123);
    REQUIRE(result2 == "abc");

    auto [result3, result4] = integer("-123abc").value();
    REQUIRE(result3 == -123);
    REQUIRE(result4 == "abc");
}

} // namespace xi
