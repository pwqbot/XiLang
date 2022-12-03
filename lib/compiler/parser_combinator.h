#include <compiler/inner_type.h>
#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace xi {

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

// ----------------------- monadic interface ----------------------------------

// Return a Parser that returns an instance of T and the unconsumed input
// return in monad
template <typename T>
constexpr auto unit(T thing) -> Parser auto{
    return [thing](std::string_view input) -> Parsed_t<T> {
        return std::make_pair(thing, input);
    };
}

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
    return p >> [q](auto) {
        return q;
    };
}

// --------------------------- alternative interface ---------------------------

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

// Parse thing 0+ times
template <typename T, Parser P, std::regular_invocable<T, Parser_value_t<P>> F>
    requires std::convertible_to<std::invoke_result_t<F, T, Parser_value_t<P>>,
                                 T>
class reduce_many {
    T init;
    P parser;
    F func;

  public:
    reduce_many(T init, P parser, F fn)
        : init(init), parser(parser), func{fn} {}

    constexpr auto operator()(std::string_view input) const -> Parsed_t<T> {
        return ((parser >>
                 [this](auto first) {
                     return reduce_many{std::invoke(func, init, first), parser,
                                        func};
                 }) ||
                unit(init))(input);
    }
};

// Repeat a char parser 0+ times and concatenate the result into a string
template <Parser P>
    requires std::same_as<Parser_value_t<P>, char>
constexpr auto many(P parser) -> Parser auto{
    return reduce_many{std::string{}, parser, [](auto acc, auto c) {
                           return acc + c;
                       }};
}

// Repeat a char parser 1+ times and concatenate the result into a string
template <Parser P>
    requires std::same_as<Parser_value_t<P>, char>
constexpr auto some(P parser) -> Parser auto{
    return parser >> [parser](auto first) {
        return many(parser) >> [first](auto rest) {
            return unit(std::string{first} + rest);
        };
    };
}

} // namespace xi
