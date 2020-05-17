/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

#include <RTDX_access.h>        /* RTDX_CODE, RTDX_DATA                 */

#ifndef BUFRSZ
#define BUFRSZ 256
#endif

#if RTDX_USE_DATA_SECTION
#pragma DATA_SECTION(RTDX_Buffer,".rtdx_data")
#pragma DATA_SECTION(RTDX_Buffer_Start,".rtdx_data")
#pragma DATA_SECTION(RTDX_Buffer_End,".rtdx_data")
#endif

/* DMAs used by RTDX will require alignment on word boundaries...       */
#pragma DATA_ALIGN(RTDX_Buffer,4)
int RTDX_DATA RTDX_Buffer[BUFRSZ];

/*
 * The buffer used by RTDX is defined by 2 symbols: RTDX_Buffer_Start
 * and RTDX_Buffer_End.  We use the following declarations in order to
 * export these names
*/

const void RTDX_DATA *RTDX_Buffer_Start = &RTDX_Buffer[0];
const void RTDX_DATA *RTDX_Buffer_End  = &RTDX_Buffer[BUFRSZ-1];
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

