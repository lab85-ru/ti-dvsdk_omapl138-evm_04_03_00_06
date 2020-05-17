/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== bcache.h ========
 *
 */

#ifndef BCACHE_
#define BCACHE_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Enumerated list of L1 cache sizes
 */
typedef enum {
    BCACHE_L1_0K = 0,
    BCACHE_L1_4K = 1,
    BCACHE_L1_8K = 2,
    BCACHE_L1_16K = 3,
    BCACHE_L1_32K = 4
} BCACHE_L1_Size;

/*
 *  Enumerated list of L2 cache sizes
 */
typedef enum {
    BCACHE_L2_0K = 0,
    BCACHE_L2_32K = 1,
    BCACHE_L2_64K = 2,
    BCACHE_L2_128K = 3,
    BCACHE_L2_256K = 4,
    BCACHE_L2_512K = 5,
    BCACHE_L2_1024K = 6
} BCACHE_L2_Size;

/*
 *  Enumerated list of cache modes for L1 and L2 caches
 */
typedef enum {
    BCACHE_NORMAL,
    BCACHE_FREEZE,
    BCACHE_BYPASS
} BCACHE_Mode;

/*
 *  Enumerated list of caches
 */
typedef enum {
    BCACHE_L1D,
    BCACHE_L1P,
    BCACHE_L2
} BCACHE_Level;

/*
 *  Enumerated list of MAR values
 */
typedef enum {
    BCACHE_MAR_DISABLE = 0,
    BCACHE_MAR_ENABLE = 1
} BCACHE_Mar;

/*
 *  L1 and L2 Cache size structure
 */
typedef struct BCACHE_Size {
    BCACHE_L1_Size l1psize;
    BCACHE_L1_Size l1dsize;
    BCACHE_L2_Size l2size;
} BCACHE_Size;


/* function prototypes */
extern Void BCACHE_setSize(BCACHE_Size *size);
extern Void BCACHE_getSize(BCACHE_Size *size);
extern BCACHE_Mode BCACHE_setMode(BCACHE_Level level, BCACHE_Mode mode);
extern BCACHE_Mode BCACHE_getMode(BCACHE_Level level);
extern Void BCACHE_setMar(Ptr baseAddr, size_t byteSize, BCACHE_Mar value);
extern BCACHE_Mar BCACHE_getMar(Ptr baseAddr);

extern Void BCACHE_wait();
extern Void BCACHE_inv(Ptr blockPtr, size_t byteCnt, Bool wait);
extern Void BCACHE_wb(Ptr blockPtr, size_t byteCnt, Bool wait);
extern Void BCACHE_wbInv(Ptr blockPtr, size_t byteCnt, Bool wait);
extern Void BCACHE_invL1pAll();
extern Void BCACHE_wbAll();
extern Void BCACHE_wbInvAll();

/*
 *  Support for legacy implementation
 */
#define BCACHE_invalidate(ptr, cnt)             BCACHE_inv(ptr, cnt, 1)

/* BCACHE_writeback() performs a writeback invalidate */
#define BCACHE_writeback(ptr, cnt)              BCACHE_wbInv(ptr, cnt, 1)

#define BCACHE_writebackInvalidate(ptr, cnt)    BCACHE_wbInv(ptr, cnt, 1)
#define BCACHE_writebackInvalidateAll()         BCACHE_wbInvAll()

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
