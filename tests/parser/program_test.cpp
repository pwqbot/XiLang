#include "test_header.h"

#include <compiler/parser/program.h>

namespace xi
{

TEST_CASE("Parse expr", "[Xi_Expr]")
{
    REQUIRE_THAT(
        Xi_expr("1 + 1"),
        AstNodeMatcher(
            Xi_Binop{
                Xi_Integer{1},
                Xi_Integer{1},
                Xi_Op::Add,
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_program("1 + 2\n"
                   "2 * 3\n"
                   "true\n"
                   "?x y -> (x + y - 1)\n"
                   "f(1 b true)"
                   ),
        AstNodeMatcher(
            Xi_Program{std::vector<Xi_Stmt>{
                Xi_Binop{
                    Xi_Integer{1},
                    Xi_Integer{2},
                    Xi_Op::Add,
                },
                Xi_Binop{
                    Xi_Integer{2},
                    Xi_Integer{3},
                    Xi_Op::Mul,
                },
                Xi_Boolean{true},
                Xi_Lam{
                    {
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                    },
                    Xi_Binop{
                        Xi_Iden{"x"},
                        Xi_Binop{
                            Xi_Iden{"y"},
                            Xi_Integer{1},
                            Xi_Op::Sub,
                        },
                        Xi_Op::Add,
                    },
                },
                Xi_Call{
                    Xi_Iden{"f"},
                    {
                        Xi_Integer{1},
                        Xi_Iden{"b"},
                        Xi_Boolean{true},
                    },
                },
            }},
            ""
        )
    );
}
} // namespace xi
