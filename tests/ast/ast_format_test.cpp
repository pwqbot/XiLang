#include <catch2/catch_test_macros.hpp>
#include <compiler/ast/ast.h>
#include <compiler/ast/ast_format.h>
#include <compiler/parser/utils.h>
#include <test_header.h>

namespace xi
{

// NOLINTBEGIN(cppcoreguidelines-*, readability*)
TEST_CASE("Format Xi_Integer", "[Xi_Integer]")
{
    auto result = Xi_Integer{123};
    REQUIRE(fmt::format("{}", result) == "Xi_Integer 123");

    auto result2 = Xi_Integer{-123};
    REQUIRE(fmt::format("{}", result2) == "Xi_Integer -123");
}

TEST_CASE("Format Xi_Real", "[Xi_Real]")
{
    auto result = Xi_Real{123.456};
    REQUIRE(fmt::format("{}", result) == "Xi_Real 123.456");
}

TEST_CASE("Format Xi_Boolean", "[Xi_Boolean]")
{
    auto result = Xi_Boolean{true};
    REQUIRE(fmt::format("{}", result) == "Xi_Boolean true");

    auto result2 = Xi_Boolean{false};
    REQUIRE(fmt::format("{}", result2) == "Xi_Boolean false");
}

TEST_CASE("Format Xi_String", "[Xi_String]")
{
    auto result = Xi_String{"hello"};
    REQUIRE(fmt::format("{}", result) == "Xi_String \"hello\"");
}

TEST_CASE("Format recursive_wrapper", "[recursive_wrapper]")
{
    auto result = recursive_wrapper{Xi_Integer{123}};
    REQUIRE(fmt::format("{}", result) == "Xi_Integer 123");
}

TEST_CASE("Format Xi_Unop", "[Xi_Unop]")
{
    {
        auto result = Xi_Unop{Xi_Integer{123}, Xi_Op::Add};
        REQUIRE(
            fmt::format("{}", result) == "Xi_Unop +\n"
                                         "\tXi_Integer 123"
        );
    }
}

TEST_CASE("Format Xi_Binop", "[Xi_Binop]")
{
    {
        auto result = Xi_Binop{Xi_Integer{123}, Xi_Integer{456}, Xi_Op::Add};
        REQUIRE(
            fmt::format("{}", result) == "Xi_Binop +\n"
                                         "\tXi_Integer 123\n"
                                         "\tXi_Integer 456"
        );
    }

    {
        auto result = Xi_Binop{
            Xi_Integer{123},
            Xi_Binop{Xi_Integer{123}, Xi_Integer{456}, Xi_Op::Add},
            Xi_Op::Sub};
        REQUIRE(
            fmt::format("{}", result) == "Xi_Binop -\n"
                                         "\tXi_Integer 123\n"
                                         "\tXi_Binop +\n"
                                         "\t\tXi_Integer 123\n"
                                         "\t\tXi_Integer 456"
        );
    }

    {
        auto resut = Xi_Binop{
            Xi_Unop{Xi_Integer{123}, Xi_Op::Add},
            Xi_Unop{
                Xi_Binop{Xi_Integer{123}, Xi_Integer{456}, Xi_Op::Add},
                Xi_Op::Sub},
            Xi_Op::Add,
        };
        REQUIRE(
            fmt::format("{}", resut) == "Xi_Binop +\n"
                                        "\tXi_Unop +\n"
                                        "\t\tXi_Integer 123\n"
                                        "\tXi_Unop -\n"
                                        "\t\tXi_Binop +\n"
                                        "\t\t\tXi_Integer 123\n"
                                        "\t\t\tXi_Integer 456"
        );
    };
}

TEST_CASE("Format Xi_If", "[Xi_If]")
{
    {
        auto result = Xi_If{Xi_Boolean{true}, Xi_Integer{456}, Xi_Integer{789}};
        REQUIRE(
            fmt::format("{}", result) == "Xi_If\n"
                                         "\tXi_Boolean true\n"
                                         "\tXi_Integer 456\n"
                                         "\tXi_Integer 789"
        );
    }

    {
        auto result = Xi_If{
            Xi_Boolean{true},
            Xi_Unop{Xi_Integer{456}, Xi_Op::Add},
            Xi_If{Xi_Boolean{true}, Xi_Integer{456}, Xi_Integer{789}}};
        REQUIRE(
            fmt::format("{}", result) == "Xi_If\n"
                                         "\tXi_Boolean true\n"
                                         "\tXi_Unop +\n"
                                         "\t\tXi_Integer 456\n"
                                         "\tXi_If\n"
                                         "\t\tXi_Boolean true\n"
                                         "\t\tXi_Integer 456\n"
                                         "\t\tXi_Integer 789"
        );
    }
}

TEST_CASE("Format Xi_Iden", "[Xi_Iden]")
{
    {
        auto result = Xi_Iden{"hello"};
        REQUIRE(fmt::format("{}", result) == "Xi_Iden hello unknown");
    }
}

TEST_CASE("Format Xi_Lam", "[Xi_Lam]")
{
    {
        auto result = Xi_Lam{{Xi_Iden{"x"}}, Xi_Integer{456}};
        REQUIRE(
            fmt::format("{}", result) == "Xi_Lam\n"
                                         "\tA Xi_Iden x unknown\n"
                                         "\tB Xi_Integer 456"
        );
    }

    {
        auto result =
            Xi_Lam{{Xi_Iden{"x"}, Xi_Iden{"y"}, Xi_Iden{"z"}}, Xi_Integer{456}};
        REQUIRE(
            fmt::format("{}", result) ==
            "Xi_Lam\n"
            "\tA Xi_Iden x unknown, Xi_Iden y unknown, Xi_Iden z unknown\n"
            "\tB Xi_Integer 456"
        );
    }
}
// NOLINTEND(cppcoreguidelines-*, readability*)

} // namespace xi
