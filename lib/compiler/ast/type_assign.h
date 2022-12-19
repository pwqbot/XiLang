#include <compiler/ast/ast.h>
#include <compiler/ast/type_format.h>
#include <compiler/ast/utils.h>
#include <compiler/functional/monad.h>
#include <compiler/util/expected.h>
#include <range/v3/algorithm/find_if.hpp>
#include <unordered_map>
#include <variant>

namespace xi
{

static auto GetSymbolTable() -> std::unordered_map<std::string, type::Xi_Type> &
{
    static std::unordered_map<std::string, type::Xi_Type> symbol_table;
    return symbol_table;
}

static auto GetFunctionDefinitionTable()
    -> std::unordered_map<std::string, type::Xi_Type> &
{
    static std::unordered_map<std::string, type::Xi_Type> symbol_table;
    return symbol_table;
}

inline auto ClearTypeAssignState()
{
    GetSymbolTable().clear();
    GetFunctionDefinitionTable().clear();
}

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
        VarargNotLast,
    };
    Error       err;
    std::string message;
    Xi_Stmt     node;
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

auto TypeAssign(Xi_Expr &expr, LocalVariableRecord = LocalVariableRecord{})
    -> TypeAssignResult;

template <typename T>
struct unit_<ExpectedTypeAssign<T>>
{
    static auto unit(T t) -> ExpectedTypeAssign<T> { return t; }
};

auto TypeAssign(
    Xi_Integer /*unused*/,
    LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult
{
    return type::i64{};
}

auto TypeAssign(
    Xi_Real /*unused*/, LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult
{
    return type::real{};
}

auto TypeAssign(
    Xi_String /*unused*/, LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult
{
    return type::string{};
}

auto TypeAssign(
    Xi_Boolean /*unused*/,
    LocalVariableRecord /*unused*/ = LocalVariableRecord{}
) -> TypeAssignResult
{
    return type::buer{};
}

auto findTypeInSymbolTable(std::string_view name, Xi_Stmt node)
    -> TypeAssignResult
{
    auto t = type::ToBuiltinTypes(name);
    if (t.has_value())
    {
        return t.value();
    }
    if (auto type = GetSymbolTable().find(std::string(name));
        type != GetSymbolTable().end())
    {
        return type->second;
    }
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::UnknownType, fmt::format("{}", name), node});
}

auto TypeAssign(Xi_Set &set) -> TypeAssignResult
{
    if (GetSymbolTable().contains(set.name))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDeclaration,
            fmt::format("set {}", set.name),
            set});
    }
    return flatmap(
               set.members,
               [set](std::pair<std::string, std::string> name_type)
               { return findTypeInSymbolTable(name_type.second, set); }
           ) >>= [&set](auto member_types) -> TypeAssignResult
    {
        auto set_type = type::set{set.name, member_types};
        set.type      = set_type;
        GetSymbolTable().insert({set.name, set_type});
        return set.type;
    };
}

auto TypeAssign(Xi_Binop &binop, LocalVariableRecord record) -> TypeAssignResult
{
    return TypeAssign(binop.lhs, record) >>=
           [&binop, record](type::Xi_Type lhs_type) -> TypeAssignResult
    {
        return TypeAssign(binop.rhs, record) >>=
               [&binop, lhs_type](type::Xi_Type rhs_type) -> TypeAssignResult
        {
            if (lhs_type != rhs_type)
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect same type, lhs: {}, rhs: {}", lhs_type, rhs_type
                    ),
                    binop});
            }
            switch (binop.op)
            {
            case Xi_Op::Add:
            case Xi_Op::Sub:
            case Xi_Op::Mul:
            case Xi_Op::Div:
                if (lhs_type == type::i64{} || lhs_type == type::real{})
                {
                    return binop.type = lhs_type;
                }
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect int or real, lhs: {}, rhs: {}",
                        lhs_type,
                        rhs_type
                    ),
                    binop});
            case Xi_Op::Eq:
            case Xi_Op::Neq:
                if (lhs_type == type::i64{} || lhs_type == type::real{} ||
                    lhs_type == type::buer{})
                {
                    return binop.type = type::buer{};
                }
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect int or real or buer, lhs: {}, rhs: {}",
                        lhs_type,
                        rhs_type
                    ),
                    binop});
            case Xi_Op::Lt:
            case Xi_Op::Gt:
                if (lhs_type == type::i64{} || lhs_type == type::real{})
                {
                    return binop.type = type::buer{};
                }
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect buer, lhs: {}, rhs: {}", lhs_type, rhs_type
                    ),
                    binop});
            case Xi_Op::Or:
            case Xi_Op::And:
                if (lhs_type == type::buer{})
                {
                    return binop.type = type::buer{};
                }
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect buer, lhs: {}, rhs: {}", lhs_type, rhs_type
                    ),
                    binop});
            default:
                return lhs_type;
            }
        };
    };
}

auto TypeAssign(Xi_Unop &unop, LocalVariableRecord record) -> TypeAssignResult
{
    return TypeAssign(unop.expr, record) >>=
           [&unop](type::Xi_Type expr_type) -> TypeAssignResult
    {
        switch (unop.op)
        {
        case Xi_Op::Sub:
        case Xi_Op::Add:
            if (expr_type != type::i64{} && expr_type != type::real{})
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format("expect i64, has {}", expr_type),
                    unop});
            }
            else
            {
                return unop.type = type::i64{};
            }
        case Xi_Op::Not:
            if (expr_type != type::buer{})
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format("expect buer, has {}", expr_type),
                    unop});
            }
            else
            {
                return unop.type = type::buer{};
            }
        default:
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch, "", unop});
        }
    };
}

auto TypeAssign(Xi_Decl &decl) -> TypeAssignResult
{
    if (GetSymbolTable().contains(decl.name))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDeclaration,
            fmt::format("Decl {}", decl.name),
            decl});
    }

    return findTypeInSymbolTable(decl.return_type, decl) >>=
           [&decl](auto return_type)
    {
        return flatmap(
                   decl.params_type,
                   [decl](auto x) { return findTypeInSymbolTable(x, decl); }
               ) >>= [return_type, &decl](std::vector<type::Xi_Type> param_types
                     ) -> TypeAssignResult
        {
            auto func_type = type::function{
                .return_type = return_type, .param_types = param_types};

            auto vararg_iter =
                ranges::find(param_types, type::Xi_Type{type::vararg{}});
            if (vararg_iter != param_types.end())
            {
                if (*vararg_iter != param_types.back())
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::VarargNotLast,
                        fmt::format("vararg must be the last param"),
                        decl});
                }
                func_type.param_types.pop_back();
                func_type.is_vararg = true;
            }

            GetSymbolTable().insert({decl.name, func_type});
            return decl.type = func_type;
        };
    };
}

auto TypeAssign(Xi_If &if_expr, LocalVariableRecord record) -> TypeAssignResult
{
    return TypeAssign(if_expr.cond, record) >>=
           [&if_expr, record](auto cond_type) -> TypeAssignResult
    {
        return TypeAssign(if_expr.then, record) >>=
               [cond_type, &if_expr, record](auto then_type) -> TypeAssignResult
        {
            return TypeAssign(if_expr.els, record) >>=
                   [cond_type, then_type, &if_expr](auto else_type
                   ) -> TypeAssignResult
            {
                if (cond_type != type::buer{})
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format("expect buer, find {}", cond_type),
                        if_expr});
                }
                if (then_type != else_type)
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format(
                            "expect same type, lhs: {}, rhs {}",
                            then_type,
                            else_type
                        ),
                        if_expr,
                    });
                }
                return if_expr.type = then_type;
            };
        };
    };
}

auto TypeAssign(Xi_Func &func) -> TypeAssignResult
{
    if (GetFunctionDefinitionTable().contains(func.name))
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::DuplicateDefinition,
            fmt::format("Func {}", func.name),
            func});
    }

    return findTypeInSymbolTable(func.name, func) >>= [&func](auto decl_type)
    {
        return std::visit(
            [&func]<typename T>(T &decl_type_) -> TypeAssignResult
            {
                if constexpr (std::same_as<
                                  std::decay_t<T>,
                                  recursive_wrapper<type::function>>)
                {
                    if (decl_type_.get().param_types.size() !=
                        func.params.size())
                    {
                        return tl::make_unexpected(TypeAssignError{
                            TypeAssignError::ParameterCountMismatch,
                            fmt::format(
                                "expect {} params, find {}",
                                decl_type_.get().param_types.size(),
                                func.params.size()
                            ),
                            func});
                    }

                    LocalVariableRecord record;
                    auto                func_param_iter = func.params.begin();
                    for (auto &decl_param : decl_type_.get().param_types)
                    {
                        if (record.contains(*func_param_iter))
                        {
                            return tl::make_unexpected(TypeAssignError{
                                TypeAssignError::DuplicateDeclaration,
                                fmt::format(
                                    "Decl function parameter {}",
                                    *func_param_iter
                                ),
                                func});
                        }
                        record.insert({*func_param_iter, decl_param});
                        ++func_param_iter;
                    }
                    return flatmap_(
                               func.let_idens,
                               [record](auto &x)
                               { return TypeAssign(x.expr, record); }
                           ) >>=
                           [&func, record, decl_type_](
                               std::vector<type::Xi_Type> let_expr_types
                           ) mutable -> TypeAssignResult
                    {
                        auto let_iter = func.let_idens.begin();
                        for (auto &let_expr_type : let_expr_types)
                        {
                            if (record.contains(let_iter->name))
                            {
                                return tl::make_unexpected(TypeAssignError{
                                    TypeAssignError::DuplicateDeclaration,
                                    fmt::format(
                                        "Decl let variable {}", let_iter->name
                                    ),
                                    func});
                            }

                            let_iter->type = let_expr_type;
                            record.insert({let_iter->name, let_expr_type});
                            ++let_iter;
                        }
                        return TypeAssign(func.expr, record) >>=
                               [decl_type_,
                                &func](auto expr_type) -> TypeAssignResult
                        {
                            if (expr_type != decl_type_.get().return_type)
                            {
                                return tl::make_unexpected(TypeAssignError{
                                    TypeAssignError::TypeMismatch,
                                    fmt::format(
                                        "expect return {}, find {}",
                                        decl_type_.get().return_type,
                                        expr_type
                                    ),
                                    func});
                            }
                            GetFunctionDefinitionTable().insert(
                                {func.name, func.type}
                            );
                            return func.type = decl_type_.get();
                        };
                    };
                }
                else
                {
                    return tl::make_unexpected(TypeAssignError{
                        TypeAssignError::TypeMismatch,
                        fmt::format("expect function, find {}", decl_type_),
                        func});
                }
            },
            decl_type
        );
    };
}

auto TypeAssign(Xi_Stmt &stmt) -> TypeAssignResult
{
    return std::visit([](auto &x) mutable { return TypeAssign(x); }, stmt);
}

auto TypeAssign(Xi_Lam & /*unused*/, LocalVariableRecord /*unused*/)
    -> TypeAssignResult
{
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::TypeMismatch, "not implemented", Xi_Lam{}});
}

auto TypeAssign(Xi_Call call_expr, LocalVariableRecord record)
    -> TypeAssignResult
{
    return flatmap_(
               call_expr.args,
               [record](auto &x) { return TypeAssign(x, record); }
           ) >>= [&call_expr, record](auto args_type)
    {
        return findTypeInSymbolTable(call_expr.name, call_expr) >>=
               [args_type, &call_expr, record](auto func_type
               ) -> TypeAssignResult
        {
            return std::visit(
                [&call_expr,
                 args_type]<typename T>(T &&func_type_) -> TypeAssignResult
                {
                    if constexpr (std::same_as<
                                      std::decay_t<T>,
                                      recursive_wrapper<type::function>>)
                    {
                        if (!func_type_.get().is_vararg &&
                            func_type_.get().param_types != args_type)
                        {
                            return tl::make_unexpected(TypeAssignError{
                                TypeAssignError::TypeMismatch,
                                fmt::format(
                                    "param type mismatch, expect {}, find {}",
                                    func_type_.get().param_types,
                                    args_type
                                ),
                                call_expr});
                        }
                        return call_expr.type = func_type_.get().return_type;
                    }
                    else
                    {
                        return tl::make_unexpected(TypeAssignError{
                            TypeAssignError::TypeMismatch,
                            fmt::format(
                                "expect function type, find {}", func_type_
                            ),
                            call_expr});
                    }
                },
                func_type
            );
        };
    };
}

auto TypeAssign(Xi_Iden iden, LocalVariableRecord record) -> TypeAssignResult
{
    auto iden_type = record.find(iden.name);
    if (iden_type == record.end())
    {
        return tl::make_unexpected(TypeAssignError{
            TypeAssignError::UnknownVariable,
            fmt::format("undeclared variable {}", iden.name),
            iden});
    }
    return iden.type = iden_type->second;
}

auto TypeAssign(std::monostate /*unused*/, LocalVariableRecord)
    -> TypeAssignResult
{
    return tl::make_unexpected(TypeAssignError{
        TypeAssignError::TypeMismatch, "not implemented", Xi_Iden{}});
}

template <typename T>
auto TypeAssign(recursive_wrapper<T> &wrapper, LocalVariableRecord record)
    -> TypeAssignResult
{
    auto &x = wrapper.get();
    return TypeAssign(x, record);
}

template <typename T>
auto TypeAssign(
    recursive_wrapper<T &> &wrapper, LocalVariableRecord /*unused*/ record
) -> TypeAssignResult
{
    auto &x = wrapper.get();
    return TypeAssign(x, record);
}

auto TypeAssign(Xi_Expr &expr, LocalVariableRecord record) -> TypeAssignResult
{
    return std::visit(
        [record](auto &expr_) mutable { return TypeAssign(expr_, record); },
        expr
    );
}

auto TypeAssign(Xi_Program &program)
    -> ExpectedTypeAssign<std::vector<type::Xi_Type>>
{
    return flatmap_(program.stmts, [](auto &x) { return TypeAssign(x); });
}

} // namespace xi
