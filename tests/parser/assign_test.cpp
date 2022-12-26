#include "compiler/parser/assign.h"
#include "test_header.h"

namespace xi
{

TEST_CASE("Parse basic assign")
{
    REQUIRE_THAT(
        Xi_basic_assign("a = 1"),
        AstNodeMatcher(Xi_Assign{
            .name = "a",
            .expr = Xi_Expr{Xi_Integer{1}},
        })
    );

    REQUIRE_THAT(
        Xi_compound_assign("a += 1"),
        AstNodeMatcher(Xi_Assign{
            .name = "a",
            .expr =
                Xi_Expr{
                    Xi_Binop{
                        .lhs = Xi_Iden{.name = "a", .expr = std::monostate{}},
                        .rhs = Xi_Integer{1},
                        .op  = Xi_Op::Add,
                    },
                },
        })
    );
}

} // namespace xi
