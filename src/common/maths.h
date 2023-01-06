#pragma once

#include <math.h>
#include <float.h>

#include "src/platform.h"
#include "src/build_config.h"

namespace math
{
// Use floating point M_PI instead explicitly.
#ifndef M_PIf
#define M_PIf       3.14159265358979323846f
#endif

#ifndef M_EULERf
#define M_EULERf    2.71828182845904523536f
#endif

#ifndef M_LN2f
#define M_LN2f      0.69314718055994530942f
#endif

#ifndef M_Ef
#define M_Ef        2.71828182845904523536f
#endif

#define CM_S_TO_KM_H(centimetersPerSecond) ((centimetersPerSecond)*36 / 1000)
#define CM_S_TO_MPH(centimetersPerSecond) ((centimetersPerSecond)*10000 / 5080 / 88)

// Limits teplates
template<class _type>
inline _type MIN( _type x )
{
	return x;
}

template<class _type, typename... T>
inline _type MIN( _type x, T... args )
{
    _type y = MIN( args... );
	return x < y ? x : y;
}

template<class _type>
inline _type MAX( _type x )
{
	return x;
}

template<class _type, typename... T>
inline _type MAX(_type x, T... args)
{
    _type y =  MAX( args... );
	return x > y ? x : y;
}

// commmon templates
template<class T>
constexpr void SWAP( T& a, T& b )
{
	T tmp = a;
	a = b;
	b = tmp;
}

template<class T>
constexpr T ABS( const T& x )
{
	return ( x < 0 ) ? -( x ) : ( x );
}

template<class T>
inline char SIGN( const T& x )
{
	return ( ( x ) < 0 ) ? -1 : 1;
}

template<typename _type>
_type sq(_type val)
{
	return val * val;
}

template<typename _type>
constexpr _type constrain(_type val, _type min, _type max)
{
	return (val < min) ? min : ((val > max) ? max : val);
}

template<typename _type>
constexpr _type scaleRange(_type x, _type srcMin, _type srcMax, _type destMin, _type destMax)
{
	_type a = ((_type) destMax - (_type) destMin) * ((_type) x - (_type) srcMin);
    _type b = (_type) srcMax - (_type) srcMin;
    return ((a / b) + destMin);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrainFloatToInt16(float value)
{
    return static_cast<int16_t>(constrain(value, static_cast<float>(INT16_MIN), static_cast<float>(INT16_MAX)));
}

template<typename _type>
constexpr bool isInRange(_type val, _type min, _type max)
{
	return (min <= val) && (val <= max);
}

template<typename T>
constexpr T radians(T degrees)
{
    return degrees * (static_cast<T>(M_PIf) / static_cast<T>(180));
}

template<typename T>
constexpr T degrees(T radians)
{
    return radians * (static_cast<T>(180) / static_cast<T>(M_PIf));
}

/** Safe way to check if float is zero */
inline bool isZero(float val)
{
	return fabsf(val - 0.0f) < FLT_EPSILON;
}

/** Safe way to check if double is zero */
inline bool isZero(double val)
{
	return fabs(val - 0.0) < DBL_EPSILON;
}

#if defined(BUILD_USE_FAST_MATH) || defined(BUILD_USE_VERY_FAST_MATH)
float sin_approx(float x);
float cos_approx(float x);
float atan2_approx(float y, float x);
float acos_approx(float x);
#define tan_approx(x)       (sin_approx(x) / cos_approx(x))
#define asin_approx(x)      (M_PIf / 2 - acos_approx(x))
#else
#define asin_approx(x)      asinf(x)
#define sin_approx(x)       sinf(x)
#define cos_approx(x)       cosf(x)
#define atan2_approx(y,x)   atan2f(y,x)
#define acos_approx(x)      acosf(x)
#define tan_approx(x)       tanf(x)
#endif

int32_t gcd(int32_t num, int32_t denom);
float fastInvSqrtf(float value);
float fastSqrtf(float number);

}
