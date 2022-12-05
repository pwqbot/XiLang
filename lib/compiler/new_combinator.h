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

template <typename T, typename Input = std::string_view>
class Parser_t {
    using result_t = std::optional<std::pair<T, Input>>;

    std::function<result_t(Input)> parser;

    explicit Parser_t(std::function<result_t(Input)> parser) : parser{parser} {}
    auto operator()(Input input) const -> result_t { return parser(input); }
};

template <typename T>
constexpr auto unit(T thing) -> Parser_t<T> {
    return Parser_t<T>{[thing](auto input) {
        return std::make_pair(thing, input);
    }};
}

template <typename T, typename U>
constexpr auto operator||(Parser_t<T> p, Parser_t<U> q) -> Parser_t<U>
    requires std::convertible_to<T, U>
{
    return Parser_t<U>{[p, q](auto input) {
        auto result = p(input);
        if (result) {
            return result;
        }
        return q(input);
    }};
}

template <template <typename T> class P, typename T, typename F>
struct bind;

template <template <typename T> class P, typename T, typename Ret,
          typename... Args>
struct bind<P, T, Ret(Args...)> {
    using F = Ret(Args...);
    using Result_t =
        std::invoke_result_t<std::invoke_result_t<F, T>, std::string_view>;
    static auto bind_(P<T> parser, F f) {
        return Parser_t<Ret>{[parser, f](auto input) {
            auto result = parser(input);
            if (result) {
                return f(result->first)(result->second);
            }
            return std::nullopt;
        }};
    }
};

template <typename F, typename T>
constexpr auto operator>>(Parser_t<T> parser, F func) {
    using Result_Parser = std::invoke_result_t<F, T>;
    return Result_Parser{[parser, func](auto input) {
        auto result = parser(input);
        if (result) {
            return f(result->first)(result->second);
        }
        return std::nullopt;
    }};
}

template <template <typename T> class P, template <typename U> class Q,
          typename T, typename U>
constexpr auto operator>(P<T> p, Q<U> q) -> U {
    return p >> [q](auto) {
        return q;
    };
}

// --------------------------- alternative interface ---------------------------

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
template <Parser P>
    requires std::same_as<Parser_value_t<P>, char>
auto many(P parser) -> Parser auto{
    return reduce_many(std::string{}, parser,
                       [](auto acc, auto c) { return acc + c; });
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

template <Parser P>
auto maybe(P parser) -> Parser auto{
    return parser >> [](auto thing) {
        return unit(std::optional{thing});
    } || unit(std::optional<Parser_value_t<P>>{});
}

} // namespace xi
