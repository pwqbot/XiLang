#include "test_header.h"

#include <compiler/parser/math_expr.h>

namespace xi
{

TEST_CASE("Parse boolean", "[Xi_Expr][Xi_Boolean]")
{
    auto [result1, result2] = Xi_boolean("trueabc").value();
    REQUIRE(result1 == Xi_Boolean{true});
    REQUIRE(result2 == "abc");

    auto [result3, result4] = Xi_boolean("falseabc").value();
    REQUIRE(result3 == Xi_Boolean{false});
    REQUIRE(result4 == "abc");
}

TEST_CASE("Parse boolexpr", "[Xi_Expr][Xi_Boolean]")
{
    auto [boolean1, boolean2] = Xi_mathexpr("true && false").value();
    REQUIRE(
        boolean1 ==
        Xi_Binop{
            Xi_Boolean{true},
            Xi_Boolean{false},
            Xi_Op::And,
        }
    );

    auto [boolean3, boolean4] = Xi_mathexpr("true || false").value();
    REQUIRE(
        boolean3 ==
        Xi_Binop{
            Xi_Boolean{true},
            Xi_Boolean{false},
            Xi_Op::Or,
        }
    );

    auto [boolean5, boolean6] = Xi_mathexpr("true && false || true").value();
    REQUIRE(
        boolean5 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Boolean{true},
                Xi_Boolean{false},
                Xi_Op::And,
            },
            Xi_Boolean{true},
            Xi_Op::Or,
        }
    );

    auto [boolean7, boolean8] = Xi_mathexpr("true || false && true").value();
    REQUIRE(
        boolean7 ==
        Xi_Binop{
            Xi_Boolean{true},
            Xi_Binop{
                Xi_Boolean{false},
                Xi_Boolean{true},
                Xi_Op::And,
            },
            Xi_Op::Or,
        }
    );

    auto [boolean9, boolean10] =
        Xi_mathexpr("true && false || true && false").value();
    REQUIRE(
        boolean9 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Boolean{true},
                Xi_Boolean{false},
                Xi_Op::And,
            },
            Xi_Binop{
                Xi_Boolean{true},
                Xi_Boolean{false},
                Xi_Op::And,
            },
            Xi_Op::Or,
        }
    );

    // test paren
    auto [boolean11, boolean12] =
        Xi_mathexpr("(true || false) && true").value();
    REQUIRE(
        boolean11 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Boolean{true},
                Xi_Boolean{false},
                Xi_Op::Or,
            },
            Xi_Boolean{true},
            Xi_Op::And,
        }
    );

    auto [boolean13, boolean14] =
        Xi_mathexpr("true && (false || true)").value();
    REQUIRE(
        boolean13 ==
        Xi_Binop{
            Xi_Boolean{true},
            Xi_Binop{
                Xi_Boolean{false},
                Xi_Boolean{true},
                Xi_Op::Or,
            },
            Xi_Op::And}
    );

    // test mathbool
    auto [boolean15, boolean16] = Xi_mathexpr("1 + 2 > 3 * 4").value();
    REQUIRE(
        boolean15 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Integer{1},
                Xi_Integer{2},
                Xi_Op::Add,
            },
            Xi_Binop{
                Xi_Integer{3},
                Xi_Integer{4},
                Xi_Op::Mul,
            },
            Xi_Op::Gt,
        }
    );

    auto [boolean17, boolean18] =
        Xi_mathexpr("(1 + (2 + 3) < 3 * 4) || true").value();
    REQUIRE(
        boolean17 ==
        Xi_Binop{
            Xi_Binop{
                Xi_Binop{
                    Xi_Integer{1},
                    Xi_Binop{
                        Xi_Integer{2},
                        Xi_Integer{3},
                        Xi_Op::Add,
                    },
                    Xi_Op::Add,
                },
                Xi_Binop{
                    Xi_Integer{3},
                    Xi_Integer{4},
                    Xi_Op::Mul,
                },
                Xi_Op::Lt},
            Xi_Boolean{true},
            Xi_Op::Or,
        }
    );

    REQUIRE_THAT(
        Xi_mathexpr("1 == 2"),
        AstNodeMatcher(
            Xi_Binop{
                Xi_Integer{1},
                Xi_Integer{2},
                Xi_Op::Eq,
            },
            ""
        )
    );
}

} // namespace xi
