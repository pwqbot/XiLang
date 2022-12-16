#include <compiler/functional/monad.h>
#include <fmt/core.h>
#include <numeric>
#include <string>
#include <tl/expected.hpp>

namespace xi
{
struct ErrorCodeGen
{
    enum ErrorType
    {
        UnknownVariable,
        UnknownType,
        UnknownOperator,
        UnknownFunction,
        UnknownStruct,
        UnknownEnum,
        UnknownField,
        UnknownMethod,
        UnknownParameter,
        UnknownArgument,
        UnknownReturn,
        Unknown,
        Redefinition,
        InvalidArgumentCount,
        NotImplemented,
    };
    ErrorType   type_;
    std::string message_;
    ErrorCodeGen(ErrorType type, std::string message) :
        type_(type), message_(std::move(message))
    {
    }

    [[nodiscard]] auto what() const -> std::string
    {
        return fmt::format("ErrorCodeGen: {}", message_);
    }
};

template <typename T>
using ExpectedCodeGen = tl::expected<T, ErrorCodeGen>;

template <typename T>
struct unit_<ExpectedCodeGen<T>>
{
    static auto unit(T t) -> ExpectedCodeGen<T> { return t; }
};

template <typename T, typename E, typename F>
auto operator>>=(tl::expected<T, E> expected, F func)
{
    return expected.and_then(func);
}

} // namespace xi
