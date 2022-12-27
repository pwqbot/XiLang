#include "compiler/parser/program.h"

#include "compiler/ast/stmt/stmt.h"
#include "compiler/parser/comment_parser.h"
#include "compiler/parser/decl.h"
#include "compiler/parser/func.h"
#include "compiler/parser/set.h"

#include <string_view>

namespace xi
{

inline auto Xi_block_stmt_(std::string_view input) -> Parsed_t<Xi_Stmt>
{
    return (Xi_exprStmt || Xi_comment || Xi_for || Xi_while || Xi_return)(input);
}

inline auto Xi_top_stmt_(std::string_view input) -> Parsed_t<Xi_Stmt>
{
    return (Xi_func || Xi_decl || Xi_set || Xi_comment)(input);
}

} // namespace xi
