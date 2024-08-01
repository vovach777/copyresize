#pragma once
#include <algorithm>
#include <limits>


    template<class InputT1, class InputT2>
    constexpr inline auto cubicPolate(const InputT1 v0, const InputT1 v1, const InputT1 v2, const InputT1 v3, const InputT2 frac)
    {
        auto A = (v3-v2)-(v0-v1);
        auto B = (v0-v1)-A;
        auto C = v2-v0;
        auto D = v1;
        return D + frac * (C + frac * (B + frac * A));
    }

    template<class InputT = float, class ElementT>
    constexpr inline auto bicubicPolate(const ElementT* const ndata, const InputT fracx, const InputT fracy)
    {
        auto x1 = cubicPolate( ndata[0], ndata[1], ndata[2], ndata[3], fracx );
        auto x2 = cubicPolate( ndata[4], ndata[5], ndata[6], ndata[7], fracx );
        auto x3 = cubicPolate( ndata[8], ndata[9], ndata[10], ndata[11], fracx );
        auto x4 = cubicPolate( ndata[12], ndata[13], ndata[14], ndata[15], fracx );

        return cubicPolate( x1, x2, x3, x4, fracy );
    }

    //  copyResizeBicubic template function implementation
    template<class FloatingType = float, class ElementT, typename InputFunctor, typename OutputFunctor>
    inline void copyResizeBicubic(int src_width, int src_height, InputFunctor && image, int width,  int height, OutputFunctor && output)
    {
        auto ratiox = FloatingType(src_width-1) / FloatingType(width-1);
        auto ratioy = FloatingType(src_height-1) / FloatingType(height-1);

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                FloatingType xMappingToOrigin = x * ratiox;
                FloatingType yMappingToOrigin = y * ratioy;
                int xMappingToOriginFloor = xMappingToOrigin;
                int yMappingToOriginFloor = yMappingToOrigin;
                FloatingType xMappingToOriginFrac = xMappingToOrigin - xMappingToOriginFloor;
                FloatingType yMappingToOriginFrac = yMappingToOrigin - yMappingToOriginFloor;

                ElementT ndata[4 * 4];
                for (int ndatay = -1; ndatay <= 2; ++ndatay)
                {
                    for (int ndatax = -1; ndatax <= 2; ++ndatax)
                    {
                        const auto src_x = std::clamp(xMappingToOriginFloor + ndatax, 0, src_width-1);
                        const auto src_y = std::clamp(yMappingToOriginFloor + ndatay, 0, src_height-1);
                        ndata[(ndatay + 1) * 4 + (ndatax + 1)] = image(src_x, src_y);
                    }

                }
                output(x,y, bicubicPolate(ndata, xMappingToOriginFrac, yMappingToOriginFrac) );
            }
        }
    }