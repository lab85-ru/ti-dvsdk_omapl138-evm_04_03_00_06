/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== obj.h ========
 *
 */

#ifndef OBJ_
#define OBJ_

#include <stddef.h>
#include <que.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OBJ_Header {
    QUE_Elem    link;
    Uns         type;
#if (defined(_54_) && defined(_FAR_MODE)) || defined(_55_) || defined(_28_)
    Uns         pad;
#endif
} OBJ_Header;

/*
 * Object types, current support debug SEM, MBX, TSK, SIO, SWI modules.
 */

#define OBJ_SEM         0
#define OBJ_SWI         1
#define OBJ_TSK         2
#define OBJ_MBX         3
#define OBJ_SIO         4
#define OBJ_DEV         5
#define OBJ_BUF         6

#define OBJ_MODULES     7
#define OBJ_KEY         0xabc0      /* key for type check */
#define OBJ_DEADKEY     0xdead      /* indicates object has been deleted */

extern QUE_Obj          OBJ_table[OBJ_MODULES];
                        /* defined in OBJ_end (macro invoked in *cfg.sxx) as:
                           .bss _OBJ_table, OBJ_MODULES * STD_TARGWORDMAUS * 2,
                                0, STD_TARGWORDMAUS
                           In case of 55x OBJ_table is declared in obj_init.c
                           which is needed to support ROM-ing.
                        */

extern Int              OBJ_segtab[OBJ_MODULES];    /* used by OBJ_delete() */
                        /* defined in obj_init.c */

#define OBJ_setsegid(type, id)  ( OBJ_segtab[(type)] = (id) )

/*
 *  ======== OBJ_create ========
 */
extern Ptr OBJ_create(Uns type, size_t size);

/*
 *  ======== OBJ_delete ========
 */
extern Void OBJ_delete(Uns type, Ptr ptr, size_t size);

/*
 *  ======== OBJ_init ========
 *  This is a function rather than a "#define OBJ_init SYS_nop" to allow
 *  for different OBJ implementations.
 */
extern Void OBJ_init();

/*
 *  ======== OBJ_validate ========
 *  Validate an object.
 *  Returns:
 *      SYS_OK      a valid object.
 *      SYS_EDEAD   object was previously deleted
 *      SYS_EBADOBJ invalid object
 */
extern Int OBJ_validate(Uns type, Ptr ptr);

#ifdef _BIOSROM_

extern Ptr (*OBJ_createPtr)(Uns type, size_t size);
extern Void (*OBJ_deletePtr)(Uns type, Ptr ptr, size_t size);

#define OBJ_createJ(type, size)         (OBJ_createPtr)(type, size)
#define OBJ_deleteJ(type, ptr, size)    (OBJ_deletePtr)(type, ptr, size)

#else

#define OBJ_createJ(type, size)         OBJ_create(type, size)
#define OBJ_deleteJ(type, ptr, size)    OBJ_delete(type, ptr, size)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* OBJ_ */

