/*
 * Copyright (c) 2014 Advanced Micro Devices, Inc.
 *
 * Copyright (c) 2017 Michal Babej / Tampere University of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



_CL_OVERLOADABLE vtype exp2(vtype x)
{
    // Reduce x
    const vtype ln2HI = (vtype)0x1.62e300p-1f;
    const vtype ln2LO = (vtype)0x1.2fefa2p-17f;

    vtype t = rint(x);
    itype p = convert_itype(t);
    vtype tt = x - t;
    vtype hi = tt * ln2HI;
    vtype lo = tt * ln2LO;

    // Evaluate poly
    t = hi + lo;
    tt  = t*t;
    vtype v = mad(tt,
                  -mad(tt,
                      mad(tt,
                          mad(tt,
                             mad(tt, (vtype)0x1.637698p-25f,
                                  (vtype)-0x1.bbd41cp-20f),
                               (vtype)0x1.1566aap-14f),
                           (vtype)-0x1.6c16c2p-9f),
                       (vtype)0x1.555556p-3f),
                  t);

    vtype y = (vtype)1.0f - (((-lo) - MATH_DIVIDE(t * v,
                                            (vtype)2.0f - v)) - hi);

    // Scale by 2^p
    vtype r =  as_vtype(as_itype(y) + (p << 23));

    const vtype ulim = (vtype)128.0f;
    const vtype llim = (vtype)-126.0f;

    r = x < llim ? (vtype)0.0f : r;
    r = x < ulim ? r : as_vtype((utype)EXPBITS_SP32);
    return isnan(x) ? x : r;
}
