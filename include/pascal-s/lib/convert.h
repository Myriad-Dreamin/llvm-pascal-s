//
// Created by kamiyoru on 2020/6/18.
//

#ifndef PASCAL_S_LIB_CONVERT_H
#define PASCAL_S_LIB_CONVERT_H

bool parseBool(const char *literal) {
    if (!strcmp(literal, "true")) {
        return true;
    }
    if (!strcmp(literal, "false")) {
        return false;
    }
    throw std::invalid_argument("convert failed");
}

#endif //PASCAL_S_CONVERT_H
