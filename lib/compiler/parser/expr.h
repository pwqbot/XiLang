#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>;

} // namespace xi
