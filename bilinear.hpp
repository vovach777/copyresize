#pragma once
#include <algorithm>
#include <limits>
#include <cmath>

template<class FloatingType = float, class ElementT, typename InputFunctor, typename OutputFunctor>
inline void copyResizeBilinear(int input_width, int input_height, InputFunctor && image, int output_width,  int output_height, OutputFunctor && output)
{
    auto ratiox = FloatingType(input_width-1) / FloatingType(output_width-1);
    auto ratioy = FloatingType(input_height-1) / FloatingType(output_height-1);


    for (int i = 0; i < output_height; i++) {
        const FloatingType iF = i;
        const int y_l = std::floor(ratioy * iF);
        const int y_h = std::min<int>( std::ceil(ratioy * iF), input_height-1);
        const auto y_weight = (ratioy * iF) - y_l;
        const auto y_weight_inv = FloatingType(1.0) - y_weight;
        for (int j = 0; j < output_width; j++) {
            const FloatingType jF = j;
            const int x_l = std::floor(ratiox * jF);
            const int x_h = std::min<int>( std::ceil(ratiox * jF), input_width-1);

            const auto x_weight = (ratiox * jF) - x_l;
            const auto x_weight_inv = FloatingType(1.0) - x_weight;

            const auto a = image(x_l,y_l);
            const auto b = image(x_h,y_l);
            const auto c = image(x_l,y_h);
            const auto d = image(x_h,y_h);

            output(j,i, a * x_weight_inv * y_weight_inv +
                        b * x_weight * y_weight_inv +
                        c * y_weight * x_weight_inv +
                        d * x_weight * y_weight);
        }
    }
}