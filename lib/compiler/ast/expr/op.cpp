#include "compiler/ast/expr/op.h"

#include "compiler/ast/all.h"
#include "compiler/ast/ast_format.h"
#include "compiler/ast/error.h"

namespace xi
{

auto typeAssignDot(Xi_Binop &binop, LocalVariableRecord record)
    -> TypeAssignResult
{
    return TypeAssign(binop.lhs, record) >>=
           [&binop, record](type::Xi_Type lhs_type) -> TypeAssignResult
    {
        if (!type::isSet(lhs_type))
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format(
                    "Dot operator can only be applied to set, but got {}",
                    lhs_type
                ),
            });
        }
        if (!std::holds_alternative<recursive_wrapper<Xi_Iden>>(binop.rhs))
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format(
                    "rhs of dot operator must be an identifier, but got {}",
                    binop.rhs
                ),
            });
        }
        auto set_type = std::get<recursive_wrapper<type::set>>(lhs_type);
        auto member_name =
            std::get<recursive_wrapper<Xi_Iden>>(binop.rhs).get().name;
        auto member_type = std::find_if(
            set_type.get().members.begin(),
            set_type.get().members.end(),
            [&member_name](auto member)
            {
                return member.first == member_name;
            }
        );
        if (member_type == set_type.get().members.end())
        {
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                fmt::format(
                    "set {} has no member named {}",
                    set_type.get().name,
                    member_name
                ),
            });
        }
        binop.index =
            std::distance(set_type.get().members.begin(), member_type);
        return binop.type = member_type->second;
    };
}

auto TypeAssign(Xi_Binop &binop, LocalVariableRecord record) -> TypeAssignResult
{
    if (binop.op == Xi_Op::Dot)
    {
        return typeAssignDot(binop, record);
    }
    return TypeAssign(binop.lhs, record) >>=
           [&binop, record](type::Xi_Type lhs_type) -> TypeAssignResult
    {
        return TypeAssign(binop.rhs, record) >>=
               [&binop, lhs_type, record](type::Xi_Type rhs_type
               ) -> TypeAssignResult
        {
            if (lhs_type != rhs_type)
            {
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect same type, lhs: {}, rhs: {}", lhs_type, rhs_type
                    ),
                });
            }
            switch (binop.op)
            {
            case Xi_Op::Mod:
                if (lhs_type == type::i64{})
                {
                    return binop.type = lhs_type;
                }
                return tl::make_unexpected(TypeAssignError{
                    TypeAssignError::TypeMismatch,
                    fmt::format(
                        "expect int, lhs: {}, rhs: {}", lhs_type, rhs_type
                    ),
                });
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
                });
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
                });
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
                });
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
                });
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
                });
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
                });
            }
            else
            {
                return unop.type = type::buer{};
            }
        default:
            return tl::make_unexpected(TypeAssignError{
                TypeAssignError::TypeMismatch,
                "",
            });
        }
    };
}

} // namespace xi
