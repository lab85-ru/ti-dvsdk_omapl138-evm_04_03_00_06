/*
 * Fvid.h
 *
 * This file contains interface definition for a FVID interface which is
 * used by various application for using video drivers.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   Fvid.h
 *
 *  \brief  This file is used by the Application to call the GIO functions
 *
 */
#ifndef _FVID_H_
#define _FVID_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <gio.h>
#include <iom.h>

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/*
 * =========== command codes for IOM_Packet =============
 */

/** \brief IOM user defined command base address */
#define FVID_BASE          (IOM_USER)

/**
 *  \brief command for FVID_exchange to exchange buffers between Driver and
 *  Application
 */
#define FVID_EXCHANGE      (FVID_BASE + 0)

/**
 *  \brief command for FVID_queue to submit a video buffer back to video device
 *  driver
 */
#define FVID_QUEUE         (FVID_BASE + 1)

/**
 *  \brief command for FVID_dequeue to request the video device driver to give
 *  ownership of a data buffer
 */
#define FVID_DEQUEUE       (FVID_BASE + 2)

/**
 *  \brief command for FVID_allocBuffer to request the video device driver to
 *  allocate one data buffer
 */
#define FVID_ALLOC_BUFFER  (FVID_BASE + 3)

/**
 *  \brief command for FVID_freeBuffer to request the video device driver to
 *  free memory of given data buffer
 */
#define FVID_FREE_BUFFER   (FVID_BASE + 4)


/** \brief FVID Slice VBI Service: NONE                                       */
#define FVID_SLICE_VBI_SERVICES_NONE               0x0
/** \brief FVID Slice VBI Service: Wide screen signaling (WSS) for PAL        */
#define FVID_SLICE_VBI_SERVICES_WSS_PAL            0x1u
/** \brief FVID Slice VBI Service: Copy generation management system (CGMS)
 for NTSC                                                                     */
#define FVID_SLICE_VBI_SERVICES_CGMS_NTSC          0x2u
/** \brief FVID Slice VBI Service: Closed caption for NTSC                    */
#define FVID_SLICE_VBI_SERVICES_CC_NTSC            0x4u
/** \brief FVID Slice VBI Service: MAX                                        */
#define FVID_SLICE_VBI_SERVICES_MAX                3
/** \brief Maximum data size for FVID Slice VBI data in bytes                 */
#define FVID_SLICE_VBI_DATA_SIZE_BYTES_MAX         4


/** \brief Driver channel handle */
typedef GIO_Handle FVID_Handle;

/**
 *  \brief Interlaced frame.
 *  Interlaced frame has two fields in one frame
 */
typedef struct FVID_IFrame_t
{
    Char* y1;
    /**< Character pointer for field 1 Y data */
    Char* cb1;
    /**< Character pointer for field 1 CB data */
    Char* cr1;
    /**< Character pointer for field 1 CR data */
    Char* y2;
    /**< Character pointer for field 2 Y data */
    Char* cb2;
    /**< Character pointer for field 2 CB data */
    Char* cr2;
    /**< Character pointer for field 2 CR data */
}FVID_IFrame;

/**
 * \brief Definition of Progressive Frame
 *
 */
typedef struct FVID_PFrame_t
{
    Char* y;
    /**< Character pointer for frame Y data */
    Char* cb;
    /**< Character pointer for frame CB data */
    Char* cr;
    /**< Character pointer for frame CR data */
}FVID_PFrame;



/**
 *  \brief Definition of Slice VBI frame
 *  Structure used to enable set of sliced VBI services in decoder
 */
typedef struct FVID_SliceFrame_t
{
    Uint32                         fvidSliceServiceId;
    /**< Type of Slice service. Available values for this field are defined
     *   with FVID Slice VBI Service title in Fvid.h.                         */
    Uint8                          fvidField;
    /**< Field for which VBI data is required. 0: first field, 1: second field*/
    Uint8                          fvidData[FVID_SLICE_VBI_DATA_SIZE_BYTES_MAX];
    /**< Place holder for getting the slice VBI data.                         */
}FVID_SliceFrame;


/**
 * \brief Definition of Semi planar Frame
 *  Semi planar frame having two fields in one frame. For interlaced y1, y2, c1,
 *  and c2 are valid but for progressive only y1, and c1 are valid. The c data
 *  is CbCr packed.
 */
typedef struct FVID_SpFrame_t
{
    Uint8 *y1;
    /**< Pointer for top field Y data                                         */
    Uint8 *c1;
    /**< Pointer for top field CB/CR data                                     */
    Uint8 *y2;
    /**< Pointer for bottom field Y data. Not used for progressive format.    */
    Uint8 *c2;
    /**< Pointer for bottom field CB/CR data. Not used for progressive format.*/
}FVID_SpFrame;


/**
 * \brief Definition of Vbi Frame
 *
 */
typedef struct FVID_VbiFrame_t
{
    Uint8 *h1;
    /**< Pointer for top field RAW HANC data. Not used if RAW HANC data
    is not required */
    Uint8 *h2;
    /**< Pointer for bottom field RAW HANC data. Not used if RAW HANC data
    is not required */
    Uint8 *v1;
    /**< Pointer for top field RAW VANC data. Not used if RAW VANC data
    is not required */
    Uint8 *v2;
    /**< Pointer for bottom field RAW VANC data. Not used if RAW VANC data
    is not required */

    FVID_SliceFrame     *s1;
    /**< Slice VBI data structure for top field*/
    FVID_SliceFrame     *s2;
    /**< Slice VBI data structure for bottom field*/
}FVID_VbiFrame;

/**
 * \brief Definition of Raw frame (Interlaced). Could be RGB,
 *        monochrome or any type of data interleaved Y/C frame etc.
 *
 */
typedef struct FVID_RawIFrame_t
{
    Char* buf1;
    /**< Character pointer for field 1 */
    Char* buf2;
    /**< Character pointer for field 2 */
}FVID_RawIFrame;

/**
 * \brief Definition of Raw frame (Progressive). Could be RGB,
 *        monochrome or any type of data interleaved Y/C frame etc.
 *
 */
typedef struct FVID_RawPFrame_t
{
    Char* buf;
    /**< Character pointer for frame */
}FVID_RawPFrame;

/**
 *  \brief Enum for color format
 */
typedef enum FVID_colorFormat_t
{
    FVID_YCbCr422_INTERLEAVED = 0,
    FVID_YCbCr422_PLANAR,
    FVID_YCrCb422_INTERLEAVED,
    FVID_YCbCr422_SEMIPLANAR_UV,
    /* YCbCr4:2:2 YC Semi Planar(YUV422UVP) */
    FVID_RGB_888_INTERLEAVED,
    FVID_RGB565_INTERLEAVED,
    FVID_DVD_MODE,
    FVID_CLUT_INDEXED,
    FVID_ATTRIBUTE,
    FVID_BAYER_PATTERN,
    FVID_RAW_FORMAT,
    FVID_COLORFORMAT_INVALID
}FVID_colorFormat; /**< Typedef for enum FVID_colorFormat_t */

/**
 *  \brief Enum for storage format
 */
typedef enum FVID_storageFormat_t
{
    FVID_STORAGE_FORMAT_FRAME,
    FVID_STORAGE_FORMAT_FIELD
}FVID_storageFormat;


/**
 * \brief Enum for type of VBI service
 */
typedef enum FVID_vbiService_t
{
    FVID_VBI_SERVICE_NONE = 0x0,
    FVID_VBI_SERVICE_HBI = 0x1,
    FVID_VBI_SERVICE_RAW_VBI = 0x2,
    FVID_VBI_SERVICE_SLICE_VBI = 0x4
}FVID_vbiService;


/**
 *  \brief Enum for video interface
 */
typedef enum FVID_videoInterface_t
{
    FVID_VI_BT656_8BIT,
    /**< 8-bit BT.656 interface with embedded sync */
    FVID_VI_BT656_10BIT,
    /**< 10-bit BT.656 interface with embedded sync */
    FVID_VI_YC_8BIT_CS,
    /**< 8-bit YC interface with external control sync */
    FVID_VI_YC_10BIT_CS,
    /**< 10-bit YC interface with external control sync */
    FVID_VI_YC_16BIT_ES,
    /**< 16-bit YC interface with embedded sync */
    FVID_VI_YC_16BIT_CS,
    /**< 16-bit YC interface with external control sync */
    FVID_VI_RAW_8BIT_CS,
    /**< 8-bit RAW interface with external control sync */
    FVID_VI_RAW_10BIT_CS,
    /**< 10-bit RAW interface with external control sync */
    FVID_VI_RAW_16BIT_CS,
    /**< 16-bit RAW interface with external control sync */
    FVID_VIDEOINTERFACE_INVALID
}FVID_videoInterface;  /**< Typedef for enum FVID_videoInterface_t */

/**
 * \brief Enum for the Field Frame Modes
 *
 */
typedef enum FVID_FieldFrame_t
{
    FVID_FIELD_MODE = 0,
    /**< Interlaced Mode */
    FVID_FRAME_MODE
    /**< Progressive Mode */
}FVID_FieldFrame;  /**< Typedef for enum FVID_FieldFrame_t */

/**
 * \brief Enum for the Bits Per Pixel for different Module
 *
 */
typedef enum FVID_bitsPerPixel_t
{
    FVID_BPP_BITS1  = 1,
    FVID_BPP_BITS2  = 2,
    FVID_BPP_BITS4  = 4,
    FVID_BPP_BITS8  = 8,
    FVID_BPP_BITS10 = 10,
    FVID_BPP_BITS12 = 12,
    FVID_BPP_BITS16 = 16,
    FVID_BPP_BITS24 = 24
}FVID_bitsPerPixel;    /**< Typedef for enum FVID_bitsPerPixel_t */


/**
 * \brief FVID frame buffer descriptor
 */
typedef struct FVID_Frame_t
{
    QUE_Elem            queElement;
    /**< for queuing */

    union {
        FVID_IFrame     iFrm;
        /**< y/c frame buffer for interlaced mode         */
        FVID_PFrame     pFrm;
        /**< y/c frame buffer for progressive mode        */
        FVID_RawIFrame  riFrm;
        /**< raw frame buffer for interlaced mode         */
        FVID_RawPFrame  rpFrm;
        /**< raw frame buffer for progressive mode        */
        Ptr             frameBufferPtr;
        FVID_SpFrame    spFrm;
        /**< y/c frame buffer for semi planar data         */
    } frame;    /**< \brief union for frame type as used by driver */

    Uint32              timeStamp;
    /**< Time Stamp for captured or displayed frame */

    Uint32              pitch;
    /**< Pitch parameters for given plane */

    Uint32              lines;
    /**< Number of lines per frame */

    FVID_bitsPerPixel   bpp;
    /**< Number of bits per pixel */

    FVID_colorFormat    frameFormat;
    /**< Frame Color Format */

    FVID_storageFormat  storeFormat;
    /**< Storage Format */

    FVID_VbiFrame       vbiFrm;
    /**< VBI frame */
    
    FVID_vbiService     vbiService;
    /**< VBI Service */

    Ptr                 userParams;
    /**< In/Out Additional User Parameters per frame */

    Ptr                 misc;
    /**< For future use */
}FVID_Frame;   /**< Typedef for structure FVID_Frame_t */


/**
 *  \brief ======== FVID_control ========
 *  An application calls FVID_control to send device-specific control
 *  commands to the mini-driver.
 */
#define FVID_control(fvidChan, cmd, args) \
        (GIO_control(fvidChan, cmd, args))


/**
 *  \brief ======== FVID_create ========
 *  FVID_create() allocates and initializes an GIO_Obj structure. FVID_create()
 *  returns a non-NULL GIO_Handle object on success and NULL for failure.
 *  The 'name' parameter is used to find a matching name in the device
 *  table.  Associated IOM_Fxns table and params structure are then used
 *  to create a channel for that device.   The 'attrs->nPackets' parameter
 *  specifies the number of asynchronous I/O calls that can be outstanding.
 *  The status field is the address where a device status is returned, ignored
 *  if status is specified as NULL.
 */
#define FVID_create(name, mode, status, optArgs, attrs) \
            (GIO_create(name, mode, status, optArgs, attrs))


/**
 *  \brief ======== FVID_delete ========
 *  Application calls FVID_delete to close the logical channel
 *  associated with fvidChan.
 */
#define FVID_delete(fvidChan) \
            (GIO_delete(fvidChan))

/**
 *  \brief ======== FVID_exchange ========
 *  Exchange one driver-managed buffer for another driver-managed buffer.
 *  This operation is similar to an FVID_queue()/dequeue() pair but has less
 *  overhead since it involves only one call into the driver.
 */
#define FVID_exchange(fvidChan, bufp) \
            (GIO_submit(fvidChan, FVID_EXCHANGE, bufp, NULL, NULL))

/**
 *  \brief ======== FVID_allocBuffer ========
 *  \brief  An application calls FVID_allocBuffer to request the video device
 *  driver to allocate one data buffer.
 */
#define FVID_allocBuffer(fvidChan, bufp) \
            (GIO_control(fvidChan, FVID_ALLOC_BUFFER, bufp))

/**
 *  \brief ======== FVID_freeBuffer ========
 *  An application calls FVID_freeBuffer to request the video device
 *  driver to free memory of given data buffer.
 *  FVID_freeBuffer() frees the buffer allocated via FVID_allocBuffer().
 */
#define FVID_freeBuffer(fvidChan, bufp) \
            (GIO_control(fvidChan, FVID_FREE_BUFFER, bufp))

/**
 *  \brief ======== FVID_queue ========
 *  An application calls FVID_queue to submit a video buffer to the video
 *  device driver.
 */
#define FVID_queue(fvidChan,bufp) \
            (GIO_submit(fvidChan,FVID_QUEUE,bufp,NULL,NULL))

/**
 *  \brief ======== FVID_dequeue ========
 *  An application calls FVID_dequeue to request the video device
 *  driver to give ownership of a data buffer.
 */
#define FVID_dequeue(fvidChan,bufp) \
            (GIO_submit(fvidChan,FVID_DEQUEUE,bufp,NULL,NULL))

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _FVID_H_ */

/******************************************************************************/
/*  End of file                                                               */
/******************************************************************************/
