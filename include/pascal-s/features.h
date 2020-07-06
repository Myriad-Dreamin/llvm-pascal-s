//
// Created by kamiyoru on 2020/5/5.
//

#ifndef PASCAL_S_FEATURES_H
#define PASCAL_S_FEATURES_H

#include "interface.h"
#include "token.h"
#include <functional>
#include <algorithm>
#include <fmt/color.h>

namespace feature {

    template<typename OStream>
    using output_tokens_t = std::function<void(const std::vector<Token *> &tokens, WriterProxy<OStream> &os)>;

    template<typename File, typename OStream, typename Error>
    using format_line_column_error_t = std::function<void(FileProxy<File> f, ErrorProxy<Error> err,
                                                          WriterProxy<OStream> &os)>;

    template<typename OStream>
    void output_tokens(const std::vector<Token *> &tokens, WriterProxy<OStream> &os) {
        os << "out tokens\n";
        for (auto &token: tokens) {
            os.write_data(fmt::format("{}:{}: {}\n", token->line, token->column, convertToString(token)));
        }
    }

    namespace text_style {
        static const struct _format_color_t {
            fmt::text_style style{};

            _format_color_t() FMT_NOEXCEPT {
                style = fmt::fg(
                        fmt::color::tomato
//                        fmt::rgb(0xde6c74)
                );
            }
        } fmt;

        using fmt_t = decltype(fmt::format(""));
        static const int show_width = 50;
    }


    template<typename File, typename OStream>
    int print_char_ref_near(FileProxy<File> f, WriterProxy<OStream> &os,
                            pascal_s::offset_t offset, pascal_s::length_t length, pascal_s::column_t column,
                            bool rev = false) {
        static char buffer[(text_style::show_width * 2) + 5];
        char *pBuffer = buffer;
        size_t buffer_read_l, buffer_read_r;

        if (rev) {
            length = std::min<size_t>(text_style::show_width, column);
        }

        buffer_read_l = offset - std::min<size_t>(text_style::show_width, column);
        buffer_read_r = offset + text_style::show_width;
        assert(buffer_read_l >= 0);

        f.seek(buffer_read_l);
        f.read(pBuffer, buffer_read_r - buffer_read_l);
        pBuffer = pBuffer - buffer_read_l;
        for (int i = offset; i < buffer_read_r; i++) {
            if (pBuffer[i] == '\n' || pBuffer[i] == 0) {
                pBuffer[i] = 0;
                buffer_read_r = i;
                if (!rev && length > i - offset) length = i - offset;
            }
        }

        if (rev) {
            for (int i = offset; i >= buffer_read_l; i--) {
                if (pBuffer[i] == '\n' || pBuffer[i] == 0) {
                    pBuffer[i] = 0;
                    buffer_read_l = i + 1;
                    length = offset - i;
                }
            }
        }

        int hl = column;
        if (text_style::show_width < column) {
            auto h = fmt::format(text_style::fmt.style, "(omitting {} chars) ", column - text_style::show_width);
            os.write_data(fmt::format(text_style::fmt.style, "{}", h));
            hl = fmt::format("{}", column - text_style::show_width).length() + 18 + offset - buffer_read_l;
        }

        os.write_data(fmt::format(text_style::fmt.style, "{}", pBuffer + buffer_read_l)) << '\n';
        os.write_data(fmt::format(text_style::fmt.style, "{:>{}}\n", fmt::format("{:^<{}}", "", length),
                                  hl + (rev ? 0 : length)));
        return length;
    }


    template<typename File, typename OStream, typename Error>
    void format_line_column_error(FileProxy<File> f, ErrorProxy<Error> err, WriterProxy<OStream> &os,
                                  const char *file_path = nullptr, const char *function_name = nullptr) {
        size_t line = err.visit_line();
        size_t column = err.visit_column();
        size_t length = err.visit_length();
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        size_t offset = err.visit_offset() + line - 1;
#else
        size_t offset = err.visit_offset();
#endif
        const char *hint = err.visit_hint();

        text_style::fmt_t fh;

        if (file_path != nullptr) {
            fh = fmt::format(text_style::fmt.style, "{}:{}:{}: ", file_path, line, column + 1);
        } else {
            fh = fmt::format(text_style::fmt.style, "{}:{}: ", line, column + 1);
        }

        if (function_name != nullptr) {
            os.write_data(fh);
            os.write_data(fmt::format(text_style::fmt.style, "in function {}", function_name));
            os << '\n';
        }

        os.write_data(fh);
        if (hint != nullptr) {
            os.write_data(fmt::format(text_style::fmt.style, "{}", hint));
        }
        os << '\n';

        int showed = print_char_ref_near(f, os, offset, length, column);
        if (length > showed) {
            os.write_data(fmt::format(text_style::fmt.style,
                                      "rightest offset of this ast node is {} (too many bytes to show, so truncate)\n",
                                      offset + length));
            print_char_ref_near(f, os, offset + length, 0, length - showed, true);
        }
    }
}


#endif //PASCAL_S_FEATURES_H
