//
// Created by kamiyoru on 2020/5/6.
//

#ifndef PASCAL_S_STL_H
#define PASCAL_S_STL_H

#ifndef later_than_cxx14
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define msvc_later_than_cxx14 true
#else
#define msvc_later_than_cxx14 false
#endif
#define later_than_cxx14 (msvc_later_than_cxx14 || __cplusplus > 201402L)
#endif

#if later_than_cxx14
#include <string_view>
using string_view = std::string_view;
using string_view_return = std::string_view;
#else
using string_view = const std::string &;
using string_view_return = std::string;
#endif

#endif //PASCAL_S_STL_H
