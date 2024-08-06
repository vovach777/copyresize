#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>

#include <algorithm>
#include <limits>
#include <cfloat>
#include <cmath>

    template<class InputT1, class InputT2>
    constexpr inline auto cubicPolate(const InputT1 y0, const InputT1 y1, const InputT1 y2, const InputT1 y3, const InputT2 mu) 
    {
        const InputT2 mu2 = mu*mu;
        const InputT1 a0 = 3 * y1 - 3 * y2 + y3 - y0;
        const InputT1 a1 = 2 * y0 - 5 * y1 + 4 * y2 - y3;
        const InputT1 a2 = y2 - y0;
        const InputT1 a3 = 2 * y1;
        return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3) / 2;
    }    


template<class FloatingType = float, class ElementT, typename InputFunctor, typename OutputFunctor>
inline void copyResizeBicubic(int input_width, int input_height, InputFunctor && image, int output_width,  int output_height, OutputFunctor && output)
{
    auto ratiox = FloatingType(input_width-1) / FloatingType(output_width-1);
    auto ratioy = FloatingType(input_height-1) / FloatingType(output_height-1);


    for (int i = 0; i < output_height; i++) {
        const int y_l = std::floor(ratioy * i);
        const int y_h = std::min<int>( std::ceil(ratioy * i), input_height-1);
        const int y_ll = std::max<int>(0, std::floor(ratioy * (i-1)));
        const int y_hh = std::min<int>( std::ceil(ratioy * (i+1)), input_height-1);
        const auto y_weight = (ratioy * i) - y_l;
        const auto y_weight_inv = FloatingType(1.0) - y_weight;
        for (int j = 0; j < output_width; j++) {
            const int x_l = std::floor(ratiox * j);
            const int x_ll = j == 0 ? x_l : std::floor(ratiox * (j-1));
            const int x_h = std::min<int>( std::ceil(ratiox * j), input_width-1);
            const int x_hh = std::min<int>( std::ceil(ratiox * (j+1)), input_width-1);

            const auto x_weight = (ratiox * j) - x_l;
            const auto x_weight_inv = FloatingType(1.0) - x_weight;

            const auto polate_xll = cubicPolate(image(x_ll,y_ll),image(x_l,y_ll),image(x_h,y_ll),image(x_hh,y_ll),x_weight);
            const auto polate_xl = cubicPolate(image(x_ll,y_l),image(x_l,y_l),image(x_h,y_l),image(x_hh,y_l),x_weight);
            const auto polate_xh = cubicPolate(image(x_ll,y_h),image(x_l,y_h),image(x_h,y_h),image(x_hh,y_h),x_weight);
            const auto polate_xhh = cubicPolate(image(x_ll,y_hh),image(x_l,y_hh),image(x_h,y_hh),image(x_hh,y_hh),x_weight);

     

            output(j,i) = cubicPolate(polate_xll,polate_xl,polate_xh,polate_xhh,y_weight);
                        
        }
    }
}

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

            output(j,i) = a * x_weight_inv * y_weight_inv +
                        b * x_weight * y_weight_inv +
                        c * y_weight * x_weight_inv +
                        d * x_weight * y_weight;
        }
    }
}
int main() {

    float in[16][16]{};
    for (int x=0;x<16;++x)
    for (int y=0;y<16;++y)
    {
        in[y][x] = std::sqrt( (x-7.5)*(x-7.5)+(y-7.5)*(y-7.5) );
    }

 

    for (int y = 0; y < 16; ++y, std::cout << std::endl)
    for (int x = 0; x < 16; ++x)
    {
        std::cout << std::fixed << std::setprecision(1) << std::setw(5) << in[y][x];
    }
    std::cout << std::endl;

   float out[7][7]{};

    copyResizeBilinear<float,float>(16,16,[&in](int x, int y) {
        return in[y][x];
    }, 3,3,[&out](int x,int y) -> float& {
        return out[y][x];
    } );

    for (int y = 0; y < 3; ++y, std::cout << std::endl)
    for (int x = 0; x < 3; ++x)
    {
        std::cout << std::fixed << std::setprecision(1) << std::setw(5) << out[y][x];
    }
    std::cout << std::endl;

   copyResizeBicubic<float,float>(16,16,[&in](int x, int y) {
        return in[y][x];
    }, 3,3,[&out](int x,int y) -> float& {
        return out[y][x];
    } );

    for (int y = 0; y < 3; ++y, std::cout << std::endl)
    for (int x = 0; x < 3; ++x)
    {
        std::cout << std::fixed << std::setprecision(1) << std::setw(5) << out[y][x];
    }
    std::cout << std::endl;

}
