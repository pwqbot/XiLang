#pragma once

#include "compiler/parser/utils.h"

#include <compiler/ast/ast.h>
#include <compiler/ast/ast_format.h>
#include <compiler/generator/error.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <map>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

namespace xi
{
using codegen_result_t = ExpectedCodeGen<llvm::Value *>;

static std::unique_ptr<llvm::LLVMContext>   context;
static std::unique_ptr<llvm::IRBuilder<>>   builder;
static std::unique_ptr<llvm::Module>        module;
static std::map<std::string, llvm::Value *> namedValues;
constexpr int                               llvm_int_precision = 64;

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
        *context,
        llvm::APInt(
            llvm_int_precision, static_cast<uint64_t>(integer.value), true
        )
    );
}

auto CodeGen(Xi_Boolean boolean) -> codegen_result_t
{
    return llvm::ConstantInt::get(
        *context, llvm::APInt(1, static_cast<uint64_t>(boolean.value))
    );
}

auto CodeGen(Xi_If) -> codegen_result_t
{
    return tl::unexpected(ErrorCodeGen(ErrorCodeGen::NotImplemented, "If Expr")
    );
}

auto CodeGen(Xi_Iden iden) -> codegen_result_t
{
    auto *value = namedValues[iden.name];
    if (value == nullptr)
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownVariable, iden.name)
        );
    }
    return value;
}

auto CodeGen(Xi_Binop bop) -> codegen_result_t
{
    return CodeGen(bop.lhs) >>= [bop](llvm::Value *lhs)
    {
        return CodeGen(bop.rhs) >>=
               [lhs, bop](llvm::Value *rhs) -> codegen_result_t
        {
            switch (bop.op)
            {
            case Xi_Op::Add:
                return builder->CreateAdd(lhs, rhs, "addtmp");
            case Xi_Op::Sub:
                return builder->CreateSub(lhs, rhs, "subtmp");
            case Xi_Op::Mul:
                return builder->CreateMul(lhs, rhs, "multmp");
            case Xi_Op::Div:
                return builder->CreateSDiv(lhs, rhs, "divtmp");
            case Xi_Op::Lt:
                return builder->CreateICmpULT(lhs, rhs, "cmptmp");
            case Xi_Op::Gt:
                return builder->CreateICmpUGT(lhs, rhs, "cmptmp");
            case Xi_Op::Eq:
                return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
            case Xi_Op::Neq:
                return builder->CreateICmpNE(lhs, rhs, "cmptmp");
            case Xi_Op::Leq:
                return builder->CreateICmpULE(lhs, rhs, "cmptmp");
            case Xi_Op::Geq:
                return builder->CreateICmpUGE(lhs, rhs, "cmptmp");
            default:
                return tl::unexpected(ErrorCodeGen(
                    ErrorCodeGen::UnknownOperator, magic_enum::enum_name(bop.op)
                ));
            }
        };
    };
}

auto CodeGen(Xi_Unop uop) -> codegen_result_t
{
    return CodeGen(uop.expr) >>= [uop](auto expr_code) -> codegen_result_t
    {
        switch (uop.op)
        {
        case Xi_Op::Sub:
            return builder->CreateNeg(expr_code);
        case Xi_Op::Not:
            return builder->CreateNot(expr_code);
        default:
            return tl::unexpected(ErrorCodeGen(
                ErrorCodeGen::UnknownOperator, magic_enum::enum_name(uop.op)
            ));
        }
    };
}

auto CodeGen(Xi_Call call_expr) -> codegen_result_t
{
    llvm::Function *calleeF = module->getFunction(call_expr.name.name);
    if (calleeF == nullptr)
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownFunction, call_expr.name.name)
        );
    }
    if (calleeF->arg_size() != call_expr.args.size())
    {
        return tl::unexpected(ErrorCodeGen(
            ErrorCodeGen::InvalidArgumentCount,
            fmt::format(
                "want {}, get {}", calleeF->arg_size(), call_expr.args.size()
            )
        ));
    }

    return flatmap(call_expr.args, [](auto arg) { return CodeGen(arg); }) >>=
           [calleeF](auto argsV) -> codegen_result_t
    {
        return builder->CreateCall(calleeF, argsV, "calltmp");
    };
}

auto CodeGen(Xi_Lam) -> codegen_result_t
{
    return tl::unexpected(ErrorCodeGen(ErrorCodeGen::NotImplemented, "Lambda"));
}

auto XiTypeToLLVMType(Xi_Type xi_t) -> ExpectedCodeGen<llvm::Type *>
{
    switch (xi_t)
    {
    case Xi_Type::real:
        return llvm::Type::getDoubleTy(*context);
    case Xi_Type::i64:
        return llvm::Type::getInt64Ty(*context);
    default:
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownType, magic_enum::enum_name(xi_t))
        );
    }
}

auto CodeGen(Xi_String) -> codegen_result_t
{
    return tl::unexpected(ErrorCodeGen(ErrorCodeGen::NotImplemented, "String"));
}

auto CodeGen(Xi_Decl decl) -> codegen_result_t
{
    return flatmap(decl.params_type, XiTypeToLLVMType) >>=
           [decl](auto arg_types) -> codegen_result_t
    {
        return XiTypeToLLVMType(decl.return_type) >>=
               [decl, arg_types](auto return_type) -> codegen_result_t
        {
            auto *func_type =
                llvm::FunctionType::get(return_type, arg_types, false);

            auto *function = llvm::Function::Create(
                func_type,
                llvm::Function::ExternalLinkage,
                static_cast<std::string>(decl.name),
                module.get()
            );

            return function;
        };
    };
}

auto CodeGen(Xi_Expr expr) -> codegen_result_t
{
    return std::visit([](auto expr_) { return CodeGen(expr_); }, expr);
}

auto checkFunc(Xi_Func xi) -> ExpectedCodeGen<llvm::Function *>
{
    auto *func = module->getFunction(static_cast<std::string>(xi.name));
    if (func == nullptr)
    {
        spdlog::error("Unknown function referenced");
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::UnknownFunction, xi.name.name)
        );
    }
    if (!func->empty())
    {
        return tl::unexpected(
            ErrorCodeGen(ErrorCodeGen::Redefinition, xi.name.name)
        );
    }
    if (xi.params.size() != func->arg_size())
    {
        return tl::unexpected(ErrorCodeGen(
            ErrorCodeGen::InvalidArgumentCount,
            fmt::format("want {}, get {}", func->arg_size(), xi.params.size())
        ));
    }
    return func;
}

auto CodeGen(Xi_Func xi) -> codegen_result_t
{
    return checkFunc(xi) >>= [xi](auto func) -> codegen_result_t
    {
        auto *bb = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(bb);
        namedValues.clear();
        auto param_it = xi.params.begin();
        for (auto &arg : func->args())
        {
            arg.setName(param_it->name);
            param_it++;
            namedValues[arg.getName().str()] = &arg;
        }
        auto body = CodeGen(xi.expr);
        if (body)
        {
            builder->CreateRet(body.value());
            llvm::verifyFunction(*func);
            return func;
        }
        func->eraseFromParent();
        return body;
    };
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
        if (value.has_value())
        {
            auto *vv = value.value();
            vv->print(llvm::outs());
        }
        else
        {
            spdlog::error("Error: {}", value.error().what());
            return "";
        }
    }
    std::string              output;
    llvm::raw_string_ostream os(output);
    module->print(os, nullptr);
    return output;
}
} // namespace xi
