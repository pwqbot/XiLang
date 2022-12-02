#include <catch2/catch_test_macros.hpp>
#include <compiler/parser.h>

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
