//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>


llvm::Type *LLVMBuilder::create_type(const ast::TypeSpec *spec) {
    if (spec->type == ast::Type::BasicTypeSpec) {
        return create_basic_type(reinterpret_cast<const ast::BasicTypeSpec *>(spec));
    } else if (spec->type == ast::Type::ArrayTypeSpec) {

        // multiply对应辅助函数calc_periods_size
        // sz = multiply each delta(period) * period.offset of periods
        auto sz = calc_periods_size(reinterpret_cast<const ast::ArrayTypeSpec *>(spec));

        if (sz <= 0) {
            return nullptr;
        }

        // 映射成一维数组
        // return llvm_array_type( create_basic_type(ps_type), sz )
        return llvm::ArrayType::get(create_basic_type_from_keyword(
                reinterpret_cast<const ast::ArrayTypeSpec *>(spec)->keyword), sz);
    } else {
        llvm_pascal_s_report_semantic_error_n(
                spec,
                fmt::format("type spec {} has no gen function", static_cast<uint16_t>(spec->type)));
        return nullptr;
    }
}

llvm::Type *LLVMBuilder::create_basic_type(const ast::BasicTypeSpec *spec) {
    return create_basic_type_from_keyword(spec->keyword);
}

llvm::Type *LLVMBuilder::create_basic_type_from_keyword(const Keyword *spec) {
    // iN 对应C语言stdint.h中的int_N
    switch (spec->type) {
        case KeywordType::Char:
            // return i8
            return llvm::Type::getInt8Ty(ctx);
        case KeywordType::Integer:
            // return i64
            return llvm::Type::getInt64Ty(ctx);
        case KeywordType::Real:
            // return double
            return llvm::Type::getDoubleTy(ctx);
        case KeywordType::Boolean:
            // return i1
            return llvm::Type::getInt1Ty(ctx);
//            case KeywordType::String:
        default:
            llvm_pascal_s_report_semantic_error_n(
                    spec,
                    fmt::format("basic type spec {} has no gen function",
                                static_cast<uint16_t>(spec->type)));
            return nullptr;
    }
}
