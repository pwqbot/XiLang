#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/expr/expr.h"
#include "compiler/ast/expr/iden.h"

#include <string>
#include <vector>

namespace xi
{

struct Xi_Func
{
    std::string              name;
    std::vector<std::string> params;
    Xi_Expr                  expr;
    type::Xi_Type            type      = type::unknown{};
    std::vector<Xi_Iden>     let_idens = {};
    auto                     operator<=>(const Xi_Func &rhs) const = default;
};

auto TypeAssign(Xi_Func &func_def) -> TypeAssignResult;

} // namespace xi
