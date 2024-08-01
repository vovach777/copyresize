#pragma once
#include "dct.hpp"
#include "utils.hpp"

template <typename Float, typename ElementT, typename InputFunctor, typename OutputFunctor>
inline void copyResizeDCT(int src_width, int src_height, InputFunctor && image, int width,  int height, OutputFunctor && output)
{
    //copy src for implicit DCT
    std::vector<std::vector<ElementT>> img(bitceil(std::max(height,src_height)), std::vector<ElementT>(bitceil(std::max(width,src_width)),ElementT{}));
    for (int y = 0; y < src_height; ++y)
    for (int x = 0; x < src_width; ++x)
        img[y][x] = image(x,y);

    DCT2D<Float,ElementT>([&](int x, int y) -> ElementT&{
        return img[y][x];
    }, bitceil(src_width),bitceil(src_height));

    iDCT2D<Float,ElementT>([&](int x, int y) -> ElementT&{
        return img[y][x];
    },bitceil(width),bitceil(height));


    //copy result to dest
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
        output(x,y,img[y][x]);

}