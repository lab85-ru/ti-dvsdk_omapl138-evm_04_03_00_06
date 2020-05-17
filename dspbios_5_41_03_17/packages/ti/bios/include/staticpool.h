/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== staticpool.h ========
 *
 */

#ifndef STATICPOOL_
#define STATICPOOL_

#include <stddef.h>
#include <pool.h>
#include <fxn.h>
#include <que.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Both the init and exit functions currently do nothing */
#define STATICPOOL_init    FXN_F_nop
#define STATICPOOL_exit    FXN_F_nop

/*
 *  Allocator's parameters.
 *     addr:       Start of the block of memory that contains the buffers.
 *     length:     Length of the block of memory that contains the buffers.
 *     bufferSize: Size of the individual buffers.
 */
typedef struct STATICPOOL_Params {
    Ptr         addr;
    size_t      length;
    size_t      bufferSize;
} STATICPOOL_Params;

typedef struct STATICPOOL_Obj {
    QUE_Obj   queue;        /* Queue to hold the messages */
    size_t    bufferSize;   /* Size of each buffer.      */
} STATICPOOL_Obj, *STATICPOOL_Handle;

/* This allocator's interface functions */
extern const POOL_Fxns STATICPOOL_FXNS;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* STATICPOOL_ */
