/*
 * gsttividenc1.c
 *
 * This file defines the "TIVidenc1" element, which encodes an xDM 1.x video
 * stream.
 *
 * Example usage:
 *     gst-launch videotestsrc num-buffers=10 !
 *         TIVidenc1 engineName="<engine name>" codecName="<codecName>" !
 *         fakesink silent=TRUE
 *
 * Original Author:
 *     Brijesh Singh, Texas Instruments, Inc.
 *
 * Copyright (C) 2008-2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed #as is# WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <gst/gst.h>

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/BufferGfx.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/ce/Venc1.h>
#include <ti/sdo/dmai/Cpu.h>
#include <ti/sdo/dmai/Ccv.h>
#include <ti/sdo/dmai/ColorSpace.h>

#include "gsttividenc1.h"
#include "gsttidmaibuffertransport.h"
#include "gstticodecs.h"
#include "gsttithreadprops.h"
#include "gstticommonutils.h"
#include "gsttiquicktime_h264.h"

/* Declare variable used to categorize GST_LOG output */
GST_DEBUG_CATEGORY_STATIC (gst_tividenc1_debug);
#define GST_CAT_DEFAULT gst_tividenc1_debug

#define DEFAULT_BIT_RATE 2000000
#define INVALID_DEVICE   Cpu_Device_COUNT

/* Element property identifiers */
enum
{
  PROP_0,
  PROP_ENGINE_NAME,     /* engineName     (string)  */
  PROP_CODEC_NAME,      /* codecName      (string)  */
  PROP_RESOLUTION,      /* resolution     (string)  */
  PROP_BITRATE,         /* bitrate        (int)     */
  PROP_IN_COLORSPACE,   /* iColorSpace    (string)  */
  PROP_NUM_OUTPUT_BUFS, /* numOutputBuf   (int)     */
  PROP_FRAMERATE,       /* framerate      (GstFraction) */
  PROP_CONTIG_INPUT_BUF,/* contiguousInputFrame  (boolean) */
  PROP_GEN_TIMESTAMPS,  /* genTimeStamps  (boolean) */
  PROP_RATE_CTRL_PRESET,/* rateControlPreset  (gint) */
  PROP_ENCODING_PRESET, /* encodingPreset  (gint) */
  PROP_BYTE_STREAM      /* byteStream      (gboolean) */

};

/* Define source (output) pad capabilities.  Currently, MPEG2/4 and H264 are 
 * supported.
 */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE(
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS
    ("video/mpeg, " 
     "mpegversion=(int){ 2, 4 }, "  /* MPEG versions 2 and 4 */
         "systemstream=(boolean)false, "
         "framerate=(fraction)[ 0, MAX ], "
         "width=(int)[ 1, MAX ], "
         "height=(int)[ 1, MAX ] ;"
     "video/x-h264, "                             /* H264                  */
         "framerate=(fraction)[ 0, MAX ], "
         "width=(int)[ 1, MAX ], "
         "height=(int)[ 1, MAX ]"
    )
);

/* Declare supperted sink pad capabilities. */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE(
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS
    ("video/x-raw-yuv, "                        /* UYVY - YUV422 interleaved */
         "format=(fourcc)UYVY, "                
         "framerate=(fraction)[ 0, MAX ], "
         "width=(int)[ 1, MAX ], "
         "height=(int)[ 1, MAX ];"
     "video/x-raw-yuv, "                        /* NV12 - YUV420 semi planar */
         "format=(fourcc)NV12, "               
         "framerate=(fraction)[ 0, MAX ], "
         "width=(int)[ 1, MAX ], "
         "height=(int)[ 1, MAX ]"
    )
);

/* Declare a global pointer to our element base class */
static GstElementClass *parent_class = NULL;

/* Static Function Declarations */
static void
 gst_tividenc1_base_init(gpointer g_class);
static void
 gst_tividenc1_class_init(GstTIVidenc1Class *g_class);
static void
 gst_tividenc1_init(GstTIVidenc1 *object, GstTIVidenc1Class *g_class);
static void
 gst_tividenc1_set_property (GObject *object, guint prop_id,
     const GValue *value, GParamSpec *pspec);
static void
 gst_tividenc1_get_property (GObject *object, guint prop_id, GValue *value,
     GParamSpec *pspec);
static gboolean
 gst_tividenc1_set_sink_caps(GstPad *pad, GstCaps *caps);
static gboolean
 gst_tividenc1_set_source_caps(GstTIVidenc1 *videnc1, Buffer_Handle hBuf);
static gboolean
 gst_tividenc1_sink_event(GstPad *pad, GstEvent *event);
static GstFlowReturn
 gst_tividenc1_chain(GstPad *pad, GstBuffer *buf);
static gboolean
 gst_tividenc1_init_video(GstTIVidenc1 *videnc1);
static gboolean
 gst_tividenc1_exit_video(GstTIVidenc1 *videnc1);
static GstStateChangeReturn
 gst_tividenc1_change_state(GstElement *element, GstStateChange transition);
static void
 gst_tividenc1_populate_codec_header (GstTIVidenc1 *videnc1,
     Buffer_Handle hDstBuf);
static GstFlowReturn
 gst_tividenc1_parse_and_push (GstTIVidenc1 *videnc1, GstBuffer *outBuf);
static GstFlowReturn
 gst_tividenc1_encode(GstTIVidenc1 *videnc1, GstBuffer *inBuf,
     GstBuffer **outBuf);
static GstClockTime
 gst_tividenc1_frame_duration(GstTIVidenc1 *videnc1);
static ColorSpace_Type 
 gst_tividenc1_find_colorSpace (const gchar *colorSpace);
static gboolean
 gst_tividenc1_codec_start (GstTIVidenc1 *videnc1);
static gboolean
 gst_tividenc1_codec_stop (GstTIVidenc1 *videnc1);

/******************************************************************************
 * gst_tividenc1_class_init_trampoline
 *    Boiler-plate function auto-generated by "make_element" script.
 ******************************************************************************/
static void gst_tividenc1_class_init_trampoline(gpointer g_class, gpointer data)
{
    parent_class = (GstElementClass*) g_type_class_peek_parent(g_class);
    gst_tividenc1_class_init((GstTIVidenc1Class*)g_class);
}

/******************************************************************************
 * gst_tividenc1_get_type
 *    Boiler-plate function auto-generated by "make_element" script.
 *    Defines function pointers for initialization routines for this element.
 ******************************************************************************/
GType gst_tividenc1_get_type(void)
{
    static GType object_type = 0;

    if (G_UNLIKELY(object_type == 0)) {
        static const GTypeInfo object_info = {
            sizeof(GstTIVidenc1Class),
            gst_tividenc1_base_init,
            NULL,
            gst_tividenc1_class_init_trampoline,
            NULL,
            NULL,
            sizeof(GstTIVidenc1),
            0,
            (GInstanceInitFunc) gst_tividenc1_init
        };

        object_type = g_type_register_static((gst_element_get_type()),
                          "GstTIVidenc1", &object_info, (GTypeFlags)0);

        /* Initialize GST_LOG for this object */
        GST_DEBUG_CATEGORY_INIT(gst_tividenc1_debug, "TIVidenc1", 0,
            "TI xDM 1.x Video Encoder");

        GST_LOG("initialized get_type\n");
    }

    return object_type;
};

/******************************************************************************
 * gst_tividenc1_base_init
 *    Boiler-plate function auto-generated by "make_element" script.
 *    Initializes element base class.
 ******************************************************************************/
static void gst_tividenc1_base_init(gpointer gclass)
{
    static GstElementDetails element_details = {
        "TI xDM 1.x Video Encoder",
        "Codec/Encoder/Video",
        "Encodes video using an xDM 1.x-based codec",
        "Brijesh Singh; Texas Instruments, Inc."
    };

    GstElementClass *element_class = GST_ELEMENT_CLASS(gclass);

    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get (&src_factory));
    gst_element_class_add_pad_template(element_class,
        gst_static_pad_template_get (&sink_factory));
    gst_element_class_set_details(element_class, &element_details);

}

/******************************************************************************
 * gst_tividenc1_class_init
 *    Boiler-plate function auto-generated by "make_element" script.
 *    Initializes the TIVidenc1 class.
 ******************************************************************************/
static void gst_tividenc1_class_init(GstTIVidenc1Class *klass)
{
    GObjectClass    *gobject_class;
    GstElementClass *gstelement_class;

    gobject_class    = (GObjectClass*)    klass;
    gstelement_class = (GstElementClass*) klass;

    gobject_class->set_property = gst_tividenc1_set_property;
    gobject_class->get_property = gst_tividenc1_get_property;

    gstelement_class->change_state = gst_tividenc1_change_state;

    g_object_class_install_property(gobject_class, PROP_ENGINE_NAME,
        g_param_spec_string("engineName", "Engine Name",
            "Engine name used by Codec Engine", "unspecified",
            G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_CODEC_NAME,
        g_param_spec_string("codecName", "Codec Name", "Name of video codec",
            "unspecified", G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_RESOLUTION,
        g_param_spec_string("resolution", "Input resolution", 
            "Input resolution for the input video ('width''x''height')",
            "720x480", G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_IN_COLORSPACE,
        g_param_spec_string("iColorSpace", "Input colorspace",
            "Input color space (UYVY, Y8C8, NV16 or NV12)",
            "unspecified", G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_BITRATE,
        g_param_spec_int("bitRate",
            "encoder bitrate",
            "Set the video encoder bit rate",
            1, G_MAXINT32, DEFAULT_BIT_RATE, G_PARAM_WRITABLE));

    g_object_class_install_property(gobject_class, PROP_RATE_CTRL_PRESET,
        g_param_spec_int("rateControlPreset",
            "Rate control",
            "Bit rate control"
            "\n\t\t\t1 - No rate control"
            "\n\t\t\t2 - Constant bit rate (CBR)"
            "\n\t\t\t3 - Variable bit rate (VBR)",
            1, G_MAXINT32, 1, G_PARAM_WRITABLE));

    g_object_class_install_property(gobject_class, PROP_ENCODING_PRESET,
        g_param_spec_int("encodingPreset",
            "Encoding preset",
            "Algorithm specific creation time parameters"
            "\n\t\t\t1 - Use codec default"
            "\n\t\t\t2 - High quality"
            "\n\t\t\t3 - High speed",
            1, G_MAXINT32, 1, G_PARAM_WRITABLE));

    g_object_class_install_property(gobject_class, PROP_FRAMERATE,
        gst_param_spec_fraction("framerate", "frame rate of video",
            "Frame rate of the video expressed as a fraction.  A value "
            "of 0/1 indicates the framerate is not specified", 0, 1,
            G_MAXINT, 1, 0, 1, G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_BYTE_STREAM,
        g_param_spec_boolean("byteStream", "byte stream",
            "Generate byte stream format of NALU",
            TRUE, G_PARAM_WRITABLE));

    g_object_class_install_property(gobject_class, PROP_CONTIG_INPUT_BUF,
        g_param_spec_boolean("contiguousInputFrame", "Contiguous Input frame",
            "Set this if elemenet recieved contiguous input frame",
            FALSE, G_PARAM_WRITABLE));

    g_object_class_install_property(gobject_class, PROP_GEN_TIMESTAMPS,
        g_param_spec_boolean("genTimeStamps", "Generate Time Stamps",
            "Set timestamps on output buffers",
            TRUE, G_PARAM_WRITABLE));
}

/******************************************************************************
 * gst_tividenc1_init
 *    Initializes a new element instance, instantiates pads and sets the pad
 *    callback functions.
 ******************************************************************************/
static void gst_tividenc1_init(GstTIVidenc1 *videnc1, GstTIVidenc1Class *gclass)
{
    /* Instantiate encoded video sink pad.
     *
     * Fixate on our static template caps instead of writing a getcaps
     * function, which is overkill for this element.
     */
    videnc1->sinkpad =
        gst_pad_new_from_static_template(&sink_factory, "sink");
    gst_pad_set_setcaps_function(
        videnc1->sinkpad, GST_DEBUG_FUNCPTR(gst_tividenc1_set_sink_caps));
    gst_pad_set_event_function(
        videnc1->sinkpad, GST_DEBUG_FUNCPTR(gst_tividenc1_sink_event));
    gst_pad_set_chain_function(
        videnc1->sinkpad, GST_DEBUG_FUNCPTR(gst_tividenc1_chain));
    gst_pad_fixate_caps(videnc1->sinkpad,
        gst_caps_make_writable(
            gst_caps_copy(gst_pad_get_pad_template_caps(videnc1->sinkpad))));

    /* Instantiate deceoded video source pad.
     *
     * Fixate on our static template caps instead of writing a getcaps
     * function, which is overkill for this element.
     */
    videnc1->srcpad =
        gst_pad_new_from_static_template(&src_factory, "src");
    gst_pad_fixate_caps(videnc1->srcpad,
        gst_caps_make_writable(
            gst_caps_copy(gst_pad_get_pad_template_caps(videnc1->srcpad))));

    /* Add pads to TIVidenc1 element */
    gst_element_add_pad(GST_ELEMENT(videnc1), videnc1->sinkpad);
    gst_element_add_pad(GST_ELEMENT(videnc1), videnc1->srcpad);

    /* Determine target board type */
    if (Cpu_getDevice(NULL, &videnc1->device) < 0) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("Failed to determine target board\n"), (NULL));
        videnc1->device = INVALID_DEVICE;
    }

    /* Initialize TIVidenc1 state */
    videnc1->engineName             = NULL;
    videnc1->codecName              = NULL;
    videnc1->genTimeStamps          = TRUE;

    videnc1->hEngine                = NULL;
    videnc1->hVe1                   = NULL;

    videnc1->sinkAdapter            = NULL;
    videnc1->inBufMetadata          = NULL;
    videnc1->hEncOutBuf             = NULL;
    videnc1->hContigInBuf           = NULL;
    videnc1->hInBufRef              = NULL;
    videnc1->zeroCopyEncode         = FALSE;

    videnc1->width                  = 0;
    videnc1->height                 = 0;
    videnc1->bitRate                = -1;
    videnc1->colorSpace             = ColorSpace_NOTSET;

    videnc1->upstreamBufSize        = -1;
    videnc1->frameDuration          = GST_CLOCK_TIME_NONE;
    videnc1->hCcv                   = NULL;
    videnc1->hFc                    = NULL;
    videnc1->rateControlPreset      = 1;
    videnc1->contiguousInputFrame   = FALSE;
    videnc1->encodingPreset         = 1;
    videnc1->byteStream             = TRUE;
    videnc1->codec_data             = NULL;

    /* Initialize GValue members */
    memset(&videnc1->framerate, 0, sizeof(GValue));
    g_value_init(&videnc1->framerate, GST_TYPE_FRACTION);
    g_assert(GST_VALUE_HOLDS_FRACTION(&videnc1->framerate));
    gst_value_set_fraction(&videnc1->framerate, 0, 1);

}

/******************************************************************************
 * gst_tividenc1_find_colorSpace
 *****************************************************************************/
static ColorSpace_Type gst_tividenc1_find_colorSpace (const gchar *colorSpace)
{
    if (!strcmp(colorSpace, "UYVY"))
        return ColorSpace_UYVY;
    else if (!strcmp(colorSpace, "NV12")) 
        return ColorSpace_YUV420PSEMI;
    else 
        return ColorSpace_NOTSET; 
}

/******************************************************************************
 * gst_tividenc1_set_property
 *     Set element properties when requested.
 ******************************************************************************/
static void gst_tividenc1_set_property(GObject *object, guint prop_id,
                const GValue *value, GParamSpec *pspec)
{
    GstTIVidenc1 *videnc1 = GST_TIVIDENC1(object);

    GST_LOG("begin set_property\n");

    switch (prop_id) {
        case PROP_IN_COLORSPACE:
            if (videnc1->iColor) {
                g_free((gpointer)videnc1->iColor);
            }
            videnc1->iColor =
                (gchar*)g_malloc(strlen(g_value_get_string(value)) + 1);
            strcpy((gchar *)videnc1->iColor, g_value_get_string(value));
            videnc1->colorSpace =
                 gst_tividenc1_find_colorSpace(g_value_get_string(value));
            GST_LOG("setting \"iColorSpace\" to \"%d\"\n",videnc1->colorSpace);
            break; 
        case PROP_RESOLUTION:
            if (videnc1->resolution) {
                g_free((gpointer)videnc1->resolution);
            }
            videnc1->resolution =
                (gchar*)g_malloc(strlen(g_value_get_string(value)) + 1);
            strcpy((gchar *)videnc1->resolution, g_value_get_string(value));
            sscanf(g_value_get_string(value), "%dx%d", &videnc1->width,
                        &videnc1->height);
            GST_LOG("setting \"resolution\" to \"%dx%d\"\n", videnc1->width,
                        videnc1->height);
            break;
        case PROP_BITRATE:
            videnc1->bitRate =  g_value_get_int(value);
            GST_LOG("setting \"bitRate\" to \"%d\" \n", videnc1->bitRate);
            break; 
        case PROP_ENGINE_NAME:
            if (videnc1->engineName) {
                g_free((gpointer)videnc1->engineName);
            }
            videnc1->engineName =
                (gchar*)g_malloc(strlen(g_value_get_string(value)) + 1);
            strcpy((gchar *)videnc1->engineName, g_value_get_string(value));
            GST_LOG("setting \"engineName\" to \"%s\"\n", videnc1->engineName);
            break;
        case PROP_CODEC_NAME:
            if (videnc1->codecName) {
                g_free((gpointer)videnc1->codecName);
            }
            videnc1->codecName =
                (gchar*)g_malloc(strlen(g_value_get_string(value)) + 1);
            strcpy((gchar*)videnc1->codecName, g_value_get_string(value));
            GST_LOG("setting \"codecName\" to \"%s\"\n", videnc1->codecName);
            break;
        case PROP_RATE_CTRL_PRESET:
            videnc1->rateControlPreset = g_value_get_int(value);
            GST_LOG("setting \"rateControlPreset\" to \"%d\" \n",
                     videnc1->rateControlPreset);
            break;
        case PROP_ENCODING_PRESET:
            videnc1->encodingPreset = g_value_get_int(value);
            GST_LOG("setting \"encodingPreset\" to \"%d\" \n",
                     videnc1->encodingPreset);
            break;
        case PROP_FRAMERATE:
        {
            g_value_copy(value, &videnc1->framerate);
            GST_LOG("Setting framerate=%d/%d\n", 
                gst_value_get_fraction_numerator(&videnc1->framerate),
                gst_value_get_fraction_denominator(&videnc1->framerate));
            break;
        }
        case PROP_CONTIG_INPUT_BUF:
            videnc1->contiguousInputFrame = g_value_get_boolean(value);
            GST_LOG("setting \"contiguousInputFrame\" to \"%s\"\n",
                videnc1->contiguousInputFrame ? "TRUE" : "FALSE");
            break;
        case PROP_BYTE_STREAM:
            videnc1->byteStream = g_value_get_boolean(value);
            GST_LOG("setting \"byteStream\" to \"%s\"\n",
                videnc1->byteStream ? "TRUE" : "FALSE");
            break;
        case PROP_GEN_TIMESTAMPS:
            videnc1->genTimeStamps = g_value_get_boolean(value);
            GST_LOG("setting \"genTimeStamps\" to \"%s\"\n",
                videnc1->genTimeStamps ? "TRUE" : "FALSE");
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }

    GST_LOG("end set_property\n");
}

/******************************************************************************
 * gst_tividenc1_get_property
 *     Return values for requested element property.
 ******************************************************************************/
static void gst_tividenc1_get_property(GObject *object, guint prop_id,
                GValue *value, GParamSpec *pspec)
{
    GstTIVidenc1 *videnc1 = GST_TIVIDENC1(object);

    GST_LOG("begin get_property\n");

    switch (prop_id) {
        case PROP_ENGINE_NAME:
            g_value_set_string(value, videnc1->engineName);
            break;
        case PROP_CODEC_NAME:
            g_value_set_string(value, videnc1->codecName);
            break;
        case PROP_RESOLUTION:
            g_value_set_string(value, videnc1->resolution);
            break;
        case PROP_IN_COLORSPACE:
            g_value_set_string(value, videnc1->iColor);
            break;
        case PROP_FRAMERATE:
            g_value_copy(&videnc1->framerate, value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }

    GST_LOG("end get_property\n");
}


/******************************************************************************
 * gst_tividenc1_set_sink_caps
 *     Negotiate our sink pad capabilities.
 ******************************************************************************/
static gboolean gst_tividenc1_set_sink_caps(GstPad *pad, GstCaps *caps)
{
    GstTIVidenc1  *videnc1;
    GstStructure *capStruct;
    const gchar  *mime;
    char         *string;

    videnc1    = GST_TIVIDENC1(gst_pad_get_parent(pad));
    capStruct = gst_caps_get_structure(caps, 0);
    mime      = gst_structure_get_name(capStruct);

    string = gst_caps_to_string(caps);
    GST_INFO("requested sink caps:  %s", string);
    g_free(string);

    /* Shut-down any running video encoder */
    if (!gst_tividenc1_exit_video(videnc1)) {
        gst_object_unref(videnc1);
        return FALSE;
    }

    /* Generic Video Properties */
    if (!strncmp(mime, "video/", 6)) {
        gint  framerateNum;
        gint  framerateDen;

        if (gst_structure_get_fraction(capStruct, "framerate", &framerateNum,
                &framerateDen)) {
            if (gst_value_get_fraction_numerator(&videnc1->framerate) == 0) {
                gst_value_set_fraction(&videnc1->framerate, framerateNum,
                    framerateDen);
            }
        }
    }

    /* RAW YUV input */
    if (!strcmp(mime,"video/x-raw-yuv")) {
        guint32 fourcc;

        if (gst_structure_get_fourcc(capStruct, "format", &fourcc)) {

            switch (fourcc) {
                case GST_MAKE_FOURCC('U', 'Y', 'V', 'Y'):
                    videnc1->colorSpace = ColorSpace_UYVY;
                    break;

                case GST_MAKE_FOURCC('N', 'V', '1', '2'):
                    videnc1->colorSpace = ColorSpace_YUV420PSEMI;
                    break;

                default:
                    GST_ELEMENT_ERROR(videnc1, STREAM, NOT_IMPLEMENTED,
                    ("unsupported fourcc in video stream\n"), (NULL));
                    gst_object_unref(videnc1);
                    return FALSE;
            }

            if (videnc1->width == 0 && 
               !gst_structure_get_int(capStruct, "width", &videnc1->width)) {
                videnc1->width = 0;
            }

            if (videnc1->height == 0 &&
               !gst_structure_get_int(capStruct, "height", &videnc1->height)) {
                videnc1->height = 0;
            }
        }
    }

    /* Mime type not supported */
    else {
        GST_ELEMENT_ERROR(videnc1, STREAM, NOT_IMPLEMENTED,
        ("stream type not supported"), (NULL));
        gst_object_unref(videnc1);
        return FALSE;
    }

    gst_object_unref(videnc1);

    GST_LOG("sink caps negotiation successful\n");
    return TRUE;
}

/******************************************************************************
 * gst_tividenc1_set_source_caps
 *     Negotiate our source pad capabilities.
 ******************************************************************************/
static gboolean gst_tividenc1_set_source_caps(
                    GstTIVidenc1 *videnc1, Buffer_Handle hBuf)
{
    BufferGfx_Dimensions  dim;
    GstCaps              *caps = NULL;
    gboolean              ret;
    GstPad               *pad;
    char                 *string;
    GstTICodec           *h264Codec  = NULL;
    GstTICodec           *mpeg4Codec = NULL;
    GstTICodec           *mpeg2Codec = NULL;

    h264Codec  =  gst_ticodec_get_codec("H.264 Video Encoder");
    mpeg4Codec  =  gst_ticodec_get_codec("MPEG4 Video Encoder");
    mpeg2Codec  =  gst_ticodec_get_codec("MPEG2 Video Encoder");

    pad = videnc1->srcpad;

    /* Create a video caps object using the dimensions from the given buffer */
    BufferGfx_getDimensions(hBuf, &dim);

    /* Create H.264 source cap */
    if (h264Codec && (!strcmp(h264Codec->CE_CodecName, videnc1->codecName))) {
        caps =
            gst_caps_new_simple("video/x-h264",
                "framerate",    GST_TYPE_FRACTION,
                    gst_value_get_fraction_numerator(&videnc1->framerate),
                    gst_value_get_fraction_denominator(&videnc1->framerate),
                "width",        G_TYPE_INT,         dim.width,
                "height",       G_TYPE_INT,         dim.height,
                NULL);
   
        if (videnc1->byteStream == FALSE) {
            gst_caps_set_simple(caps, "codec_data", GST_TYPE_BUFFER, 
                videnc1->codec_data, (char*)NULL);
        }
     
        string =  gst_caps_to_string(caps);
        GST_LOG("setting source caps to x-h264: %s", string);
        g_free(string);
    }
 
    /* Create MPEG-2 source cap */
    if (mpeg2Codec && (!strcmp(mpeg2Codec->CE_CodecName, videnc1->codecName))) {
        gint mpegversion = 2;

        caps =
            gst_caps_new_simple("video/mpeg",
                "mpegversion",  G_TYPE_INT,         mpegversion,
                "framerate",    GST_TYPE_FRACTION,
                    gst_value_get_fraction_numerator(&videnc1->framerate),
                    gst_value_get_fraction_denominator(&videnc1->framerate),
                "width",        G_TYPE_INT,         dim.width,
                "height",       G_TYPE_INT,         dim.height,
                NULL);

        string =  gst_caps_to_string(caps); 
        GST_LOG("setting source caps to mpeg2: %s", string);
        g_free(string);
    }

    /* Create MPEG4 source cap */
    if (mpeg4Codec && (!strcmp(mpeg4Codec->CE_CodecName, videnc1->codecName))) {
        gint mpegversion = 4;

        caps =
            gst_caps_new_simple("video/mpeg",
                "mpegversion",  G_TYPE_INT,         mpegversion,
                "framerate",    GST_TYPE_FRACTION,
                    gst_value_get_fraction_numerator(&videnc1->framerate),
                    gst_value_get_fraction_denominator(&videnc1->framerate),
                "width",        G_TYPE_INT,         dim.width,
                "height",       G_TYPE_INT,         dim.height,
                NULL);

        string =  gst_caps_to_string(caps); 
        GST_LOG("setting source caps to mpeg4: %s", string);
        g_free(string);
    }

    /* Set the source pad caps */
    ret = gst_pad_set_caps(pad, caps);
    gst_caps_unref(caps);

    return ret;
}

/******************************************************************************
 * gst_tividenc1_sink_event
 *     Perform event processing on the input stream.  At the moment, this
 *     function isn't needed as this element doesn't currently perform any
 *     specialized event processing.  We'll leave it in for now in case we need
 *     it later on.
 ******************************************************************************/
static gboolean gst_tividenc1_sink_event(GstPad *pad, GstEvent *event)
{
    GstTIVidenc1 *videnc1;
    gboolean     ret;

    videnc1 = GST_TIVIDENC1(GST_OBJECT_PARENT(pad));

    GST_DEBUG("pad \"%s\" received:  %s\n", GST_PAD_NAME(pad),
        GST_EVENT_TYPE_NAME(event));

    switch (GST_EVENT_TYPE(event)) {

        case GST_EVENT_NEWSEGMENT:
            /* maybe save and/or update the current segment (e.g. for output
             * clipping) or convert the event into one in a different format
             * (e.g. BYTES to TIME) or drop it and set a flag to send a
             * newsegment event in a different format later
             */
            ret = gst_pad_push_event(videnc1->srcpad, event);
            break;

        case GST_EVENT_EOS:
        case GST_EVENT_FLUSH_STOP:
            ret = gst_pad_push_event(videnc1->srcpad, event);
            break;

        /* Unhandled events */
        case GST_EVENT_BUFFERSIZE:
        case GST_EVENT_CUSTOM_BOTH:
        case GST_EVENT_CUSTOM_BOTH_OOB:
        case GST_EVENT_CUSTOM_DOWNSTREAM:
        case GST_EVENT_CUSTOM_DOWNSTREAM_OOB:
        case GST_EVENT_CUSTOM_UPSTREAM:
        case GST_EVENT_FLUSH_START:
        case GST_EVENT_NAVIGATION:
        case GST_EVENT_QOS:
        case GST_EVENT_SEEK:
        case GST_EVENT_TAG:
        default:
            ret = gst_pad_event_default(pad, event);
            break;

    }

    return ret;
}

/******************************************************************************
 * gst_tividenc1_convert_gst_to_dmai
 *  This function convert gstreamer buffer into DMAI graphics buffer.
 *****************************************************************************/
static Buffer_Handle gst_tividenc1_convert_gst_to_dmai(GstTIVidenc1 *videnc1, 
    GstBuffer *buf, gboolean reference)
{
    BufferGfx_Attrs gfxAttrs   = BufferGfx_Attrs_DEFAULT;
    Buffer_Handle   hBuf       = NULL;
 
    gfxAttrs.bAttrs.reference  = reference;
    gfxAttrs.dim.width         = videnc1->width;
    gfxAttrs.dim.height        = videnc1->height;
    gfxAttrs.colorSpace        = videnc1->colorSpace;
    gfxAttrs.dim.lineLength    = BufferGfx_calcLineLength(
                                    gfxAttrs.dim.width, 
                                     videnc1->colorSpace);
    hBuf = Buffer_create(GST_BUFFER_SIZE(buf),
                BufferGfx_getBufferAttrs(&gfxAttrs));
    if (hBuf == NULL) {
        GST_ERROR("failed to create  buffer\n");
        return NULL;
    }
    Buffer_setUserPtr(hBuf, (Int8*)GST_BUFFER_DATA(buf));
    Buffer_setNumBytesUsed(hBuf, GST_BUFFER_SIZE(buf));

    return hBuf;
}

/*****************************************************************************
 * gst_tividenc1_prepare_input
 *  Prepare the codec input buffer.
 ****************************************************************************/
static Buffer_Handle
gst_tividenc1_prepare_input(GstTIVidenc1 *videnc1, GstBuffer **inBuf)
{
    /* If the input buffer is a physically contiguous DMAI buffer, it can
     * be passed directly to the codec.
     */
    if (GST_IS_TIDMAIBUFFERTRANSPORT(*inBuf)) {
        return GST_TIDMAIBUFFERTRANSPORT_DMAIBUF(*inBuf);
    }

    /* If zeroCopyEncode is TRUE, we have a non-DMAI buffer that can still
     * be used directly by the codec.  Wrap it in a DMAI buffer reference
     * so it can be passed to Venc1_process.
     */
    if (videnc1->zeroCopyEncode) {
        if (!videnc1->hInBufRef) {
            videnc1->hInBufRef =
                gst_tividenc1_convert_gst_to_dmai(videnc1, *inBuf, TRUE);
            if (videnc1->hInBufRef == NULL) {
                GST_ERROR("failed to get dmai buffer\n");
                return NULL;
            }
        } else {
            Buffer_setUserPtr(videnc1->hInBufRef,
                (Int8*)GST_BUFFER_DATA(*inBuf));
            Buffer_setNumBytesUsed(videnc1->hInBufRef,
                GST_BUFFER_SIZE(*inBuf));
        }
        return videnc1->hInBufRef;
    }

    /* Otherwise, copy the buffer contents into our local physically contiguous
     * DMAI buffer and pass it to the codec.  We should only ever need to do
     * this for file input -- if we're doing capture+encode and don't support
     * zero-copy, the TIPrepEncBuf element should have been used to copy
     * the input buffer to a DMAI transport buffer, which we handle above.
     */
    if (videnc1->contiguousInputFrame) {
        static gboolean warned_already = FALSE;

        if (!warned_already) {
            GST_ELEMENT_WARNING(videnc1, STREAM, ENCODE,
            ("performing a slow memcpy of the input buffer to a "
                "physically contiguous buffer; you probably want to use "
                "the TIPrepEncBuf element before TIVidenc1 to properly "
                "prepare your input buffer for encoding"), (NULL));
            warned_already = TRUE;
        }
    }
    memcpy(Buffer_getUserPtr(videnc1->hContigInBuf), GST_BUFFER_DATA(*inBuf),
        GST_BUFFER_SIZE(*inBuf));
    Buffer_setNumBytesUsed(videnc1->hContigInBuf, GST_BUFFER_SIZE(*inBuf));
    gst_buffer_unref(*inBuf);
    *inBuf = NULL;
    return videnc1->hContigInBuf;
}


/******************************************************************************
 * gst_tividenc1_chain
 *    This is the main processing routine.  This function receives a buffer
 *    from the sink pad, processes it, and pushes the result to the source
 *    pad.
 ******************************************************************************/
static GstFlowReturn gst_tividenc1_chain(GstPad * pad, GstBuffer * buf)
{
    GstTIVidenc1 *videnc1 = GST_TIVIDENC1(GST_OBJECT_PARENT(pad));

    /* If our engine handle is currently NULL, then either this is our first
     * buffer or the upstream element has re-negotiated our capabilities which
     * resulted in our engine being closed.  In either case, we need to
     * initialize (or re-initialize) our video encoder to handle the new
     * stream.
     */
    if (videnc1->hEngine == NULL) {

        /* If we know the input buffer is physically contiguous in memory, use
         * its size as the expected size for all input buffers.  Otherwise,
         * the codec will be asked later on for the size to expect.
         */
        if (videnc1->contiguousInputFrame ||
            GST_IS_TIDMAIBUFFERTRANSPORT(buf)) {
            videnc1->upstreamBufSize = GST_BUFFER_SIZE(buf);
        }

        /* Initialize video encoder */
        if (!gst_tividenc1_init_video(videnc1)) {
            GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
            ("unable to initialize video\n"), (NULL));
            return GST_FLOW_UNEXPECTED;
        }
    }

    /* We can't easily check to make sure a buffer is physically contiguous in
     * memory, but we can make sure it's the right size, which is better than
     * nothing.
     */
    if (videnc1->contiguousInputFrame &&
        GST_BUFFER_SIZE(buf) != videnc1->upstreamBufSize) {
            GST_ELEMENT_ERROR(videnc1, RESOURCE, WRITE,
            ("if contiguousInputFrame=TRUE the input buffer should be of size"
             "%lu\n", (unsigned long) videnc1->upstreamBufSize), (NULL));
            return GST_FLOW_UNEXPECTED;
    }

    gst_adapter_push (videnc1->sinkAdapter, buf);
    while (gst_adapter_available(videnc1->sinkAdapter) >=
           videnc1->upstreamBufSize) {
        GstBuffer     *qBuf;
        GstBuffer     *outBuf;

        qBuf = gst_adapter_take_buffer(videnc1->sinkAdapter,
                   videnc1->upstreamBufSize);

        if (gst_tividenc1_encode(videnc1, qBuf, &outBuf) != GST_FLOW_OK) {
            GST_ELEMENT_ERROR(videnc1, RESOURCE, WRITE,
            ("Failed to encode input buffer\n"), (NULL));
            return GST_FLOW_UNEXPECTED;
        }

        /* Parse and Push the transport buffer to the source pad */
        GST_LOG("pushing display buffer to source pad\n");
        if (gst_tividenc1_parse_and_push(videnc1, outBuf) != GST_FLOW_OK) {
            GST_DEBUG("push to source pad failed\n");
            gst_buffer_unref(qBuf);
            return GST_FLOW_UNEXPECTED;
        }
    }

    return GST_FLOW_OK;
}

/******************************************************************************
 * gst_tividenc1_init_video
 *     Initialize or re-initializes the video stream
 ******************************************************************************/
static gboolean gst_tividenc1_init_video(GstTIVidenc1 *videnc1)
{
    GST_LOG("begin init_video\n");

    /* If video has already been initialized, shut down previous encoder */
    if (videnc1->hEngine) {
        if (!gst_tividenc1_exit_video(videnc1)) {
            GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
            ("failed to shut down existing video encoder\n"), (NULL));
            return FALSE;
        }
    }

    /* Make sure we know what codec we're using */
    if (!videnc1->engineName) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("engine name not specified\n"), (NULL));
        return FALSE;
    }

    if (!videnc1->codecName) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("codec name not specified\n"), (NULL));
        return FALSE;
    }

    /* Initialize the inBufMetadata */
    if (!videnc1->inBufMetadata) {
        videnc1->inBufMetadata = gst_buffer_new();
    }

    /* Initialize the sinkAdapter */
    if (!videnc1->sinkAdapter) {
        videnc1->sinkAdapter = gst_adapter_new();
    }

    /* Calculate the duration of a single frame in this stream */
    videnc1->frameDuration = gst_tividenc1_frame_duration(videnc1);

    /* Determine if we can pass input buffer directly to codec */
    #if defined(Platform_dm365) || defined(Platform_dm355) || defined(Platform_dm368)
        if (videnc1->contiguousInputFrame) {
            videnc1->zeroCopyEncode = TRUE;
        }
    #endif

    /* Start the codec */
    if (!gst_tividenc1_codec_start(videnc1)) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("failed to initialize codec\n"),
        (NULL));
        return FALSE;
    }

    GST_LOG("end init_video\n");
    return TRUE;
}


/******************************************************************************
 * gst_tividenc1_exit_video
 *    Shut down any running video encoder, and reset the element state.
 ******************************************************************************/
static gboolean gst_tividenc1_exit_video(GstTIVidenc1 *videnc1)
{
    GST_LOG("begin exit_video\n");

    if (videnc1->sinkAdapter) {
        g_object_unref(videnc1->sinkAdapter);
        videnc1->sinkAdapter = NULL;
    }

    if (videnc1->inBufMetadata) {
        gst_buffer_unref(videnc1->inBufMetadata);
        videnc1->inBufMetadata = NULL;
    }

    if (videnc1->hFc) {
        GST_LOG("freeing framecopy handle\n");
        Framecopy_delete(videnc1->hFc);
        videnc1->hFc = NULL;
    }

    if (videnc1->hCcv) {
        GST_LOG("freeing ccv handle\n");
        Ccv_delete(videnc1->hCcv);
        videnc1->hCcv = NULL;
    }

    if (videnc1->codec_data) {
        GST_LOG("freeing codec_data buffer\n");
        gst_buffer_unref(videnc1->codec_data);
        videnc1->codec_data = NULL;
    }

    videnc1->frameDuration = GST_CLOCK_TIME_NONE;

    /* Stop the codec */
    if (gst_tividenc1_codec_stop(videnc1) < 0) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("failed to stop codec\n"),
        (NULL));
    }

    GST_LOG("end exit_video\n");
    return TRUE;
}


/******************************************************************************
 * gst_tividenc1_change_state
 *     Manage state changes for the video stream.  The gStreamer documentation
 *     states that state changes must be handled in this manner:
 *        1) Handle ramp-up states
 *        2) Pass state change to base class
 *        3) Handle ramp-down states
 ******************************************************************************/
static GstStateChangeReturn gst_tividenc1_change_state(GstElement *element,
                                GstStateChange transition)
{
    GstStateChangeReturn  ret    = GST_STATE_CHANGE_SUCCESS;
    GstTIVidenc1          *videnc1 = GST_TIVIDENC1(element);

    GST_LOG("begin change_state (%d)\n", transition);

    /* Handle ramp-up state changes */
    switch (transition) {
        case GST_STATE_CHANGE_NULL_TO_READY:
            break;
        default:
            break;
    }

    /* Pass state changes to base class */
    ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
    if (ret == GST_STATE_CHANGE_FAILURE)
        return ret;

    /* Handle ramp-down state changes */
    switch (transition) {
        case GST_STATE_CHANGE_READY_TO_NULL:
            /* Shut down any running video encoder */
            if (!gst_tividenc1_exit_video(videnc1)) {
                return GST_STATE_CHANGE_FAILURE;
            }
            break;

        default:
            break;
    }

    GST_LOG("end change_state\n");
    return ret;
}

/******************************************************************************
 * gst_tividenc1_codec_stop
 *   stop codec engine
 *****************************************************************************/
static gboolean gst_tividenc1_codec_stop (GstTIVidenc1 *videnc1)
{
    if (videnc1->hContigInBuf) {
        Buffer_delete(videnc1->hContigInBuf);
        videnc1->hContigInBuf = NULL;
    }

    if (videnc1->hInBufRef) {
        Buffer_delete(videnc1->hInBufRef);
        videnc1->hInBufRef = NULL;
    }

    videnc1->zeroCopyEncode = FALSE;

    if (videnc1->hEncOutBuf) {
        Buffer_delete(videnc1->hEncOutBuf);
        videnc1->hEncOutBuf = NULL;
    }

    if (videnc1->hVe1) {
        GST_LOG("closing video encoder\n");
        Venc1_delete(videnc1->hVe1);
        videnc1->hVe1 = NULL;
    }

    if (videnc1->hEngine) {
        GST_LOG("closing codec engine\n");
        Engine_close(videnc1->hEngine);
        videnc1->hEngine = NULL;
    }

    return TRUE;
}

/******************************************************************************
 * gst_tividenc1_codec_start
 *   start codec engine
 *****************************************************************************/
static gboolean gst_tividenc1_codec_start (GstTIVidenc1 *videnc1)
{
    VIDENC1_DynamicParams dynParams   = Venc1_DynamicParams_DEFAULT;
    BufferGfx_Attrs       gfxAttrsIn  = BufferGfx_Attrs_DEFAULT;
    BufferGfx_Attrs       gfxAttrsOut = BufferGfx_Attrs_DEFAULT;
    VIDENC1_Params        params      = Venc1_Params_DEFAULT;
    Int                   inBufSize;

    /* Open the codec engine */
    GST_LOG("opening codec engine \"%s\"\n", videnc1->engineName);
    videnc1->hEngine = Engine_open((Char *) videnc1->engineName, NULL, NULL);

    if (videnc1->hEngine == NULL) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED,
        ("failed to open codec engine \"%s\"\n", videnc1->engineName), (NULL));
        return FALSE;
    }

    /* setup codec parameters depending on device */
    switch(videnc1->device) {
        case Cpu_Device_OMAP3530:
        #if defined(Platform_omapl138)
        case Cpu_Device_OMAPL138:
        #endif
        #if defined(Platform_dm3730)
        case Cpu_Device_DM3730:
            params.maxInterFrameInterval = 1;
        #endif
        case Cpu_Device_DM6446:
            params.inputChromaFormat = XDM_YUV_422ILE;
            break;

        case Cpu_Device_DM355:
            params.inputChromaFormat = XDM_YUV_422ILE;
            params.reconChromaFormat = XDM_YUV_420P;
            break;

        case Cpu_Device_DM6467:
            #if defined(Platform_dm6467t)
            params.inputChromaFormat = XDM_YUV_420SP;
            #else
            params.inputChromaFormat = XDM_YUV_420P;
            #endif
            params.reconChromaFormat = XDM_CHROMA_NA;
            break;

        #if defined(Platform_dm365)
        case Cpu_Device_DM365:
            if (videnc1->colorSpace == ColorSpace_UYVY) {
                params.inputChromaFormat = XDM_YUV_422ILE;
            }
            else {
                params.inputChromaFormat = XDM_YUV_420SP;
                params.reconChromaFormat = XDM_YUV_420SP;
            }
            break;
        #endif

        #if defined(Platform_dm368)
        case Cpu_Device_DM368:
            if (videnc1->colorSpace == ColorSpace_UYVY) {
                params.inputChromaFormat = XDM_YUV_422ILE;
            }
            else {
                params.inputChromaFormat = XDM_YUV_420SP;
                params.reconChromaFormat = XDM_YUV_420SP;
            }
            break;
        #endif

        default:
            GST_ELEMENT_ERROR(videnc1, RESOURCE, FAILED, 
             ("unsupport cpu type\n"), (NULL));
            return FALSE;
    }

    /* set the bit rate control preset */
    switch(videnc1->rateControlPreset) {
        case 1:
            params.rateControlPreset = IVIDEO_NONE;
            break;
        case 2:
            params.rateControlPreset = IVIDEO_LOW_DELAY;
            break;
        case 3:
            params.rateControlPreset = IVIDEO_STORAGE;
            break;
        default:
            params.rateControlPreset = IVIDEO_NONE;
            break;
    }
    
    /* Set the encoding preset */
    switch(videnc1->encodingPreset) {
        case 1:
            break;
        case 2:
            params.encodingPreset    = XDM_HIGH_QUALITY;
            break;
        case 3: 
            params.encodingPreset    = XDM_HIGH_SPEED;
            break;
        default:
            break;
    }

    /* set the encoding bitrate */
    params.maxBitRate = videnc1->bitRate < 0 ? 
        DEFAULT_BIT_RATE : videnc1->bitRate;

    params.maxWidth         = videnc1->width;
    params.maxHeight        = videnc1->height;
    dynParams.targetBitRate = params.maxBitRate;
    dynParams.inputWidth    = videnc1->width;
    dynParams.inputHeight   = videnc1->height;
    
    GST_LOG("configuring video encode width=%ld, height=%ld, bitrate=%ld\n", 
            params.maxWidth, params.maxHeight, params.maxBitRate);

    GST_LOG("opening video encoder \"%s\"\n", videnc1->codecName);
    videnc1->hVe1 = Venc1_create(videnc1->hEngine, (Char*)videnc1->codecName,
                      &params, &dynParams);

    if (videnc1->hVe1 == NULL) {
        GST_ELEMENT_ERROR(videnc1, STREAM, CODEC_NOT_FOUND,
        ("failed to create video encoder: %s\n", videnc1->codecName), (NULL));
        GST_LOG("closing codec engine\n");
        gst_tividenc1_exit_video(videnc1);
        return FALSE;
    }

    /* Determine the size of the physically contiguous input buffer.  If
     * it has already been determined, leave it.  Otherwise ask the codec how
     * big it should be.
     */
    if (videnc1->upstreamBufSize > 0) {
        inBufSize = videnc1->upstreamBufSize;
    }
    else {
        inBufSize = Venc1_getInBufSize(videnc1->hVe1);
        videnc1->upstreamBufSize = inBufSize;
    }

    /* Create a physically contiguous input buffer */
    gfxAttrsIn.dim.width        = videnc1->width;
    gfxAttrsIn.dim.height       = videnc1->height;
    gfxAttrsIn.colorSpace       = videnc1->colorSpace;

    gfxAttrsIn.dim.lineLength   =
        BufferGfx_calcLineLength(gfxAttrsIn.dim.width, gfxAttrsIn.colorSpace);

    /* allocate input buffer in physically contiguous memory */
    if (!videnc1->zeroCopyEncode) {
        videnc1->hContigInBuf = Buffer_create(videnc1->upstreamBufSize, 
            BufferGfx_getBufferAttrs(&gfxAttrsIn));

        if (videnc1->hContigInBuf == NULL) {
            gst_tividenc1_exit_video(videnc1);
            GST_ELEMENT_ERROR(videnc1, RESOURCE, NO_SPACE_LEFT,
            ("failed to allocate input buffer for encoder\n"), (NULL));
            return FALSE;
        }
    }

    /* Create codec output buffers */
    GST_LOG("creating output buffer table\n");
    gfxAttrsOut.colorSpace     = videnc1->colorSpace;
    gfxAttrsOut.dim.width      = videnc1->width;
    gfxAttrsOut.dim.height     = videnc1->height;
    gfxAttrsOut.dim.lineLength = BufferGfx_calcLineLength(
        gfxAttrsOut.dim.width, gfxAttrsOut.colorSpace);

    gfxAttrsOut.bAttrs.memParams.align = 128;

    videnc1->hEncOutBuf = Buffer_create(Venc1_getOutBufSize(videnc1->hVe1),
        BufferGfx_getBufferAttrs(&gfxAttrsOut));

    return TRUE;
}

/******************************************************************************
 * gst_tividenc1_populate_codec_header
 *  This function populates codec_data field for H.264.
 *****************************************************************************/
static void
gst_tividenc1_populate_codec_header (GstTIVidenc1 *videnc1,
    Buffer_Handle hDstBuf)
{
    if (gst_is_h264_encoder(videnc1->codecName)) {
        if (!videnc1->byteStream) {
            /* Generate codec_data field for packetized stream */
            if (!videnc1->codec_data) {
                videnc1->codec_data = gst_h264_create_codec_data(hDstBuf);
            }
        }
    }
}

/******************************************************************************
 * gst_tividenc1_parse_and_queue 
 *  Function parses and push the output buffer .
 *****************************************************************************/
static GstFlowReturn
gst_tividenc1_parse_and_push (GstTIVidenc1 *videnc1, GstBuffer *outBuf)
{
    guint8 *data;
    guint   len;

    /* perform H.264 specific parsing before pushing the data */
    if (gst_is_h264_encoder(videnc1->codecName)) {

        /* convert byte-stream to packetized */
        if ((!videnc1->byteStream) && (videnc1->codec_data)) {

            /* Prefix the NALU with length field */
            data = GST_BUFFER_DATA(outBuf);
            len = GST_BUFFER_SIZE(outBuf)-4;
           
            data[3] = len & 0xff;
            data[2] = (len >> 8) & 0xff;
            data[1] = (len >> 16) & 0xff;
            data[0] = (len >> 24) & 0xff;
        }
    }

    return gst_pad_push(videnc1->srcpad, outBuf);
}

/******************************************************************************
 * gst_tividenc1_encode
 *     Call the video codec to process a full input buffer
 ******************************************************************************/
static GstFlowReturn
gst_tividenc1_encode(GstTIVidenc1 *videnc1, GstBuffer *inBuf,
    GstBuffer **outBuf)
{
    Buffer_Handle  hContigInBuf = NULL;
    GstFlowReturn  flowRet      = GST_FLOW_OK;
    Int            ret;

    *outBuf = NULL;

    /* Make sure the input buffer is the expected size */
    if (GST_BUFFER_SIZE(inBuf) != videnc1->upstreamBufSize) {
        GST_ELEMENT_ERROR(videnc1, RESOURCE, NO_SPACE_LEFT,
        ("input buffer is an invalid size (%lu != %lu)\n",
        (unsigned long)GST_BUFFER_SIZE(inBuf),
        (unsigned long)videnc1->upstreamBufSize),
        (NULL));
        goto exit_fail;
    }

    /* Get the metadata from the input buffer */
    gst_buffer_copy_metadata(videnc1->inBufMetadata, inBuf, 
        GST_BUFFER_COPY_ALL);

    /* Prepare the codec input buffer.  If the input buffer is copied and
     * unref'd, inBuf will be set to NULL. */
    if (!(hContigInBuf = gst_tividenc1_prepare_input(videnc1, &inBuf))) {
        goto exit_fail;
    }

    /* Reset metadata for encoded output buffer */
    BufferGfx_resetDimensions(videnc1->hEncOutBuf);

    /* Invoke the video encoder */
    GST_LOG("invoking the video encoder\n");
    ret   = Venc1_process(videnc1->hVe1, hContigInBuf, videnc1->hEncOutBuf);

    if (ret < 0) {
        GST_ELEMENT_ERROR(videnc1, STREAM, ENCODE,
        ("failed to encode video buffer\n"), (NULL));
        goto exit_fail;
    }
    else if (ret > 0) {
        GST_LOG("Venc1_process returned success code %d\n", ret); 
    }

    /* Release the input buffer if we haven't already */
    if (inBuf) {
        gst_buffer_unref(inBuf);
        inBuf = NULL;
    }

    /* Populate codec header */
    gst_tividenc1_populate_codec_header(videnc1, videnc1->hEncOutBuf);

    /* Set the source pad capabilities based on the encoded frame properties.
     */
    gst_tividenc1_set_source_caps(videnc1, videnc1->hEncOutBuf);

    /* Create a DMAI transport buffer object to carry a DMAI buffer to
     * the source pad.  The transport buffer knows how to release the
     * buffer for re-use in this element when the source pad calls
     * gst_buffer_unref().
     */
    *outBuf =
        gst_buffer_new_and_alloc(Buffer_getNumBytesUsed(videnc1->hEncOutBuf));

    memcpy(GST_BUFFER_DATA(*outBuf), Buffer_getUserPtr(videnc1->hEncOutBuf),
        Buffer_getNumBytesUsed(videnc1->hEncOutBuf));

    gst_buffer_set_caps(*outBuf, GST_PAD_CAPS(videnc1->srcpad));

    /* Get the metadata from the input buffer */
    gst_buffer_copy_metadata(*outBuf, videnc1->inBufMetadata,
        GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_TIMESTAMPS);

    goto exit_ok;

exit_fail:
    flowRet = GST_FLOW_UNEXPECTED;

exit_ok:
    if (inBuf) gst_buffer_unref(inBuf);
    return flowRet;
}


/******************************************************************************
 * gst_tividenc1_frame_duration
 *    Return the duration of a single frame in nanoseconds.
 ******************************************************************************/
static GstClockTime gst_tividenc1_frame_duration(GstTIVidenc1 *videnc1)
{
    /* Default to 29.97 if the frame rate was not specified */
    if (gst_value_get_fraction_numerator(&videnc1->framerate) == 0) {
         GST_WARNING("framerate not specified; using 29.97fps");
        gst_value_set_fraction(&videnc1->framerate, 30000, 1001);
    }

    return
     ((GstClockTime) gst_value_get_fraction_denominator(&videnc1->framerate)) *
     GST_SECOND /
     ((GstClockTime) gst_value_get_fraction_numerator(&videnc1->framerate));
}


/******************************************************************************
 * Custom ViM Settings for editing this file
 ******************************************************************************/
#if 0
 Tabs (use 4 spaces for indentation)
 vim:set tabstop=4:      /* Use 4 spaces for tabs          */
 vim:set shiftwidth=4:   /* Use 4 spaces for >> operations */
 vim:set expandtab:      /* Expand tabs into white spaces  */
#endif
