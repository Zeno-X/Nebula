// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <algorithm>
#include <assert.h>
#include <limits>

#define ZENDO_MAKE_UINT64(high32, low32)  ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#define ZENDO_MAKE_UINT32(high16, low16)  ((static_cast<uint32_t>(high16) << 16) | static_cast<uint32_t>(low16))
#define ZENDO_MAKE_UINT16(high8, low8)    ((static_cast<uint16_t>(high8)  << 8)  | static_cast<uint16_t>(low8))
#define ZENDO_HALF                        5.000000000000000000000000000000000000e-01 // 1/2
#define ZENDO_THIRD                       3.333333333333333333333333333333333333e-01 // 1/3
#define ZENDO_QUARTER                     2.500000000000000000000000000000000000e-01 // 1/4
#define ZENDO_FIFTH                       2.000000000000000000000000000000000000e-01 // 1/5
#define ZENDO_SIXTH                       1.666666666666666666666666666666666666e-01 // 1/6
#define ZENDO_TWO_THIRD                   6.666666666666666666666666666666666666e-01 // 2/3
#define ZENDO_THREE_QUARTERS              7.500000000000000000000000000000000000e-01 // 3/4
#define ZENDO_ROOT_TWO                    1.414213562373095048801688724209698078e+00 // √2
#define ZENDO_ROOT_THREE                  1.732050807568877293527446341505872366e+00 // √3
#define ZENDO_THIRD_ROOT_THREE            5.773502691896257645091487805019574553e-01 // √3/3
#define ZENDO_HALF_ROOT_THREE             8.660254037844386467637231707529361830e+00 // √3/2
#define ZENDO_HALF_ROOT_TWO               7.071067811865475244008443621048490392e-01 // √2/2
#define ZENDO_LN_TWO                      6.931471805599453094172321214581765680e-01 // ln2
#define ZENDO_LN_LN_TWO                   3.665129205816643270124391582326694694e-01 // ln(ln2)
#define ZENDO_ROOT_LN_FOUR                1.177410022515474691011569326459699637e+00 // √(ln4)
#define ZENDO_ONE_DIV_ROOT_TWO            7.071067811865475244008443621048490392e-01 // 1/√2
#define ZENDO_PI                          3.141592653589793238462643383279502884e+00 // π
#define ZENDO_TWO_PI                      6.283185307179586476925286766559005768e+00 // 2π
#define ZENDO_HALF_PI                     1.570796326794896619231321691639751442e+00 // π/2
#define ZENDO_THIRD_PI                    1.047197551196597746154214461093167628e+00 // π/3
#define ZENDO_QUARTER_PI                  7.853981633974483096156608458198757210e-01 // π/4
#define ZENDO_SIXTH_PI                    5.235987755982988730771072305465838140e-01 // π/6
#define ZENDO_TWO_DIV_PI                  6.366197723675813430755350534900574481e-01 // 2/π
#define ZENDO_TWO_THIRDS_PI               2.094395102393195492308428922186335256e+00 // (2/3)π
#define ZENDO_THREE_QUARTERS_PI           2.356194490192344928846982537459627163e+00 // (3/4)π
#define ZENDO_FOUR_THIRDS_PI              4.188790204786390984616857844372670512e+00 // (4/3)π
#define ZENDO_ONE_DIV_TWO_PI              1.591549430918953357688837633725143620e-01 // 1/(2π)
#define ZENDO_ONE_DIV_ROOT_TWO_PI         3.989422804014326779399460599343818684e-01 // 1/(√(2π))
#define ZENDO_ROOT_TWO_DIV_PI             7.978845608028653558798921198687637369e-01 // √(2/π)
#define ZENDO_ROOT_PI                     1.772453850905516027298167483341145182e+00 // √π
#define ZENDO_ROOT_HALF_PI                1.253314137315500251207882642405522626e+00 // √(π/2)
#define ZENDO_ROOT_TWO_PI                 2.506628274631000502415765284811045253e+00 // √(2π)
#define ZENDO_LOG_ROOT_TWO_PI             9.189385332046727417803297364056176398e-01 // log(√(2π))
#define ZENDO_ONE_DIV_ROOT_PI             5.641895835477562869480794515607725858e-01 // 1/(√π)
#define ZENDO_ROOT_ONE_DIV_PI             5.641895835477562869480794515607725858e-01 // √(1/π)
#define ZENDO_PI_MINUS_THREE              1.415926535897932384626433832795028841e-01 // π-3
#define ZENDO_FOUR_MINUS_PI               8.584073464102067615373566167204971158e-01 // 4-π
#define ZENDO_PI_POW_E                    2.245915771836104547342715220454373502e+01 // π^e
#define ZENDO_PI_SQUARE                   9.869604401089358618834490999876151135e+00 // π^2
#define ZENDO_PI_SQUARE_DIV_SIX           1.644934066848226436472415166646025189e+00 // (π^2)/6
#define ZENDO_PI_CUBED                    3.100627668029982017547631506710139520e+01 // π^3
#define ZENDO_CBRT_PI                     1.464591887561523263020142527263790391e+00 // ³√π
#define ZENDO_ONE_DIV_CBRT_PI             6.827840632552956814670208331581645981e-01 // 1/(³√π)
#define ZENDO_E                           2.718281828459045235360287471352662497e+00 // e
#define ZENDO_E_POW_PI                    2.314069263277926900572908636794854738e+01 // e^π
#define ZENDO_ROOT_E                      1.648721270700128146848650787814163571e+00 // √e
#define ZENDO_LOG10_E                     4.342944819032518276511289189166050822e-01 // log10(e)
#define ZENDO_ONE_DIV_LOG10_E             2.302585092994045684017991454684364207e+00 // 1/(log10(e))
#define ZENDO_LN_TEN                      2.302585092994045684017991454684364207e+00 // ln10
#define ZENDO_DEGREE                      1.745329251994329576923690768488612713e-02 // π/180  => 角度转弧度
#define ZENDO_RADIAN                      5.729577951308232087679815481410517033e+01 // 180/π  => 弧度转角度
#define ZENDO_SIN_ONE                     8.414709848078965066525023216302989996e-01 // sin(1)
#define ZENDO_SIN_ZERO                    0.000000000000000000000000000000000000e+00 // sin(0)
#define ZENDO_SIN_SIXTH_PI                5.000000000000000000000000000000000000e-01 // sin(π/6)
#define ZENDO_SIN_QUARTER_PI              7.071067811865475244008443621048490392e-01 // sin(π/4)
#define ZENDO_SIN_THIRD_PI                8.660254037844386467637231707529361830e+00 // sin(π/3)
#define ZENDO_SIN_HALF_PI                 1.000000000000000000000000000000000000e+00 // sin(π/2)
#define ZENDO_COS_ONE                     5.403023058681397174009366074429766037e-01 // cos(1)
#define ZENDO_COS_ZERO                    1.000000000000000000000000000000000000e+00 // cos(0)
#define ZENDO_COS_SIXTH_PI                8.660254037844386467637231707529361830e+00 // cos(π/6)
#define ZENDO_COS_QUARTER_PI              7.071067811865475244008443621048490392e-01 // cos(π/4)
#define ZENDO_COS_THIRD_PI                5.000000000000000000000000000000000000e-01 // cos(π/3)
#define ZENDO_COS_HALF_PI                 0.000000000000000000000000000000000000e+00 // cos(π/2)
#define ZENDO_TAN_ZERO                    0.000000000000000000000000000000000000e+00 // tan(0)
#define ZEND_TAN_SIXTH_PI                 5.773502691896257645091487805019574553e-01 // tan(π/6)
#define ZEND_TAN_QUARTER_PI               1.000000000000000000000000000000000000e+00 // tan(π/4)
#define ZEND_TAN_THIRD_PI                 1.732050807568877293527446341505872366e+00 // tan(π/3)
#define ZENDO_SINH_ONE                    1.175201193643801456882381850595600815e+00 // sinh(1)
#define ZENDO_COSH_ONE                    1.543080634815243778477905620757061682e+00 // cosh(1)
#define ZENDO_PHI                         1.618033988749894848204586834365638117e+00 // φ 黄金分割比例常数 1/0.61803... = 1.61803...
#define ZENDO_LN_PHI                      4.812118250596034474977589134243684231e-01 // lnφ
#define ZENDO_ONE_DIV_LN_PHI              2.078086921235027537601322606117795767e+00 // 1/(lnφ)
#define ZENDO_EULER                       5.772156649015328606065120900824024310e-01 // γ 欧拉常数
#define ZENDO_ONE_DIV_EULER               1.732454714600633473583025315860829681e+00 // 1/γ
#define ZENDO_EULER_SQUARE                3.331779238077186743183761363552442266e-01 // γ^2


namespace zendo
{
    
    inline bool is_zero(float val, float epsilon = 1e-6f)
    {
        return val * val < epsilon;
    }
    
    inline bool is_zero(long double val, float epsilon = 1e-12)
    {
        return val * val < epsilon;
    }
    
    inline bool is_zero(int val)
    {
        return val == 0;
    }
    
    // 1/√n
    inline static long double inv_sqrt(long double value)
    {
        const long double x2 = value * 0.5L;
        long double ret = value;
        uint64_t i = *(uint64_t*)&ret;
        i = 0x5fe6eb50c7aa19f9L - (i >> 1); // 32位则为0x5f3759df
        ret = *(long double*)&i;
        // 迭代次数越多，精度越高(牛顿迭代法)
        ret = ret * (1.5L - (x2 * ret * ret)); // 1st 迭代
        ret = ret * (1.5L - (x2 * ret * ret)); // 2nd 迭代
        ret = ret * (1.5L - (x2 * ret * ret)); // 3nd 迭代
        //ret = ret * (1.5L - (x2 * ret * ret)); // 4nd 迭代
        //ret = ret * (1.5L - (x2 * ret * ret)); // 5nd 迭代
        //ret = ret * (1.5L - (x2 * ret * ret)); // 6nd 迭代
        return ret;
    }
    
    inline static float reciprocal(float value)
    {
#if defined(_MSC_VER)
        float rec;
        __asm rcpss xmm0, f
        __asm movss xmm1, f
        __asm mulss xmm1, xmm0
        __asm mulss xmm1, xmm0
        __asm addss xmm0, xmm0
        __asm subss xmm0, xmm1
        __asm movss rec, xmm0
        return rec;
#else
        return 1.f / value;
#endif
    }
    
    inline static long double reciprocal(long double value)
    {
        return 1.0 / value;
    }
    
    template<typename T>
    inline static T max(T a, T b, T c)
    {
        return a > b ? (a > c ? a : c) : (b > c ? b : c);
    }
    
    template<typename T>
    inline static T min(T a, T b, T c)
    {
        return a < b ? (a < c ? a : c) : (b < c ? b : c);
    }
    
    // 随机-1或1
    inline int random_signed(int random_value)
    {
        return (random_value & 2) - 1;
    }
    
}
