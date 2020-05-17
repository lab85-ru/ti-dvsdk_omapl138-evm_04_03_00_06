/*
 *  Copyright 2006 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 * @(#) framework_components_1_0 1,0,0,1 1-14-2006 (fcp-b04)
 */

/*
 *  ======== bios_rts.h ========
 *
 * This file contains Non-Bios version of MEM, SYS, HWI, and DBC implementation
 */


#ifndef _BIOS_RTS_
#define _BIOS_RTS_


#include <stdlib.h>

#if DBC_ASSERTS

extern void SYS_abort(char *s, ...);

#define DBC_assert(c) if (!(c)) { \
        SYS_abort("Assertion Failed: file=%s, line=%d.\n", __FILE__, \
        __LINE__); }

#define DBC_require	DBC_assert
#define DBC_ensure 	DBC_assert

#else

#define DBC_assert(c)
#define DBC_require(c)
#define DBC_ensure(c)

#endif  /* DBC_ASSERTS */

#define MEM_alloc(segid, size, align)  MEM_valloc(segid, size, align, 0)
#define MEM_calloc(segid, size, align) MEM_valloc(segid, size, align, 0)

extern Void * MEM_valloc(Int segid, size_t size, size_t align, Char val);
extern Bool MEM_free(Int segid, Void * buf, size_t size);

extern Void TSK_enable(Void);
extern Void TSK_disable(Void);

#endif /* _BIOS_RTS_ */

