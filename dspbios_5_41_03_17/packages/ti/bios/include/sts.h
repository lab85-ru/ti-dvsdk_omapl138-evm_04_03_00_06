/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== sts.h ========
 *
 */

#ifndef STS_
#define STS_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_30_) || defined(_40_) || defined(_6x_) || defined (_55_)|| defined (_28_)

typedef struct STS_Obj {
    LgInt       num;
    LgInt       acc;
    LgInt       max;
} STS_Obj;

#else

/*
 * The following are not defined as LgInt since the
 * C5x and C54x differ in the word ordering for LgInt's.
 * Defining these as LgInt would require a change to
 * vbd.dll to deal with the C5x and C54x differently.
 */
typedef struct STS_Obj {
    Int   numh;
    Int   numl;
    Int   acch;
    Int   accl;
    Int   maxh;
    Int   maxl;
} STS_Obj;

#endif

typedef struct STS_Obj *STS_Handle;

/*
 *  ======== STS_add ========
 */
extern Void STS_add(STS_Handle sts, LgInt value);

/*
 *  ======== STS_delta ========
 */
extern Void STS_delta(STS_Handle sts, LgInt value);

/*
 *  ======== STS_set ========
 */
extern Void STS_set(STS_Handle sts, LgInt value);

/*
 *  ======== STS_reset ========
 */
extern Void STS_reset(STS_Handle sts);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* STS_ */
