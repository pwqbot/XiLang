#pragma once
#include <compiler/ast/ast.h>
#include <compiler/parser/expr.h>
#include <compiler/parser/func.h>
#include <compiler/parser/decl.h>
#include <string_view>
#include <vector>

namespace xi
{

auto Xi_exprStmt = Xi_expr >> [](auto expr)
{
    return unit(Xi_Stmt{expr});
};

auto Xi_stmt(std::string_view input)
{
    return (Xi_decl || Xi_func || Xi_exprStmt)(input);
}

const auto Xi_program = many(Xi_stmt) >> [](auto progam)
{
    return unit(Xi_Program{progam});
};

} // namespace xi
