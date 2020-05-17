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
/***********************************************************************
 *   File: 711enc_tii.h                                             *
 *                                                                     *
 ***********************************************************************/
/*  
 *  Interface for the G711_TII module; TII's implementation 
 *  of the IG711 interface
 */
#ifndef G711ENC_TII_
#define G711ENC_TII_

#include <ialg.h>
#include "isphenc1.h"
#include "ispeech1_pcm.h"

/*         
 *  ======== G711ENC_TII_IALG ========
 *  TII's implementation of the IALG interface for G711
 */
extern IALG_Fxns G711ENC_TII_IALG;


/*
 *  ======== G711ENC_TII_IG711ENC ========
 *  TII's implementation of the IG711 interface
 */
extern ISPHENC1_Fxns G711ENC_TII_IG711ENC;

/*
 *  ======== G711ENC_TII_alloc ========
 */
extern Int G711ENC_TII_alloc(const IALG_Params *instParams,
                              struct IALG_Fxns **fxns, IALG_MemRec memTab[]);

/*
 *  ======== G711ENC_TII_exit ========
 */
extern Void G711ENC_TII_exit(Void);

/*
 *  ======== G711ENC_TII_free ========
 */
extern Int G711ENC_TII_free(IALG_Handle handle, IALG_MemRec memTab[]);

/*
 *  ======== G711ENC_TII_init ========
 */
extern Void G711ENC_TII_init(Void);

/*
 *  ======== G711ENC_TII_initObj ========
 */
extern Int G711ENC_TII_initObj(IALG_Handle handle,
                                 const IALG_MemRec memTab[], 
                                 IALG_Handle pHandle, 
                                  const IALG_Params *instParams);
    
/*
 *  ======== G711ENC_TII_numAlloc ========
 */
extern Int G711ENC_TII_numAlloc(Void);

/*
 *  ======== G711ENC_TII_711Control ========
 */
extern XDAS_Int32 G711ENC_TII_g711Control(ISPHENC1_Handle handle,
                                    ISPHENC1_Cmd cmd, 
                                    ISPHENC1_DynamicParams *params,
                                    ISPHENC1_Status *status);
/*
 *  ======== G711ENC_TII_711Encode ========
 */
extern XDAS_Int32 G711ENC_TII_g711Encode (ISPHENC1_Handle handle,
                                            XDM1_SingleBufDesc *inBufs, 
                                            XDM1_SingleBufDesc *outBufs,
                                            ISPHENC1_InArgs *inargs, 
                                            ISPHENC1_OutArgs *outargs);

#endif  /* G711ENC_TII_ */
