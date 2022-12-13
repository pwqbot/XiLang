#pragma once

#include "compiler/parser/basic_parsers.h"
#include "compiler/parser/expr.h"

#include <utility>

namespace xi
{

const auto Xi_xi = token(str("xi")) > Xi_iden >> [](const Xi_Iden &iden)
{
    return token(s_equals) > Xi_expr >> [iden](Xi_Expr expr)
    {
        return unit(Xi_Stmt{Xi_Xi{iden, std::move(expr)}});
    };
};
} // namespace xi
