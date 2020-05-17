/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pip.h ========
 *
 */

#ifndef PIP_
#define PIP_

#include <stddef.h>
#include <fxn.h>
#include <sts.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PIP_Obj PIP_Obj;
#if (defined(_55_) || defined(_28_))    /* If 55x or 28x processor */
typedef struct PIP_Sock PIP_Sock;
struct  PIP_Sock {
    PIP_Obj *   takeProbe;              /* take (get/alloc) probe*/
    Ptr         frameAddr;              /* Frame addr set by    */
                                        /*  PIP_get/alloc()*/
    size_t      frameSize;              /* ditto                */
    Ptr         curDesc;                /*pointer to list of    */ 
                                        /*pipe descs            */
    FXN_Obj *   pnotify;                /* ptr to notify        */ 
                                        /* reader/writer        */
                                        /* function             */
    Uns         numFrames;              /* number of frames     */
    PIP_Obj *   pGiveProbe;             /* give (free) probe    */
    Uns *       pNumFrames;             /* ptr to numframes on  */
    FXN_Obj     notifyFxn;              /* notify function*/
    STS_Obj *   preaderSts;             /* statistics configured*/
 }  ;

struct PIP_Obj {
    Uns         threshold;              /* max size of frames in pip */
    PIP_Sock    readerSock;             /* Reader socket  of pip     */
    PIP_Sock    writerSock ;            /* Writer socket  of pip     */
#if (defined(_28_))
    Int padblock[14];   
#endif

    }  ;

#else                                   /* else  if not 55x     */
struct PIP_Obj {
    Uns         threshold;              /* max size of frames in pip */

    PIP_Obj *   preaderTakeProbe;       /* take (get) probe */
    Ptr         readerAddr;             /* set by PIP_get() */
    Uns         readerSize;             /* ditto */
    Ptr         readerCurdesc;          /* pointer to list of pipe descs */
    FXN_Obj *   pnotifyReader;          /* ptr to notify reader function */
    Uns         readerNumFrames;        /* number of frames at reader end */
    PIP_Obj *   preaderGiveProbe;       /* give (free) probe */
    Uns *       pwriterNumFrames;       /* ptr to numframes on writer end */
#if (defined(_54_) && defined(_FAR_MODE))
    Uns         pad0;
#endif
    FXN_Obj     notifyWriter;           /* notify writer function */
    STS_Obj *   preaderSts;             /* statistics configured for reader */
    PIP_Obj *   pwriterTakeProbe;       /* take (alloc) probe */
    Ptr         writerAddr;             /* set by PIP_alloc() */
    Uns         writerSize;             /* ditto */
    Ptr         writerCurdesc;          /* pointer to list of pipe descs */
    FXN_Obj *   pnotifyWriter;          /* ptr to notify writer function */
    Uns         writerNumFrames;        /* number of numframes at writer end */
    PIP_Obj *   pwriterGiveProbe;       /* give (put) probe */
    Uns *       preaderNumFrames;       /* ptr to numframes at reader end */
#if (defined(_54_) && defined(_FAR_MODE))
    Uns         pad1;
#endif
    FXN_Obj     notifyReader;           /* notify reader function */
    STS_Obj *   pwriterSts;             /* statistics configured for writer */
#if (defined(_54_) && defined(_FAR_MODE))
    Uns         pad2;
#endif
};
#endif                                  /* ifdef 55x processor  */


typedef struct PIP_Obj *PIP_Handle;

extern Void     PIP_alloc(PIP_Handle pipe);
extern Void     PIP_free(PIP_Handle pipe);
extern Void     PIP_get(PIP_Handle pipe);
extern Void     PIP_put(PIP_Handle pipe);
extern Int      PIP_peek(PIP_Handle pipe, Ptr *addr, Uns rw);
extern Void     PIP_reset(PIP_Handle pipe);

#define PIP_READER 0
#define PIP_WRITER 1


#if (defined(_55_) || defined(_28_))    /* If 55x or 28x processor */
#define PIP_getReaderAddr(pipe)         (((pipe)->readerSock).frameAddr)
#define PIP_getReaderNumFrames(pipe)    (((pipe)->readerSock).numFrames)
#define PIP_getReaderSize(pipe)         (((pipe)->readerSock).frameSize)
#define PIP_setReaderSize(pipe,size)    (((pipe)->readerSock).frameSize = size)
#define PIP_getWriterAddr(pipe)         (((pipe)->writerSock).frameAddr)
#define PIP_getWriterNumFrames(pipe)    (((pipe)->writerSock).numFrames)
#define PIP_getWriterSize(pipe)         (((pipe)->writerSock).frameSize)
#define PIP_setWriterSize(pipe,size)    (((pipe)->writerSock).frameSize = size)

#else                                   /* If not 55x processor */

#define PIP_getReaderAddr(pipe)         ((pipe)->readerAddr)
#define PIP_getReaderNumFrames(pipe)    ((pipe)->readerNumFrames)
#define PIP_getReaderSize(pipe)         ((pipe)->readerSize)
#define PIP_setReaderSize(pipe,size)    ((pipe)->readerSize = size)
#define PIP_getWriterAddr(pipe)         ((pipe)->writerAddr)
#define PIP_getWriterNumFrames(pipe)    ((pipe)->writerNumFrames)
#define PIP_getWriterSize(pipe)         ((pipe)->writerSize)
#define PIP_setWriterSize(pipe,size)    ((pipe)->writerSize = size)
#endif                                  /* ifdef 55x processor  */

typedef struct PIP_Curdesc {
    Ptr addr;
    size_t size;
    struct PIP_Curdesc *next;
} PIP_Curdesc;

#if (defined(_55_) || defined(_28_))    /* if 55x or 28x processor */
#define PIP_getThreshold(pipe)          ((pipe)->threshold)
#define PIP_setReaderAddr(pipe,addr)    (((pipe)->readerSock).frameAddr = (Ptr)addr)
#define PIP_getReaderCurdesc(pipe)      (((pipe)->readerSock).curDesc)

#define PIP_setReaderCurdesc(pipe,desc) (((pipe)->readerSock).curDesc = (Ptr)desc)
#define PIP_setReaderNumFrames(pipe,nf) (((pipe)->readerSock).numFrames = nf)
#define PIP_setWriterAddr(pipe,addr)    (((pipe)->writerSock).frameAddr = addr)
#define PIP_getWriterCurdesc(pipe)      (((pipe)->writerSock).curDesc)
#define PIP_setWriterCurdesc(pipe,desc) (((pipe)->writerSock).curDesc = (Ptr)desc)
#define PIP_setWriterNumFrames(pipe,nf) (((pipe)->writerSock).numFrames = nf)

#define PIP_getAddrInDesc(desc)         ((desc)->addr)
#define PIP_getSizeInDesc(desc)         ((desc)->size)
#else                                   /* if Not 55x           */
#define PIP_getThreshold(pipe)          ((pipe)->threshold)
#define PIP_setReaderAddr(pipe,addr)    ((pipe)->readerAddr = (Ptr)addr)
#define PIP_getReaderCurdesc(pipe)      ((pipe)->readerCurdesc)
#define PIP_setReaderCurdesc(pipe,desc) ((pipe)->readerCurdesc = (Ptr)desc)
#define PIP_setReaderNumFrames(pipe,nf) ((pipe)->readerNumFrames = nf)
#define PIP_setWriterAddr(pipe,addr)    ((pipe)->writerAddr = addr)
#define PIP_getWriterCurdesc(pipe)      ((pipe)->writerCurdesc)
#define PIP_setWriterCurdesc(pipe,desc) ((pipe)->writerCurdesc = (Ptr)desc)
#define PIP_setWriterNumFrames(pipe,nf) ((pipe)->writerNumFrames = nf)

#define PIP_getAddrInDesc(desc)         ((desc)->addr)
#define PIP_getSizeInDesc(desc)         ((desc)->size)
#endif                                  /* ifdef 55x processor  */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* PIP_ */

