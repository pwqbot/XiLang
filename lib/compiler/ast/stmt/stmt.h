#pragma once

#include "compiler/ast/expr/expr.h"
#include "compiler/ast/expr/iden.h"

#include <variant>

namespace xi
{

struct Xi_Set
{
    std::string                                      name;
    std::vector<std::pair<std::string, std::string>> members;
    type::Xi_Type                                    type = type::unknown{};
    auto operator<=>(const Xi_Set &) const                = default;
};

struct Xi_Decl
{
    std::string              name;
    std::string              return_type;
    std::vector<std::string> params_type;
    bool                     is_vararg = false;
    type::Xi_Type            type      = type::unknown{};
    auto                     operator<=>(const Xi_Decl &) const = default;
};

struct Xi_Func
{
    std::string              name;
    std::vector<std::string> params;
    Xi_Expr                  expr;
    type::Xi_Type            type      = type::unknown{};
    std::vector<Xi_Iden>     let_idens = {};
    auto                     operator<=>(const Xi_Func &rhs) const = default;
};

struct Xi_Comment
{
    std::string text;
    auto        operator<=>(const Xi_Comment &rhs) const = default;
};

using Xi_Stmt = std::variant<Xi_Expr, Xi_Func, Xi_Decl, Xi_Set, Xi_Comment>;

struct Xi_Program
{
    std::vector<Xi_Stmt> stmts;
    auto                 operator<=>(const Xi_Program &rhs) const = default;
};

auto TypeAssign(Xi_Set &set) -> TypeAssignResult;
auto TypeAssign(Xi_Decl &decl) -> TypeAssignResult;
auto TypeAssign(Xi_Func &func_def) -> TypeAssignResult;
auto TypeAssign(Xi_Comment &) -> TypeAssignResult;
auto TypeAssign(Xi_Stmt &stmt) -> TypeAssignResult;
auto TypeAssign(Xi_Program &program)
    -> ExpectedTypeAssign<std::vector<type::Xi_Type>>;

} // namespace xi
