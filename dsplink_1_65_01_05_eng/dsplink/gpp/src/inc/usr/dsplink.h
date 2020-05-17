/** ============================================================================
 *  @file   dsplink.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines data types and structures used by DSP/BIOS(tm) Link.
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


#if !defined (DSPLINK_H)
#define DSPLINK_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <gpptypes.h>
#include <constants.h>
#include <errbase.h>
#include <archdefs.h>
#include <linkcfgdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*  ============================================================================
 *  @const  MAX_IPS
 *
 *  @desc   Maximum number of IPS objects supported for each DSP.
 *
 *  ============================================================================
 */
#define MAX_IPS             16u

/** ============================================================================
 *  @const  WAIT_FOREVER
 *
 *  @desc   Wait indefinitely.
 *  ============================================================================
 */
#define WAIT_FOREVER           (~((Uint32) 0u))

/** ============================================================================
 *  @const  WAIT_NONE
 *
 *  @desc   Do not wait.
 *  ============================================================================
 */
#define WAIT_NONE              ((Uint32) 0u)


/** ============================================================================
 *  @macro  IS_GPPID
 *
 *  @desc   Is the GPP ID valid.
 *  ============================================================================
 */
#define IS_GPPID(id)        (id == ID_GPP)


#if defined (__cplusplus)
}
#endif


#endif /* !defined (DSPLINK_H) */
