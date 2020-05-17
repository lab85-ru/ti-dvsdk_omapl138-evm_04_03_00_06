/*
 *  Copyright 2005 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== ividenc.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the video encoder algorithms.
 *  Version 0.6
 */
#ifndef IVIDENC_
#define IVIDENC_

#include <ialg.h>
#include <xdas.h>
#include "xdm.h"
#include "ivideo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IVIDENC_CID      "ti.xdais.xdm.IVIDENC"

#define IVIDENC_ERUNTIME  XDM_ERUNTIME
#define IVIDENC_EOK       XDM_EOK
#define IVIDENC_EFAIL     XDM_EFAIL

/*
 *  ======== IVIDENC_Obj ========
 *  This structure must be the first field of all VIDENC instance objects.
 */
typedef struct IVIDENC_Obj {
    struct IVIDENC_Fxns *fxns;
} IVIDENC_Obj;

/*
 *  ======== IVIDENC_Handle ========
 *  This handle is used to reference all VIDENC instance objects.
 */
typedef struct IVIDENC_Obj  *IVIDENC_Handle;

/*
 *  ======== IVIDENC_Params ========
 *  This structure defines the creation & run time parameters for all VIDENC
 *  instance objects.
 */
typedef struct IVIDENC_Params {
    XDAS_Int32 size;                 /*  size of this structure              */
    XDAS_Int32 encodingPreset;       /*  Encoding preset                     */
    XDAS_Int32 rateControlPreset;    /*  Rate control presets                */
    XDAS_Int32 maxHeight;            /*  Maximum video height to be supported*/
    XDAS_Int32 maxWidth;             /*  Maximum video width to be supported */
    XDAS_Int32 maxFrameRate ;        /*  Maximum Framerate * 1000 to be
                                      *  supported
                                      */
    XDAS_Int32 maxBitRate;           /*  Maximum Bitrate to be supported in
                                      *  bits per second
                                      */
    XDAS_Int32 dataEndianness;       /*  Endianness of output data. Refer to
                                      *  video data format enum above
                                      */
    XDAS_Int32 maxInterFrameInterval;/*  I to P frame distance. e.g. = 1 if
                                      *  no B frames, 2 to insert one B frame
                                      */
    XDAS_Int32 inputChromaFormat;    /*  Set to XDM_ChromaFormat type.
                                      *  Set to DEFAULT to avoid re-sampling.
                                      */
    XDAS_Int32 inputContentType;     /*  XDM Content Type: IVIDEO_PROGRESSIVE,
                                      *  IVIDEO_INTERLACED
                                      */
}IVIDENC_Params;

/*
 *  ======== IVIDENC_DynamicParams ========
 *  This structure defines the codec parameters that can be modified after
 *  creation via 'control' calls.
 *
 *  It is not necessary that given implementation support all dynamic
 *  parameters to be configurable at run time. If a particular codec
 *  does not support run-time updates to a parameter that  the application
 *  is attempting to change at runtime, the codec may indicate this as an
 *  error.
 */
typedef struct IVIDENC_DynamicParams {
     XDAS_Int32    size;              /*  size of this structure             */
     XDAS_Int32    inputHeight;       /*  Input frame height                 */
     XDAS_Int32    inputWidth;        /*  Input frame width                  */
     XDAS_Int32    refFrameRate;      /*  Reference or input frame rate*1000 */
     XDAS_Int32    targetFrameRate;   /*  Target frame rate * 1000           */
     XDAS_Int32    targetBitRate;     /*  Target bit rate in bits per second */
     XDAS_Int32    intraFrameInterval;/*  I frame interval e.g. 30           */
     XDAS_Int32    generateHeader;    /*  XDM_ENCODE_AU, XDM_GENERATE_HEADER */
     XDAS_Int32    captureWidth;      /*  DEFAULT(0): use imagewidth as pitch
                                       *  else use given capture width for
                                       *  pitch provided it is greater than
                                       *  image width.
                                       */
     XDAS_Int32    forceIFrame;       /*  Force given frame as I or IDR (in H.264)
	                                      frame       */
}IVIDENC_DynamicParams;

/*
 *  ======== IVIDENC_InArgs ========
 *  This structure defines the run time arguments for all VIDENC objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IVIDENC_InArgs {
    XDAS_Int32 size;               /*  size of this structure                */
}IVIDENC_InArgs;


/*
 *  ======== IVIDENC_Status ========
 *  This structure defines instance status parameters (read-only).
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IVIDENC_Status {
     XDAS_Int32     size;           /*  size of this structure               */
     XDAS_Int32 extendedError; /* Extended Error code. (see XDM_ErrorBit) */
	 XDM_AlgBufInfo bufInfo;        /*  Input & output buffer information    */
} IVIDENC_Status;


/*
 *  ======== IVIDENC_OutArgs ========
 *  This structure defines the run time output arguments for all VIDENC
 *  instance objects.
 *  This structure may be extended by individual codec implementation
 *  allowing customization with vendor specific parameters.
 */
typedef struct IVIDENC_OutArgs {
    XDAS_Int32 size;               /*  size of this structure  */
    XDAS_Int32 extendedError;      /*  Extended Error code
                                    *  see XDM::XDM_ErrorBit for details
                                    */
    XDAS_Int32 bytesGenerated;     /*  The bytes Generated  */
    XDAS_Int32 encodedFrameType;   /*  Frame Types:
                                    *  IVIDEO_I_FRAME, IVIDEO_P_FRAME,
                                    *  IVIDEO_B_FRAME, IVIDEO_IDR_FRAME
                                    */
    XDAS_Int32 inputFrameSkip;     /*  IVIDEO_FRAME_ENCODED or
                                    *  IVIDEO_FRAME_SKIPPED
                                    */
    
    IVIDEO_BufDesc reconBufs;       /* pointer to reconstruction frames */
} IVIDENC_OutArgs;

/*
 *  ======== IVIDENC_Cmd ========
 *  This structure defines the control commands for the VIDENC module.
 *  This ID can be extended in IMOD interface for additional controls
 */
typedef  IALG_Cmd IVIDENC_Cmd;

/*
 *  ======== IVIDENC_Fxns ========
 *  This structure defines all of the operations on VIDENC objects.
 *
 *
 *  process() - Basic video encoding call. It takes input and output buffer
 *              descriptors and structures for input and output arguments.
 *              The algorithm may also modify the output buffer pointers.
 *              The return value is IALG_EOK for success or IALG_EFAIL
 *              in case of failure. The extendedError field of the outArgs
 *              contain further error conditions flagged by the algorithm.
 *
 *  control() - This will control behaviour of algorithm via Cmd ID.
 *              All XDM Control Commands are supported:
 *                  XDM_GETSTATUS =0, XDM_SETPARAMS, XDM_RESET, XDM_SETDEFAULT,
 *                  XDM_FLUSH, XDM_GETBUFINFO
 *              See xdm.h for details.
 *              The return value is IALG_EOK for success or IALG_EFAIL.
 *              In case of failure extended error code contains details.
 */
typedef struct IVIDENC_Fxns {
    IALG_Fxns      ialg;
    XDAS_Int32   (*process)(IVIDENC_Handle handle, XDM_BufDesc *inBufs,
            XDM_BufDesc *outBufs, IVIDENC_InArgs *inargs,
            IVIDENC_OutArgs *outargs);
    XDAS_Int32   (*control)(IVIDENC_Handle handle, IVIDENC_Cmd id,
            IVIDENC_DynamicParams *params, IVIDENC_Status *status);

} IVIDENC_Fxns;

#ifdef __cplusplus
}
#endif

#endif  /* IVIDENC_ */



