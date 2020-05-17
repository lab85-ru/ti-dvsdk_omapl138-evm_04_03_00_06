/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== buf.h ========
 *  
 */

#ifndef BUF_
#define BUF_

#include <mem.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct BUF_Obj {
    Ptr startaddr;           /* Starting address of buffer pool */
    MEM_sizep size;          /* Original size of buffer before alignment */
    MEM_sizep postalignsize; /* Size of buffer after alignment  */
    Ptr nextfree;            /*  Pointer to next free buffer    */
    Uns totalbuffers;        /*  Total number of buffers in pool*/ 
    Uns freebuffers;         /*  Number of free buffers in pool */
    Int segid;               /*  Memory Segment for buffer pool */
} BUF_Obj, *BUF_Handle;

typedef struct BUF_Attrs {
    Int         segid;       /* segment for element allocation*/
} BUF_Attrs;

typedef struct BUF_Header {
    struct BUF_Header *next; /* Pointer to next buffer header */
    Uns watermk;             /* Water mark field to indicate if 
                                the buffer has been used */ 
} BUF_Header;

typedef struct BUF_Stat {
    MEM_sizep postalignsize; /* Size of buffer after alignment*/ 
    MEM_sizep size;          /* Original size of buffer       */
    Uns totalbuffers;        /* Total buffers in pool         */    
    Uns freebuffers;         /* Number of free buffers in pool*/ 
} BUF_Stat;

#define BUF_ALLOCSTAMP 0xcafe/* Stamp indicates that buffer is allocated 
                                atleast once*/ 

#define BUF_FREESTAMP 0xbeef /* Stamp indicates that buffer is not
                                allocated yet */


extern BUF_Attrs BUF_ATTRS;  /* Default attributes */

extern BUF_Handle BUF_create(Uns numbuff, MEM_sizep size, Uns align, BUF_Attrs *attrs);
extern Uns BUF_delete(BUF_Handle buf);
extern Ptr BUF_alloc(BUF_Handle buf);
extern Bool BUF_free(BUF_Handle buf, Ptr bufaddr);
extern Void BUF_stat(BUF_Handle buf, BUF_Stat *statbuf);
extern Uns BUF_maxbuff(BUF_Handle buf);

#ifdef _BIOSROM_

extern BUF_Handle (*BUF_createPtr)(Uns numbuff, MEM_sizep size, Uns align, BUF_Attrs *attrs);
extern Uns (*BUF_deletePtr)(BUF_Handle buf);
extern Ptr (*BUF_allocPtr)(BUF_Handle buf);
extern Bool (*BUF_freePtr)(BUF_Handle buf, Ptr bufaddr);
extern Void (*BUF_statPtr)(BUF_Handle buf, BUF_Stat *statbuf);
extern Uns (*BUF_maxbuffPtr)(BUF_Handle buf);

#define BUF_createJ(numbuff, size, align, attrs)  (BUF_createPtr)(numbuff, size, align, attrs)
#define BUF_deleteJ(buf)                (BUF_deletePtr)(buf)
#define BUF_allocJ(buf)                 (BUF_allocPtr)(buf)
#define BUF_freeJ(buf, bufaddr)         (BUF_freePtr)(buf, bufaddr)
#define BUF_statJ(buf, statbuf)         (BUF_statPtr)(buf, statbuf)
#define BUF_maxbuffJ(buf)               (BUF_maxbuffPtr)(buf)

#else

#define BUF_createJ(numbuff, size, align, attrs)  BUF_create(numbuff, size, align, attrs)
#define BUF_deleteJ(buf)                BUF_delete(buf)
#define BUF_allocJ(buf)                 BUF_alloc(buf)
#define BUF_freeJ(buf, bufaddr)         BUF_free(buf, bufaddr)
#define BUF_statJ(buf, statbuf)         BUF_stat(buf, statbuf)
#define BUF_maxbuffJ(buf)               BUF_maxbuff(buf)

#endif


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* BUF_ */
