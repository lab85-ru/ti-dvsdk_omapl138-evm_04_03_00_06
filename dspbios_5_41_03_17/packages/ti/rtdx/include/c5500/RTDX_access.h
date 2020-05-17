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
#ifndef __RTDX_ACCESS_H
#define __RTDX_ACCESS_H

#ifndef __TMS320C55X__
#error "Wrong Target - This code for C55x only!"
#endif

#ifndef __LARGE_MODEL__
#define RTDX_FAR_MODE           0
#define RTDX_USE_CODE_SECTION   1
#define RTDX_USE_DATA_SECTION   1
#else
#define RTDX_FAR_MODE           1
#define RTDX_USE_CODE_SECTION   1
#define RTDX_USE_DATA_SECTION   1
#endif

/* C55x does not use the "far" keyword  */
#define RTDX_CODE
#define RTDX_DATA

#endif /* __RTDX_ACCESS_H */
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

