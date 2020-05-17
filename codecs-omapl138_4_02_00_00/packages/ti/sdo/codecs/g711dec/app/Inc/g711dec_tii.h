/*******************************************************************************
*-----------------------------------------------------------------------------*
*                     TEXAS INSTRUMENTS INC                                   *
*                     Multimedia Codecs Group                                 *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
* (C) Copyright 2006  Texas Instruments Inc.  All rights reserved.            *
*                                                                             *
* Exclusive property of the Multimedia Codecs Group, Texas Instruments Inc.   *
* Any handling, use, disclosure, reproduction, duplication, transmission      *
* or storage of any part of this work by any means is subject to restrictions *
* and prior written permission set forth.                                     *
*                                                                             *
* This copyright notice, restricted rights legend, or other proprietary       *
* markings must be reproduced without modification in any authorized copies   *
* of any part of this work. Removal or modification of any part of this       *
* notice is prohibited.                                                       *
*                                                                             *
******************************************************************************/  
 
#ifndef G711DEC_TII_
#define G711DEC_TII_

#include "isphdec1.h"
#include "ispeech1_pcm.h"

/*         
 *  ======== G711DEC_TII_IALG ========
 *  TII's implementation of the IALG interface for G711
 */

extern IALG_Fxns G711DEC_TII_IALG;

/*
 *  ======== G711DEC_TII_IG711DEC ========
 *  TII's implementation of the IG711 interface
 */

extern ISPHDEC1_Fxns G711DEC_TII_IG711DEC;

/*
 *  ======== G711DEC_TII_alloc ========
 */

extern Int G711DEC_TII_alloc(const IALG_Params *instParams, 
                                struct IALG_Fxns **fxns,
                                IALG_MemRec memTab[]);

/*
 *  ======== G711DEC_TII_exit ========
 */

extern Void G711DEC_TII_exit(Void);

/*
 *  ======== G711DEC_TII_free ========
 */

extern Int G711DEC_TII_free(IALG_Handle handle, IALG_MemRec memTab[]);

/*
 *  ======== G711DEC_TII_init ========
 */

extern Void G711DEC_TII_init(Void);

/*
 *  ======== G711DEC_TII_initObj ========
 */

extern Int G711DEC_TII_initObj(IALG_Handle handle,
                                 const IALG_MemRec memTab[], 
                                 IALG_Handle pHandle, 
                                 const IALG_Params *instParams);
    
/*
 *  ======== G711DEC_TII_numAlloc ========
 */

extern Int G711DEC_TII_numAlloc(Void);


/*
 *  ======== G711DEC_TII_g711Control ========
 */

extern Int G711DEC_TII_g711Control(ISPHDEC1_Handle handle,ISPHDEC1_Cmd id, 
                                        ISPHDEC1_DynamicParams *params,
                                        ISPHDEC1_Status *status);
/*
 *  ======== G711DEC_TII_g711Decode ========
 */

extern XDAS_Int32 G711DEC_TII_g711Decode(ISPHDEC1_Handle handle, 
                                          XDM1_SingleBufDesc *inBufs, 
                                          XDM1_SingleBufDesc *outBufs,
                                          ISPHDEC1_InArgs *inArgs,
                                          ISPHDEC1_OutArgs *outArgs);

#endif  /* G711DEC_TII_ */
