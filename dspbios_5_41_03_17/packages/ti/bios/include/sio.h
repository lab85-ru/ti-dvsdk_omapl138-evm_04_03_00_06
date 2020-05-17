/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== sio.h ========
 *  Stream I/O Manager
 *
 */

#ifndef SIO_
#define SIO_

#include <stddef.h>
#include <dev.h>
#include <que.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIO_INPUT       DEV_INPUT               /* input stream */
#define SIO_OUTPUT      DEV_OUTPUT              /* output stream */

#define SIO_STANDARD            0
#define SIO_ISSUERECLAIM        1

typedef DEV_Callback SIO_Callback;      /* for callback mode */

typedef struct SIO_Attrs {      /* stream attributes */
    Int         nbufs;          /* number of buffers */
    Int         segid;          /* buffer segment */
    size_t      align;          /* buffer alignment */
    Bool        flush;          /* TRUE -> don't block in DEV_idle() */
    Uns         model;          /* SIO_STANDARD, SIO_ISSUERECLAIM */
    Uns         timeout;        /* passed to all calls to DEV_reclaim() */
    SIO_Callback *callback;     /* initializes callback in DEV_Obj */
} SIO_Attrs;

typedef struct SIO_Obj {        /* stream object */
    DEV_Obj     dobj;           /* device object */
    Bool        flush;          /* flush */
    QUE_Obj     framelist;      /* start of frame list */
    DEV_Frame   *standardFrame; /* frame for use in SIO_STANDARD model only */
    Uns         model;          /* SIO_STANDARD or SIO_ISSUERECLAIM */
    DEV_Fxns    *pfxns;         /* pointer to DEV_Fxns for static stream init */
} SIO_Obj, *SIO_Handle;

extern SIO_Attrs SIO_ATTRS;     /* default attributes */

extern Int _SIO_idle(SIO_Handle stream, Bool flush);
#define SIO_idle(stream)                _SIO_idle(stream, 0)
#define SIO_flush(stream)               _SIO_idle(stream, 1)

#define SIO_ctrl(stream, cmd, arg)      DEV_ctrl((DEV_Handle)stream, cmd, arg)
#define SIO_ready(stream)               DEV_ready((DEV_Handle)stream, NULL)

#define SIO_segid(stream)               ((stream)->dobj.segid)
#define SIO_bufsize(stream)             ((stream)->dobj.bufsize)
#define SIO_nbufs(stream)               ((stream)->dobj.nbufs)

extern SIO_Handle SIO_create(String name, Int mode, size_t size, SIO_Attrs *attrs);
extern Int SIO_delete(SIO_Handle stream);
extern Int SIO_get(SIO_Handle stream, Ptr *buf);
extern Int SIO_staticbuf(SIO_Handle stream, Ptr *buf);
extern Int SIO_put(SIO_Handle stream, Ptr *buf, size_t nbytes);
extern Uns SIO_select(SIO_Handle streamtab[], Int n, Uns timeout);
extern Int SIO_issue(SIO_Handle stream, Ptr pbuf, size_t nbytes, Arg arg);
extern Int SIO_reclaim(SIO_Handle stream, Ptr *ppbuf, Arg *parg);
extern Int SIO_reclaimx(SIO_Handle stream, Ptr *ppbuf, Arg *parg, Int *pfstatus);

#ifdef _BIOSROM_

extern SIO_Handle (*SIO_createPtr)(String name, Int mode, size_t size, SIO_Attrs *attrs);
extern Int (*SIO_deletePtr)(SIO_Handle stream);
extern Int (*SIO_getPtr)(SIO_Handle stream, Ptr *buf);
extern Int (*SIO_staticbufPtr)(SIO_Handle stream, Ptr *buf);
extern Int (*SIO_putPtr)(SIO_Handle stream, Ptr *buf, size_t nbytes);
extern Uns (*SIO_selectPtr)(SIO_Handle streamtab[], Int n, Uns timeout);
extern Int (*SIO_issuePtr)(SIO_Handle stream, Ptr pbuf, size_t nbytes, Arg arg);
extern Int (*SIO_reclaimPtr)(SIO_Handle stream, Ptr *ppbuf, Arg *parg);
extern Int (*SIO_reclaimxPtr)(SIO_Handle stream, Ptr *ppbuf, Arg *parg, Int *pfstatus);

#define SIO_createJ(name, mode, size, attrs)    (SIO_createPtr)(name, mode, size, attrs)
#define SIO_deleteJ(stream)             (SIO_deletePtr)(stream)
#define SIO_getJ(stream, buf)           (SIO_getPtr)(stream, buf)
#define SIO_staticbufJ(stream, buf)     (SIO_staticbufPtr)(stream, buf)
#define SIO_putJ(stream, buf, nbytes)   (SIO_putPtr)(stream, buf, nbytes)
#define SIO_selectJ(streamtab, n, timeout)      (SIO_selectPtr)(streamtab, n, timeout)
#define SIO_issueJ(stream, pbuf, nbytes, arg)   (SIO_issuePtr)(stream, pbuf, nbytes, arg)
#define SIO_reclaimJ(stream, ppbuf, parg)       (SIO_reclaimPtr)(stream, ppbuf, parg)
#define SIO_reclaimxJ(stream, ppbuf, parg, pfstatus) \
        (SIO_reclaimxPtr)(stream, ppbuf, parg, pfstatus)

#else

#define SIO_createJ(name, mode, size, attrs)    SIO_create(name, mode, size, attrs)
#define SIO_deleteJ(stream)             SIO_delete(stream)
#define SIO_getJ(stream, buf)           SIO_get(stream, buf)
#define SIO_staticbufJ(stream, buf)     SIO_staticbuf(stream, buf)
#define SIO_putJ(stream, buf, nbytes)   SIO_put(stream, buf, nbytes)
#define SIO_selectJ(streamtab, n, timeout)      SIO_select(streamtab, n, timeout)
#define SIO_issueJ(stream, pbuf, nbytes, arg)   SIO_issue(stream, pbuf, nbytes, arg)
#define SIO_reclaimJ(stream, ppbuf, parg)       SIO_reclaim(stream, ppbuf, parg)
#define SIO_reclaimxJ(stream, ppbuf, parg, pfstatus) \
        SIO_reclaimx(stream, ppbuf, parg, pfstatus)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* SIO_ */
