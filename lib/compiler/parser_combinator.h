#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

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
        requires std::regular_invocable<P, std::string_view>;
        requires std::same_as<
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

template <typename V>
concept is_variant = requires(V v) {
                         []<typename... Ts>(std::variant<Ts...> const &) {
                         }(v);
                     };

template <typename F>
concept no_overload = requires(
    F f) { typename decltype(std::function{std::declval<F>()})::result_type; };

template <Parser P, Parser_combinator<Parser_value_t<P>> F>
    requires(!is_variant<Parser_value_t<P>> || !no_overload<F>)
constexpr auto operator>>(P parser, F func) -> Parser auto{
    using Parser_t = Parser_combinator_value_t<F, Parser_value_t<P>>;
    return [=](std::string_view input) -> Parser_result_t<Parser_t> {
        if (auto result = std::invoke(parser, input)) {
            return std::invoke(std::invoke(func, result->first),
                               result->second);
        }
        return std::nullopt;
    };
}

template <typename Callable>
using return_type_of_t =
    typename decltype(std::function{std::declval<Callable>()})::result_type;

template <typename Callable>
struct trait_return_type;

template <typename Ret, typename... Args>
struct trait_return_type<Ret(Args...)> {
    using type = Ret;
};

template <typename T, typename F>
struct _call_on_variant;

template <typename F, typename... V>
struct _call_on_variant<F, std::variant<V...>>
    : std::disjunction<std::is_invocable<F, V>...> {};

template <typename F, typename V>
concept call_on_variant = _call_on_variant<F, V>::value;

// If the parser returns a variant, then we unwrap it
template <Parser P, typename F>
constexpr auto operator>>(P parser, F func) -> auto
    requires is_variant<Parser_value_t<P>> &&
             call_on_variant<F, Parser_value_t<P>> && no_overload<F>
{
    return [=](std::string_view input) -> Parser_result_t<return_type_of_t<F>> {
        if (auto result = std::invoke(parser, input)) {
            auto n_parser = std::visit(
                [&](auto &&thing) -> std::optional<return_type_of_t<F>> {
                    if constexpr (std::invocable<F, decltype(thing)>) {
                        return std::optional(std::invoke(func, thing));
                    }
                    return std::nullopt;
                },
                result->first);
            return n_parser.and_then(
                [result](auto f) { return std::invoke(f, result->second); });
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
    constexpr reduce_many(T init, P parser, F fn)
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
// TODO(ding.wang): refactor to constexpr after std::string is constepxr
// actually it is fold...
template <Parser P>
    requires std::same_as<Parser_value_t<P>, char>
auto many(P parser) -> Parser auto{
    return reduce_many(std::string{}, parser,
                       [](auto acc, auto c) { return acc + c; });
}

template <Parser P, typename F, typename T = Parser_value_t<P>>
auto many(P parser, F func, T init = T{}) -> Parser auto{
    return reduce_many<T, P, F>(init, parser, func);
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

template <Parser P, typename F>
constexpr auto some(P parser, F func) -> Parser auto{
    using T = Parser_value_t<P>;
    return parser >> [parser, func](T first) {
        return many(parser, func, first);
    };
}

template <Parser P>
auto maybe(P parser) -> Parser auto{
    return parser >> [](auto thing) {
        return unit(std::optional{thing});
    } || unit(std::optional<Parser_value_t<P>>{});
}

} // namespace xi
