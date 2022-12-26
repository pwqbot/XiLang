#pragma once

#include "compiler/ast/expr/expr.h"
#include "compiler/ast/expr/iden.h"
#include "compiler/ast/stmt/set.h"
#include "compiler/ast/stmt/func.h"
#include "compiler/ast/stmt/decl.h"
#include "compiler/ast/stmt/comment.h"

#include <variant>

namespace xi
{

struct Xi_While;
auto operator<=>(const Xi_While &lhs, const Xi_While &rhs)
    -> std::partial_ordering;

using Xi_Stmt = std::variant<
    Xi_Expr,
    Xi_Func,
    Xi_Decl,
    Xi_Set,
    Xi_Comment,
    recursive_wrapper<Xi_While>>;

auto TypeAssign(Xi_Stmt &stmt) -> TypeAssignResult;

} // namespace xi
