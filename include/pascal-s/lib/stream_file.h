//
// Created by kamiyoru on 2020/6/19.
//

#ifndef PASCAL_S_STREAM_FILE_H
#define PASCAL_S_STREAM_FILE_H

#include <istream>

namespace pascal_s {

    class CPPStreamFile {
        std::istream &f;

    public:
        explicit CPPStreamFile(std::istream &f) : f(f) {}

        pascal_errno seek(size_t offset) {
            f.seekg(offset, std::ios::beg);
            assert(!f.fail());
            return 0;
        }

        pascal_errno read(char *buf, size_t len) {
            f.get(buf, len, '\x00');
            assert(!f.fail());
            return 0;
        }
    };
}

#endif //PASCAL_S_STREAM_FILE_H
