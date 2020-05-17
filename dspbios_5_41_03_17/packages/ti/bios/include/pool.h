/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== POOL.h ========
 *
 */
#ifndef POOL_
#define POOL_

#include <stddef.h>
#include <fxn.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Reserved for denoting an invalid pool Id */
#define POOL_INVALIDID   (Uint16)-1

/* Typedefs for the allocator interface and init functions */
typedef Void  (*POOL_Init)(Void);
typedef Int   (*POOL_Open)(Ptr *object, Ptr params);
typedef Void  (*POOL_Close)(Ptr object);
typedef Int   (*POOL_Alloc)(Ptr object, Ptr *buf, size_t size);
typedef Void  (*POOL_Free)(Ptr object, Ptr buf, size_t size);

/* Not implemented defines for POOL functions */
#define POOL_OPENNOTIMPL    (POOL_Open)SYS_zero
#define POOL_CLOSENOTIMPL   (POOL_Close)FXN_F_nop
#define POOL_ALLOCNOTIMPL   (POOL_Alloc)SYS_one
#define POOL_FREENOTIMPL    (POOL_Free)FXN_F_nop

/* The allocator interface function definition. */
typedef struct POOL_Fxns {
    POOL_Open     open;
    POOL_Close    close;
    POOL_Alloc    alloc;
    POOL_Free     free;
} POOL_Fxns;

/* The allocator object */
typedef struct POOL_Obj {
    POOL_Init     initFxn;  /* Allocator init function             */
    POOL_Fxns    *fxns;     /* Allocator interface functions       */
    Ptr           params;   /* Allocator-specific setup parameters */
    Ptr           object;   /* Allocator-specific object           */
} POOL_Obj, *POOL_Handle;

/* Used in the nop allocator */
extern POOL_Fxns POOL_FXNS;

/* No allocator object definition */
#define POOL_NOENTRY {FXN_F_nop, &POOL_FXNS, NULL, NULL}

/* The Allocator Configuration */
typedef struct POOL_Config {
    POOL_Obj   *allocators;     /* Array of allocators            */
    Uint16      numAllocators;  /* Number of allocators in the array */    
} POOL_Config;

extern Void POOL_init(Void);
extern Void POOL_exit(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* POOL_ */
