/** ============================================================================
 *  @file   _dsplink.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Consolidate Include file to include all internal generic definition
 *          include files.
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


#if !defined (_DSPLINK_H)
#define _DSPLINK_H


#include <_bitops.h>
#include <_dspdefs.h>
#include <_linkdefs.h>
#include <_safe.h>
#include <_intobject.h>
#include <loaderdefs.h>

#include <_loaderdefs.h>

#if defined (POOL_COMPONENT)
#include <_pooldefs.h>
#endif

#if defined (CHNL_COMPONENT)
#include <_datadefs.h>
#endif

#endif /* if !defined (_DSPLINK_H) */
