#include <compiler/parser/if_expr.h>
#include <compiler/parser/lam_expr.h>
#include <compiler/parser/math_expr.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline auto Xi_expr(std::string_view input) -> Parsed_t<Xi_Expr>
{
    return (
        Xi_bool_conjunction >>
        [](Xi_Expr lhs)
        {
            return some(
                       combine_to_unop(Xi_or, Xi_bool_conjunction), binop_fold
                   ) >> [lhs](Xi_Expr rhs)
            {
                return Xi_binop_fold_go(lhs, rhs);
            } || unit(lhs);
        }
    )(input);
}

} // namespace xi
