
#include "compiler/ast/ast.h"

#include <compiler/parser/basic_parsers.h>

namespace xi
{

// <iden> "." <iden>
inline const auto Xi_setGetM = s_iden >> [](std::string var_name)
{
    return token(symbol('.')) > s_iden >> [var_name](std::string member_name)
    {
        return unit(Xi_Expr{
            Xi_SetGetM{
                .set_var_name = var_name,
                .member_name  = member_name,
            },
        });
    };
};

} // namespace xi
