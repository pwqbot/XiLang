#include "test_header.h"

#include <compiler/parser/func.h>

namespace xi
{

TEST_CASE("Parse Xi_func", "[Xi_Xi]")
{
    REQUIRE_THAT(
        Xi_func("x = 1"),
        AstNodeMatcher(
            Xi_Func{
                .name      = "x",
                .params    = {},
                .expr      = Xi_Integer{1},
                .let_idens = {},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_func("x = 1 + 3 + f @ 1 2 3"),
        AstNodeMatcher(
            Xi_Func{
                .name   = "x",
                .params = {},
                .expr =
                    Xi_Binop{
                        .lhs = Xi_Integer{1},
                        .rhs =
                            Xi_Binop{
                                .lhs = Xi_Integer{3},
                                .rhs =
                                    Xi_Call{
                                        .name = "f",
                                        .args =
                                            {
                                                Xi_Integer{1},
                                                Xi_Integer{2},
                                                Xi_Integer{3},
                                            },
                                    },
                                .op = Xi_Op::Add,
                            },
                        .op = Xi_Op::Add,
                    },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_func("func x y = x + y + 1"),
        AstNodeMatcher(
            Xi_Func{
                .name = "func",
                .params =
                    {
                        "x",
                        "y",
                    },
                .expr =
                    Xi_Binop{
                        .lhs = Xi_Iden{.name = "x", .expr = std::monostate{}},
                        .rhs =
                            Xi_Binop{
                                .lhs =
                                    Xi_Iden{
                                        .name = "y", .expr = std::monostate{}},
                                .rhs = Xi_Integer{1},
                                .op  = Xi_Op::Add,
                            },
                        .op = Xi_Op::Add,
                    },
                .let_idens = {},
            },
            ""
        )
    );
}

TEST_CASE("Parse assign", "[Parser][Xi_Func]")
{
    REQUIRE_THAT(
        Xi_let_assign("x = 1"),
        AstNodeMatcher(
            Xi_Iden{
                .name = "x",
                .expr = Xi_Integer{1},
            },
            ""
        )
    );
}

TEST_CASE("Parse let in", "[Parser][Xi_Func]")
{
    REQUIRE_THAT(
        Xi_let("let x = 1 in"),
        AstNodeMatcher(
            std::vector<Xi_Iden>{
                Xi_Iden{
                    .name = "x",
                    .expr = Xi_Integer{1},
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_let("let a = [1, 2, 3, 4, 5] in"),
        AstNodeMatcher(
            std::vector<Xi_Iden>{
                Xi_Iden{
                    .name = "a",
                    .expr =
                        Xi_Array{
                            .elements =
                                {
                                    Xi_Integer{1},
                                    Xi_Integer{2},
                                    Xi_Integer{3},
                                    Xi_Integer{4},
                                    Xi_Integer{5},
                                },
                        },
                },
            },
            ""
        )
    );
}

TEST_CASE("Parse Func with let", "[Parser][Xi_Func]")
{
    REQUIRE_THAT(
        Xi_func("func x y = let z = x + y in z + 1"),
        AstNodeMatcher(
            Xi_Func{
                .name = "func",
                .params =
                    {
                        "x",
                        "y",
                    },
                .expr = {Xi_Binop{
                    Xi_Iden{.name = "z", .expr = std::monostate{}},
                    Xi_Integer{1},
                    Xi_Op::Add,
                }},
                .let_idens =
                    {
                        Xi_Iden{
                            .name = "z",
                            .expr =
                                Xi_Binop{
                                    Xi_Iden{
                                        .name = "x", .expr = std::monostate{}},
                                    Xi_Iden{
                                        .name = "y", .expr = std::monostate{}},
                                    Xi_Op::Add,
                                },
                        },
                    },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_func("func x = let a = [1, 2, 3, 4, 5] in a[0]"),
        AstNodeMatcher(
            Xi_Func{
                .name = "func",
                .params =
                    {
                        "x",
                    },
                .expr =
                    {
                        Xi_ArrayIndex{
                            Xi_Iden{.name = "a", .expr = std::monostate{}},
                            Xi_Integer{0},
                        },
                    },
                .let_idens =
                    {
                        Xi_Iden{
                            .name = "a",
                            .expr =
                                Xi_Array{
                                    .elements =
                                        {
                                            Xi_Integer{1},
                                            Xi_Integer{2},
                                            Xi_Integer{3},
                                            Xi_Integer{4},
                                            Xi_Integer{5},
                                        },
                                },
                        },
                    },
            },
            ""
        )
    );
}

TEST_CASE("Parse decl func")
{
    REQUIRE_THAT(
        Xi_decl_func("func x y = {x + 1;}"),
        AstNodeMatcher(
            Xi_Func{
                .name = "func",
                .params =
                    {
                        "x",
                        "y",
                    },
                .expr      = std::monostate{},
                .let_idens = {},
                .stmts =
                    {
                        Xi_Stmt{
                            Xi_Binop{
                                Xi_Iden{.name = "x", .expr = std::monostate{}},
                                Xi_Integer{1},
                                Xi_Op::Add,
                            },
                        },
                    },
            },
            ""
        )
    );
        
    REQUIRE_THAT(
        Xi_func("func x y = {x + 1; x + 2;}"),
        AstNodeMatcher(
            Xi_Func{
                .name = "func",
                .params =
                    {
                        "x",
                        "y",
                    },
                .expr      = std::monostate{},
                .let_idens = {},
                .stmts =
                    {
                        Xi_Stmt{
                            Xi_Binop{
                                Xi_Iden{.name = "x", .expr = std::monostate{}},
                                Xi_Integer{1},
                                Xi_Op::Add,
                            },
                        },
                        Xi_Stmt{
                            Xi_Binop{
                                Xi_Iden{.name = "x", .expr = std::monostate{}},
                                Xi_Integer{2},
                                Xi_Op::Add,
                            },
                        },
                    },
            },
            ""
        )
    );
}

} // namespace xi
