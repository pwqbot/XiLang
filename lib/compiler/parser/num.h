#pragma once

#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

inline const Parser auto s_natural = some(s_digit);

inline const Parser auto Xi_integer = token(maybe(symbol('-'))) >> [](auto x)
{
    return token(s_natural) >> [x](auto nat)
    {
        return unit(Xi_Expr{
            Xi_Integer{
                .value = x ? -std::stoi(nat) : std::stoi(nat),
            },
        });
    };
};

// real = integer "." integer
inline const Parser auto Xi_real = Xi_integer >> [](const Xi_Integer &integer)
{
    return token(s_dot) > token(s_natural) >> [integer](auto nat)
    {
        return unit(Xi_Expr{
            Xi_Real{
                .value = std::stod(std::to_string(integer.value) + "." + nat),
            },
        });
    };
};

inline const Parser auto Xi_number = Xi_real || Xi_integer;

} // namespace xi
