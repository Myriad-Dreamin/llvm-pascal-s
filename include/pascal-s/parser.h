//
// Created by kamiyoru on 2020/5/8.
//

#ifndef PASCAL_S_PARSER_H
#define PASCAL_S_PARSER_H

#include <vector>
#include "interface.h"
#include "llvm-ast.h"
#include "exception.h"
#include <set>
#include <pascal-s/lib/guess.h>

struct Parser {

    using error_references = std::vector<PascalSError *>;

    // 解析语法
    virtual ast::Node *parse() = 0;

    // 判断是否存在语法错误
    virtual bool has_error() = 0;

    // 获取所有的语法错误
    virtual const error_references &get_all_errors() = 0;

    virtual ~Parser() {}
};


#endif //PASCAL_S_PARSER_H
