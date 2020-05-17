/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn.h ========
 *  Generator Device Module Interface
 *
 */

#ifndef DGN_
#define DGN_

#include <stddef.h>
#include <dev.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_Fxns DGN_FXNS;

#ifdef _FLOAT_

/*
 *  ======== DGN_Params ========
 */
typedef struct {                /* device parameters */
    struct {
        Float value;            /* constant value */
    } constant;
    struct {
        LgUns    seed;          /* seed value */
        Float   lower;          /* lower limit of generator */
        Float   upper;          /* upper limit of generator */
    } rand;
    struct {
        Float gain;             /* gain */
        Float freq;             /* frequency (cycles / sec) */
        Float phase;            /* phase (in radians) */
        LgUns  rate;            /* sample rate (samples / sec) */
    } sine;
    struct {
        Fxn     fxn;            /* user function */
        Arg     arg;            /* and argument */
    } user;
} DGN_Params;

extern DGN_Params DGN_PARAMS;           /* default parameter values */

typedef struct DGN_GenObj {     
    Fxn         fxn;
    Arg         arg;
    Int         seed;           /* current seed value */
    Int         count;          /* number of sine values generated */
    DGN_Params  *dparams;       /* parameters */
} DGN_GenObj;

extern Void DGN_fconst(DGN_GenObj *gen, Float *addr, size_t size);
extern Void DGN_frand(DGN_GenObj *gen, Float *addr, size_t size);
extern Void DGN_fsine(DGN_GenObj *gen, Float *addr, size_t size);
extern Void DGN_user(DGN_GenObj *gen, Ptr addr, size_t size);

/*
 * Function address size is LgUns for 54f,55 and 28
 */
#if (defined(_54_) && defined(_FAR_MODE)) || defined(_55_) || defined(_28_)
#define DGN_CONST       (LgUns)DGN_fconst       /* constant series device id */
#define DGN_RAND        (LgUns)DGN_frand        /* random series device id */
#define DGN_SINE        (LgUns)DGN_fsine        /* sine wave device id */
#define DGN_USER        (LgUns)DGN_user         /* user function */
#else
#define DGN_CONST       (Int)DGN_fconst         /* constant series device id */
#define DGN_RAND        (Int)DGN_frand          /* random series device id */
#define DGN_SINE        (Int)DGN_fsine          /* sine wave device id */
#define DGN_USER        (Int)DGN_user           /* user function */
#endif

#else  /* _FLOAT_ */

/*
 *  ======== DGN_IParams ========
 */
typedef struct {                /* device parameters */
    struct {
        Int     value;          /* constant value */
    } constant;
    struct {
        LgUns   seed;           /* seed value */
        Int     lower;          /* lower limit of generator */
        Int     upper;          /* upper limit of generator */
    } rand;
    struct {
        Int     gain;           /* gain */
        Int     freq;           /* frequency (cycles / sec) */
        Int     phase;          /* phase (in radians) */
        Int     rate;           /* sample rate (samples / sec) */
    } sine;
    struct {
        Fxn     fxn;            /* user function */
        Arg     arg;            /* and argument */
    } user;
} DGN_IParams;

extern DGN_IParams DGN_PARAMS;          /* default parameter values */

typedef struct DGN_GenObj {     
    Fxn         fxn;
    Arg         arg;
    LgUns       seed;           /* current seed value */
    Int         index;          /* current index into sine value table */
    Int         step;           /* the step for indexing the sine table */
    Int         shift;          /* sine shift value to achieve gain */
    DGN_IParams *dparams;       /* parameters */
} DGN_GenObj;

extern Void DGN_iconst(DGN_GenObj *gen, Int16 *addr, size_t size);
extern Void DGN_irand(DGN_GenObj *gen, Int16 *addr,  size_t size);
extern Void DGN_isine(DGN_GenObj *gen, Int16 *addr, size_t size);
extern Void DGN_user(DGN_GenObj *gen, Ptr addr, size_t size);

/*
 * Function address size is LgUns for 54f,55 and 28
 */
#if (defined(_54_) && defined(_FAR_MODE)) || defined(_55_) || defined(_28_)
#define DGN_CONST       (LgUns)DGN_iconst       /* constant series device id */
#define DGN_RAND        (LgUns)DGN_irand        /* random series device id */
#define DGN_SINE        (LgUns)DGN_isine        /* sine wave device id */
#define DGN_USER        (LgUns)DGN_user         /* user function */
#else
#define DGN_CONST       (Int)DGN_iconst         /* constant series device id */
#define DGN_RAND        (Int)DGN_irand          /* random series device id */
#define DGN_SINE        (Int)DGN_isine          /* sine wave device id */
#define DGN_USER        (Int)DGN_user           /* user function */
#endif

#endif /* _FLOAT_ */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DGN_ */
