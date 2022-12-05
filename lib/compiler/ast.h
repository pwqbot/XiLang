#include <string>
#include <variant>

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
using Xi_Expr = std::variant<Xi_Integer, Xi_Boolean, Xi_Real, Xi_String, Xi_Binop>;

// binary expression
struct Xi_Binop {
    // use pointer to avoid recursive type
    std::string text;
    Xi_Expr* lhs;
    Xi_Expr* rhs;
    Xi_Op   op;
    auto    operator<=>(const Xi_Binop &) const = default;
};

struct Xi_Iden {
    std::string name;
    Xi_Expr     expr;
    auto        operator<=>(const Xi_Iden &) const = default;
};

} // namespace xi
