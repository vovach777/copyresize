#pragma once
#include <algorithm>
#include <cmath>


template <int a = 3, typename FloatType> inline FloatType lanczos(FloatType x) {
    x = std::abs(x) * FloatType(M_PI);
    if (x > a * FloatType(M_PI))
        return 0.;
    return x < FloatType(.0001) ? FloatType(1.)
                                : a * std::sin(x) * std::sin(x / a) / (x * x);
}

template <int a = 3, typename FloatType>
inline FloatType lanczos(FloatType x, FloatType y) {
    return lanczos<a, FloatType>(x) * lanczos<a, FloatType>(y);
}

template <int N = 5, typename FloatType, typename ElementType,
          typename sampler2D>
inline ElementType lanczossample(sampler2D &&samp, FloatType fx, FloatType fy) {
    struct {
        int x;
        int y;
    } F{int(fx), int(fy)};
    struct {
        FloatType x;
        FloatType y;
    } f{fx - F.x, fy - F.y};
    ElementType v{};
    for (int y = -N + 1; y <= N; y++)
        for (int x = -N + 1; x <= N; x++) {
            v += samp(F.x + x, F.y + y) *
                 lanczos<3, FloatType>(f.x - x, f.y - y);
        }
    return v;
}

template <class FloatingType = float, int Radius = 5, class ElementT,
          typename InputFunctor, typename OutputFunctor>
inline void copyResizeLanczos(int src_width, int src_height,
                              InputFunctor &&image, int width, int height,
                              OutputFunctor &&output) {

    FloatingType x_multipler = (src_width - 1) / FloatingType(width - 1);
    FloatingType y_multipler = (src_height - 1) / FloatingType(height - 1);

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            output(x, y,
                   lanczossample<Radius, FloatingType, ElementT>(
                       [&](int x, int y) {
                           return image(std::clamp<int>(x, 0, src_width - 1),
                                        std::clamp<int>(y, 0, src_height - 1));
                       },
                       x * x_multipler, y * y_multipler));
        }
}
