#pragma once

#include <compiler/ast/all.h>
#include <compiler/parser/parser_combinator.h>
#include <variant>

namespace xi
{

// Return a Parser that matched the beginning of the input
constexpr auto str(std::string_view match) -> Parser auto
{
    return [match](std::string_view input) -> Parsed_t<std::string>
    {
        if (input.starts_with(match))
        {
            return std::make_pair(
                std::string(match), input.substr(match.size())
            );
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

// return a parser combinator that filter result of parser
template <typename Pr>
constexpr auto filter(Pr pred)
{
    return [pred]<typename T>(T c) -> Parser auto
    {
        return [pred, c](std::string_view input) -> Parsed_t<T>
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
    return satisfy(
        [x](char c)
        {
            return c == x;
        }
    );
}

constexpr auto op(std::string_view op) -> Parser auto
{
    return token(str(op)) >> [](auto op_s)
    {
        return unit(OpStr_To_Xi_Op(op_s));
    };
}

inline const Parser auto s_digit      = satisfy(::isdigit);
inline const Parser auto s_lower      = satisfy(::islower);
inline const Parser auto s_upper      = satisfy(::isupper);
inline const Parser auto s_alpha      = satisfy(::isalpha);
inline const Parser auto s_alphanum   = s_alpha || s_digit;
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

inline const Parser auto Xi_add    = op("+");
inline const Parser auto Xi_minus  = op("-");
inline const Parser auto Xi_mul    = op("*");
inline const Parser auto Xi_divide = op("/");
inline const Parser auto Xi_lt     = op("<");
inline const Parser auto Xi_le     = op("<=");
inline const Parser auto Xi_gt     = op(">");
inline const Parser auto Xi_ge     = op(">=");
inline const Parser auto Xi_eq     = op("==");
inline const Parser auto Xi_ne     = op("!=");
inline const Parser auto Xi_and    = op("&&");
inline const Parser auto Xi_or     = op("||");
inline const Parser auto Xi_not    = op("!");
inline const Parser auto Xi_dot    = op(".");

inline const Parser auto s_iden = token(some(s_alphanum || s_underscore));

const auto Xi_iden = s_iden >> filter(
                                   [](std::string_view s)
                                   {
                                       return !IsKeyWords(s);
                                   }
                               ) >>
                     [](auto name)
{
    return unit(Xi_Iden{
        .name = name,
        .expr = std::monostate{},
    });
};

const auto Xi_idenexpr = Xi_iden >> [](auto iden)
{
    return unit(Xi_Expr(iden));
};

// string literal
inline const Parser auto Xi_string = token(s_quote) > many(satisfy(
                                                          [](char c)
                                                          {
                                                              return c != '\"';
                                                          }
                                                      )) >>
                                     [](auto s)
{
    return s_quote > unit(Xi_Expr{Xi_String{s}});
};

} // namespace xi
