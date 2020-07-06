//
// Created by kamiyoru on 2020/7/3.
//

#ifndef PASCAL_S_LLVM_HELPER_CPP
#define PASCAL_S_LLVM_HELPER_CPP

#include <target/llvm.h>
#include <string>

llvm::Value *create_int64_literal(llvm::LLVMContext &ctx, int64_t literal) {
    return llvm::Constant::getIntegerValue(
            llvm::IntegerType::get(ctx, 64), llvm::APInt(64, literal, true));
}

std::string format_type(llvm::Type *t) {
    switch (t->getTypeID()) {
        case llvm::Type::PointerTyID:
            return "*" + format_type(t->getPointerElementType());
        case llvm::Type::IntegerTyID:
            switch (t->getIntegerBitWidth()) {
                case 1:
                    return "boolean";
                case 8:
                    return "char";
                case 64:
                    return "integer";
                default:
                    std::cerr << "format llvm type error type id: " << t->getTypeID();
                    assert(false);
                    return "";
            }
        case llvm::Type::DoubleTyID:
            return "real";
        case llvm::Type::ArrayTyID:
            return "array-" + format_type(t->getArrayElementType());
        default:
            std::cerr << "format llvm type error type id: " << t->getTypeID();
            return "llvm-type(" + std::to_string(t->getTypeID()) + ")";
    }
}

#endif //PASCAL_S_LLVM_HELPER_CPP
