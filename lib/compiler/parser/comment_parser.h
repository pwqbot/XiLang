#pragma once

#include <compiler/ast/ast.h>
#include <compiler/parser/basic_parsers.h>
#include <compiler/parser/parser_combinator.h>

namespace xi
{

// parse comment '//'
inline const auto Xi_comment = token(str("//")) > many(satisfy(
                                                      [](char c)
                                                      {
                                                          return c != '\n';
                                                      }
                                                  )) >>
                               [](auto s)
{
    return unit(Xi_Stmt{Xi_Comment{std::string(s.begin(), s.end())}});
};

} // namespace xi
