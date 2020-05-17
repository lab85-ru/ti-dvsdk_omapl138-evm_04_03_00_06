/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== _gio.h ========
 *    GIO private header file.
 *
 */
 
#ifndef _GIO_
#define _GIO_

#include <std.h>
#include <iom.h>
#include <mem.h>


#ifdef __cplusplus
extern "C" {
#endif

#define _GIO_mkPacket()         MEM_allocJ(0, sizeof(IOM_Packet), 0);
#define _GIO_rmPacket(packet)   MEM_freeJ(0, packet, sizeof(IOM_Packet));

/*
 * Mini-driver's i/o completion callback routine. 
 */
extern Void _GIO_iomCallback(Ptr cbArg, IOM_Packet *packet);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _GIO_ */
