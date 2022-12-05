// #include <boost/variant/recursive_wrapper.hpp>
#include <compare>
#include <memory>
#include <string>
#include <variant>
#include <compiler/utils.h>

namespace xi {
// A trait to check that T is one of 'Types...'
template <typename T, typename... Types>
concept is_one_of = std::disjunction_v<std::is_same<T, Types>...>;

template <typename... Types, typename T>
auto operator==(const std::variant<Types...> &v, T const &t) noexcept
    requires is_one_of<T, Types...>
{
    return std::get<T>(v) == t;
}

struct Xi_Boolean {
    std::string text;
    bool        value;
    auto        operator<=>(const Xi_Boolean &) const = default;
};

struct Xi_Integer {
    std::string text;
    int         value;
    auto        operator<=>(const Xi_Integer &) const = default;
};

struct Xi_Real {
    std::string text;
    double      value;
    auto        operator<=>(const Xi_Real &) const = default;
};

struct Xi_String {
    std::string text;
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

struct Xi_Binop;

// <expr> ::= <term> | <expr> +- <expr>
// <term> ::= <number> | <number> "*" <number>
// <number> ::= <integer> | <real>
using Xi_Expr = std::variant<Xi_Integer, Xi_Boolean, Xi_Real, Xi_String,
                             recursive_wrapper<Xi_Binop>>;

auto GetText(const Xi_Expr &expr) -> std::string {
    return std::visit(
        [](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Xi_Integer>) {
                return arg.text;
            } else if constexpr (std::is_same_v<T, Xi_Boolean>) {
                return arg.text;
            } else if constexpr (std::is_same_v<T, Xi_Real>) {
                return arg.text;
            } else if constexpr (std::is_same_v<T, Xi_String>) {
                return arg.text;
            } else if constexpr (std::is_same_v<T, Xi_Binop>) {
                return arg.text;
            } else {
                return "";
            }
        },
        expr);
}

// binary expression
struct Xi_Binop {
    std::string text;
    Xi_Expr     lhs;
    Xi_Expr     rhs;
    Xi_Op op;
    auto  operator<=>(const Xi_Binop &) const = default;
};

struct Xi_Iden {
    std::string name;
    Xi_Expr     expr;
    auto        operator<=>(const Xi_Iden &) const = default;
};

} // namespace xi
