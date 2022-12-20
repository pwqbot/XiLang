#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/parser/program.h>

namespace xi
{

TEST_CASE("Parse program", "[Xi_Expr]")
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
        Xi_program("fn test_array :: i64 -> arr[i64]\n"
                   "test_array x = let a = [1, 2, 3, 4, 5] in a[0]"),
        AstNodeMatcher(
            Xi_Program{
                {
                    Xi_Decl{
                        .name        = Xi_Iden{"test_array"},
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
                    Xi_Iden{"add"},
                    "i64",
                    {
                        "i64",
                        "i64",
                    },
                },
                Xi_Func{
                    Xi_Iden{"add"},
                    {
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                    },
                    Xi_Binop{
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                        Xi_Op::Add,
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
                   "add2 x y = add(x y) + add (x y)"),
        AstNodeMatcher(
            Xi_Program{std::vector<Xi_Stmt>{
                Xi_Decl{
                    Xi_Iden{"add"},
                    "i64",
                    {
                        "i64",
                        "i64",
                    },
                    false,
                },
                Xi_Func{
                    Xi_Iden{"add"},
                    {
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                    },
                    Xi_Binop{
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                        Xi_Op::Add,
                    },
                },
                Xi_Decl{
                    Xi_Iden{"add2"},
                    "i64",
                    {
                        "i64",
                        "i64",
                    },
                    false,
                },
                Xi_Func{
                    Xi_Iden{"add2"},
                    {
                        Xi_Iden{"x"},
                        Xi_Iden{"y"},
                    },
                    Xi_Binop{
                        Xi_Call{
                            Xi_Iden{"add"},
                            {
                                Xi_Iden{"x"},
                                Xi_Iden{"y"},
                            },
                        },
                        Xi_Call{
                            Xi_Iden{"add"},
                            {
                                Xi_Iden{"x"},
                                Xi_Iden{"y"},
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
        Xi_program("1 + 2\n"
                   "2 * 3\n"
                   "true\n"
                   "?x y -> (x + y - 1)\n"
                   "f(1 b true)\n"
                   "[1, 2, 3, 4, 5]"),
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
                Xi_Array{
                    5,
                    {
                        Xi_Integer{1},
                        Xi_Integer{2},
                        Xi_Integer{3},
                        Xi_Integer{4},
                        Xi_Integer{5},
                    },
                },
            }},
            ""
        )
    );
}
} // namespace xi
