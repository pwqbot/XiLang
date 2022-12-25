#pragma once

#include "compiler/ast/error.h"
#include "compiler/ast/type.h"

#include <string>

namespace xi
{

struct Xi_Boolean
{
    bool          value;
    type::Xi_Type type                                  = type::buer{};
    auto          operator<=>(const Xi_Boolean &) const = default;
};

auto TypeAssign(
    Xi_Boolean /*unused*/,
    LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult;

struct Xi_Integer
{
    int64_t       value;
    type::Xi_Type type                                  = type::i64{};
    auto          operator<=>(const Xi_Integer &) const = default;
};

auto TypeAssign(
    Xi_Integer /*unused*/,
    LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult;

struct Xi_Real
{
    double        value;
    type::Xi_Type type                               = type::real{};
    auto          operator<=>(const Xi_Real &) const = default;
};

auto TypeAssign(
    Xi_Real /*unused*/, LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult;

struct Xi_String
{
    std::string   value;
    type::Xi_Type type                                 = type::string{};
    auto          operator<=>(const Xi_String &) const = default;
};

auto TypeAssign(
    Xi_String /*unused*/, LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult;

} // namespace xi
