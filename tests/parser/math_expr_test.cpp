#include "test_header.h"

#include <compiler/parser/math_expr.h>

namespace xi
{

TEST_CASE("Parse natural", "[string]")
{
    auto [result1, result2] = s_natural("123abc").value();
    REQUIRE(result1 == "123");
    REQUIRE(result2 == "abc");

    auto [result3, result4] = s_natural("123 123").value();
    REQUIRE(result3 == "123");
    REQUIRE(result4 == " 123");
}

TEST_CASE("Parse integer", "[Xi_Integer]")
{
    auto [result1, result2] = Xi_integer("123abc").value();
    REQUIRE(result1 == Xi_Integer{123});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_integer("-123abc").value();
    REQUIRE(result3 == Xi_Integer{-123});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Parse real", "[Xi_Real]")
{
    auto [result1, result2] = Xi_real("123.456abc").value();
    REQUIRE(result1 == Xi_Real{123.456});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_real("-123.456abc").value();
    REQUIRE(result3 == Xi_Real{-123.456});
    REQUIRE(result4 == "abc");

    auto result5 = Xi_real("123abc");
    REQUIRE(result5 == std::nullopt);
}

TEST_CASE("Parse mathexpr", "[Xi_MathExpr]")
{
    auto [integer1, integer2] = Xi_arithmeticexpr("1 + 2").value();
    REQUIRE(
        integer1 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Integer{2},
            Xi_Op::Add,
        }
    );

    // test multiply
    auto [integer3, integer4] = Xi_arithmeticexpr(" 1*2").value();
    REQUIRE(
        integer3 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Integer{2},
            Xi_Op::Mul,
        }
    );

    // test add and multiply
    auto [integer5, integer6] = Xi_arithmeticexpr(" 1 + 2 * 3").value();
    REQUIRE(
        integer5 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Binop{
                Xi_Integer{2},
                Xi_Integer{3},
                Xi_Op::Mul,
            },
            Xi_Op::Add,
        }
    );

    auto [integer7, integer8] = Xi_arithmeticexpr(" 1 + 2 + 3").value();
    REQUIRE(
        integer7 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Binop{
                Xi_Integer{2},
                Xi_Integer{3},
                Xi_Op::Add,
            },
            Xi_Op::Add,
        }
    );

    // test paren
    auto [integer9, integer10] = Xi_arithmeticexpr(" (1 + 2) * 3").value();
    REQUIRE(
        integer9 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Integer{1},
                Xi_Integer{2},
                Xi_Op::Add,
            },
            Xi_Integer{3},
            Xi_Op::Mul,
        }
    );

    auto [integer11, integer12] = Xi_arithmeticexpr(" (1 * 2) * 3 + 4").value();
    REQUIRE(
        integer11 ==
        Xi_Binop{
            .lhs =
                Xi_Binop{
                    .lhs =
                        Xi_Binop{
                            .lhs = Xi_Integer{1},
                            .rhs = Xi_Integer{2},
                            .op  = Xi_Op::Mul,
                        },
                    .rhs = Xi_Integer{3},
                    .op  = Xi_Op::Mul},
            .rhs = Xi_Integer{4},
            .op  = Xi_Op::Add,
        }
    );
}

TEST_CASE("Parse mathexpr with identify", "[Xi_MathExpr]")
{
    auto [integer1, integer2] = Xi_arithmeticexpr("a + 2").value();
    REQUIRE(
        integer1 ==
        Xi_Binop{
            .lhs =
                Xi_Iden{
                    .name = "a",
                    .expr = std::monostate{},
                },
            .rhs = Xi_Integer{2},
            .op  = Xi_Op::Add,
        }
    );

    auto [integer3, integer4] = Xi_arithmeticexpr("a + b").value();
    REQUIRE(
        integer3 ==
        Xi_Binop{
            .lhs = Xi_Iden{.name = "a", .expr = std::monostate{}},
            .rhs = Xi_Iden{.name = "b", .expr = std::monostate{}},
            .op  = Xi_Op::Add,
        }
    );

    REQUIRE_THAT(
        Xi_arithmeticexpr("(a_1 + b) * 3"),
        AstNodeMatcher(Xi_Binop{
            Xi_Binop{
                .lhs = Xi_Iden{.name = "a_1", .expr = std::monostate{}},
                .rhs = Xi_Iden{.name = "b", .expr = std::monostate{}},
                .op  = Xi_Op::Add,
            },
            Xi_Integer{3},
            Xi_Op::Mul,
        })
    );

    REQUIRE_THAT(
        Xi_arithmeticexpr(" (x + y - 1)"),
        AstNodeMatcher(Xi_Binop{
            .lhs = Xi_Iden{.name = "x", .expr = std::monostate{}},
            .rhs =
                Xi_Binop{
                    Xi_Iden{.name = "y", .expr = std::monostate{}},
                    Xi_Integer{1},
                    Xi_Op::Sub,
                },
            .op = Xi_Op::Add,
        })
    );
    REQUIRE(Xi_arithmeticexpr(" (x + y - 1)") == Xi_expr(" (x + y - 1)"));
}

} // namespace xi
