#pragma once

#include "compiler/ast/expr/basic.h"
#include "compiler/ast/type.h"
#include "compiler/ast/type_format.h"
#include "compiler/utils/recursive_wrapper.h"
#include "compiler/utils/variant_cmp.h"

#include <compare>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view.hpp>
#include <string>
#include <utility>
#include <variant>

namespace xi
{

enum class Xi_Op
{
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Xor,
    Eq,
    Neq,
    Lt,
    Gt,
    Leq,
    Geq,
    And,
    Or,
    Not,
    Dot,
};

constexpr auto OpMaps = std::array<
    std::pair<std::string_view, Xi_Op>,
    magic_enum::enum_count<Xi_Op>() + 2>{{
    {"+", Xi_Op::Add},
    {"-", Xi_Op::Sub},
    {"*", Xi_Op::Mul},
    {"/", Xi_Op::Div},
    {"%", Xi_Op::Mod},
    {"^", Xi_Op::Xor},
    {"==", Xi_Op::Eq},
    {"!=", Xi_Op::Neq},
    {"<", Xi_Op::Lt},
    {">", Xi_Op::Gt},
    {"<=", Xi_Op::Leq},
    {">=", Xi_Op::Geq},
    {"&&", Xi_Op::And},
    {"||", Xi_Op::Or},
    {"!", Xi_Op::Not},
    {".", Xi_Op::Dot},
    {"++", Xi_Op::Add},
    {"--", Xi_Op::Sub},
}};

constexpr auto KeyWords = std::array<std::string_view, 13>{
    "if",
    "then",
    "else",
    "true",
    "false",
    "xi",
    "let",
    "arr",
    "i64",
    "int",
    "real",
    "while",
    "return",
};

constexpr auto IsKeyWords(std::string_view str) -> bool
{
    return ranges::find_if(
               KeyWords,
               [&](auto kw)
               {
                   return kw == str;
               }
           ) != KeyWords.end();
}

constexpr auto OpStr_To_Xi_Op(std::string_view s) -> Xi_Op
{
    return ranges::find_if(
               OpMaps,
               [&](auto p)
               {
                   return p.first == s;
               }
    )->second;
}

constexpr auto Xi_Op_To_OpStr(Xi_Op op) -> std::string_view
{
    return ranges::find_if(
               OpMaps,
               [&](auto p)
               {
                   return p.second == op;
               }
    )->first;
}

} // namespace xi
