#pragma once

#include "compiler/ast/ast.h"
#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

#include <map>
#include <range/v3/algorithm/find_if.hpp>
#include <variant>

namespace xi
{

inline auto ClearTypeAssignState()
{
    GetSymbolTable().clear();
    GetFunctionDefinitionTable().clear();
}

inline auto
TypeAssign(std::monostate /*unused*/, LocalVariableRecord /*unused*/)
    -> TypeAssignResult
{
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::TypeMismatch, "not implemented"});
}

template <typename T>
inline auto
TypeAssign(recursive_wrapper<T> &wrapper, LocalVariableRecord record)
    -> TypeAssignResult
{
    auto &x = wrapper.get();
    return TypeAssign(x, record);
}

template <typename T>
inline auto TypeAssign(recursive_wrapper<T> &wrapper) -> TypeAssignResult
{
    auto &x = wrapper.get();
    return TypeAssign(x);
}

template <typename T>
inline auto
TypeAssign(recursive_wrapper<T &> &wrapper, LocalVariableRecord record)
    -> TypeAssignResult
{
    auto &x = wrapper.get();
    return TypeAssign(x, record);
}

} // namespace xi
