/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio.h ========
 *
 *  General Input/Output Module's public header file.
 *
 */

#ifndef GIO_
#define GIO_

#include <iom.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  These function pointers are used to bind blocking functions to IOM.
 *  In DSP/BIOS TSK based applications, these function pointers
 *  will be assigned to SEM_create, SEM_delete, SEM_pend and SEM_post. These
 *  pointers can be assigned to simple block/unblock operations for users
 *  who aren't using TSK.
 */
typedef Ptr  (*GIO_TsemCreate)(Int count, Ptr attrs);
typedef Void (*GIO_TsemDelete)(Ptr semHandle);
typedef Bool (*GIO_TsemPend)(Ptr semHandle, Uns timeout);
typedef Void (*GIO_TsemPost)(Ptr semHandle);

/*
 *  A pointer named 'GIO' and a global GIO_CONFIG structure will be initialized
 *  by the configuration tool to point to an GIO_Config structure.  This 
 *  structure will be referenced at run-time so that GIO will not have any
 *  hard-coded reference to SEM_pend, SEM_post, etc. This will allow GIO to
 *  be used in TSK and non-TSK based applications.
 */
typedef struct GIO_Config {
    GIO_TsemCreate      SEMCREATE;      /* typically SEM_create */
    GIO_TsemDelete      SEMDELETE;      /* typically SEM_delete */
    GIO_TsemPend        SEMPEND;        /* typically SEM_pend */
    GIO_TsemPost        SEMPOST;        /* typically SEM_post */
} GIO_Config;

/*
 *  This attributes structure is passed to GIO_create() to specify channel-
 *  specific parameters.
 */
typedef struct GIO_Attrs  {
    Int         nPackets;       /* number of asynch I/O packets */
    Uns         timeout;        /* for blocking calls (SYS_FOREVER) */
} GIO_Attrs;

/*
 *  This is the application-level callback function.
 */
typedef Void    (*GIO_TappCallback)(Ptr arg, Int status, Ptr bufp, size_t size);

/*
 *  Application layer code can pass a pointer to one of these structures
 *  for the optional asynchronous call extensions.  The callback
 *  function 'fxn' is called with 'arg' and the other params as defined
 *  by the GIO_TappCallback typedef.
 */
typedef struct GIO_AppCallback {
    GIO_TappCallback    fxn;
    Ptr                 arg;
} GIO_AppCallback;

/*
 *  GIO_create() allocates and returns a pointer to one of these structures
 *  when successful.  This structure contains all of the fields necessary
 *  for subsequent calls to GIO.
 */
typedef struct GIO_Obj {
    IOM_Fxns    *fxns;          /* pointer to mini-driver function table */
    Uns         mode;           /* IOM_INPUT, IOM_OUTPUT or IOM_INOUT */
    Uns         timeout;        /* timeout parameter used for blocking calls */
    IOM_Packet  syncPacket;     /* used only for synchronous operations */
    QUE_Obj     freeList;       /* available frames for asynchronous I/O */
    Ptr         syncObj;        /* opaque pointer to synchronization object */
    Ptr         mdChan;         /* pointer to mini-driver channel object */
} GIO_Obj, *GIO_Handle;

/*
 *  Default IOM Attributes structure.  This structure is used to assign
 *  GIO_Attrs defaults and when 'NULL' is passed to GIO_create().
 */
extern GIO_Attrs GIO_ATTRS;

/*
 *  Pointer to global GIO_Config structure (GIO_CONFIG). GIO_CONFIG is 
 *  defined by the configuration tool.  
 */
extern GIO_Config *GIO;

/*
 *  -------- class APIs --------
 */
extern Void GIO_init();

/*
 *  ======== GIO_abort ========
 *  Abort all input and output.  GIO_abort() is a synchronous call and only 
 *  returns when all I/O has been successfully aborted.
 */
#define GIO_abort(gioChan) \
        GIO_submit(gioChan, IOM_ABORT, NULL, NULL, NULL)

/*
 *  ======== IOM_control ========
 *  Device specific control call. 
 */
extern Int GIO_control(GIO_Handle gioChan, Uns cmd, Ptr args);


/*
 *  ======== GIO_create ========
 *  GIO_create() allocates and initializes an GIO_Obj structure.  GIO_create()
 *  returns a non-NULL GIO_Handle object on success and NULL for failure.
 *  The 'name' parameter is used to find a matching name in the device
 *  table.  Associated IOM_Fxns table and params structure are then used
 *  to create a channel for that device.   The 'attrs->nPackets' parameter
 *  specifies the maximum number of queued asynchronous requests that
 *  can be outstanding.
 */
extern GIO_Handle GIO_create(String name, Int mode, Int *status, Ptr optArgs,
         GIO_Attrs *attrs);


/*
 *  ======== GIO_new ========
 *  GIO_new() initializes the provided GIO_Obj structure.  GIO_new()
 *  returns the non-NULL GIO_Handle object on success and NULL for failure.
 *  The 'name' parameter is used to find a matching name in the device
 *  table.  Associated IOM_Fxns table and params structure are then used
 *  to create a channel for that device.   The 'attrs->nPackets' parameter
 *  specifies the maximum number of queued asynchronous requests that
 *  can be outstanding and specifies the length of the provided packetBuf.
 *  The syncObject is usually a SEM handle.
 */
extern GIO_Handle GIO_new(GIO_Handle gioChan, String name, Int mode, Int *status, Ptr optArgs,
        IOM_Packet packetBuf[], Ptr syncObject, GIO_Attrs *attrs);

/*
 *  ======== GIO_delete ========
 *  GIO_delete() deletes the underlying mini-drivers and then frees up
 *  the GIO_Obj structure and any associated GIO_Packet structures.
 */
extern Int GIO_delete(GIO_Handle gioChan);


/*
 *  ======== GIO_flush ========
 *  Flush all input and output.  Flush drains all output buffers and discards
 *  any pending input.  GIO_flush() is synchronous and only returns when
 *  all I/O has been successfully flushed.
 */
#define GIO_flush(gioChan) \
        GIO_submit(gioChan, IOM_FLUSH, NULL, NULL, NULL)


/*
 *  ======== GIO_read ========
 *  Synchronous read command.  GIO_read() returns 'IOM_COMPLETE' when I/O
 *  is complete. GIO_read() returns 'IOM_ETIMEOUT' error if timeout occured
 *  before read could complete.
 */
#define GIO_read(gioChan, bufp, psize) \
        GIO_submit(gioChan, IOM_READ, bufp, psize, NULL)


/*
 *  ======== GIO_submit ========
 *  GIO_submit() is not typically called by the application level. Assorted
 *  macros use GIO_submit() to do the needed work.
 * 
 *  The appCallback parameter causes GIO_submit() to be synchronous or
 *  asynchronous.  If appCallback is  NULL, GIO_submit() will call the 
 *  GIO->PEND blocking function(synchronous).  Otherwise, GIO_submit()
 *  will call the callback function and argument when the I/O operation 
 *  completes(asynchronous).
 */
extern Int GIO_submit(GIO_Handle gioChan, Uns cmd, Ptr bufp,
        size_t *psize, GIO_AppCallback *appCallback);


/*
 *  ======== GIO_write ========
 *  Synchronous write command.  Same semantics as GIO_read() above.
 */
#define GIO_write(gioChan, bufp, psize) \
        GIO_submit(gioChan, IOM_WRITE, bufp, psize, NULL)

#ifdef _BIOSROM_

extern Int (*GIO_controlPtr)(GIO_Handle gioChan, Uns cmd, Ptr args);
extern GIO_Handle (*GIO_createPtr)(String name, Int mode, Int *status, 
        Ptr optArgs, GIO_Attrs *attrs);
extern Int (*GIO_deletePtr)(GIO_Handle gioChan);
extern Int (*GIO_submitPtr)(GIO_Handle gioChan, Uns cmd, Ptr bufp,
        size_t *psize, GIO_AppCallback *appCallback);

#define GIO_controlJ(gioChan, cmd, args) (GIO_controlPtr)(gioChan, cmd, args)
#define GIO_createJ(name, mode, status, optArgs, attrs) \
        (GIO_createPtr)(name, mode, status, optArgs, attrs)
#define GIO_deleteJ(gioChan)    (GIO_deletePtr)(gioChan)
#define GIO_submitJ(gioChan, cmd, bufp, psize, appCallback) \
        (GIO_submitPtr)(gioChan, cmd, bufp, psize, appCallback)

#else

#define GIO_controlJ(gioChan, cmd, args) GIO_control(gioChan, cmd, args)
#define GIO_createJ(name, mode, status, optArgs, attrs) \
        GIO_create(name, mode, status, optArgs, attrs)
#define GIO_deleteJ(gioChan)    GIO_delete(gioChan)
#define GIO_submitJ(gioChan, cmd, bufp, psize, appCallback) \
        GIO_submit(gioChan, cmd, bufp, psize, appCallback)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* GIO_ */
