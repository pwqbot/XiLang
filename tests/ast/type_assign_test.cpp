#include "test_header.h"

#include <compiler/ast/ast.h>
#include <compiler/ast/type.h>
#include <compiler/ast/type_assign.h>

namespace xi
{

template <typename T>
struct TypeAssignMatcher : Catch::Matchers::MatcherGenericBase
{
    T expected_;

    explicit TypeAssignMatcher(T expected) : expected_{expected} {}

    template <typename V>
    auto match(V v) const -> bool
    {
        if (v.has_value())
        {
            return v.value() == expected_;
        }
        return false;
    }

    auto describe() const -> std::string override
    {
        return fmt::format("is equal to {}", expected_);
    }
};

TEST_CASE("Type Assign Basic", "[Xi_Integer]")
{
    REQUIRE_THAT(TypeAssign(Xi_Integer{123}), TypeAssignMatcher{type::i64{}});
    REQUIRE_THAT(TypeAssign(Xi_Real{123}), TypeAssignMatcher{type::real{}});
    REQUIRE_THAT(
        TypeAssign(Xi_String{"aa"}), TypeAssignMatcher{type::string{}}
    );
    REQUIRE_THAT(TypeAssign(Xi_Boolean{true}), TypeAssignMatcher{type::buer{}});
}

TEST_CASE("Type Assign Iden")
{
    LocalVariableRecord record;
    record.insert({"x", type::i64{}});
    auto iden_x = Xi_Iden{"x"};
    REQUIRE_THAT(TypeAssign(iden_x, record), TypeAssignMatcher{type::i64{}});

    auto iden_y = Xi_Iden{"y"};
    REQUIRE(!TypeAssign(iden_y, record).has_value());
}

TEST_CASE("Type Assign Decl", "[Xi_Decl]")
{
    auto decl = Xi_Decl{"i64", "i64", {"i64", "i64"}};
    REQUIRE_THAT(
        TypeAssign(decl),
        TypeAssignMatcher(type::function{
            type::i64{}, {type::i64{}, type::i64{}}})
    );
}

TEST_CASE("Type Assign Set", "[Xi_Set]")
{
    auto set = Xi_Set{"point", {{"x", "i64"}, {"y", "i64"}}};
    REQUIRE_THAT(
        TypeAssign(set),
        TypeAssignMatcher(type::set{"point", {type::i64{}, type::i64{}}})
    );
}

TEST_CASE("Type Assign Binop", "[Xi_Binop]")
{
    LocalVariableRecord record;
    auto add = Xi_Binop{Xi_Integer{2}, Xi_Integer{2}, Xi_Op::Add};
    REQUIRE_THAT(TypeAssign(add, record), TypeAssignMatcher(type::i64{}));

    auto eq = Xi_Binop{Xi_Integer{2}, Xi_Integer{2}, Xi_Op::Eq};
    REQUIRE_THAT(TypeAssign(eq, record), TypeAssignMatcher(type::buer{}));

    auto beq = Xi_Binop{Xi_Boolean{true}, Xi_Boolean{true}, Xi_Op::Eq};
    REQUIRE_THAT(TypeAssign(beq, record), TypeAssignMatcher(type::buer{}));

    auto lt = Xi_Binop{Xi_Integer{2}, Xi_Integer{2}, Xi_Op::Lt};
    REQUIRE_THAT(TypeAssign(lt, record), TypeAssignMatcher(type::buer{}));

    auto and_ = Xi_Binop{Xi_Boolean{true}, Xi_Boolean{true}, Xi_Op::And};
    REQUIRE_THAT(TypeAssign(and_, record), TypeAssignMatcher(type::buer{}));
}

TEST_CASE("Type Assign Unop")
{
    LocalVariableRecord record;
    auto                sub = Xi_Unop{Xi_Integer{2}, Xi_Op::Sub};
    REQUIRE_THAT(TypeAssign(sub, record), TypeAssignMatcher(type::i64{}));

    auto not_ = Xi_Unop{Xi_Boolean{true}, Xi_Op::Not};
    REQUIRE_THAT(TypeAssign(not_, record), TypeAssignMatcher(type::buer{}));
}

TEST_CASE("Type Assign If_Expr")
{
    LocalVariableRecord record;
    auto                if_expr = Xi_If{
        Xi_Boolean{true},
        Xi_Integer{2},
        Xi_Integer{2},
    };
    REQUIRE_THAT(TypeAssign(if_expr, record), TypeAssignMatcher(type::i64{}));

    auto if_expr2 = Xi_If{
        Xi_Boolean{true},
        Xi_Integer{2},
        Xi_Boolean{true},
    };
    auto if_expr2_type = TypeAssign(if_expr2, record);
    REQUIRE(!if_expr2_type.has_value());
}

TEST_CASE("Type Assign Func")
{
    auto func_match = Xi_Program{
        {
            Xi_Decl{"f", "i64", {"i64", "i64"}},
            Xi_Func{"f", {{Xi_Iden{"x"}, Xi_Iden{"y"}}}, Xi_Integer{2}},
        },
    };
    REQUIRE_THAT(
        TypeAssign(func_match),
        TypeAssignMatcher(std::vector<type::Xi_Type>{
            type::function{type::i64{}, {type::i64{}, type::i64{}}},
            type::function{type::i64{}, {type::i64{}, type::i64{}}}})
    );

    GetSymbolTable().clear();
    auto func_match_binop = Xi_Program{
        {
            Xi_Decl{"f", "buer", {"i64", "i64"}},
            Xi_Func{
                "f",
                {{Xi_Iden{"x"}, Xi_Iden{"y"}}},
                Xi_Binop{Xi_Iden{"x"}, Xi_Iden{"y"}, Xi_Op::Lt}},
        },
    };
    REQUIRE_THAT(
        TypeAssign(func_match_binop),
        TypeAssignMatcher(std::vector<type::Xi_Type>{
            type::function{type::buer{}, {type::i64{}, type::i64{}}},
            type::function{type::buer{}, {type::i64{}, type::i64{}}},
        })
    );

    GetSymbolTable().clear();
    auto func_mismatch_binop = Xi_Program{
        {
            Xi_Decl{"f", "buer", {"i64", "i64"}},
            Xi_Func{
                "f",
                {{Xi_Iden{"x"}, Xi_Iden{"y"}}},
                Xi_Binop{Xi_Iden{"x"}, Xi_Iden{"y"}, Xi_Op::And}},
        },
    };
    auto func_mismatch_binop_type = TypeAssign(func_mismatch_binop);
    REQUIRE(!func_mismatch_binop_type.has_value());
    REQUIRE(
        func_mismatch_binop_type.error().err == TypeAssignError::TypeMismatch
    );
}

TEST_CASE("Assign Call")
{
    auto func_call_match = Xi_Program{
        {
            Xi_Decl{"f", "i64", {"i64", "i64"}},
            Xi_Func{"f", {{Xi_Iden{"x"}, Xi_Iden{"y"}}}, Xi_Integer{2}},
            Xi_Call{"f", {Xi_Integer{2}, Xi_Integer{2}}},
        },
    };
    REQUIRE_THAT(
        TypeAssign(func_call_match),
        TypeAssignMatcher(std::vector<type::Xi_Type>{
            type::function{type::i64{}, {type::i64{}, type::i64{}}},
            type::function{type::i64{}, {type::i64{}, type::i64{}}},
            type::i64{},
        })
    );

    GetSymbolTable().clear();
    auto func_call_mismatch = Xi_Program{
        {
            Xi_Decl{"f", "i64", {"i64", "i64"}},
            Xi_Func{"f", {{Xi_Iden{"x"}, Xi_Iden{"y"}}}, Xi_Integer{2}},
            Xi_Call{"f", {Xi_Boolean{true}, Xi_Integer{2}}},
        },
    };

    auto func_call_mismatch_type = TypeAssign(func_call_mismatch);
    REQUIRE(!func_call_mismatch_type.has_value());
    REQUIRE(
        func_call_mismatch_type.error().err == TypeAssignError::TypeMismatch
    );
}
} // namespace xi