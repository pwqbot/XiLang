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

constexpr auto symbol(char x) -> Parser auto{
    return satisfy([x](char c) { return c == x; });
}

inline constexpr Parser auto s_digit      = satisfy(::isdigit);
inline constexpr Parser auto s_letter     = satisfy(::isalpha);
inline constexpr Parser auto s_space      = satisfy(::isspace);
inline const Parser auto     s_whitespace = many(s_space);
inline constexpr Parser auto s_lower      = satisfy(::islower);
inline constexpr Parser auto s_upper      = satisfy(::isupper);
inline constexpr Parser auto s_alpha      = s_letter || s_digit;
inline constexpr Parser auto s_alphanum   = s_alpha || s_space;
inline constexpr Parser auto s_plus       = symbol('+');

constexpr auto token(Parser auto parser) -> Parser auto{
    return s_whitespace > parser;
}

// string literal
inline const Parser auto
    Xi_string = token(str("\"")) > many(s_alphanum) >> [](auto s) {
        return str("\"") >> [s](auto) {
            return unit(Xi_Expr{Xi_String{.text{s}, .value{s}}});
        };
    };

inline const Parser auto Xi_true = token(str("true")) >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{.text{"true"}, .value{true}}});
};

inline const Parser auto Xi_false = token(str("false")) >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{.text{"false"}, .value{false}}});
};

inline const Parser auto Xi_boolean = Xi_true || Xi_false;

inline const Parser auto s_natural = token(some(s_digit));

inline const Parser auto Xi_integer = token(maybe(symbol('-'))) >> [](auto x) {
    return s_natural >> [x](auto nat) {
        if (x) {
            return unit(Xi_Expr{Xi_Integer{.text{std::string("-") + nat},
                                           .value{-std::stoi(nat)}}});
        }
        return unit(Xi_Expr{Xi_Integer{.text{nat}, .value{std::stoi(nat)}}});
    };
};

// real = integer "." integer
inline const Parser auto Xi_real = Xi_integer >> [](const Xi_Integer &integer) {
    return token(symbol('.')) > s_natural >> [integer](auto nat) {
        return unit(Xi_Expr{Xi_Real{
            .text{integer.text + "." + nat},
            .value{std::stod(std::to_string(integer.value) + "." + nat)}}});
    };
};

inline const Parser auto Xi_expr =
    Xi_true || Xi_false || Xi_integer || Xi_real || Xi_string;

} // namespace xi
