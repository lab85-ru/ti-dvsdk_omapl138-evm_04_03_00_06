/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== mbx.h ========
 *
 */

#ifndef MBX_
#define MBX_

#include <stddef.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MBX_Obj {
    QUE_Obj     dataQue;        /* message queue */
    QUE_Obj     freeQue;        /* free queue */
    SEM_Obj     dataSem;        /* count = number of messages */
    SEM_Obj     freeSem;        /* count = number of free slots */

    Int         segid;          /* elements are allocated here */
    size_t      size;           /* size of mailbox elements */
    Uns         length;         /* number of elements in mailbox */
    String      name;           /* printable name */
} MBX_Obj, *MBX_Handle;

typedef struct MBX_Attrs {
    Int         segid;          /* segment for element allocation */
    String      name;           /* printable name */
} MBX_Attrs;

extern MBX_Attrs MBX_ATTRS;     /* default attributes */

#define MBX_getname(mbx)       ((mbx)->name)

extern MBX_Handle MBX_create(size_t size, Uns length, MBX_Attrs *attrs);
extern Void MBX_delete(MBX_Handle mbx);
extern Bool MBX_pend(MBX_Handle mbx, Ptr msg, Uns timeout);
extern Bool MBX_post(MBX_Handle mbx, Ptr msg, Uns timeout);

#ifdef _BIOSROM_

extern MBX_Handle (*MBX_createPtr)(size_t size, Uns length, MBX_Attrs *attrs);
extern Void (*MBX_deletePtr)(MBX_Handle mbx);
extern Bool (*MBX_pendPtr)(MBX_Handle mbx, Void *msg, Uns timeout);
extern Bool (*MBX_postPtr)(MBX_Handle mbx, Void *msg, Uns timeout);

#define MBX_createJ(size, length, attrs)        (MBX_createPtr)(size, length, attrs)
#define MBX_deleteJ(mbx)                (MBX_deletePtr)(mbx)
#define MBX_pendJ(mbx, msg, timeout)    (MBX_pendPtr)(mbx, msg, timeout)
#define MBX_postJ(mbx, msg, timeout)    (MBX_postPtr)(mbx, msg, timeout)

#else

#define MBX_createJ(size, length, attrs)        MBX_create(size, length, attrs)
#define MBX_deleteJ(mbx)                MBX_delete(mbx)
#define MBX_pendJ(mbx, msg, timeout)    MBX_pend(mbx, msg, timeout)
#define MBX_postJ(mbx, msg, timeout)    MBX_post(mbx, msg, timeout)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* MBX_ */
