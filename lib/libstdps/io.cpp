//
// Created by kamiyoru on 2020/5/9.
//

#include <cstdio>
#include <cstdint>

#ifdef _WIN32
#define LL_FMT "%I64d"
#else
#define LL_FMT "%lld"
#endif
extern "C" {

int read_char(char *s) {
    return scanf("%c", s);
}

int read_bool(char *s) {
    int bool_input;
    int si = scanf("%d", &bool_input);
    if (bool_input != 0) { *s = 1; } else { *s = 0; }
    return si;
}

int read_int64(int64_t *s) {
    return scanf(LL_FMT, s);
}

int read_int32(int32_t *s) {
    return scanf("%d", s);
}

int read_real(double *s) {
    return scanf("%lf", s);
}

int write_char(char s) {
    return printf("%c", s);
}

int write_bool(char s) {
    return printf("%c", s ? '1' : '0');
}

int write_int64(int64_t s) {
    return printf(LL_FMT, s);
}

int write_int32(int32_t s) {
    return printf("%d", s);
}

int write_real(double s) {
    return printf("%lf", s);
}

//int read_(char *s) {
//    return scanf("%c", s);
//}


#define xps_convert_function(fr_type, to_type, fr_type_c, to_type_c) to_type_c ps_ ##fr_type## _to_ ##to_type (fr_type_c f) { return (to_type_c)(f); }

xps_convert_function(int64, int32, int64_t, int32_t)
xps_convert_function(int64, char, int64_t, char)
xps_convert_function(int32, char, int32_t, char)
xps_convert_function(int64, boolean, int64_t, bool)
xps_convert_function(int32, boolean, int32_t, bool)
xps_convert_function(char, boolean, char, bool)
xps_convert_function(int64, real, int64_t, double)
xps_convert_function(int32, real, int32_t, double)
xps_convert_function(char, real, char, double)
xps_convert_function(boolean, real, bool, double)

#undef xps_convert_function


}

