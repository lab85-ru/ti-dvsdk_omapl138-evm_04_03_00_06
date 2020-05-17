/** ============================================================================
 *  @file   _zcpy_data.h
 *
 *  @path   $(DSPLINK)/dsp/src/data/DspBios/
 *
 *  @desc   Internal declarations for the ZCPY DATA for GPP-DSP communication.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (_ZCPY_DATA_H)
#define _ZCPY_DATA_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const  MAX_SUPPORTED_CHHANNELS
 *
 *  @desc   Maximum number of Channels supported by each data layer.
 *  ============================================================================
 */
#define MAX_SUPPORTED_CHHANNELS     16

/** ============================================================================
 *  @const  ZCPYDATA_QUE_PADDING
 *
 *  @desc   Padding required for L2 cache line alignment within ZCPYDATA control
 *          objects for aligning a QUE element.
 *  ============================================================================
 */
#define ZCPYDATA_QUE_PADDING  ((CACHE_L2_LINESIZE - sizeof (QUE_Elem)) / 2)

/** ============================================================================
 *  @const  ZCPYDATA_IRP_PADDING
 *
 *  @desc   Padding required for L2 cache line alignment within ZCPYDATA control
 *          objects for aligning a CHNL_Irp element.
 *  ============================================================================
 */
#define ZCPYDATA_IRP_PADDING  ((CACHE_L2_LINESIZE - sizeof (CHNL_Irp)) / 2)


/** ============================================================================
 *  @name   ZCPYDATA_ChnlIrp
 *
 *  @desc   Forward declaration of ZCPYDATA_ChnlIrp.
 *  ============================================================================
 */
typedef struct ZCPYDATA_ChnlIrp_tag ZCPYDATA_ChnlIrp ;


/** ============================================================================
 *  @name   ZCPYDATA_ChnlMask
 *
 *  @desc   Defines the mask value for a channel.
 *
 *  @field  bitValue
 *              Bit mask with a '1' bit indicating that a command is pending on
 *              this channel.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct ZCPYDATA_ChnlMask_tag {
    volatile Uint16   bitValue ;
    ADD_PADDING (padding, DSPLINK_16BIT_PADDING)
} ZCPYDATA_ChnlMask ;

/** ============================================================================
 *  @name   ZCPYDATA_Ctrl
 *
 *  @desc   Defines the control structure used by GPP and DSP for ZCPY data
 *          driver
 *
 *  @field  gppFreeMask
 *              Indicating that a command is pending for the DSP on the channel
 *              corresponding to the index.
 *              This field is modified by the GPP only.
 *  @field  dspFreeMask
 *              Indicating that a command is pending for the GPP on the channel
 *              corresponding to the index.
 *              This field is modified by the DSP only.
 *  @field  gppOutputFull
 *              Output full flag mask for gpp channel.
 *  @field  dspOutputFull
 *              Output full flag mask for dsp channel.
 *  @field  toDspList
 *              Holds the list of buffers to be sent to the DSP.
 *  @field  toDspPadding
 *              Padding for alignment.
 *  @field  fmDspList
 *              Holds the list of buffers to be received from the DSP.
 *  @field  fmDspPadding
 *              Padding for alignment.
 *  @field  csToDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the toDspList.
 *  @field  csFmDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the fmDspList.
 *  ============================================================================
 */
typedef struct ZCPYDATA_Ctrl_tag {
    volatile ZCPYDATA_ChnlMask   gppFreeMask   [MAX_SUPPORTED_CHHANNELS] ;
    volatile ZCPYDATA_ChnlMask   dspFreeMask   [MAX_SUPPORTED_CHHANNELS] ;
    volatile ZCPYDATA_ChnlMask   gppOutputFull [MAX_SUPPORTED_CHHANNELS] ;
    volatile ZCPYDATA_ChnlMask   dspOutputFull [MAX_SUPPORTED_CHHANNELS] ;

    volatile QUE_Elem            toDspList ;
    ADD_PADDING (toDspPadding, ZCPYDATA_QUE_PADDING)
    volatile QUE_Elem            fmDspList ;
    ADD_PADDING (fmDspPadding, ZCPYDATA_QUE_PADDING)
    volatile MPCS_ShObj          csToDspList ;
    volatile MPCS_ShObj          csFmDspList ;

    volatile QUE_Elem            freeChirps ;
    ADD_PADDING (freeChirpsPadding, ZCPYDATA_QUE_PADDING)
    volatile MPCS_ShObj          csFreeChirps ;
} ZCPYDATA_Ctrl ;


/** ============================================================================
 *  @name   ZCPYDATA_ChnlIrp_tag
 *
 *  @desc   Encapsulates the CHNL_Irp object to align it to the L2 cache line
 *          size.
 *
 *  @field  chnlIrp
 *              CHNL IRP object.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
struct ZCPYDATA_ChnlIrp_tag {
    volatile CHNL_Irp     chnlIrp ;
             Uint16       padding [ZCPYDATA_IRP_PADDING] ;
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_ZCPY_DATA_H) */
