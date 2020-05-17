/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/***********************************************************************
* The RTDX Target Library User Interface
************************************************************************/
#ifndef __RTDXPOLL_H
#define __RTDXPOLL_H

/* This controls configuration to be either polling or interrupt driven */
/* It must be defined - values are 0 or 1.                              */
#undef RTDX_POLLING_IMPLEMENTATION
#define RTDX_POLLING_IMPLEMENTATION 0
#if RTDX_POLLING_IMPLEMENTATION
extern void RTDX_Poll( void );
#endif

#endif  /* __RTDXPOLL_H */
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

