#pragma once
/*
 * Fast discrete cosine transform algorithms (C++)
 *
 * Copyright (c) 2019 Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/fast-discrete-cosine-transform-algorithms
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising
 * from, out of or in connection with the Software or the use or other dealings
 * in the Software.
 */

#include <cassert>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

template <typename Float, typename Vec>
inline void forwardTransform_normalization(Vec &&vec, int len) {
    vec(0) *= Float(1.0 / len);
    Float scale = Float(2.0 / len);
    for (int i = 1; i < len; ++i) {
        vec(i) *= scale;
    }
}

template <typename Float, typename Vec, typename Temp>
inline void forwardTransform(Vec &&vec, Temp &&temp, int v, int t, int len) {
    if (len == 1)
        return;

    int halfLen = len / 2;
    for (int i = 0; i < halfLen; i++) {
        auto x = vec(i + v);
        auto y = vec(len - 1 - i + v);
        temp(i + t) = x + y;
        temp(i + t + halfLen) =
            (x - y) / (std::cos((i + float(0.5)) * float(M_PI) / len) * 2);
    }
    forwardTransform<Float>(std::forward<Temp>(temp), std::forward<Vec>(vec), t,
                            v, halfLen);
    forwardTransform<Float>(std::forward<Temp>(temp), std::forward<Vec>(vec),
                            t + halfLen, v, halfLen);
    for (int i = 0; i < halfLen - 1; i++) {
        vec(i * 2 + 0 + v) = temp(i + t);
        vec(i * 2 + 1 + v) = temp(i + halfLen + t) + temp(i + halfLen + 1 + t);
    }
    vec(len - 2 + v) = temp(halfLen - 1 + t);
    vec(len - 1 + v) = temp(len - 1 + t);
}

template <typename Float, typename Vec, typename Temp>
inline void inverseTransform(Vec &&vec, Temp &&temp, int v, int t, int len) {
    if (len == 1)
        return;
    int halfLen = len / 2;
    temp(0 + t) = vec(0 + v);
    temp(halfLen + t) = vec(1 + v);
    for (int i = 1; i < halfLen; i++) {
        temp(i + t) = vec(i * 2 + v);
        temp(i + halfLen + t) = vec(i * 2 - 1 + v) + vec(i * 2 + 1 + v);
    }
    inverseTransform<Float>(std::forward<Temp>(temp), std::forward<Vec>(vec), t,
                            v, halfLen);
    inverseTransform<Float>(std::forward<Temp>(temp), std::forward<Vec>(vec),
                            t + halfLen, v, halfLen);

    for (int i = 0; i < halfLen; i++) {
        auto x = temp(i + t);
        auto y = temp(i + halfLen + t) /
                 (std::cos((i + float(0.5)) * float(M_PI) / len) * 2);
        vec(i + v) = x + y;
        vec(len - 1 - i + v) = x - y;
    }
}

namespace Utils {
// compute the next highest power of 2 of 32-bit v
inline unsigned int bitceil(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline bool is_power_of_2(int i) {
    if (i <= 0) {
        return false;
    }
    return !(i & (i - 1));
}

} // namespace Utils

template <typename Float, typename ElemenT, typename InOut>
inline void DCT2D(InOut &&inout, int width, int height) {

    assert(Utils::is_power_of_2(width));
    assert(Utils::is_power_of_2(height));

    std::vector<ElemenT> temp(std::max(width, height));

    for (int y = 0; y < height; ++y) {
        forwardTransform<Float>([&](int x) -> ElemenT & { return inout(x, y); },
                                [&](int x) -> ElemenT & { return temp[x]; }, 0,
                                0, width);

        forwardTransform_normalization<Float>(
            [&](int x) -> ElemenT & { return inout(x, y); }, width);
    }

    for (int x = 0; x < width; ++x) {
        forwardTransform<Float>([&](int y) -> ElemenT & { return inout(x, y); },
                                [&](int y) -> ElemenT & { return temp[y]; }, 0,
                                0, height);

        forwardTransform_normalization<Float>(
            [&](int y) -> ElemenT & { return inout(x, y); }, height);
    }
}

template <typename Float, typename ElemenT, typename InOut>
inline void iDCT2D(InOut &&inout, int width, int height) {

    assert(Utils::is_power_of_2(width));
    assert(Utils::is_power_of_2(height));

    std::vector<ElemenT> temp(std::max(width, height));

    for (int x = 0; x < width; ++x) {
        inverseTransform<Float>([&](int y) -> ElemenT & { return inout(x, y); },
                                [&](int y) -> ElemenT & { return temp[y]; }, 0,
                                0, height);
    }

    for (int y = 0; y < height; ++y) {
        inverseTransform<Float>([&](int x) -> ElemenT & { return inout(x, y); },
                                [&](int x) -> ElemenT & { return temp[x]; }, 0,
                                0, width);
    }
}
