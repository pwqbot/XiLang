#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/program.h>

namespace xi
{

TEST_CASE("Parse program", "[Xi_Expr]")
{
    REQUIRE_THAT(
        Xi_program("fn test_array :: i64 -> arr[i64]\n"
                   "test_array x = let a = [1, 2, 3, 4, 5] in a[0]"),
        AstNodeMatcher(
            Xi_Program{
                {
                    Xi_Decl{
                        .name =
                            Xi_Iden{
                                .name = "test_array",
                                .expr = std::monostate{},
                            },
                        .return_type = "arr[i64]",
                        .params_type = {"i64"},
                    },
                    Xi_Func{
                        .name   = "test_array",
                        .params = {"x"},
                        .expr =
                            Xi_Expr{
                                Xi_ArrayIndex{
                                    .array_var_name = "a",
                                    .index          = Xi_Integer{0},
                                },
                            },
                        .let_idens =
                            {
                                Xi_Iden{
                                    .name = "a",
                                    .expr =
                                        Xi_Expr{
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
                    },
                },
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_program("fn add :: i64 -> i64 -> i64\n"
                   "add x y = x + y"),
        AstNodeMatcher(
            Xi_Program{std::vector<Xi_Stmt>{
                Xi_Decl{
                    Xi_Iden{.name = "add", .expr = std::monostate{}},
                    "i64",
                    {
                        "i64",
                        "i64",
                    },
                },
                Xi_Func{
                    .name = "add",
                    .params{
                        Xi_Iden{.name = "x", .expr = std::monostate{}},
                        Xi_Iden{.name = "y", .expr = std::monostate{}},
                    },
                    .expr =
                        Xi_Binop{
                            .lhs =
                                Xi_Iden{.name = "x", .expr = std::monostate{}},
                            .rhs =
                                Xi_Iden{.name = "y", .expr = std::monostate{}},
                            .op = Xi_Op::Add,
                        },
                },
            }},
            ""
        )
    );

    REQUIRE_THAT(
        Xi_program("fn add :: i64 -> i64 -> i64\n"
                   "add x y = x + y\n"
                   "fn add2 :: i64 -> i64 -> i64\n"
                   "add2 x y = (add @x y) + (add @x y)"),
        AstNodeMatcher(
            Xi_Program{std::vector<Xi_Stmt>{
                Xi_Decl{
                    .name        = "add",
                    .return_type = "i64",
                    .params_type =
                        {
                            "i64",
                            "i64",
                        },
                    .is_vararg = false,
                },
                Xi_Func{
                    .name = "add",
                    .params =
                        {
                            Xi_Iden{.name = "x", .expr = std::monostate{}},
                            Xi_Iden{.name = "y", .expr = std::monostate{}},
                        },
                    .expr =
                        Xi_Binop{
                            Xi_Iden{.name = "x", .expr = std::monostate{}},
                            Xi_Iden{.name = "y", .expr = std::monostate{}},
                            Xi_Op::Add,
                        },
                },
                Xi_Decl{
                    .name        = "add2",
                    .return_type = "i64",
                    .params_type =
                        {
                            "i64",
                            "i64",
                        },
                    .is_vararg = false,
                },
                Xi_Func{
                    .name = "add2",
                    .params =
                        {
                            Xi_Iden{.name = "x", .expr = std::monostate{}},
                            Xi_Iden{.name = "y", .expr = std::monostate{}},
                        },
                    .expr =
                        Xi_Binop{
                            Xi_Call{
                                .name = "add",
                                .args =
                                    {
                                        Xi_Iden{
                                            .name = "x",
                                            .expr = std::monostate{}},
                                        Xi_Iden{
                                            .name = "y",
                                            .expr = std::monostate{}},
                                    },
                            },
                            Xi_Call{
                                .name = "add",
                                .args =
                                    {
                                        Xi_Iden{
                                            .name = "x",
                                            .expr = std::monostate{}},
                                        Xi_Iden{
                                            .name = "y",
                                            .expr = std::monostate{}},
                                    },
                            },
                            Xi_Op::Add,
                        },
                },
            }},
            ""
        )
    );

    REQUIRE_THAT(
        Xi_program("main = { 1 + 2;\n"
                   "2 * 3;\n"
                   "true;\n"
                   "?x y -> (x + y - 1);\n"
                   "f @ 1 b true;\n"
                   "[1, 2, 3, 4, 5];}"),
        AstNodeMatcher(
            Xi_Program{
                std::vector<Xi_Stmt>{Xi_Func{
                    .name      = "main",
                    .params    = {},
                    .expr      = std::monostate{},
                    .let_idens = {},
                    .stmts =
                        {
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
                                .args =
                                    {
                                        Xi_Iden{
                                            .name = "x",
                                            .expr = std::monostate{}},
                                        Xi_Iden{
                                            .name = "y",
                                            .expr = std::monostate{}},
                                    },
                                .body =
                                    Xi_Binop{
                                        Xi_Iden{
                                            .name = "x",
                                            .expr = std::monostate{}},
                                        Xi_Binop{
                                            Xi_Iden{
                                                .name = "y",
                                                .expr = std::monostate{}},
                                            Xi_Integer{1},
                                            Xi_Op::Sub,
                                        },
                                        Xi_Op::Add,
                                    },
                            },
                            Xi_Call{
                                .name = "f",
                                .args =
                                    {
                                        Xi_Integer{1},
                                        Xi_Iden{
                                            .name = "b",
                                            .expr = std::monostate{}},
                                        Xi_Boolean{true},
                                    },
                            },
                            Xi_Array{
                                .size = 5,
                                .elements =
                                    {
                                        Xi_Integer{1},
                                        Xi_Integer{2},
                                        Xi_Integer{3},
                                        Xi_Integer{4},
                                        Xi_Integer{5},
                                    },
                            },

                        }}},
            },
            ""
        )
    );

    REQUIRE_THAT(
        Xi_program("main = { int a; a = 2; }"),
        AstNodeMatcher(Xi_Program{
            std::vector<Xi_Stmt>{
                Xi_Func{
                    .name      = "main",
                    .params    = {},
                    .expr      = std::monostate{},
                    .let_idens = {},
                    .stmts =
                        {
                            Xi_Var{
                                .name      = "a",
                                .value     = std::monostate{},
                                .type_name = "int",
                            },
                            Xi_Assign{
                                .name = "a",
                                .expr = Xi_Integer{2},
                            },
                        },
                },
            },
        })
    );

    REQUIRE_THAT(
        Xi_program("main = {while (a < b) {\n"
                   "a++;\n"
                   "}\n"
                   "for (i = 0; i < 10; i--){\n"
                   "a = 10;\n"
                   "}}"),
        AstNodeMatcher(Xi_Program{
            std::vector<Xi_Stmt>{
                Xi_Func{
                    .name      = "main",
                    .params    = {},
                    .expr      = std::monostate{},
                    .let_idens = {},
                    .stmts =
                        {
                            Xi_While{
                                .cond =
                                    Xi_Binop{
                                        Xi_Iden{
                                            .name = "a",
                                            .expr = std::monostate{}},
                                        Xi_Iden{
                                            .name = "b",
                                            .expr = std::monostate{}},
                                        Xi_Op::Lt,
                                    },
                                .body =
                                    {
                                        Xi_Expr{
                                            Xi_Assign{
                                                .name = "a",
                                                .expr =
                                                    Xi_Binop{
                                                        .lhs =
                                                            Xi_Iden{
                                                                .name = "a",
                                                                .expr = std::
                                                                    monostate{}},
                                                        .rhs = Xi_Integer{1},
                                                        .op  = Xi_Op::Add,
                                                    },
                                            },
                                        },
                                    },
                            },
                            Xi_Stmts{
                                {
                                    Xi_Assign{
                                        .name = "i",
                                        .expr = Xi_Integer{0},
                                    },
                                    Xi_While{
                                        .cond =
                                            Xi_Binop{
                                                Xi_Iden{
                                                    .name = "i",
                                                    .expr = std::monostate{}},
                                                Xi_Integer{10},
                                                Xi_Op::Lt,
                                            },
                                        .body =
                                            {
                                                Xi_Assign{
                                                    .name = "a",
                                                    .expr = Xi_Integer{10},
                                                },
                                                Xi_Assign{
                                                    .name = "i",
                                                    .expr =
                                                        Xi_Binop{
                                                            .lhs =
                                                                Xi_Iden{
                                                                    .name = "i",
                                                                    .expr = std::
                                                                        monostate{},
                                                                },
                                                            .rhs =
                                                                Xi_Integer{1},
                                                            .op = Xi_Op::Sub,
                                                        },
                                                },
                                            },
                                    },
                                },
                            },
                        },
                },
            },
        })
    );
}

} // namespace xi
