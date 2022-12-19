#pragma once

#include <compiler/functional/monad.h>
#include <compiler/util/expected.h>
#include <fmt/core.h>
#include <magic_enum.hpp>
#include <string>

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
        TypeMismatch,
    };
    ErrorType   type_;
    std::string message_;
    ErrorCodeGen(ErrorType type, std::string_view message) :
        type_(type), message_(message)
    {
    }

    [[nodiscard]] auto what() const -> std::string
    {
        return fmt::format(
            "ErrorCodeGen: {} {}", magic_enum::enum_name(type_), message_
        );
    }
};

template <typename T>
using ExpectedCodeGen = tl::expected<T, ErrorCodeGen>;

template <typename T>
struct unit_<ExpectedCodeGen<T>>
{
    static auto unit(T t) -> ExpectedCodeGen<T> { return t; }
};

} // namespace xi
