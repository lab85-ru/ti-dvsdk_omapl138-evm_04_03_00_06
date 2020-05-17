/*
//============================================================================
//
//    FILE NAME : ih264vdec.h
//
//    ALGORITHM : H264VDEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : IH264VDEC Interface Header
//
//============================================================================
*/
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


#ifndef IH264VDEC_
#define IH264VDEC_


#include <ti/bios/include/std.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec2.h>
#include <ti/xdais/idma3.h>


#define H264D_TI_MAXCPBCNT      32
#define H264D_TI_MB_NO_ERR      0
#define H264D_TI_MB_ERR_I       1
#define H264D_TI_MB_ERR_P       2
#define H264D_TI_MB_ERR_I_DP    3
#define H264D_TI_MB_ERR_P_DP    4

/* Defined for NAL array size */
#define H264D_TI_NUM_BYTES_IN_NAL_ARR    1200


typedef enum Profile{
    PROFILE_INVALID=-1,       //!<-1: Used to indicate unsupported profile
    BASELINE=0,              //!< 0: BASELINE PROFILE
    MAIN    =1,              //!< 1: MAIN PROFILE
	EXTENDED = 2,
    MAX_PROFILES_TYPES=3     //!< Maximum Number of Profile == 3
}eH264VDEC_Profile;

//! Level number
//! Level number
/****************************************/
/*Sreni: 16th Aug 06 : Added level 1b
****************************************/
typedef enum level_id {
	Level1 = 0,  //!<  0: Level 1
	Level11,     //!<  1: Level 1.1
	Level12,     //!<  2: Level 1.2
	Level13,     //!<  3: Level 1.3
	Level1b,     //!<  4: Level 1b
	Level2,      //!<  5: Level 2
	Level21,     //!<  6: Level 2.1
	Level22,     //!<  7: Level 2.2
	Level3,      //!<  8: Level 3
	Level31,     //!<  9: Level 3.1
	Level32,     //!< 10: Level 3.1
	Level4,      //!< 11: Level 4
	Level41,     //!< 12: Level 4.1
	Level42,     //!< 13: Level 4.2
	Level5,      //!< 14: Level 5
	Level51,     //!< 15: Level 5.1
	MAXLEVELID   //!< 16:
} eLevelNum_t;


typedef enum profile_id {
 Profile_Baseline = 0,  //!<  0: Baseline profile
 Profile_Main,          //!<  1: Main profile
 Profile_High,          //!<  2: High profile
 Profile_Any            //!<  3: As decoded from the bitstream
                        //!<     This is needed to pass compliance
} eProfile_t;
typedef struct  hrd_parm {
	unsigned int  cpb_cnt;                                          //!< ue(v)
	unsigned int  bit_rate_scale;                                   //!< u(4)
	unsigned int  cpb_size_scale;                                   //!< u(4)
	unsigned int  bit_rate_value [H264D_TI_MAXCPBCNT];                       //!< ue(v)
	unsigned int  cpb_size_value[H264D_TI_MAXCPBCNT];                        //!< ue(v)
	unsigned int  vbr_cbr_flag[H264D_TI_MAXCPBCNT];                          //!< u(1)
	unsigned int  initial_cpb_removal_delay_length_minus1;          //!< u(5)
	unsigned int  cpb_removal_delay_length_minus1;                  //!< u(5)
	unsigned int  dpb_output_delay_length_minus1;                   //!< u(5)
	unsigned int  time_offset_length;                               //!< u(5)
} sHrdParm_t;

typedef struct  vui_seq_parm {
    unsigned int  parsed_flag;
  unsigned int  aspect_ratio_info_present_flag;                    //!< u(1)
	unsigned int  aspect_ratio_idc;                                  //!< u(8)
	unsigned int  sar_width;                                         //!< u(16)
	unsigned int  sar_height;                                        //!< u(16)
  unsigned int  overscan_info_present_flag;                        //!< u(1)
	unsigned int  overscan_appropriate_flag;                         //!< u(1)
  unsigned int  video_signal_type_present_flag;                    //!< u(1)
	unsigned int  video_format;                                      //!< u(3)
	unsigned int  video_full_range_flag;                             //!< u(1)
	unsigned int  colour_description_present_flag;                   //!< u(1)
	unsigned int  colour_primaries;                                  //!< u(8)
	unsigned int  transfer_characteristics;                          //!< u(8)
	unsigned int  matrix_coefficients;                               //!< u(8)
  unsigned int  chroma_location_info_present_flag;                 //!< u(1)
	unsigned int  chroma_sample_loc_type_top_field;                  //!< ue(v)
	unsigned int  chroma_sample_loc_type_bottom_field;               //!< ue(v)
  unsigned int  timing_info_present_flag;                          //!< u(1)
	unsigned int  num_units_in_tick;                                 //!< u(32)
	unsigned int  time_scale;                                        //!< u(32)
	unsigned int  fixed_frame_rate_flag;                             //!< u(1)
  unsigned int  nal_hrd_parameters_present_flag;                   //!< u(1)
	sHrdParm_t nal_hrd_parameters;                              //!< hrd_paramters_t
  unsigned int  vcl_hrd_parameters_present_flag;                   //!< u(1)
	sHrdParm_t vcl_hrd_parameters;                              //!< hrd_paramters_t
	unsigned int  low_delay_hrd_flag;                                //!< u(1)
  unsigned int  pic_struct_present_flag;                           //!< u(1)
  unsigned int  bitstream_restriction_flag;                        //!< u(1)
	unsigned int  motion_vectors_over_pic_boundaries_flag;           //!< u(1)
	unsigned int  max_bytes_per_pic_denom;                           //!< ue(v)
	unsigned int  max_bits_per_mb_denom;                             //!< ue(v)
	unsigned int  log2_max_mv_length_vertical;                       //!< ue(v)
	unsigned int  log2_max_mv_length_horizontal;                     //!< ue(v)
	unsigned int  num_reorder_frames;                                //!< ue(v)
	unsigned int  max_dec_frame_buffering;                           //!< ue(v)
} sVSP_t;


//#ifdef SEI_SUPPORT

typedef struct prog_refinement_start{
    unsigned int parsed_flag;
    unsigned int progressive_refinement_id;
    unsigned int num_refinement_steps_minus1;
} sProgRefineStart_t;

typedef struct prog_refinement_end{
    unsigned int parsed_flag;
    unsigned int progressive_refinement_id;
} sProgRefineEnd_t;

typedef struct recovery_point_info {
  unsigned int  parsed_flag;
  unsigned int  recovery_frame_cnt;
  unsigned char   exact_match_flag;
  unsigned char   broken_link_flag;
  unsigned char   changing_slice_group_idc;
} sRecoveryPointInfo_t;

typedef struct picture_timing_SEI{
   unsigned int parsed_flag;
   unsigned int cpb_removal_delay;
   unsigned int dpb_output_delay;
   unsigned int pic_struct;
   unsigned int clock_timestamp_flag;
   unsigned int ct_type;
   unsigned int nuit_field_based_flag;
   unsigned int counting_type;
   unsigned int full_timestamp_flag;
   unsigned int discontinuity_flag;
   unsigned int cnt_dropped_flag;
   unsigned int n_frames;
   unsigned int seconds_value;
   unsigned int minutes_value;
   unsigned int hours_value;
   unsigned int time_offset;
} sPictureTiming_t;

typedef struct frame_freeze_repetetion{
   unsigned int parsed_flag;
   unsigned int full_frame_freeze_repetition_period;
} sFullFrameFreezeRepetition_t;

typedef struct frame_freeze_release{
   unsigned int parsed_flag;
   unsigned char  full_frame_freeze_release_flag;
} sFullFrameFreezeRelease_t;

typedef struct sei_messages {
  unsigned int parsed_flag;
  sFullFrameFreezeRepetition_t frame_freeze_repetition;
  sFullFrameFreezeRelease_t frame_freeze_release;
  sProgRefineStart_t prog_refine_start;
  sProgRefineEnd_t   prog_refine_end;
  sRecoveryPointInfo_t recovery_pt_info;
  sPictureTiming_t   pic_timing;
} sSeiMessages_t;

//#endif /* SEI_SUPPORT */

typedef struct  sei_vui_parm {
   unsigned int parsed_flag;
   sVSP_t vui_params;
   sSeiMessages_t sei_messages;
} sSeiVuiParams_t;



/*
 *  ======== IH264VDEC_Handle ========
 *  This handle is used to reference all H264VDEC instance objects
 */
typedef struct IH264VDEC_Obj *IH264VDEC_Handle;

/*
 *  ======== IH264VDEC_Obj ========
 *  This structure must be the first field of all H264VDEC instance objects
 */
typedef struct IH264VDEC_Obj {
    struct IH264VDEC_Fxns *fxns;
} IH264VDEC_Obj;


/*
 *  ======== IH264VDEC_Cmd ========
 *  The Cmd enumeration defines the control commands for the H264VDEC
 *  control method.
 */

typedef IVIDDEC2_Cmd IH264VDEC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define IH264VDEC_GETSTATUS      XDM_GETSTATUS
#define IH264VDEC_SETPARAMS      XDM_SETPARAMS
#define IH264VDEC_RESET          XDM_RESET
#define IH264VDEC_FLUSH          XDM_FLUSH
#define IH264VDEC_SETDEFAULT     XDM_SETDEFAULT
#define IH264VDEC_GETBUFINFO     XDM_GETBUFINFO
#define IH264VDEC_GETVERSION        XDM_GETVERSION

/*
 *  ======== IH264VDEC_Params ========
 *  This structure defines the creation parameters for all H264VDEC objects
 */
typedef struct IH264VDEC_Params {
    IVIDDEC2_Params viddecParams; //Should be the first argument
	XDAS_Int32 inputStreamFormat; /*  0 - ByteStreamFormat
								   *  1 - NALUnitStreamFormat */
	/* Display delay will decide the initial delay before which decode call */
    /* starts  */
    XDAS_Int32   maxDisplayDelay;
} IH264VDEC_Params;

/*
 *  ======== IH264VDEC_PARAMS ========
 *  Default parameter values for H264VDEC instance objects
 */
extern IH264VDEC_Params IH264VDEC_PARAMS;

/*
 *  ======== IH264VDEC_Status ========
 *  Status structure defines the parameters that can be changed or read
 *  during real-time operation of the alogrithm.
 */
typedef struct IH264VDEC_Status {
    IVIDDEC2_Status viddecStatus;   /* Should be the first argument */
	eH264VDEC_Profile profile;     // Profile of the bitstream
	eLevelNum_t level;             // Level number of the Bitstream
	XDAS_Int32  Qp;                // Frame Quantization parameter

    /* --------------------------------------------------------------------
     * Ramesh 20 Sep 2006. Adding some fields to extended params status
     * structure on customer request.
     * For erroneous FMO bitstreams, this field may not be so enlightening.
	 *---------------------------------------------------------------------
     */
    XDAS_UInt32 last_decoded_mb_addr;
    XDAS_UInt32 slice_header_frame_num;
    XDAS_UInt32 full_frame_decoded;
    XDAS_Int32 poc_num;

	XDAS_Int32        display_frame_skip_flag;
	XDAS_Int32        numNALdecoded;

} IH264VDEC_Status;

/*
// ===========================================================================
// IH264VDEC_DynamicParams
//
// This structure defines the run time parameters for all H264DEC objects
*/
typedef struct IH264VDEC_DynamicParams {
    IVIDDEC2_DynamicParams viddecDynamicParams; //Should be the first argument
	XDAS_Int32        mbErrorBufFlag;
    XDAS_UInt32       mbErrorBufSize;
    XDAS_Int32        Sei_Vui_parse_flag;
	XDAS_Int32        numNALunits;
} IH264VDEC_DynamicParams;


/*
// ===========================================================================
// IH264VDEC_InArgs
//
// This structure provides the Input parameters for H.264 Decode call
*/
typedef struct IH264VDEC_InArgs {
	IVIDDEC2_InArgs viddecInArgs; //Should be the first argument

	//XDAS_Int32      numNALunits;
	//XDAS_Int32      *numBytesInNALarr;

    //XDAS_Int32      maxDisplayDelay;

//#ifdef SEI_SUPPORT
    //XDAS_Int32      Sei_Vui_parse_flag;
    //sSeiVuiParams_t *SeiVui_buffer_ptr;
//#endif SEI_SUPPORT

// MB_ERROR_STAT
	//XDAS_Int32       mbErrorBufFlag;
    //XDAS_UInt32      mbErrorBufSize;
    //XDAS_UInt8      *mbErrorBufPtr;

} IH264VDEC_InArgs;

/*
// ===========================================================================
// IH264VDEC_OutArgs
//
// This structure provides the Output parameters for H.264 Decode call
*/



typedef struct IH264VDEC_OutArgs {

	IVIDDEC2_OutArgs viddecOutArgs; //Should be the first argument

//	XDAS_Int32 numNALdecoded;
//#ifdef SKIP_SUPPORT
//    XDAS_Int32 display_frame_skip_flag;
//#endif /* SKIP_SUPPORT */
//#ifdef SEI_SUPPORT
   // sSeiVuiParams_t *SeiVui_buffer_ptr;
//#endif SEI_SUPPORT

// MB_ERROR_STAT
    //XDAS_UInt8      *mbErrorBufPtr;
} IH264VDEC_OutArgs;

/*
 *  ======== IH264VDEC_Fxns ========
 *  This structure defines all of the operations on H264VDEC objects
 */
typedef struct IH264VDEC_Fxns {
	IVIDDEC2_Fxns ividdec;

} IH264VDEC_Fxns;







#endif    /* IH264VDEC_ */

/* ======================================================================== */
/* End of file : ih264vdec.h                                                */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

