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

inline constexpr Parser auto Xi_digit    = satisfy(::isdigit);
inline constexpr Parser auto Xi_letter   = satisfy(::isalpha);
inline constexpr Parser auto Xi_space    = satisfy(::isspace);
inline constexpr Parser auto Xi_lower    = satisfy(::islower);
inline constexpr Parser auto Xi_upper    = satisfy(::isupper);
inline constexpr Parser auto Xi_alpha    = Xi_letter || Xi_digit;
inline constexpr Parser auto Xi_alphanum = Xi_alpha || Xi_space;
inline constexpr Parser auto Xi_plus     = symbol('+');

inline constexpr Parser auto Xi_true = str("true") >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{true}});
};

inline constexpr Parser auto Xi_false = str("false") >> [](auto) {
    return unit(Xi_Expr{Xi_Boolean{false}});
};

inline constexpr Parser auto Xi_boolean = Xi_true || Xi_false;

// whitespace is many space
inline const Parser auto Xi_whitespace = many(Xi_space);

constexpr auto token(Parser auto parser) -> Parser auto{
    return Xi_whitespace > parser;
}

inline const Parser auto Xi_natural = some(Xi_digit) >> [](auto digits) {
    return unit(Xi_Integer{std::stoi(digits)});
};

inline const Parser auto Xi_integer = maybe(symbol('-')) >> [](auto x) {
    return Xi_natural >> [x](auto nat) {
        if (x) {
            return unit(Xi_Expr{Xi_Integer{-nat.value}});
        }
        return unit(Xi_Expr{nat});
    };
};

inline const Parser auto Xi_expr = Xi_true || Xi_false || Xi_integer;

// literal = boolean | integer | real.
// auto literal = Xi_true || Xi_false || Xi_integer;
} // namespace xi
