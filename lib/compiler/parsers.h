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


inline constexpr Parser auto P_digit    = satisfy(::isdigit);
inline constexpr Parser auto P_letter   = satisfy(::isalpha);
inline constexpr Parser auto P_space    = satisfy(::isspace);
inline constexpr Parser auto P_lower    = satisfy(::islower);
inline constexpr Parser auto P_upper    = satisfy(::isupper);
inline constexpr Parser auto P_alpha    = P_letter || P_digit;
inline constexpr Parser auto P_alphanum = P_alpha || P_space;
inline constexpr Parser auto plus       = symbol('+');
inline constexpr Parser auto P_true     = str("true");
inline constexpr Parser auto P_false    = str("false");

// whitespace is many space
inline const Parser auto whitespace = many(P_space);

constexpr auto token(Parser auto parser) -> Parser auto{
    return whitespace > parser;
}

inline const Parser auto natural = some(P_digit) >> [](auto digits) {
    return unit(std::stoi(digits));
};

inline const Parser auto integer =
    natural || (
                   symbol('-') >>
                   [](auto x) {
                       return natural;
                   } >>
                   [](int nat) { return unit(-nat); });

} // namespace xi
