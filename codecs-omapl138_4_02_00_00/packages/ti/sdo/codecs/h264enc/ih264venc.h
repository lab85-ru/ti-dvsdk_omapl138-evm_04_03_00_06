/*
//============================================================================
//
//    FILE NAME : IH264VENC.h
//
//    ALGORITHM : h264enc
//
//    VENDOR    : TII
//
//    TARGET DSP: C64x
//
//    PURPOSE   : IH264VENC Interface Header
//
//============================================================================
*/

#ifndef _IH264VENC_
#define _IH264VENC_

#ifdef C6000
#include <ti/bios/include/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividenc1.h>
#else
#include <std.h>
#include <ialg.h>
#include <ividenc1.h>
#endif

//#include <xdas.h>  //XDM1.0
//#include <ialg.h>
//#include <ividenc1.h>


#ifdef __cplusplus
extern "C" {
#endif
/*
// ===========================================================================
// IH264VENC_Handle
//
// This handle is used to reference all h264enc instance objects
*/
typedef struct IH264VENC_Obj *IH264VENC_Handle;

/*
// ===========================================================================
// IH264VENC_Obj
//
// This structure must be the first field of all h264enc instance objects
*/
typedef struct IH264VENC_Obj
{
    struct IH264VENC_Fxns *fxns;
} IH264VENC_Obj;


/*
// ===========================================================================
// IH264VENC_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct IH264VENC_Status {

  /*!
   * Mandatory fields of the status structure - Base class
   */
  IVIDENC1_Status  videncStatus;
  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
  XDAS_Int32       mvDataSize   ;


} IH264VENC_Status;


//!< H.264 Encoder Slice and Picture level Loop Filter Control
typedef enum
{
  FILTER_ALL_EDGES = 0,             //!< Enable filtering of all the edges
  DISABLE_FILTER_ALL_EDGES,         //!< Disable filtering of all the edges
  DISABLE_FILTER_SLICE_EDGES        //!< Disable filtering of slice edges

} IH264VENC_LoopFilterParams ;



/*
// ===========================================================================
// IH264VENC_Cmd
//
// The Cmd enumeration defines the control commands for the MPEG4 video encoder
// control method.
*/
typedef IVIDENC1_Cmd IH264VENC_Cmd;

//!< Level Identifier for H.264 Encoder
typedef enum
{
  IH264_LEVEL_10 = 10,  //!< Level 1.0
  IH264_LEVEL_1b =  9,  //!< Level 1.b
  IH264_LEVEL_11 = 11,  //!< Level 1.1
  IH264_LEVEL_12 = 12,  //!< Level 1.2
  IH264_LEVEL_13 = 13,  //!< Level 1.3
  IH264_LEVEL_20 = 20,  //!< Level 2.0
  IH264_LEVEL_21 = 21,  //!< Level 2.1
  IH264_LEVEL_22 = 22,  //!< Level 2.2
  IH264_LEVEL_30 = 30,   //!< Level 3.0
  IH264_LEVEL_31 = 31,   //!< Level 3.1
  IH264_LEVEL_32 = 32 ,  //!< Level 3.2
  IH264_LEVEL_40 = 40 ,  //!< Level 4.0
  IH264_LEVEL_41 = 41,  //!< Level 4.1
  IH264_LEVEL_42 = 42 ,  //!< Level 4.2
  IH264_LEVEL_50 = 50   //!< Level 5.0

} IH264VENC_Level ;
/*
// ===========================================================================
// control method commands
*/
#define IH264VENC_GETSTATUS      XDM_GETSTATUS
#define IH264VENC_SETPARAMS      XDM_SETPARAMS
#define IH264VENC_RESET          XDM_RESET
#define IH264VENC_FLUSH          XDM_FLUSH
#define IH264VENC_SETDEFAULT     XDM_SETDEFAULT
#define IH264VENC_GETBUFINFO     XDM_GETBUFINFO
#define IH264VENC_GETVERSION     XDM_GETVERSION  //XDM1.0

/* Example of extension of command ID, starting with 256 on ward  */
#define IH264VENC_PREPROCESS   256/* internal preprocssing for noise revomal */


//!< Picture Order Count Type Identifier for H.264 Encoder
typedef enum
{
  IH264_POC_TYPE_0 = 0,  //!< POC type 0
  IH264_POC_TYPE_2 = 2   //!< POC type 2

} IH264VENC_PicOrderCountType ;

//!< H264 Encoder Specific Error Code bits
typedef enum
{
  IH264VENC_SEQPARAMERR=0,          //!< Indicates error during sequence parameter set generation
  IH264VENC_PICPARAMERR,            //!< Indicates error during picture parameter set generation
  IH264VENC_COMPRESSEDSIZEOVERFLOW, //!< Compressed data exceeds the maximum compressed size limit
  IH264VENC_INVALIDQPPARAMETER,     //!< Out of Range initial Quantization parameter
  IH264VENC_INVALIDPROFILELEVEL,    //!< Invalid profile or Level
  IH264VENC_INVALIDRCALGO,          //!< Invalid RateControl Algorithm
  IH264VENC_INVALIDSEARCHRANGE,          //!< Invalid Search Range
  IH264VENC_SLICEEXCEEDSMAXBYTES,   //!< Slice exceeds the maximum allowed bytes
  IH264VENC_DEVICENOTREADY,         //!< Indicates the device is not ready
  IH264VENC_ERROR_NULLPOINTER,		//!< Indicates the invalid pointers been passed to the algorithm
  IH264VENC_ERROR_INVALIDSTRUCTSIZE //!< Indicates the invalid structure size been passed to the algorithm

} IH264VENC_ErrorBit;

/*
// ===========================================================================
// IH264VENC_Params
//
// This structure defines the creation parameters for all h264enc objects
*/
typedef struct IH264VENC_Params
{

  /*!
   * Mandatory fields of the Params structure - Base class
   */
  IVIDENC1_Params videncParams;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
 XDAS_Int32      profileIdc;     //!< profile idc
  IH264VENC_Level levelIdc;       //!< level idc
  XDAS_Int32      rcAlgo;         //!< Algorithm to be used by Rate Control Scheme Range[0,1]
  XDAS_Int32      searchRange;    //!< search range - integer pel search and 16x16 blocks.  The search window is
                                  //!< generally around the predicted vector. Max vector is 2xmcrange.  For 8x8
                                  //!< and 4x4 block sizes the search range is 1/2 of that for 16x16 blocks
} IH264VENC_Params;

/*
// ===========================================================================
// IH264VENC_PARAMS
//
// Default parameter values for h264enc instance objects
*/
extern IH264VENC_Params IH264VENC_PARAMS;

/*
 *  ======== IVIDENC_InArgs ========
 *  This structure defines the run time arguments for all VIDENC objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IH264VENC_InArgs
{
  /*!
   * Mandatory fields of the InArgs structure - Base class
   */
  IVIDENC1_InArgs   videncInArgs;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */

}IH264VENC_InArgs;

/*
// ===========================================================================
// IH264VENC_DynamicParams
//
// This structure defines the run time parameters for all h264enc objects
*/
typedef struct IH264VENC_DynamicParams
{
  /*!
   * Mandatory fields of the DynamicParams structure - Base class
   */
  IVIDENC1_DynamicParams videncDynamicParams;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */
 XDAS_Int32      qpIntra                   ; //!< initial QP of I frames Range[0,51]
  XDAS_Int32      qpInter                   ; //!< initial QP of P frames Range[0,51]
  XDAS_Int32      qpMax                     ; //!< Maximum QP to be used  Range[0,51]
  XDAS_Int32      qpMin                     ; //!< Minimum QP to be used  Range[0,51]
  XDAS_Int32      lfDisableIdc              ; //!< See IH264VENC_LoopFilterParams for more details
  XDAS_Int32      quartPelDisable           ; //!< enable/disable Quarter Pel Interpolation
  XDAS_Int32      airMbPeriod               ; //!< Adaptive Intra Refesh MB Period
  XDAS_Int32      maxMBsPerSlice            ; //!< Maximum number of macro block in a slice <minimum value is 8>
  XDAS_Int32      maxBytesPerSlice          ; //!< Maximum number of bytes in a slice
  XDAS_Int32      sliceRefreshRowStartNumber; //!< Row number from which slice needs to be intra coded
  XDAS_Int32      sliceRefreshRowNumber     ; //!< Number of rows to be coded as intra slice
  XDAS_Int32      filterOffsetA             ; //!< alpha offset for loop filter [-12, 12] even number
  XDAS_Int32      filterOffsetB             ; //!< beta offset for loop filter [-12, 12] even number
  XDAS_Int32      log2MaxFNumMinus4         ; //!< Limits the maximum frame number in the bit-stream to (1<< (log2MaxFNumMinus4 + 4)) Range[0,12]
  XDAS_Int32      chromaQPIndexOffset       ; //!< Specifies offset to be added to luma QP for addressing QPC values table for chroma components. Valid value is between -12 and 12, (inclusive)
  XDAS_Int32      constrainedIntraPredEnable; //!< Controls the intra macroblock coding in P slices [0,1]
  XDAS_Int32      picOrderCountType         ; //!< Picture Order count type Valid values 0, 2
  XDAS_Int32      mvDataEnable              ; //MVEXPOSE!< enable/Disable Motion vector access

} IH264VENC_DynamicParams;

extern IH264VENC_DynamicParams H264VENC_TI_DYNAMICPARAMS;

/*
 *  ======== IH264VENC_OutArgs ========
 *  This structure defines the run time output arguments for IH264VENC
 *  instance objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */



typedef struct IH264VENC_OutArgs
{
  /*!
   * Mandatory fields of the OutArgs structure - Base class
   */
  IVIDENC1_OutArgs  videncOutArgs;

  /*!
   * Additional elements specific to H.264 Encoder - Extensions to base class
   */

} IH264VENC_OutArgs;

/*
// ===========================================================================
// IH264VENC_Fxns
//
// This structure defines all of the operations on h264enc objects
*/
typedef struct IH264VENC_Fxns
{
  /*!
   * Mandatory functions of the function table - Base class
   */
    IVIDENC1_Fxns  ividenc;

  /*!
   * Additional functions specific to H.264 Encoder - Extensions to base class
   */

} IH264VENC_Fxns;

#ifdef __cplusplus
}
#endif

#endif  /* _IH264VENC_ */
