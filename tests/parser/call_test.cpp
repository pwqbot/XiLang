#include "test_header.h"

#include <compiler/parser/call.h>

namespace xi
{

TEST_CASE("Parse function call", "[Xi_Call]")
{
    REQUIRE_THAT(
        Xi_call("func @ a b c"),
        AstNodeMatcher(
            Xi_Call{
                .name = "func",
                .args =
                    {
                        Xi_Iden{.name = "a", .expr = std::monostate{}},
                        Xi_Iden{.name = "b", .expr = std::monostate{}},
                        Xi_Iden{.name = "c", .expr = std::monostate{}},
                    },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func@"),
        AstNodeMatcher(
            Xi_Call{
                .name = "func",
                .args = {},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func@1 2 3"),
        AstNodeMatcher(
            Xi_Call{
                .name = "func",
                .args =
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
        Xi_call("func@1 + 1 2 3"),
        AstNodeMatcher(
            Xi_Call{
                .name = "func",
                .args =
                    {
                        Xi_Binop{
                            .lhs = Xi_Integer{1},
                            .rhs = Xi_Integer{1},
                            .op  = Xi_Op::Add,
                        },
                        Xi_Integer{2},
                        Xi_Integer{3},
                    },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("func@1 b true"),
        AstNodeMatcher(
            Xi_Call{
                .name = "func",
                .args =
                    {
                        Xi_Integer{1},
                        Xi_Iden{
                            .name = "b",
                            .expr = std::monostate{},
                        },
                        Xi_Boolean{true},
                    },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_call("printf@\" pow@2 10 = %d \" pow@ (check @ 2 == 3 1 2) 10"),
        AstNodeMatcher(
            Xi_Call{
                .name = "printf",
                .args =
                    {
                        Xi_String{" pow@2 10 = %d "},
                        Xi_Call{
                            .name = "pow",
                            .args =
                                {
                                    Xi_Call{
                                        .name = "check",
                                        .args =
                                            {
                                                Xi_Binop{
                                                    .lhs = Xi_Integer{2},
                                                    .rhs = Xi_Integer{3},
                                                    .op  = Xi_Op::Eq,
                                                },
                                                Xi_Integer{1},
                                                Xi_Integer{2},
                                            },
                                    },
                                    Xi_Integer{10},
                                },
                        },
                    },
            },
            ""
        )
    );
}

} // namespace xi
