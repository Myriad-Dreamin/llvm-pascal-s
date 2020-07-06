//
// Created by kamiyoru on 2020/6/29.
//

#ifndef PASCAL_S_GUESS_H
#define PASCAL_S_GUESS_H

#include <algorithm>
#include <cstdint>

namespace pascal_s {

    class MinStrDist {

        static constexpr int v1 = 22, v1l = v1 - 1;
        uint8_t diff[v1][v1];
    public:
        int min_str_dist(const char *u, int u_len, const char *v, int v_len) {
            if (abs(u_len - v_len) > std::min(2, std::min((u_len + 1) / 2, (v_len + 1) / 2))) {
                return -1;
            }

            if (std::max(u_len, v_len) >= v1l) {
                return -1;
            }

            for (int i = 0; i < u_len; i++) diff[i][0] = i;
            for (int i = 0; i < v_len; i++) diff[0][i] = i;
            for (int i = 1; i <= u_len; i++) {
                for (int j = 1; j <= v_len; j++) {
                    diff[i][j] = std::min(diff[i][j - 1] + 1,
                                          std::min(diff[i - 1][j] + 1, diff[i - 1][j - 1] + (u[i - 1] != v[j - 1])));
                }
            }

            return diff[u_len][v_len];
        }

        bool near(const char *u, int u_len, const char *v, int v_len) {
            int dist = min_str_dist(u, u_len, v, v_len);
            // dist <= 2 -> 允许两次增删替换，或一次交换
            return 0 <= dist && dist <= std::min(2, std::min((u_len + 1) / 2, (v_len + 1) / 2));
        }
    };
}


#endif //PASCAL_S_GUESS_H
