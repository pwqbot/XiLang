#include <compiler/parser/if_expr.h>
#include <compiler/parser/lam_expr.h>
#include <compiler/parser/math_expr.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (Xi_mathexpr)(input);
}

} // namespace xi
