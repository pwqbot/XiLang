#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/set.h>
#include <compiler/parser/math_expr.h>

namespace xi
{

TEST_CASE("Parse Xi_Set", "[Xi_DeclExpr]")
{
    REQUIRE_THAT(
        Xi_set("set a = ( (x i64) (y i64) )"),
        AstNodeMatcher(
            Xi_Set{
                "a",
                {{"x", "i64"}, {"y", "i64"}},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_set("set a = ( (x i64) (y i64) (z b) )"),
        AstNodeMatcher(
            Xi_Set{
                "a",
                {
                    {"x", "i64"},
                    {"y", "i64"},
                    {"z", "b"},
                },

            },
            ""
        )
    );
}

TEST_CASE("Parse get set member")
{
    REQUIRE_THAT(
        Xi_mathexpr("a.x"),
        AstNodeMatcher(
            Xi_Binop{
                .lhs = Xi_Iden{"a"},
                .rhs = Xi_Iden{"x"},
                .op  = Xi_Op::Dot,
            },
            ""
        )
    );
}

} // namespace xi
