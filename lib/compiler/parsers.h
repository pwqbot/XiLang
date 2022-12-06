#include <compiler/ast.h>
#include <compiler/parser_combinator.h>

namespace xi {

// Return a Parser that matched the beginning of the input
constexpr auto str(std::string_view match) -> Parser auto{
    return [match](std::string_view input) -> Parsed_t<std::string_view> {
        if (input.starts_with(match)) {
            return std::make_pair(match, input.substr(match.size()));
        }
        return std::nullopt;
    };
}

// A parser that consume a char
constexpr auto item(std::string_view input) -> Parsed_t<char> {
    if (input.empty()) {
        return std::nullopt;
    }
    if (input.size() == 1) {
        return std::make_pair(input[0], "");
    }
    return std::make_pair(input[0], input.substr(1));
}

// constexpr auto dd = reduce_many{std::string{}, item("1"),
//                            [](auto acc, auto c) { return acc + c; }};

template <typename Pr, Parser P = decltype(item)>
constexpr auto satisfy(Pr pred, P parser = item) -> Parser auto{
    return parser >> [pred](auto c) -> Parser auto{
        return
            [pred, c](std::string_view input) -> Parsed_t<Parser_value_t<P>> {
                if (std::invoke(pred, c)) {
                    return {std::make_pair(c, input)};
                }
                return {};
            };
    };
}

inline constexpr Parser auto s_space      = satisfy(::isspace);
inline const Parser auto     s_whitespace = many(s_space);

constexpr auto token(Parser auto parser) -> Parser auto{
    return s_whitespace > parser;
}

constexpr auto symbol(char x) -> Parser auto{
    return token(satisfy([x](char c) { return c == x; }));
}

constexpr auto op(std::string_view s) -> Parser auto{
    return token(str(s)) >> [](auto s) {
        return unit(OpStr_To_Xi_Op(s));
    };
}

inline const Parser auto s_digit     = token(satisfy(::isdigit));
inline const Parser auto s_letter    = token(satisfy(::isalpha));
inline const Parser auto s_lower     = token(satisfy(::islower));
inline const Parser auto s_upper     = token(satisfy(::isupper));
inline const Parser auto s_alpha     = s_letter || s_digit;
inline const Parser auto s_alphanum  = s_alpha || s_space;
inline const Parser auto Xi_add      = op("+");
inline const Parser auto Xi_minus    = op("-");
inline const Parser auto Xi_mul      = op("*");
inline const Parser auto Xi_divide   = op("/");
inline const Parser auto s_lparen    = symbol('(');
inline const Parser auto s_rparen    = symbol(')');
inline const Parser auto s_lbracket  = symbol('[');
inline const Parser auto s_rbracket  = symbol(']');
inline const Parser auto s_lbrace    = symbol('{');
inline const Parser auto s_rbrace    = symbol('}');
inline const Parser auto s_semicolon = symbol(';');
inline const Parser auto s_comma     = symbol(',');
inline const Parser auto s_dot       = symbol('.');
inline const Parser auto s_colon     = symbol(':');
inline const Parser auto s_equals    = symbol('=');
inline const Parser auto s_backslash = symbol('\\');
inline const Parser auto s_bar       = symbol('|');
inline const Parser auto s_quote     = symbol('\"');

// string literal
inline const Parser auto Xi_string = s_quote > many(s_alphanum) >> [](auto s) {
    return s_quote >> [s](auto) {
        return unit(Xi_Expr{Xi_String{s}});
    };
};

inline const Parser auto Xi_true = token(str("true")) >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{true}});
};

inline const Parser auto Xi_false = token(str("false")) >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{false}});
};

inline const Parser auto Xi_boolean = Xi_true || Xi_false;

inline const Parser auto s_natural = token(some(s_digit));

inline const Parser auto Xi_integer = token(maybe(symbol('-'))) >> [](auto x) {
    return s_natural >> [x](auto nat) {
        if (x) {
            return unit(Xi_Expr{Xi_Integer{.value{-std::stoi(nat)}}});
        }
        return unit(Xi_Expr{Xi_Integer{.value{std::stoi(nat)}}});
    };
};

// real = integer "." integer
inline const Parser auto Xi_real = Xi_integer >> [](const Xi_Integer &integer) {
    return s_dot > s_natural >> [integer](auto nat) {
        return unit(Xi_Expr{Xi_Real{
            .value{std::stod(std::to_string(integer.value) + "." + nat)}}});
    };
};

// <expr> ::= <term> | <term> "+-" <term>
// <term> ::= <number> | <number> "*/" <number>
// <number> ::= <integer> | <real> | "(" expr ")"

auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr>;

auto Xi_number(std::string_view input) -> Parsed_t<Xi_Expr> {
    return (Xi_real || Xi_integer || (s_lparen > Xi_mathexpr >> [](auto expr) {
                return s_rparen >> [expr](auto) {
                    return unit(expr);
                };
            }))(input);
}

inline const Parser auto Xi_term =
    (Xi_number >>
     [](auto lhs) {
         return (Xi_mul || Xi_divide) >> [lhs](auto op) {
             return Xi_number >> [lhs, op](auto rhs) {
                 return unit(Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
             };
         };
     }) ||
    Xi_number;

auto Xi_mathexpr(std::string_view input) -> Parsed_t<Xi_Expr> {
    return ((Xi_term >>
             [](auto lhs) {
                 return (Xi_add || Xi_minus) >> [lhs](auto op) {
                     return Xi_term >> [lhs, op](auto rhs) {
                         return unit(
                             Xi_Expr{Xi_Binop{.lhs{lhs}, .rhs{rhs}, .op{op}}});
                     };
                 };
             }) ||
            Xi_term)(input);
}

const auto Xi_expr = Xi_true || Xi_false || Xi_string || Xi_mathexpr;

} // namespace xi
