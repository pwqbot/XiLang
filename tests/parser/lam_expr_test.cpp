#include "test_header.h"

#include <compiler/parser/lam_expr.h>

namespace xi
{

TEST_CASE("Parse Xi_Lam", "[Xi_Expr]")
{
    REQUIRE_THAT(
        Xi_lam("? x y -> (x + y + 1)"),
        AstNodeMatcher(
            Xi_Lam{
                {
                    Xi_Iden{
                        .name = "x",
                        .expr = std::monostate{},
                    },
                    Xi_Iden{
                        .name = "y",
                        .expr = std::monostate{},
                    },
                },
                Xi_Binop{
                    Xi_Iden{
                        .name = "x",
                        .expr = std::monostate{},
                    },
                    Xi_Binop{
                        Xi_Iden{
                            .name = "y",
                            .expr = std::monostate{},
                        },
                        Xi_Integer{1},
                        Xi_Op::Add,
                    },
                    Xi_Op::Add,
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_lam("? x -> (x + y + 1) - (5 * 2)"),
        AstNodeMatcher(
            Xi_Lam{
                {
                    Xi_Iden{.name = "x", .expr = std::monostate{}},
                },
                Xi_Binop{
                    Xi_Binop{
                        Xi_Iden{.name = "x", .expr = std::monostate{}},
                        Xi_Binop{
                            Xi_Iden{.name = "y", .expr = std::monostate{}},
                            Xi_Integer{1},
                            Xi_Op::Add,
                        },
                        Xi_Op::Add,
                    },
                    Xi_Binop{
                        Xi_Integer{5},
                        Xi_Integer{2},
                        Xi_Op::Mul,
                    },
                    Xi_Op::Sub,
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_lam("? x y -> ?z -> x + y + z"),
        AstNodeMatcher(
            Xi_Lam{
                {
                    Xi_Iden{.name = "x", .expr = std::monostate{}},
                    Xi_Iden{.name = "y", .expr = std::monostate{}},
                },
                Xi_Lam{
                    .args = {Xi_Iden{.name = "z", .expr = std::monostate{}}},
                    .body =
                        Xi_Binop{
                            Xi_Iden{.name = "x", .expr = std::monostate{}},
                            Xi_Binop{
                                Xi_Iden{.name = "y", .expr = std::monostate{}},
                                Xi_Iden{.name = "z", .expr = std::monostate{}},
                                Xi_Op::Add,
                            },
                            Xi_Op::Add,
                        },
                },
            },
            ""
        )
    );
}

} // namespace xi
