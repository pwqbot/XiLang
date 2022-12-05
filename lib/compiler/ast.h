// #include <boost/variant/recursive_wrapper.hpp>
#include <compare>
#include <compiler/utils.h>
#include <magic_enum.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace xi {
// A trait to check that T is one of 'Types...'
template <typename T, typename... Types>
concept is_one_of =
    std::disjunction_v<std::is_same<std::remove_cv_t<T>, Types>...>;

template <typename T, typename... Types>
concept is_one_of_recursive = std::disjunction_v<
    std::is_same<recursive_wrapper<std::remove_cv_t<T>>, Types>...>;

template <typename... Types, typename T>
auto operator==(const std::variant<Types...> &v, T const &t) noexcept
    requires is_one_of<T, Types...> || is_one_of_recursive<T, Types...>
{
    if constexpr (is_one_of_recursive<T, Types...>) {
        return std::get<recursive_wrapper<std::remove_cv_t<T>>>(v) == t;
    } else {
        return std::get<T>(v) == t;
    }
}

//
// template <typename... Types, typename T>
// auto operator==(const std::variant<Types...> &v,
//                 recursive_wrapper<T> const   &t) noexcept
//     requires is_one_of<T, Types...>
// {
//     return std::get<T>(v) == t;
// }

struct Xi_Boolean {
    bool value;
    auto operator<=>(const Xi_Boolean &) const = default;
};

struct Xi_Integer {
    int  value;
    auto operator<=>(const Xi_Integer &) const = default;
};

struct Xi_Real {
    double value;
    auto   operator<=>(const Xi_Real &) const = default;
};

struct Xi_String {
    std::string value;
    auto        operator<=>(const Xi_String &) const = default;
};

using Xi_Literal = std::variant<Xi_Boolean, Xi_Integer, Xi_Real, Xi_String>;

enum class Xi_Op {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Pow,
    Eq,
    Neq,
    Lt,
    Gt,
    Leq,
    Geq,
    And,
    Or,
    Not,
};

auto StrToOp(std::string_view s) -> Xi_Op {
    static std::unordered_map<std::string_view, Xi_Op> map = {
        {"+", Xi_Op::Add},  {"-", Xi_Op::Sub},  {"*", Xi_Op::Mul},
        {"/", Xi_Op::Div},  {"%", Xi_Op::Mod},  {"^", Xi_Op::Pow},
        {"==", Xi_Op::Eq},  {"!=", Xi_Op::Neq}, {"<", Xi_Op::Lt},
        {">", Xi_Op::Gt},   {"<=", Xi_Op::Leq}, {">=", Xi_Op::Geq},
        {"&&", Xi_Op::And}, {"||", Xi_Op::Or},  {"!", Xi_Op::Not},
    };
    return map.at(s);
}

struct Xi_Binop;
template <typename T>
std::strong_ordering operator<=>(const T& a, const T& b);
template <typename T>
auto operator<=>(const Xi_Binop &a, const Xi_Binop &b) -> std::strong_ordering;

// <expr> ::= <term> | <expr> +- <expr>
// <term> ::= <number> | <number> "*" <number>
// <number> ::= <integer> | <real>
using Xi_Expr = std::variant<Xi_Integer, Xi_Boolean, Xi_Real, Xi_String,
                             recursive_wrapper<Xi_Binop>>;

template <typename T>
std::strong_ordering compare(T a, T b);

// binary expression
struct Xi_Binop {
    Xi_Expr lhs;
    Xi_Expr rhs;
    Xi_Op   op;
    auto    operator<=>(const Xi_Binop &) const { return compare(lhs, rhs); }
};

template <typename T>
std::strong_ordering compare(T a, T b) {
    return a <=> b;
}

struct Xi_Iden {
    std::string name;
    Xi_Expr     expr;
    auto        operator<=>(const Xi_Iden &) const = default;
};

void test() {
    auto x1 = recursive_wrapper<Xi_Binop>(
        Xi_Binop{Xi_Integer{1}, Xi_Integer{2}, Xi_Op::Add});
    auto x2 = recursive_wrapper<Xi_Binop>(
        Xi_Binop{Xi_Integer{1}, Xi_Integer{2}, Xi_Op::Add});
}

auto GetText(const Xi_Expr &expr) -> std::string {
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::same_as<T, Xi_Integer>) {
                return std::to_string(arg.value);
            } else if constexpr (std::same_as<T, Xi_Boolean>) {
                return std::to_string(arg.value);
            } else if constexpr (std::same_as<T, Xi_Real>) {
                return std::to_string(arg.value);
            } else if constexpr (std::same_as<T, Xi_String>) {
                return arg.value;
            } else if constexpr (std::same_as<T, recursive_wrapper<Xi_Binop>>) {
                return GetText(arg.get().lhs) + GetText(arg.get().rhs);
            } else {
                return "";
            }
        },
        expr);
}

} // namespace xi
