/** ============================================================================
 *  @file   user.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/
 *
 *  @desc   Defines the interfaces and data structures for the USER access.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


#if !defined (USER_H)
#define USER_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  KMEM_STARTADDRESS
 *
 *  @desc   Start address of Kernel Virtual Memory. On I386 machines, upper 1GB
 *          is used by kernel.
 *  ============================================================================
 */
#define KMEM_STARTADDRESS       ((Pvoid) 0xC0000000u)


/** ============================================================================
 *  @func   USER_copyFromUser
 *
 *  @desc   Copy user data to kernel buffer.
 *
 *  @arg    dst
 *              Destination bufefr.
 *  @arg    src
 *              Source buffer.
 *  @arg    numBytes
 *              number of bytes to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Failed to copy.
 *
 *  @enter  src, dst must be valid.
 *
 *  @leave  None
 *
 *  @see    USER_copyToUser
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyFromUser (IN Pvoid dst, OUT Pvoid src, IN Uint32 numBytes) ;


/** ============================================================================
 *  @func   USER_copyFromUser
 *
 *  @desc   Copy kernel data to user buffer.
 *
 *  @arg    src
 *              Source buffer.
 *  @arg    dst
 *              Destination bufefr.
 *  @arg    numBytes
 *              number of bytes to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Failed to copy.
 *
 *  @enter  src, dst must be valid.
 *
 *  @leave  None
 *
 *  @see    USER_copyFromUser
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyToUser (IN Pvoid src, OUT Pvoid dst, IN Uint32 numBytes) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (USER_H) */
