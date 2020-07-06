//
// Created by kamiyoru on 2020/7/5.
//

#ifndef PASCAL_S_ERROR_CODE_H
#define PASCAL_S_ERROR_CODE_H

#include <cstdint>

namespace pascal_s::ProgramErrorCode {

    enum ProgramErrorCode : int32_t {
        CodeOK = 0,
        // has lexical error or parse error
        PascalSMagicBegin = 123,
        ParseFailed = PascalSMagicBegin - 1,
        ParserReturnNullNode = PascalSMagicBegin - 2,
        BuildFailed = PascalSMagicBegin - 3,
        BuilderCouldNotGenTarget = PascalSMagicBegin - 4,

        PascalSLLVMMagicBegin = PascalSMagicBegin - 20,
        CantOpenTargetFile = PascalSLLVMMagicBegin - 1,
        TargetFileEmitFailed = PascalSLLVMMagicBegin - 2,
    };
}

#endif //PASCAL_S_ERROR_CODE_H
