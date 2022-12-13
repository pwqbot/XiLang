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
    auto [integer1, integer2] = Xi_mathexpr("1 + 2").value();
    REQUIRE(
        integer1 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Integer{2},
            Xi_Op::Add,
        }
    );

    // test multiply
    auto [integer3, integer4] = Xi_mathexpr(" 1*2").value();
    REQUIRE(
        integer3 ==
        Xi_Binop{
            Xi_Integer{1},
            Xi_Integer{2},
            Xi_Op::Mul,
        }
    );

    // test add and multiply
    auto [integer5, integer6] = Xi_mathexpr(" 1 + 2 * 3").value();
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

    auto [integer7, integer8] = Xi_mathexpr(" 1 + 2 + 3").value();
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
    auto [integer9, integer10] = Xi_mathexpr(" (1 + 2) * 3").value();
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

    auto [integer11, integer12] = Xi_mathexpr(" (1 * 2) * 3 + 4").value();
    REQUIRE(
        integer11 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Binop{
                    Xi_Integer{1},
                    Xi_Integer{2},
                    Xi_Op::Mul,
                },
                Xi_Integer{3},
                Xi_Op::Mul},
            Xi_Integer{4},
            Xi_Op::Add,
        }
    );
}

TEST_CASE("Parse mathexpr with identify", "[Xi_MathExpr]")
{
    auto [integer1, integer2] = Xi_mathexpr("a + 2").value();
    REQUIRE(
        integer1 ==
        Xi_Binop{
            Xi_Iden{"a"},
            Xi_Integer{2},
            Xi_Op::Add,
        }
    );

    auto [integer3, integer4] = Xi_mathexpr("a + b").value();
    REQUIRE(
        integer3 ==
        Xi_Binop{
            Xi_Iden{"a"},
            Xi_Iden{"b"},
            Xi_Op::Add,
        }
    );

    REQUIRE_THAT(
        Xi_mathexpr("(a_1 + b) * 3"),
        AstNodeMatcher(
            Xi_Binop{
                Xi_Binop{
                    Xi_Iden{"a_1"},
                    Xi_Iden{"b"},
                    Xi_Op::Add,
                },
                Xi_Integer{3},
                Xi_Op::Mul,
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_mathexpr(" (x + y - 1)"),
        AstNodeMatcher(
            Xi_Binop{Xi_Binop{
                Xi_Iden{"x"},
                Xi_Binop{
                    Xi_Iden{"y"},
                    Xi_Integer{1},
                    Xi_Op::Sub,
                },
                Xi_Op::Add,
            }},
            ""
        )
    );
    REQUIRE(Xi_mathexpr(" (x + y - 1)") == Xi_expr(" (x + y - 1)"));
}

} // namespace xi
