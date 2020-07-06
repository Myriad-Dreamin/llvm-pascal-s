//
// Created by kamiyoru on 2020/6/29.
//

#ifndef PASCAL_S_STD_TYPE_H
#define PASCAL_S_STD_TYPE_H

namespace pascal_s {
    using token_type_underlying_type = uint32_t;
    using keyword_type_underlying_type = uint8_t;
    using marker_type_underlying_type = uint8_t;

    using pascal_s_integer_t = int64_t;
    using pascal_s_real_t = double;

    using errno_t = uint32_t;
    using line_t = uint32_t;
    using column_t = uint32_t;
    using length_t = uint32_t;
    using offset_t = uint64_t;
#define EmbedPascalSPosField pascal_s::line_t offset = 0;\
    pascal_s::column_t column = 0;\
    pascal_s::length_t length = 0;\
    pascal_s::offset_t line = 0
#define GetEmbedPascalSPosFieldFirstProperty offset
#define GetEmbedPascalSPosField() ((pascal_s::Pos*)(&GetEmbedPascalSPosFieldFirstProperty))

    struct Pos {

        EmbedPascalSPosField;
    };

    inline void copy_pos(Pos *dst, const Pos *src) {
        *dst = *src;
    }

    template<typename D, typename S>
    inline void copy_pos_any(D &dst, S &src) {
        copy_pos(dst.visit_pos(), src.visit_pos());
    }

    template<typename D, typename S>
    inline void copy_pos_any(D *dst, S &src) {
        copy_pos(dst->visit_pos(), src.visit_pos());
    }

    template<typename D, typename S>
    inline void copy_pos_any(D &dst, S *src) {
        copy_pos(dst.visit_pos(), src->visit_pos());
    }

    template<typename D, typename S>
    inline void copy_pos_any(D *dst, S *src) {
        copy_pos(dst->visit_pos(), src->visit_pos());
    }
}

#endif //PASCAL_S_STD_TYPE_H
