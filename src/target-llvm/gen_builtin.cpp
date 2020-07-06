//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>

// 预定义函数，链接时使用
void LLVMBuilder::prepend_lib_standard_pascal_s() {
#define pascal_s_declare_read_write_fn_prototype(param_proto)  llvm::FunctionType::get(\
    llvm::Type::getInt32Ty(ctx), {\
            param_proto,\
    }, false)

    // 读函数
    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt8PtrTy(ctx)),
            // 声明为外部链接，函数名，将函数定义附加到modules模块中
                     Function::ExternalLinkage, "read_char", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt1PtrTy(ctx)),
                     Function::ExternalLinkage, "read_bool", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt64PtrTy(ctx)),
                     Function::ExternalLinkage, "read_int64", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt32PtrTy(ctx)),
                     Function::ExternalLinkage, "read_int32", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getDoublePtrTy(ctx)),
                     Function::ExternalLinkage, "read_real", modules);

    // 写函数
    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt8Ty(ctx)),
                     Function::ExternalLinkage, "write_char", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt1Ty(ctx)),
                     Function::ExternalLinkage, "write_bool", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt64Ty(ctx)),
                     Function::ExternalLinkage, "write_int64", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getInt32Ty(ctx)),
                     Function::ExternalLinkage, "write_int32", modules);

    Function::Create(pascal_s_declare_read_write_fn_prototype(llvm::Type::getDoubleTy(ctx)),
                     Function::ExternalLinkage, "write_real", modules);

    // 有损转换函数
    auto convert_prototype = llvm::FunctionType::get(
            llvm::Type::getInt64Ty(ctx), {
                    llvm::Type::getDoubleTy(ctx),
            }, false);
    Function::Create(convert_prototype, Function::ExternalLinkage, "ps_real_to_int64", modules);
    convert_prototype = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx), {
                    llvm::Type::getDoubleTy(ctx),
            }, false);
    Function::Create(convert_prototype, Function::ExternalLinkage, "ps_real_to_int32", modules);
#define pascal_s_declare_i64_convert_prototype(ret_proto)  llvm::FunctionType::get(\
    ret_proto, {\
            llvm::Type::getInt64Ty(ctx),\
    }, false)
    Function::Create(pascal_s_declare_i64_convert_prototype(llvm::Type::getDoubleTy(ctx)),
                     Function::ExternalLinkage, "ps_int64_to_real", modules);
    Function::Create(pascal_s_declare_i64_convert_prototype(llvm::Type::getInt32Ty(ctx)),
                     Function::ExternalLinkage, "ps_int64_to_int32", modules);
    Function::Create(pascal_s_declare_i64_convert_prototype(llvm::Type::getInt8Ty(ctx)),
                     Function::ExternalLinkage, "ps_int64_to_char", modules);
    Function::Create(pascal_s_declare_i64_convert_prototype(llvm::Type::getInt1Ty(ctx)),
                     Function::ExternalLinkage, "ps_int64_to_boolean", modules);
#define pascal_s_declare_i32_convert_prototype(ret_proto)  llvm::FunctionType::get(\
    ret_proto, {\
            llvm::Type::getInt32Ty(ctx),\
    }, false)
    Function::Create(pascal_s_declare_i32_convert_prototype(llvm::Type::getDoubleTy(ctx)),
                     Function::ExternalLinkage, "ps_int32_to_real", modules);
    Function::Create(pascal_s_declare_i32_convert_prototype(llvm::Type::getInt8Ty(ctx)),
                     Function::ExternalLinkage, "ps_int32_to_char", modules);
    Function::Create(pascal_s_declare_i32_convert_prototype(llvm::Type::getInt1Ty(ctx)),
                     Function::ExternalLinkage, "ps_int32_to_boolean", modules);
#define pascal_s_declare_i8_convert_prototype(ret_proto)  llvm::FunctionType::get(\
    ret_proto, {\
            llvm::Type::getInt8Ty(ctx),\
    }, false)
    Function::Create(pascal_s_declare_i8_convert_prototype(llvm::Type::getDoubleTy(ctx)),
                     Function::ExternalLinkage, "ps_char_to_real", modules);
    Function::Create(pascal_s_declare_i8_convert_prototype(llvm::Type::getInt1Ty(ctx)),
                     Function::ExternalLinkage, "ps_char_to_boolean", modules);
#define pascal_s_declare_i1_convert_prototype(ret_proto)  llvm::FunctionType::get(\
    ret_proto, {\
            llvm::Type::getInt1Ty(ctx),\
    }, false)
    Function::Create(pascal_s_declare_i8_convert_prototype(llvm::Type::getDoubleTy(ctx)),
                     Function::ExternalLinkage, "ps_boolean_to_real", modules);

#undef pascal_s_declare_read_write_fn_prototype
#undef pascal_s_declare_i64_convert_prototype
#undef pascal_s_declare_i32_convert_prototype
#undef pascal_s_declare_i8_convert_prototype
#undef pascal_s_declare_i1_convert_prototype
}

void LLVMBuilder::LinkedContext::deconstruct() const {
    for (auto &ai: *array_infos) {
        delete ai.second;
        ai.second = nullptr;
    }
    array_infos->clear();
    ctx->clear();
    const_ctx->clear();
}
