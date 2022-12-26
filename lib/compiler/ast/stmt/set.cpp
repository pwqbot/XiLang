#include "compiler/ast/error.h"
#include "compiler/ast/stmt/stmt.h"
#include "compiler/ast/all.h"

namespace xi
{

auto TypeAssign(Xi_Set &set) -> TypeAssignResult
{
    if (GetSymbolTable().contains({set.name, SymbolType::Type}))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDeclaration,
            fmt::format("set {}", set.name),
        });
    }
    return flatmap(
               set.members,
               [set](std::pair<std::string, std::string> name_type)
               {
                   return findTypeInSymbolTable(
                       name_type.second, SymbolType::Type
                   );
               }
           ) >>= [&set](auto member_types) -> TypeAssignResult
    {
        auto name_with_type = ranges::views::zip(
                                  set.members | ranges::views::transform(
                                                    [](auto pair)
                                                    {
                                                        return pair.first;
                                                    }
                                                ),
                                  member_types
                              ) |
                              ranges::to_vector;
        auto set_type = type::set{set.name, name_with_type};
        set.type      = set_type;
        GetSymbolTable().insert({{set.name, SymbolType::Type}, set_type});

        // create constructor
        GetSymbolTable().insert({
            {set.name, SymbolType::Function},
            type::function{
                .return_type = set_type,
                .param_types = member_types,
            },
        });
        return set.type;
    };
}

} // namespace xi
