#include "test_header.h"

#include <compiler/parser/call.h>

namespace xi
{

TEST_CASE("Parser Xi_Lam call", "[Xi_CallExpr]")
{
    REQUIRE_THAT(
        Xi_call("func(a b c)"),
        AstNodeMatcher(
            Xi_Call{
                Xi_Iden{"func"},
                {
                    Xi_Iden{"a"},
                    Xi_Iden{"b"},
                    Xi_Iden{"c"},
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func()"),
        AstNodeMatcher(
            Xi_Call{
                Xi_Iden{"func"},
                {},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func(1 2 3)"),
        AstNodeMatcher(
            Xi_Call{
                Xi_Iden{"func"},
                {
                    Xi_Integer{1},
                    Xi_Integer{2},
                    Xi_Integer{3},
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func(1 + 1 2 3)"),
        AstNodeMatcher(
            Xi_Call{
                Xi_Iden{"func"},
                {
                    Xi_Binop{
                        Xi_Integer{1},
                        Xi_Integer{1},
                        Xi_Op::Add,
                    },
                    Xi_Integer{2},
                    Xi_Integer{3},
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func(1 b true)"),
        AstNodeMatcher(
            Xi_Call{
                Xi_Iden{"func"},
                {
                    Xi_Integer{1},
                    Xi_Iden{"b"},
                    Xi_Boolean{true},
                },
            },
            ""
        )
    );
}

} // namespace xi
