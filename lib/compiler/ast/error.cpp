#include "compiler/ast/error.h"

#include "compiler/ast/all.h"

namespace xi
{

auto findTypeInSymbolTable(std::string_view name, SymbolType st)
    -> TypeAssignResult
{
    if (st == SymbolType::Type || st == SymbolType::All)
    {
        auto t = type::ToBuiltinTypes(name);
        if (t.has_value())
        {
            return t.value();
        }

        // check if it is an array
        auto l_bracket_pos = name.find('[');
        auto r_bracket_pos = name.find_last_of(']');
        auto is_array      = l_bracket_pos != std::string_view::npos and
                        r_bracket_pos != std::string_view::npos and
                        r_bracket_pos > l_bracket_pos and
                        name.substr(0, l_bracket_pos) == "arr";
        if (is_array)
        {
            auto inner_type_name =

                name.substr(
                    l_bracket_pos + 1, r_bracket_pos - l_bracket_pos - 1
                );
            auto expect_inner_type = findTypeInSymbolTable(inner_type_name, st);
            if (!expect_inner_type)
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::UnknownType,
                    fmt::format("Unknown type {}", inner_type_name),
                });
            }
            return type::array{expect_inner_type.value()};
        }
    }

    if (st == SymbolType::All)
    {
        for (const auto &st_ : {SymbolType::Function, SymbolType::Type})
        {
            auto it = GetSymbolTable().find({std::string(name), st_});
            if (it != GetSymbolTable().end())
            {
                return it->second;
            }
        }
    }
    else
    {
        if (auto type = GetSymbolTable().find({std::string(name), st});
            type != GetSymbolTable().end())
        {
            return type->second;
        }
    }
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::UnknownType,
        fmt::format(
            "cannot find {}:{} "
            "in symbol_table",
            magic_enum::enum_name(st),
            name
        ),
    });
}
} // namespace xi
