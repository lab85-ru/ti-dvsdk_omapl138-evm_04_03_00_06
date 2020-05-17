/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dtr.h ========
 *  Transformer Device Module Interface
 *
 */

#ifndef DTR_
#define DTR_

#include <stddef.h>     /* for size_t definition */
#include <dev.h>
#include <fxn.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _FLOAT_
typedef Float   DTR_Scale;
#else  /* _FLOAT_ */
typedef Int     DTR_Scale;
#endif /* _FLOAT_ */

#ifndef _FLOAT_
typedef Int16   DTR_ScaleInt16;
typedef DTR_ScaleInt16  DTR_ElemInt16;
extern Void DTR_multiplyInt16(Arg arg, DTR_ElemInt16 *addr, size_t size);
#define DTR_SCALE_INT16 (Int)DTR_multiplyInt16  /* float = float * arg */
#endif

typedef DTR_Scale       DTR_Elem;

#define DTR_init        ((Fxn)FXN_F_nop)

extern DEV_Fxns DTR_FXNS;

#ifdef _FLOAT_
extern Void DTR_fix2flt(Arg arg, DTR_Elem *addr, size_t size);
extern Void DTR_flt2fix(Arg arg, DTR_Elem *addr, size_t size);
#endif /* _FLOAT_ */
extern Void DTR_multiply(Arg arg, DTR_Elem *addr, size_t size);

#ifdef _FLOAT_
#define DTR_FLOAT       (Int)DTR_fix2flt        /* fix-to-float */
#define DTR_FIX         (Int)DTR_flt2fix        /* float-to-fix */
#endif /* _FLOAT_ */
#define DTR_SCALE       (Int)DTR_multiply       /* float = float * arg */
#define DTR_USER        (Int)NULL               /* user transformation */

/*
 *  ======== DTR_Params ========
 */
typedef struct {        /* device parameters */
    struct {
        DTR_Scale       value;
    } scale;
    struct {
        Arg             arg;
        Fxn             fxn;
    } user;
} DTR_Params;

extern DTR_Params DTR_PARAMS;           /* default parameter values */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DTR_ */
