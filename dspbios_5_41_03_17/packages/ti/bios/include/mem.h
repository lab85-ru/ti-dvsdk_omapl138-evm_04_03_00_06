/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== mem.h ========
 *
 */

#ifndef MEM_
#define MEM_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>    /* for size_t */

#ifdef _6x_
extern far Fxn MEM_D_lockaddr;
extern far Fxn MEM_D_unlockaddr;
#else
extern Fxn MEM_D_lockaddr;
extern Fxn MEM_D_unlockaddr;
#endif

#if (defined(_55L_) || defined(_55H_) || defined(_28_)) /* If 55x or 28x processor */
/* If 55x or 28x processor */
/* MEM_sizep is NOT size_t for _55L_ because the heap can
 * be as big as a DataPtr
 */
typedef  unsigned long  MEM_sizep;
#else
typedef  unsigned int   MEM_sizep;
#endif

#ifdef _54_
extern void MEM_register_lock(Fxn fxn);
extern void MEM_register_unlock(Fxn fxn);
#else
#define MEM_register_lock(fxn)          MEM_D_lockaddr = (fxn)
#define MEM_register_unlock(fxn)        MEM_D_unlockaddr = (fxn)
#endif

#define MEM_ILLEGAL ((Ptr)NULL) /* illegal memory address */

typedef struct MEM_Attrs {
    Int        space;           /* x, y, or p memory */
} MEM_Attrs;

typedef struct MEM_Config {
    Int     MALLOCSEG;          /* segid used by malloc() and free() */
} MEM_Config;

typedef struct MEM_Segment {
    Ptr        base;            /* Base of the segment */
    MEM_sizep  length;          /* Size of the segment */
    Int        space;           /* x, y or p memory */
} MEM_Segment;

typedef struct MEM_Stat {
    MEM_sizep   size;           /* original size of segment */
    MEM_sizep   used;           /* number of bytes used in segment */
    size_t      length;         /* length of largest contiguous block */
    Int         space;          /* Indicate x, y or p memory */
} MEM_Stat;

/*
 *  ======== MEM_Header ========
 *  This header is used to maintain a list of free memory blocks.
 *  The memory allocation/free routines REQUIRE that the size of this
 *  header is a power of two; this simplifies several computations in
 *  these routines by eliminating the need for the C % operator.
 *  
 */
typedef struct MEM_Header {
    struct MEM_Header  *next;   /* form a free memory link list */
    MEM_sizep           size;   /* size of the free memory */
} MEM_Header;

#define MEM_headersize(space)   sizeof(MEM_Header)

#define MEM_headermask(space)   (MEM_headersize(space) - 1)

extern MEM_Config      *MEM;
extern MEM_Segment      MEM_memtab[];
extern MEM_Segment     *MEM_staticMemtab;

#define MEM_calloc(segid, size, align) MEM_vallocJ(segid, size, align, 0)

/* The default space must be defined to be 1 for all targets. */

#define MEM_32BIT       1
#define MEM_16BIT       2
#define MEM_8BIT        3

extern Void     MEM_init(void);
extern Ptr      MEM_alloc(Int segid, size_t size, size_t align);
extern Int      MEM_define(Ptr base, MEM_sizep length, MEM_Attrs *attrs);
extern Int      MEM_increaseTableSize(Uns numEntries);
extern Void     MEM_redefine(Int segid, Ptr base, MEM_sizep length);
extern Bool     MEM_free(Int segid, Ptr addr, size_t size);
extern Void     MEM_print(Int segid);
extern Bool     MEM_stat(Int segid, MEM_Stat *statbuf);
extern Void     MEM_undefine(Int segid);
extern Ptr      MEM_valloc(Int segid, size_t size, size_t align, Char val);
extern Ptr      MEM_getBaseAddress(Int segid);

#ifdef _BIOSROM_

extern Ptr (*MEM_allocPtr)(Int segid, size_t size, size_t align);
extern Bool (*MEM_freePtr)(Int segid, Ptr addr, size_t size);
extern Ptr (*MEM_vallocPtr)(Int segid, size_t size, size_t align, Char value);
extern Void (*MEM_redefinePtr)(Int segid, Ptr base, MEM_sizep length);
extern Int  (*MEM_definePtr)(Ptr base, MEM_sizep length, MEM_Attrs *attrs);
extern Int  (*MEM_increaseTableSizePtr)(Uns numEntries);
extern Void (*MEM_undefinePtr)(Int segid);

#define MEM_allocJ(segid, size, align)  (MEM_allocPtr)(segid, size, align)
#define MEM_freeJ(segid, addr, size)    (MEM_freePtr)(segid, addr, size)
#define MEM_vallocJ(segid, size, align, value) (MEM_vallocPtr)(segid, size, align, value)
#define MEM_redefineJ(segid, base, length)     (MEM_redefinePtr)(segid, base, length)
#define MEM_defineJ(base, length, attrs)       (MEM_definePtr)(base, length, attrs)
#define MEM_increaseTableSizeJ(numEntries)     (MEM_increaseTableSizePtr)(numEntries)
#define MEM_undefineJ(segid)                   (MEM_undefinePtr)(segid)

#else

#define MEM_allocJ(segid, size, align)  MEM_alloc(segid, size, align)
#define MEM_freeJ(segid, addr, size)    MEM_free(segid, addr, size)
#define MEM_vallocJ(segid, size, align, value) MEM_valloc(segid, size, align, value)
#define MEM_redefineJ(segid, base, length)     MEM_redefine(segid, base, length)
#define MEM_defineJ(base, length, attrs)       MEM_define(base, length, attrs)
#define MEM_increaseTableSizeJ(numEntries)     MEM_increaseTableSize(numEntries)
#define MEM_undefineJ(segid)                   MEM_undefine(segid)

#endif

#ifdef __cplusplus
}
#endif /* extern "C"*/

#endif /* MEM_ */
