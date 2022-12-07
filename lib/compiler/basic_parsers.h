#pragma once

#include <compiler/ast.h>
#include <compiler/parser_combinator.h>

namespace xi
{

// Return a Parser that matched the beginning of the input
constexpr auto str(std::string_view match) -> Parser auto
{
    return [match](std::string_view input) -> Parsed_t<std::string_view>
    {
        if (input.starts_with(match))
        {
            return std::make_pair(match, input.substr(match.size()));
        }
        return std::nullopt;
    };
}

// A parser that consume a char
constexpr auto item(std::string_view input) -> Parsed_t<char>
{
    if (input.empty())
    {
        return std::nullopt;
    }
    if (input.size() == 1)
    {
        return std::make_pair(input[0], "");
    }
    return std::make_pair(input[0], input.substr(1));
}

template <typename Pr, Parser P = decltype(item)>
constexpr auto satisfy(Pr pred, P parser = item) -> Parser auto
{
    return parser >> [pred](auto c) -> Parser auto
    {
        return [pred, c](std::string_view input) -> Parsed_t<Parser_value_t<P>>
        {
            if (std::invoke(pred, c))
            {
                return {std::make_pair(c, input)};
            }
            return {};
        };
    };
}

inline constexpr Parser auto s_space      = satisfy(::isspace);
inline const Parser auto     s_whitespace = many(s_space);

constexpr auto token(Parser auto parser) -> Parser auto
{
    return s_whitespace > parser;
}

constexpr auto symbol(char x) -> Parser auto
{
    return satisfy([x](char c) { return c == x; });
}

constexpr auto op(std::string_view s) -> Parser auto
{
    return token(str(s)) >> [](auto s)
    {
        return unit(OpStr_To_Xi_Op(s));
    };
}

inline const Parser auto s_digit      = satisfy(::isdigit);
inline const Parser auto s_lower      = satisfy(::islower);
inline const Parser auto s_upper      = satisfy(::isupper);
inline const Parser auto s_alpha      = satisfy(::isalpha);
inline const Parser auto s_alphanum   = s_alpha || s_digit;
inline const Parser auto Xi_add       = op("+");
inline const Parser auto Xi_minus     = op("-");
inline const Parser auto Xi_mul       = op("*");
inline const Parser auto Xi_divide    = op("/");
inline const Parser auto Xi_eq        = op("==");
inline const Parser auto Xi_lt        = op("<");
inline const Parser auto Xi_le        = op("<=");
inline const Parser auto Xi_gt        = op(">");
inline const Parser auto Xi_ge        = op(">=");
inline const Parser auto Xi_ne        = op("!=");
inline const Parser auto Xi_and       = op("&&");
inline const Parser auto Xi_or        = op("||");
inline const Parser auto Xi_not       = op("!");
inline const Parser auto s_lparen     = symbol('(');
inline const Parser auto s_rparen     = symbol(')');
inline const Parser auto s_lbracket   = symbol('[');
inline const Parser auto s_rbracket   = symbol(']');
inline const Parser auto s_lbrace     = symbol('{');
inline const Parser auto s_rbrace     = symbol('}');
inline const Parser auto s_semicolon  = symbol(';');
inline const Parser auto s_comma      = symbol(',');
inline const Parser auto s_dot        = symbol('.');
inline const Parser auto s_colon      = symbol(':');
inline const Parser auto s_equals     = symbol('=');
inline const Parser auto s_backslash  = symbol('\\');
inline const Parser auto s_bar        = symbol('|');
inline const Parser auto s_quote      = symbol('\"');
inline const Parser auto s_apostrophe = symbol('\'');
inline const Parser auto s_underscore = symbol('_');
inline const Parser auto s_hash       = symbol('#');
inline const Parser auto s_dollar     = symbol('$');
inline const Parser auto s_percent    = symbol('%');
inline const Parser auto s_ampersand  = symbol('&');
inline const Parser auto s_caret      = symbol('^');
inline const Parser auto s_tilde      = symbol('~');
inline const Parser auto s_at         = symbol('@');
inline const Parser auto s_question   = symbol('?');
inline const Parser auto s_arrow      = str("->");
inline const Parser auto s_if         = str("if");
inline const Parser auto s_then       = str("then");
inline const Parser auto s_else       = str("else");

// string literal
inline const Parser auto Xi_string = token(s_quote) >
                                     many(s_alphanum || s_space) >> [](auto s)
{
    return s_quote >> [s](auto)
    {
        return unit(Xi_Expr{Xi_String{s}});
    };
};

inline const Parser auto Xi_true = token(str("true")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{true}});
};

inline const Parser auto Xi_false = token(str("false")) >> [](auto)
{
    return unit(Xi_Expr{Xi_Boolean{false}});
};

inline const Parser auto Xi_boolean = Xi_true || Xi_false;

inline const Parser auto s_natural = some(s_digit);

inline const Parser auto Xi_integer = token(maybe(symbol('-'))) >> [](auto x)
{
    return token(s_natural) >> [x](auto nat)
    {
        if (x)
        {
            return unit(Xi_Expr{Xi_Integer{.value{-std::stoi(nat)}}});
        }
        return unit(Xi_Expr{Xi_Integer{.value{std::stoi(nat)}}});
    };
};

// real = integer "." integer
inline const Parser auto Xi_real = Xi_integer >> [](const Xi_Integer &integer)
{
    return token(s_dot) > token(s_natural) >> [integer](auto nat)
    {
        return unit(Xi_Expr{Xi_Real{
            .value{std::stod(std::to_string(integer.value) + "." + nat)}}});
    };
};

inline const Parser auto Xi_number = Xi_real || Xi_integer;

} // namespace xi
