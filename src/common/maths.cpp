#include "maths.h"

// http://lolengine.net/blog/2011/12/21/better-function-approximations
// Chebyshev http://stackoverflow.com/questions/345085/how-do-trigonometric-functions-work/345117#345117
// Thanks for ledvinap for making such accuracy possible! See: https://github.com/cleanflight/cleanflight/issues/940#issuecomment-110323384
// https://github.com/Crashpilot1000/HarakiriWebstore1/blob/master/src/mw.c#L1235
#if (BUILD_USE_FAST_MATH == 1) || (BUILD_USE_VERY_FAST_MATH == 1)
#if (VERY_FAST_MATH == 1)
#define sinPolyCoef3 -1.666568107e-1f
#define sinPolyCoef5  8.312366210e-3f
#define sinPolyCoef7 -1.849218155e-4f
#define sinPolyCoef9  0
#else
#define sinPolyCoef3 -1.666665710e-1f                                          // Double: -1.666665709650470145824129400050267289858e-1
#define sinPolyCoef5  8.333017292e-3f                                          // Double:  8.333017291562218127986291618761571373087e-3
#define sinPolyCoef7 -1.980661520e-4f                                          // Double: -1.980661520135080504411629636078917643846e-4
#define sinPolyCoef9  2.600054768e-6f                                          // Double:  2.600054767890361277123254766503271638682e-6
#endif

namespace math
{

float sin_approx(float x)
{
    int32_t xint = static_cast<int32_t>(x);
    if ((xint < -32) || (xint > 32)) return 0.0f;                              // Stop here on error input (5 * 360 Deg)
    while (x >  M_PIf) x -= (2.0f * M_PIf);                                // always wrap input angle to -PI..PI
    while (x < -M_PIf) x += (2.0f * M_PIf);
    if (x >  (0.5f * M_PIf)) x =  (0.5f * M_PIf) - (x - (0.5f * M_PIf));   // We just pick -90..+90 Degree
    else if (x < -(0.5f * M_PIf)) x = -(0.5f * M_PIf) - ((0.5f * M_PIf) + x);
    float x2 = x * x;
    return x + x * x2 * (sinPolyCoef3 + x2 * (sinPolyCoef5 + x2 * (sinPolyCoef7 + x2 * sinPolyCoef9)));
}

float cos_approx(float x)
{
    return sin_approx(x + (0.5f * M_PIf));
}

// https://github.com/Crashpilot1000/HarakiriWebstore1/blob/396715f73c6fcf859e0db0f34e12fe44bace6483/src/mw.c#L1292
// http://http.developer.nvidia.com/Cg/atan2.html (not working correctly!)
// Poly coefficients by @ledvinap (https://github.com/cleanflight/cleanflight/pull/1107)
// Max absolute error 0,000027 degree
float atan2_approx(float y, float x)
{
    #define atanPolyCoef1  3.14551665884836e-07f
    #define atanPolyCoef2  0.99997356613987f
    #define atanPolyCoef3  0.14744007058297684f
    #define atanPolyCoef4  0.3099814292351353f
    #define atanPolyCoef5  0.05030176425872175f
    #define atanPolyCoef6  0.1471039133652469f
    #define atanPolyCoef7  0.6444640676891548f

    float res, absX, absY;
    absX = ABS(x);
    absY = ABS(y);
    res  = MAX(absX, absY);
    if (res) res = MIN(absX, absY) / res;
    else res = 0.0f;
    res = -((((atanPolyCoef5 * res - atanPolyCoef4) * res - atanPolyCoef3) * res - atanPolyCoef2) * res - atanPolyCoef1) / ((atanPolyCoef7 * res + atanPolyCoef6) * res + 1.0f);
    if (absY > absX) res = (M_PIf / 2.0f) - res;
    if (x < 0) res = M_PIf - res;
    if (y < 0) res = -res;
    return res;
}

// http://http.developer.nvidia.com/Cg/acos.html
// Handbook of Mathematical Functions
// M. Abramowitz and I.A. Stegun, Ed.
// Absolute error <= 6.7e-5
float acos_approx(float x)
{
    float xa = ABS(x);
    float result = fastSqrtf(1.0f - xa) * (1.5707288f + xa * (-0.2121144f + xa * (0.0742610f + (-0.0187293f * xa))));
    if (x < 0.0f)
        return M_PIf - result;
    else
        return result;
}
#endif

int32_t gcd(int32_t num, int32_t denom)
{
    if (denom == 0)
    {
        return num;
    }

    return gcd(denom, num % denom);
}

// todo: https://habr.com/ru/post/267239/  make that code more right
float fastInvSqrtf(float value)
{
    union {
		float    f;
		unsigned int i;
	} conv = { .f = value };

    conv.i  = 0x5f3759df - (conv.i >> 1);
	conv.f *= 1.5F - (value * 0.5F * conv.f * conv.f);

    if (isnan(conv.f))
        return 0;

    return conv.f;
}
// todo: make arm math used to
float fastSqrtf(float number)
{

    union {
		float    f;
		unsigned int i;
	} conv = { .f = number };

    conv.i  = 0x1fbd1df5 + (conv.i >> 1);
	//conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);

    if (isnan(conv.f))
        return 0;

    return conv.f;
}

}
