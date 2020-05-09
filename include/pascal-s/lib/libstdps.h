//
// Created by kamiyoru on 2020/5/9.
//

#ifndef PASCAL_S_LIBSTDPS_H
#define PASCAL_S_LIBSTDPS_H

#include <cstdint>

extern "C" {

int32_t read_char(char *s);
int32_t read_int64(int64_t *s);
int32_t read_real(double *s);

int32_t write_char(char s);
int32_t write_int64(int64_t s);
int32_t write_real(double s);

};

#endif //PASCAL_S_LIBSTDPS_H
