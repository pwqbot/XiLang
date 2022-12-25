#pragma once

#include "compiler/ast/type.h"
#include "compiler/ast/type_format.h"
#include "compiler/functional/monad.h"
#include "compiler/utils/expected.h"

#include <fmt/core.h>
#include <magic_enum.hpp>
#include <string>

namespace xi
{

struct TypeAssignError
{
    enum Error
    {
        UnknownType,
        DuplicateDeclaration,
        DuplicateDefinition,
        TypeMismatch,
        ParameterCountMismatch,
        UnknownVariable,
        UnknownMember,
        VarargNotLast,
    };

    Error       err;
    std::string message;
    auto        what() -> std::string
    {
        return fmt::format(
            "TypeAssignError: {} {}", magic_enum::enum_name(err), message
        );
    }
};

template <typename T>
using ExpectedTypeAssign  = tl::expected<T, TypeAssignError>;
using TypeAssignResult    = tl::expected<type::Xi_Type, TypeAssignError>;
using LocalVariableRecord = std::unordered_map<std::string, type::Xi_Type>;

template <typename T>
struct unit_<ExpectedTypeAssign<T>>
{
    static auto unit(T t) -> ExpectedTypeAssign<T> { return t; }
};

enum class SymbolType
{
    Function,
    Type,
    All,
};

inline auto GetSymbolTable()
    -> std::map<std::pair<std::string, SymbolType>, type::Xi_Type> &
{
    static std::map<std::pair<std::string, SymbolType>, type::Xi_Type>
        symbol_table{};
    return symbol_table;
}

inline auto GetFunctionDefinitionTable()
    -> std::unordered_map<std::string, type::Xi_Type> &
{
    static std::unordered_map<std::string, type::Xi_Type> symbol_table;
    return symbol_table;
}

auto findTypeInSymbolTable(std::string_view name, SymbolType st)
    -> TypeAssignResult;

} // namespace xi
