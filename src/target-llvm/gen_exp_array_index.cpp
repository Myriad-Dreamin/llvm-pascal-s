//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>

int64_t LLVMBuilder::calc_periods_size(const ast::ArrayTypeSpec *spec) {
    int64_t ret = 1;
    for (size_t i = 0; i < spec->periods.size(); i++) {
        auto &p = spec->periods[i];
        if (p.second < p.first) {
            llvm_pascal_s_report_semantic_error_n(
                    spec,
                    fmt::format(
                            "the {}-th dim range of array is invalid, want left range({}) > right range({})",
                            i + 1, p.first, p.second));
        }

        // multiply each delta(period) * period.offset of periods
        ret *= (p.second - p.first + 1);
    }
    return ret;
}

void LLVMBuilder::code_gen_offset(std::vector<llvm::Value *> &offset, const pascal_s::ArrayInfo *ai,
                                  const ast::ExpressionList *exp_list) {
    if (ai->spec->periods.size() != exp_list->vec.size()) {
        llvm_pascal_s_report_semantic_error_n(
                exp_list,
                fmt::format(
                        "array expected dim is {}, but length of expression list is {}",
                        ai->spec->periods.size(), exp_list->vec.size()));
        offset.clear();
        return;
    }

    Value *e, *value_offset = create_int64_literal(ctx, 0);
    llvm::Type *llvm_i64 = llvm::IntegerType::get(ctx, 64);
    int64_t mp = 1;
    llvm::ConstantInt *constant_index;

    for (int64_t i = ai->spec->periods.size() - 1; i >= 0; i--) {
        auto &pd = ai->spec->periods[i];
        e = code_gen(exp_list->vec[i]);
        if (e->getType()->isIntegerTy()) {
            if (e->getType()->getIntegerBitWidth() < 64) {
                e = ir_builder.CreateSExt(e, llvm_i64, "exp_ext_tmp");
            }
        } else {
            llvm_pascal_s_report_semantic_error_n(
                    exp_list->vec[i],
                    fmt::format(
                            "the {}-th of expression's type for array index wants a integer type, got {}",
                            i + 1, format_type(e->getType())));
            offset.clear();
            return;
        }

        constant_index = llvm::dyn_cast<llvm::ConstantInt>(e);
        if (constant_index) {
            if (constant_index->getValue().getSExtValue() < pd.first) {
                llvm_pascal_s_report_semantic_error_n(
                        exp_list->vec[i],
                        fmt::format(
                                "the {}-th of expression's type for array index is underflow, "
                                "want >= {}, got {}", i + 1, pd.first, constant_index->getValue().getSExtValue()));
                offset.clear();
                return;
            }
            if (constant_index->getValue().getSExtValue() > pd.second) {
                llvm_pascal_s_report_semantic_error_n(
                        exp_list->vec[i],
                        fmt::format(
                                "the {}-th of expression's type for array index is overflow, "
                                "want <= {}, got {}", i + 1, pd.second, constant_index->getValue().getSExtValue()));
                offset.clear();
                return;
            }
        }

        value_offset = ir_builder.CreateAdd(
                value_offset,
                ir_builder.CreateMul(
                        ir_builder.CreateSub(
                                e,
                                create_int64_literal(ctx, pd.first)
                        ),
                        create_int64_literal(ctx, mp)
                )
        );
        mp *= (pd.second - pd.first + 1);
    }
    offset.push_back(value_offset);
}
