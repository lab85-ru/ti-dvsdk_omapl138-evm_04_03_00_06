/*
 *  ======== iaacdecoder.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the DECODE algorithm.
 */
#ifndef IAACDECDECODER_DECODE_
#define IAACDECDECODER_DECODE_

#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/iauddec1.h>

/*
 *
 *  AAC Specific Error codes.
 */
#define AACDEC_TII_NO_ERROR                     0
#define AACDEC_TII_SYNC_NOT_FOUND               1
#define AACDEC_TII_NOT_ENOUGH_CH        		2
#define AACDEC_TII_PRED_DETECTED        		3
#define AACDEC_TII_LFE_DETECTED         		4
#define AACDEC_TII_GAIN_DETECTED        		5
#define AACDEC_TII_CCE_DETECTED         		6
#define AACDEC_TII_PULSE_NOT_ALLOWED    		7
#define AACDEC_TII_SFB_ERROR      		        8
#define AACDEC_TII_HUFFMAN_ERROR   		        9
#define AACDEC_TII_PROFILE_ERROR   		        10
#define AACDEC_TII_ELE_NOT_SUPPORTED    		11
#define AACDEC_TII_SAM_FREQ_NOT_SUPPORTED 		12
#define AACDEC_TII_ADTS_FRAME_LEN_INCORRECT 	13
#define AACDEC_TII_INSUFFICIENT_INPUT_DATA 		14
#define AACDEC_TII_INVALID_SIZE    			    15
#define AACDEC_TII_CH_ELE_COMBINATION_ERROR 	16
#define AACDEC_TII_INVALID_POINTER      		17
#define AACDEC_TII_CRC_MISMATCH_ERROR      		18
#define AACDEC_TII_INSTANCE_TAG_MISMATCH 		19
#define AACDEC_TII_INVALID_PARAMETER            20
#define AACDEC_TII_INVALID_SBR_STREAM           21


/*
 *  ======== IDECODE_Obj ========
 *  This structure must be the first field of all DECODE instance objects.
 */

typedef struct IAACDEC_Obj
{

    struct IAACDEC_Fxns *fxns;
}

IAACDEC_Obj;

/*
 *  ======== IAACDEC_Handle ========
 *  This handle is used to reference all DECODE instance objects.
 */

typedef struct IAACDEC_Obj  *IAACDEC_Handle;


/*
 *  ======== IAACDEC_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */

typedef struct IAACDEC_Status
{
    IAUDDEC1_Status  auddec_status; /* must be followed for audio decoder */

	XDAS_Int16 mpegID;   /* 0 - MPEG4, 1- MPEG2 */
	XDAS_Int16 original_copy ; /* biststream is original = 1  or copy = 0*/
	XDAS_Int16 copyright; /* Copyright protected or not 1-protected 0-not protected*/
	XDAS_Int16 toolsUsed ; /*  Bit4     Bit3  Bit2   Bit1   Bit0 */
                           /*  PULSE     PNS   TNS    IS     MS  */

	XDAS_Int16 pcePresent; /* Program Configuration element present or not */


    XDAS_Int32 sixChannelMode;
	XDAS_Int32 isARIBDownMixed;  /* 0 : Normal downmix, 1 : ARIB downmixed */
	XDAS_Int32 pseudoSurroundEnable; /* pseudo_surround_enable field in bitstream 0: absent 1: present*/
    XDAS_Int16 sbrPresent;/* SBR present or not */
    XDAS_Int16 psPresent;/* PS present or not */
}

IAACDEC_Status;

/*
// ===========================================================================
// IAACDEC_Cmd
//
// The Cmd enumeration defines the control commands for the AACDEC
// control method.
*/
typedef IAUDDEC1_Cmd IAACDEC_Cmd;


/*
// ===========================================================================
// control method commands
*/
#define IAACDEC_GETSTATUS  XDM_GETSTATUS
#define IAACDEC_SETPARAMS  XDM_SETPARAMS
#define IAACDEC_RESET      XDM_RESET
#define IAACDEC_SETDEFAULT XDM_SETDEFAULT
#define IAACDEC_FLUSH      XDM_FLUSH
#define IAACDEC_GETBUFINFO XDM_GETBUFINFO
#define IAACDEC_GETVERSION XDM_GETVERSION

/*
// ===========================================================================
// IAACDEC_Params
//
// This structure defines the creation parameters for all AACDEC objects
*/

typedef struct IAACDEC_Params
{
    IAUDDEC1_Params auddec_params; /* must be second element of creation params */
    XDAS_Int32 sixChannelMode; /* indicates the multichannel mode ON for 1*/

}

IAACDEC_Params;

/*
// ===========================================================================
// IAACDEC_PARAMS
//
// Default parameter values for AACDEC instance objects
*/
extern const IAACDEC_Params IAACDEC_PARAMS;


/*
// ===========================================================================
// IAACDEC_Params
//
// This structure defines the run time parameters for all AACDEC objects
*/

typedef struct IAACDEC_DynamicParams
{
    IAUDDEC1_DynamicParams auddec_dynamicparams;
    XDAS_Int32 enablePS;
    XDAS_Int32 ulSamplingRateIdx;
    XDAS_Int32 nProfile;
    XDAS_Int32 bRawFormat;

    XDAS_Int32 pseudoSurroundEnableFlag;
    /* Pseudo Surround Enable to use Set2 type of downmixing 0 or 1*/
	XDAS_Int32 enableARIBDownmix;					/* 1: ARIB downmix, 0: Normal downmix */

}

IAACDEC_DynamicParams;


typedef struct IAACDEC_InArgs
{
    IAUDDEC1_InArgs auddec_inArgs;
    /* must be second field of all params structures */
}

IAACDEC_InArgs;


typedef struct IAACDEC_OutArgs
{
    IAUDDEC1_OutArgs  auddec_outArgs;
    /* must be followed for audio decoder */
}

IAACDEC_OutArgs;

/*
 *  ======== IAACDEC_Fxns ========
 *  This structure defines all of the operations on DECODE objects.
 */

typedef struct IAACDEC_Fxns
{
    IAUDDEC1_Fxns iauddec; /* must be second element of objects */
}

IAACDEC_Fxns;

/*
 *  ======== AACDEC_TII_IAACDEC ========
 *  This structure defines TI's implementation of the IDECODE  interface
 *  for the AAC_TNI module.
 */
extern const IAACDEC_Fxns AACDEC_TII_IAACDEC;

#endif /* IDECODE_ */



