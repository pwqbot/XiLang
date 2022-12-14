#pragma once

#include "compiler/parser/utils.h"

#include <compiler/ast/ast.h>
#include <compiler/ast/ast_format.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <map>
#include <optional>
#include <spdlog/spdlog.h>

namespace xi
{
using codegen_result_t = std::optional<llvm::Value *>;

static std::unique_ptr<llvm::LLVMContext>   context;
static std::unique_ptr<llvm::IRBuilder<>>   builder;
static std::unique_ptr<llvm::Module>        module;
static std::map<std::string, llvm::Value *> namedValues;

inline void InitializeModule()
{
    static const std::string moduleName = "xi module";
    context                             = std::make_unique<llvm::LLVMContext>();
    module  = std::make_unique<llvm::Module>(moduleName, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t;

template <typename T>
auto CodeGen(recursive_wrapper<T> wrapper)
{
    return CodeGen(wrapper.get());
}

auto CodeGen(Xi_Real real) -> codegen_result_t
{
    return llvm::ConstantFP::get(*context, llvm::APFloat(real.value));
}

auto CodeGen(Xi_Integer integer) -> codegen_result_t
{
    llvm::APSInt x;
    return llvm::ConstantInt::get(
        *context, llvm::APInt(64, static_cast<uint64_t>(integer.value), true)
    );
}

auto CodeGen(Xi_Boolean boolean) -> codegen_result_t
{
    return llvm::ConstantInt::get(*context, llvm::APInt(1, boolean.value));
}

auto CodeGen(Xi_If) -> codegen_result_t
{
    return std::nullopt;
}

auto CodeGen(Xi_Iden iden) -> codegen_result_t
{
    auto *value = namedValues[iden.name];
    if (!value)
    {
        spdlog::error("Unknown variable name {}", iden.name);
        return std::nullopt;
    }
    return value;
}

auto CodeGen(const Xi_Binop &bop) -> codegen_result_t
{
    auto l = CodeGen(bop.lhs);
    auto r = CodeGen(bop.rhs);
    // TODO(ding.wang): use monadic bind
    if (l == std::nullopt || r == std::nullopt)
    {
        return std::nullopt;
    }
    auto *lv = l.value();
    auto *rv = r.value();
    switch (bop.op)
    {
    case Xi_Op::Add:
        return builder->CreateAdd(lv, rv, "addtmp");
    case Xi_Op::Sub:
        return builder->CreateSub(lv, rv, "subtmp");
    case Xi_Op::Mul:
        return builder->CreateMul(lv, rv, "multmp");
    case Xi_Op::Div:
        return builder->CreateSDiv(lv, rv, "divtmp");
    case Xi_Op::Lt:
        return builder->CreateICmpULT(lv, rv, "cmptmp");
    case Xi_Op::Gt:
        return builder->CreateICmpUGT(lv, rv, "cmptmp");
    case Xi_Op::Eq:
        return builder->CreateICmpEQ(lv, rv, "cmptmp");
    case Xi_Op::Neq:
        return builder->CreateICmpNE(lv, rv, "cmptmp");
    case Xi_Op::Leq:
        return builder->CreateICmpULE(lv, rv, "cmptmp");
    case Xi_Op::Geq:
        return builder->CreateICmpUGE(lv, rv, "cmptmp");
    default:
        return std::nullopt;
    }
}

auto CodeGen(const Xi_Unop uop) -> codegen_result_t
{
    auto u = CodeGen(uop.expr);
    if (u == std::nullopt)
    {
        return std::nullopt;
    }
    auto *uv = u.value();
    switch (uop.op)
    {
    case Xi_Op::Sub:
        return builder->CreateNeg(uv);
    case Xi_Op::Not:
        return builder->CreateNot(uv);
    default:
        return std::nullopt;
    }
}

auto CodeGen(const Xi_Call call_expr) -> codegen_result_t
{
    llvm::Function *calleeF = module->getFunction(call_expr.name.name);
    if (!calleeF)
    {
        return std::nullopt;
    }
    if (calleeF->arg_size() != call_expr.args.size())
    {
        return std::nullopt;
    }
    std::vector<llvm::Value *> argsV;
    for (auto &arg : call_expr.args)
    {
        auto argV = CodeGen(arg);
        if (argV == std::nullopt)
        {
            return std::nullopt;
        }
        argsV.push_back(argV.value());
    }

    return builder->CreateCall(calleeF, argsV, "calltmp");
}

auto CodeGen(Xi_Lam) -> codegen_result_t
{
    return std::nullopt;
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t
{
    return std::visit([](auto expr_) { return CodeGen(expr_); }, expr);
}

auto CodeGen(Xi_Xi) -> codegen_result_t
{
    // std::vector<llvm::AllocaInst *> old_bindings;
    // llvm::Function *theFunction = builder->GetInsertBlock()->getParent();
    return std::nullopt;
}

auto CodeGen(Xi_Stmt stmt) -> codegen_result_t
{
    return std::visit([](auto stmt_) { return CodeGen(stmt_); }, stmt);
}

auto CodeGen(Xi_Program program) -> std::string
{
    InitializeModule();
    for (auto &stmt : program.stmts)
    {
        auto value = CodeGen(stmt);
        if (value == std::nullopt)
        {
            return "";
        }
        auto vv = value.value();
        vv->print(llvm::outs());
    }
    std::string              output;
    llvm::raw_string_ostream os(output);
    module->print(os, nullptr);
    return output;
}
} // namespace xi
