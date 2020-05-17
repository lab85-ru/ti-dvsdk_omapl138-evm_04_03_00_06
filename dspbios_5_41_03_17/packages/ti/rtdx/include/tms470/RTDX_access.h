/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/**************************************************************************
* Large Model/FAR mode support
**************************************************************************/
#ifndef __RTDX_ASCCESS_H
#define __RTDX_ASCCESS_H

#ifndef __TMS470__
#error "Wrong Target - This code for 470 only!"
#endif


#define RTDX_FAR_MODE           0
#define RTDX_USE_CODE_SECTION   0
#define RTDX_USE_DATA_SECTION   0


#if RTDX_USE_CODE_SECTION
#define RTDX_CODE       far
#else
#define RTDX_CODE
#endif

#if RTDX_USE_DATA_SECTION
#define RTDX_DATA       far
#else
#define RTDX_DATA
#endif


#endif /* __RTDX_ACCESS_H */
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

