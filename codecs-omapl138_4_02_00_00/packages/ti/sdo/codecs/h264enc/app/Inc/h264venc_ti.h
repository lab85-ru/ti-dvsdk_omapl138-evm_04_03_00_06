/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

/*!
********************************************************************************
  @file     h264venc_ti.h                                                       
  @brief    Interface for the H264VENC_TI module; TI's                          
            implementation of the IH264VENC interface                           
  @author   Pramod Kumar Swami                                                  
  @version  1.0 - Nov 05,2004                                                   
  @version  1.1 - Dec 06,2005 -> XMI changes incorporated                       
********************************************************************************
*/

#ifndef H264VENC_TI_
#define H264VENC_TI_

#include <ih264venc.h>
#ifdef C6000
#include <ti/xdais/ialg.h>
#include <ti/xdais/idma3.h>
#else
#include <ialg.h>
#include <idma3.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== H264VENC_TI_IALG ========
 *  TI's implementation of the IALG interface for H264VENC
 */
extern IALG_Fxns H264VENC_TI_IALG;

/*
 *  ======== H264VENC_TI_IH264VENC ========
 *  TI's implementation of the IH264VENC interface
 */
extern IH264VENC_Fxns H264VENC_TI_IH264VENC;

/*
 *  ======== H264VENC_TI_IDMA3 ========
 *  TI's implementation of H264VDec's IDMA3 interface
 */

extern  IDMA3_Fxns H264VENC_TI_IDMA3;

/*!
**************************************************************************************
  @fn           H264VENC_TI_init(IALG_Handle handle)
  @brief          Initialize the H264VENC_TI module as a whole
  @param          Nothing
  @return         Nothing
**************************************************************************************
*/
extern void H264VENC_TI_init(IALG_Handle handle);

/*!
**************************************************************************************
  @fn           Void H264VENC_TI_activate(IALG_Handle handle)
  @brief        Activate H264 object; e.g., initialize any scratch memory 
                required by the H264VENC_TI processing methods
  @param handle [IN]  handle of type IALG_Handle which in turns should be of 
                type IH264VENC_Handle
  @return       Nothing
**************************************************************************************
*/
extern void H264VENC_TI_activate(IALG_Handle handle);

/*!
**************************************************************************************
  @fn           H264VENC_TI_deactivate(IALG_Handle handle)
  @brief          Deactivate H264 object; e.g., save any scratch memory required
              by the H264VENC_TI processing methods to persistent memory
  @param handle [IN]  handle of type IALG_Handle which in turns should be of 
              type IH264VENC_Handle
  @return         Nothing
**************************************************************************************
*/
extern void H264VENC_TI_deactivate(IALG_Handle handle);

/*!
**************************************************************************************
  @fn           H264VENC_TI_exit(Void)
  @brief          Exit the H264VENC_TI module as a whole
  @param          Nothing
  @return         Nothing
**************************************************************************************
*/
extern void H264VENC_TI_exit(void);

/*!
**************************************************************************************
  @fn                 int H264VENC_TI_control(IH264VENC_Handle  handle,
                                     IH264VENC_Cmd cmd,
                                     IH264VENC_DynamicParams  *dynparams, 
                                     IH264VENC_Status *stats )
  @brief              cotnrols the Encoder via some get/set commands
  @param handle [IN]  handle of type IALG_Handle which in turns should be of 
                      type IH264VENC_Handle
  @param cmd    [IN]  specific command which controls the behavior of the routine;
                      eg. IH264VENC_SETPARAMS, IH264VENC_GETSTATUS or IH264VENC_GETBUFINFO
  @param dynparams [IN]  pointer to memory area to use dynamic parameters
  @param stats     [OUT] pointer to memory area to get status information
  @return              Status of the call ( FAIL /PASS )
**************************************************************************************
*/

extern int H264VENC_TI_control(IH264VENC_Handle  handle,
                               IH264VENC_Cmd cmd,
                               IH264VENC_DynamicParams  *dynparams, 
                               IH264VENC_Status *stats
                               ); 

/*!
****************************************************************************************
  @fn                 int H264VENC_TI_encode(IH264VENC_Handle handle,
                              XDM_BufDesc *inputObject,
                              XDM_BufDesc *outputObject,
                              IH264VENC_InArgs *inargs,
                              IH264VENC_OutArgs *outarg
                              )
  @brief              Encodes the raw data in H264 format
  @param handle [IN]  handle of type IALG_Handle which in turns should be of 
                      type H264VENC_TI_Obj
  @param inputObject [IN]   Input Buffer descriptor 
  @param outputObject[OUT]  Output Buffer descriptor 
  @param inargs      [IN]   Input arguments
  @param outarg      [OUT]  Output arguments

  @return         Status of the call ( FAIL /PASS )
****************************************************************************************
*/

extern int H264VENC_TI_encode(IH264VENC_Handle handle,
                              IVIDEO1_BufDescIn *inputObject,
                              XDM_BufDesc *outputObject,
                              IH264VENC_InArgs *inargs,
                              IH264VENC_OutArgs *outarg
                              );


#ifdef __cplusplus
}
#endif


#endif  /* H264VENC_TI_ */
/* ======================================================================== */ 
/* End of file : H264venc_ti.h                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */ 

