//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>

static pascal_s::Pos static_pos{0, 0, 1, 1};

LLVMBuilder::Function *LLVMBuilder::rename_program_to_pascal_s_native(LLVMBuilder::Function *f) {
    Function *maybe_conflict = modules.getFunction(pascal_main_function_name);
    if (maybe_conflict) {
        llvm_pascal_s_report_semantic_error(&static_pos, "rename function error");
        return nullptr;
    }

    f->setName(pascal_main_function_name);
    return f;
}


llvm::Value *LLVMBuilder::assign_lvalue(const pascal_s::Pos *lvalue_pos, Value *ptr, Value *rhs) {
    llvm::Type *elemType = nullptr;
    if (ptr->getType()->isPointerTy()) {
        elemType = ptr->getType()->getPointerElementType();
    } else {
        llvm_pascal_s_report_semantic_error(
                lvalue_pos,
                fmt::format(
                        "lvalue type incompatible (internal error debugging, please report it to camiyoru@gmail.com), "
                        "got lhs type = {}, rhs type = {}",
                        format_type(ptr->getType()), format_type(rhs->getType())));
        return nullptr;
    }

    if (rhs->getType()->getTypeID() != elemType->getTypeID()) {
        // todo: implicit type conversion feature
        llvm_pascal_s_report_semantic_error(lvalue_pos, fmt::format("lvalue type incompatible, "
                                                                    "got lhs type = {}, rhs type = {}",
                                                                    format_type(ptr->getType()),
                                                                    format_type(rhs->getType())));
        return nullptr;
    }

    if (elemType->getTypeID() == llvm::Type::IntegerTyID) {
        // todo: signed/unsigned integer type feature
        auto rhs_bit_width = rhs->getType()->getIntegerBitWidth();
        auto elem_type_bit_width = elemType->getIntegerBitWidth();
        if (rhs_bit_width > elem_type_bit_width) {
            auto constant_integer = llvm::dyn_cast<llvm::ConstantInt>(rhs);
            if (constant_integer == nullptr) {
                llvm_pascal_s_report_semantic_warning(lvalue_pos,
                                                      fmt::format("trunc a int{} to int{} is dangerous", rhs_bit_width,
                                                                  elem_type_bit_width));
            } else {
                if (constant_integer->getSExtValue() >= (1ULL << (uint64_t) (elemType->getIntegerBitWidth() - 1))
                    || constant_integer->getSExtValue() <
                       -(int64_t) (1ULL << (uint64_t) (elemType->getIntegerBitWidth() - 1))) {
                    llvm_pascal_s_report_semantic_error(
                            lvalue_pos,
                            fmt::format("truncated value {}", constant_integer->getSExtValue()));
                    return nullptr;
                }
            }


            rhs = ir_builder.CreateTrunc(rhs, elemType, "");
        } else if (rhs_bit_width < elem_type_bit_width) {
            rhs = ir_builder.CreateSExt(rhs, elemType, "");
        }
    } else if (elemType->getTypeID() != llvm::Type::DoubleTyID) {
        llvm_pascal_s_report_semantic_error(lvalue_pos,
                                            fmt::format("lvalue unknown llvm type, got {}",
                                                        format_type(rhs->getType())));
        return nullptr;
    }

    // out_code( store rhs into lvalue )
    ir_builder.CreateStore(rhs, ptr);
    return rhs;
}


bool
LLVMBuilder::check_extend_type(const pascal_s::Pos *pos, llvm::Value *&source, llvm::Value *&target,
                               bool target_changeable) {
    if (source->getType()->getTypeID() != target->getType()->getTypeID()) {
        // todo: signed/unsigned integer type feature
        llvm_pascal_s_report_semantic_error(
                pos,
                fmt::format("the expression's lhs(source) and rhs(target) type is incompatible, "
                            "got lhs type = {}, rhs type = {}",
                            format_type(source->getType()), format_type(target->getType())));
        return false;
    } else if (source->getType()->isIntegerTy() &&
               (source->getType()->getIntegerBitWidth() != target->getType()->getIntegerBitWidth())) {
        llvm_pascal_s_report_semantic_warning(
                pos, fmt::format(
                "the expression's lhs(source) and rhs(target) type is incompatible, got lhs type = {}{}, rhs type = {}{}",
                format_type(source->getType()), source->getType()->getIntegerBitWidth(),
                format_type(target->getType()), target->getType()->getIntegerBitWidth()));
        if (source->getType()->getIntegerBitWidth() < target->getType()->getIntegerBitWidth()) {
            source = ir_builder.CreateSExt(source, target->getType(), "ext_tmp");
        } else if (target_changeable) {
            target = ir_builder.CreateSExt(target, source->getType(), "ext_tmp");
        } else {
            llvm_pascal_s_report_semantic_error(
                    pos, fmt::format(
                    "target type not changeable, got lhs type = {}{}, rhs type = {}{}",
                    format_type(source->getType()), source->getType()->getIntegerBitWidth(),
                    format_type(target->getType()), target->getType()->getIntegerBitWidth()));
            return false;
        }
    }

    return true;
}

bool LLVMBuilder::check_extend_type(const pascal_s::Pos *pos, llvm::Value *&source, llvm::Type *target_type) {
    if (source->getType()->getTypeID() != target_type->getTypeID()) {
        // todo: signed/unsigned integer type feature
        llvm_pascal_s_report_semantic_warning(
                pos,
                fmt::format("the expression's lhs(source) and rhs(target_type) type is incompatible, "
                            "got lhs type = {}, rhs type = {}",
                            format_type(source->getType()), format_type(target_type)));
        return false;
    } else if (source->getType()->isIntegerTy() &&
               (source->getType()->getIntegerBitWidth() != target_type->getIntegerBitWidth())) {
        llvm_pascal_s_report_semantic_warning(
                pos, fmt::format(
                "the expression's lhs(source) and rhs(target_type) type is incompatible, got lhs type = {}{}, rhs type = {}{}",
                format_type(source->getType()), source->getType()->getIntegerBitWidth(),
                format_type(target_type), target_type->getIntegerBitWidth()));
        if (source->getType()->getIntegerBitWidth() < target_type->getIntegerBitWidth()) {
            source = ir_builder.CreateSExt(source, target_type, "ext_tmp");
        } else {
            return false;
        }
    }

    return true;
}

