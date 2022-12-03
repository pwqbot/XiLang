#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

// A parser of Things
// Is a function from strings
// To an optional pair
// of Things and strings

template <typename T>
using Parsed_t = std::optional<std::pair<T, std::string_view>>;

template <typename P>
using Parser_result_t = std::invoke_result_t<P, std::string_view>;

template <typename P>
using Parser_value_t = typename Parser_result_t<P>::value_type::first_type;

// P(std::string_view) -> Parsed_t<T>
template <typename P>
concept Parser =
    requires(std::invoke_result_t<P, std::string_view> result) {
        std::regular_invocable<P, std::string_view>;
        std::same_as<
            decltype(result),
            Parsed_t<typename decltype(result)::value_type::first_type>>;
    };

// function that return a parser
template <typename F, typename... Args>
concept Parser_combinator = std::regular_invocable<F, Args...> &&
                            Parser<std::invoke_result_t<F, Args...>>;

template <typename F, typename... Args>
    requires Parser_combinator<F, Args...>
using Parser_combinator_value_t = std::invoke_result_t<F, Args...>;

// Return a Parser that matched the beginning of the input
auto str(std::string_view match) -> Parser auto{
    return [match](std::string_view input) -> Parsed_t<std::string_view> {
        if (input.starts_with(match)) {
            return std::make_pair(match, input.substr(match.size()));
        }
        return std::nullopt;
    };
}

// Return a Parser that returns an instance of T and the unconsumed input
// return in monad
template <typename T>
constexpr auto unit(T thing) -> Parser auto{
    return [thing](std::string_view input) -> Parsed_t<T> {
        return std::make_pair(thing, input);
    };
}

// A parser that consume a char
constexpr auto item(std::string_view input) -> Parsed_t<char> {
    return std::make_pair(input[0], input.substr(1));
}

// Try one Parser and invoke a second Parser if the first one fails
template <Parser P, Parser Q>
constexpr auto operator||(P p, Q q) -> Parser auto
    requires std::convertible_to<Parser_value_t<P>, Parser_value_t<Q>>
{
    return [p, q](std::string_view input) -> Parser_result_t<P> {
        if (auto result = std::invoke(p, input)) {
            return result;
        }
        return std::invoke(q, input);
    };
}

// monadic action
template <Parser P, Parser_combinator<Parser_value_t<P>> F>
constexpr Parser auto operator>>(P Parser, F func) {
    using Parser_t = Parser_combinator_value_t<F, Parser_value_t<P>>;
    return [=](std::string_view input) -> Parser_result_t<Parser_t> {
        if (auto result = std::invoke(Parser, input)) {
            return std::invoke(std::invoke(func, result->first),
                               result->second);
        }
        return std::nullopt;
    };
}

constexpr auto operator>(Parser auto p, Parser auto q) -> Parser auto{
    return (p >> [q](auto) { return q; }) || p;
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

inline constexpr Parser auto P_digit = satisfy(::isdigit);
inline constexpr Parser auto P_letter = satisfy(::isalpha);
inline constexpr Parser auto P_space = satisfy(::isspace);
inline constexpr Parser auto P_lower = satisfy(::islower);
inline constexpr Parser auto P_upper = satisfy(::isupper);
inline constexpr Parser auto P_alpha = P_letter || P_digit;
inline constexpr Parser auto P_alphanum = P_alpha || P_space;

constexpr auto symbol(char x) -> Parser auto {
    return satisfy([x](char c) { return c == x; });
}

inline constexpr Parser auto plus = symbol('+');

inline constexpr decltype(auto) papply =
    []<typename F, typename... Args>(F &&f, Args &&...args) {
        if constexpr (std::invocable<F, Args...>) {
        }
    };
