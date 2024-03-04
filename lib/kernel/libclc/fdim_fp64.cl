




/*

_CL_OVERLOADABLE vtype fdim(vtype x, vtype y) {
    long n = -(isnan(x) | isnan(y)) & QNANBITPATT_DP64;
    long r = -(x > y) & as_long(x - y);
    return as_double(n | r);
}

*/

#ifdef SINGLEVEC

_CL_OVERLOADABLE vtype fdim(vtype x, vtype y) {
    vtype retval = fmax(x - y, (vtype)0.0);
    return (isnan(x) | isnan(y)) ? as_vtype((utype)QNANBITPATT_DP64) : retval;
}

#else


_CL_OVERLOADABLE vtype fdim(vtype x, vtype y) {

    /* Determine if x or y is NaN. */
    /* Vector true is -1, i.e. all-bits-set, and NaN==NaN is false. */
    /* If either is NaN, then ~((x==x) & (y==y)) will be 0 (e.g. ~(-1)), as will n. */

    itype n = ~((x == x) & (y == y)) & (itype)QNANBITPATT_DP64;

    /* Calculate x-y if x>y, otherwise positive 0, again taking */
    /* advantage of vector true being all-bits-set. */

    itype r = (x > y) & as_itype(x - y);
    return as_vtype(n | r);
}

#endif
