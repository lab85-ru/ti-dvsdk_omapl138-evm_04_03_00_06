/*==================================================================== */
/*  Copyright (c) 2010, Texas Instruments Incorporated                 */
/*  All rights reserved.                                               */
/*                                                                     */
/*                                                                     */
/* ======== C6accel_ti_imglibFunctionCall.c ========                   */
/* This file is used to invoke IMGLib kernels based on the function ID */
/*   passed by the application to C6Accel xdais algorithm              */
/*                                                                     */
/*                                                                     */
/*  Version: 0.0.1                                                     */
/*==================================================================== */

/* Include Files */
#include <xdc/std.h>
#include <string.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/xdais/dm/iuniversal.h>
#include "../../libs/include/VLIB_prototypes.h"
/* Header Intrinsic to xdais algorithm*/
#include "C6Accel.h"

/* Application-Algorithm Interface file includes */
#include "iC6Accel_ti.h"
#include "iC6Accel_ti_priv.h"


int C6ACCEL_TI_vlibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs, XDM1_BufDesc *inOutBufs)
{
 Int32 fxnidMsk= FXN_ID_MASK;

          switch ( fxnID & fxnidMsk){
              	
 		case (NONMAXIMUMSUPPRESSIONCANNY_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_nonMaximumSuppressionCanny_Params *C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr;
                    C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID3)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->Col)%2 != 0)|
                        ((C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->Row)%2 != 0)
                        ){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_nonMaximumSuppressionCanny(
					(short *)inBufs->descs[C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID1].buf,
					(short *)inBufs->descs[C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID2].buf,
					(short *)inBufs->descs[C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->indata_InArrID3].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->Pitch,
                                        C6ACCEL_TI_VLIB_nonMaximumSuppressionCanny_paramPtr->Row);
                     }
                 break;	
	
 		case (DOUBLETHRESHOLDING_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_doublethresholding_Params *C6ACCEL_TI_VLIB_doublethresholding_paramPtr;
                    C6ACCEL_TI_VLIB_doublethresholding_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID3)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->Col)%2 != 0)|
                        ((C6ACCEL_TI_VLIB_doublethresholding_paramPtr->Row)%2 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_doublethresholding(
					(signed short *)inBufs->descs[C6ACCEL_TI_VLIB_doublethresholding_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID1].buf,
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID2].buf,
                                       (int *)outBufs->descs[C6ACCEL_TI_VLIB_doublethresholding_paramPtr->outdata_OutArrID3].buf,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->Pitch,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->Row,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->loThresh,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->hiThresh,
                                        C6ACCEL_TI_VLIB_doublethresholding_paramPtr->block_offset);
                     }
                 break;	
	
 		case (EDGERELAXATION_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_edgeRelaxation_Params *C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr;
                    C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->indata_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->outdata_OutArrID1)>OUTBUF15)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_edgeRelaxation(
					(unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->outdata_OutArrID1].buf,
					(unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->indata_InArrID1].buf,
					(int *)inBufs->descs[C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->indata_InArrID2].buf,
                                        C6ACCEL_TI_VLIB_edgeRelaxation_paramPtr->Col);
                     }
                 break;
	

 		case (CANNYEDGE_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_cannyEdge_Params *C6ACCEL_TI_VLIB_cannyEdge_paramPtr;
                    C6ACCEL_TI_VLIB_cannyEdge_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->input_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf1_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf2_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf3_OutArrID3)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->output_OutArrID4)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->width)%2 != 0)|
                        ((C6ACCEL_TI_VLIB_cannyEdge_paramPtr->height)%2 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
				/*Some var in use*/
				int i;
				const char gaussian_7x7[49] = 
				{
					0, 0,  0,  0,  0, 0, 0,
					0, 0,  0,  0,  0, 0, 0,
					0, 0, 16, 32, 16, 0, 0,
					0, 0, 32, 64, 32, 0, 0,
					0, 0, 16, 32, 16, 0, 0,
					0, 0,  0,  0,  0, 0, 0,
					0, 0,  0,  0,  0, 0, 0,
				}; 
				unsigned short conv_window_size;
				unsigned short width;
				unsigned short height;
				unsigned short new_width;
			  	unsigned short new_height;

				/*Pointers to input/output buffers*/
				unsigned int * listptr;
			  	int * numItems;
			    	unsigned char * pIn ;
				short *  pGx;
				short *  pGy;
				short *  pMag;
				unsigned char * pOut;
                                unsigned char * pBufOut;

				/*Init of the var*/
				conv_window_size=7;
				width=C6ACCEL_TI_VLIB_cannyEdge_paramPtr->width;
				height=C6ACCEL_TI_VLIB_cannyEdge_paramPtr->height;
				pIn = (unsigned char*)inBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->input_InArrID1].buf;
				pGx= ( short*)outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf1_OutArrID1].buf+ width*(conv_window_size-1)/2 + (conv_window_size-1)/2;
				pGy= ( short*)outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf2_OutArrID2].buf+ width*(conv_window_size-1)/2 + (conv_window_size-1)/2;
				pMag= ( short*)outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf3_OutArrID3].buf+ width*(conv_window_size-1)/2 + (conv_window_size-1)/2;
				pBufOut= (unsigned char*)outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->output_OutArrID4].buf;
                                pOut = (unsigned char*)(pBufOut + width*(conv_window_size-1)/2 + (conv_window_size-1)/2);
			  	new_height = height -(conv_window_size-1);
			  	new_width = width -(conv_window_size-1);
	
				/*Loop equivalent to IMG_conv_7x7_i8_c8s_frame*/
				/*for(i=0;i<height;i++){
					IMG_conv_7x7_i8_c8s(pIn+i*width, 
						pOut+i*width+((conv_window_size-1)/2),  
						width-((conv_window_size-1)/2), 
						width, 
						gaussian_7x7, 
						//pgauss, 
						8);
				}*/
                                        IMG_conv_7x7_i8_c8s(            pIn, 
									pBufOut,  
									width * (new_height) - 6, 
									width, 
									gaussian_7x7, 
									8);

				pGx  +=(width + 1);
			    	pGy  +=(width + 1);
			   	pMag +=(width + 1);
			   	new_height -= 2;

				VLIB_xyGradientsAndMagnitude(pBufOut, 
					pGx, 
					pGy, 
					pMag, 
					width, 
					new_height);

				for (i=0; i < width * 5; i++){
		                     pBufOut[i] = 0;
		                     pBufOut[width * (height-5) + i] = 0;
	                         }
                                memset(pBufOut,   0, width*height*sizeof(unsigned char));
                                new_width -= 4;
				new_height -= 2;
				pGx  +=(width + 1);
				pGy  +=(width + 1);
				pOut +=(2*width + 2);

			  	VLIB_nonMaximumSuppressionCanny(pMag, 
					pGx, 
					pGy,
					pOut,
					new_width,
					width,
					new_height);

				listptr   = (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf1_OutArrID1].buf;
				numItems = (int *) outBufs->descs[C6ACCEL_TI_VLIB_cannyEdge_paramPtr->workBuf2_OutArrID2].buf;
                                pMag +=(width + 1);

			    	VLIB_doublethresholding(pMag, 
					     pOut, 
					     listptr,
					     numItems,
					     new_width,
					     width,                      
					     new_height,
					     C6ACCEL_TI_VLIB_cannyEdge_paramPtr->loThresh,
					     C6ACCEL_TI_VLIB_cannyEdge_paramPtr->hiThresh,
								 0
					     );

			    	VLIB_edgeRelaxation(pOut, 
			    			 listptr,
					         numItems,
					         width);

				/*Used to put at 0 the remaining non edge*/
				for (i=width*5; i < width*(height-10); i++) 
				{
					pBufOut[i] = (pBufOut[i] >> 7) * 255;
				}

			}
                     }
                 break;	
	
 		case (PACKMASK32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_packMask32_Params *C6ACCEL_TI_VLIB_packMask32_paramPtr;
                    C6ACCEL_TI_VLIB_packMask32_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_packMask32_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_packMask32_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_packMask32_paramPtr->pixelCount)%8 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_packMask32(
					(unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_packMask32_paramPtr->indata_InArrID1].buf,
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_packMask32_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_packMask32_paramPtr->pixelCount);
                     }
                 break;	
	
 		case (UNPACKMASK32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_unpackMask32_Params *C6ACCEL_TI_VLIB_unpackMask32_paramPtr;
                    C6ACCEL_TI_VLIB_unpackMask32_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_unpackMask32_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_unpackMask32_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_unpackMask32_paramPtr->pixelCount)%8 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_unpackMask32(
					(unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_unpackMask32_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_unpackMask32_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_unpackMask32_paramPtr->pixelCount);
                     }
                 break;	
	
 		case (DILATE_BIN_SQUARE_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_dilate_bin_square_Params *C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr;
                    C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->Col)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->Pitch)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                    /* Call underlying kernel */
                       VLIB_dilate_bin_square(
					( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_dilate_bin_square_paramPtr->Pitch);
                     }
		}
                 break;	
	
 		case (DILATE_BIN_CROSS_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_dilate_bin_cross_Params *C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr;
                    C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->Col)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->Pitch)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_dilate_bin_cross(
					( unsigned char * )inBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_dilate_bin_cross_paramPtr->Pitch);
                     }
                 break;	
	
 		case (ERODE_BIN_SQUARE_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_erode_bin_square_Params *C6ACCEL_TI_VLIB_erode_bin_square_paramPtr;
                    C6ACCEL_TI_VLIB_erode_bin_square_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->Col)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->Pitch)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_erode_bin_square(
					( unsigned char * )inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_erode_bin_square_paramPtr->Pitch);
                     }
                 break;	
	
 		case (ERODE_BIN_CROSS_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_erode_bin_cross_Params *C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr;
                    C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->Col)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->Pitch)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_erode_bin_cross(
					( unsigned char * )inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_erode_bin_cross_paramPtr->Pitch);
                     }
                 break;	
	
		case (ERODE_BIN_SINGLEPIXEL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_erode_bin_singlePixel_Params *C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr;
                    C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->Col)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->Pitch)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_erode_bin_cross(
					( unsigned char * )inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->Col,
                                        C6ACCEL_TI_VLIB_erode_bin_singlePixel_paramPtr->Pitch);
                     }
                 break;	

        	case (DILATE_BIN_SQUARE_OPT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_dilate_bin_square_opt_Params *C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr;
                    C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID3)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->height)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                    /* Call underlying kernel */
				VLIB_packMask32(
					(unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->indata_InArrID1].buf,
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID1].buf,
                                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width)*(C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->height)));
				VLIB_dilate_bin_square(
					(unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID1].buf,
                                       ((unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID2].buf)
											+((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width)/8),
                                        (C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width)*((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->height)-2),
                                        C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width);
				VLIB_unpackMask32(					
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID2].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->outdata_OutArrID3].buf,
                                        ((C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->width)*(C6ACCEL_TI_VLIB_dilate_bin_square_opt_paramPtr->height)));
			}
                     }
                 break;

        	case (ERODE_BIN_SQUARE_OPT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_erode_bin_square_opt_Params *C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr;
                    C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID3)>OUTBUF15)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width)%64 != 0)|
                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->height)%64 != 0)
			){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                    /* Call underlying kernel */
				VLIB_packMask32(
					(unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->indata_InArrID1].buf,
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID1].buf,
                                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width)*(C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->height)));
				VLIB_erode_bin_square(
					(unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID1].buf,
                                       ((unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID2].buf)
											+((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width)/8),
                                        (C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width)*((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->height)-2),
                                        C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width);
				VLIB_unpackMask32(					
                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID2].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->outdata_OutArrID3].buf,
                                        ((C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->width)*(C6ACCEL_TI_VLIB_erode_bin_square_opt_paramPtr->height)));
			}
                     }
                 break;	
                 case (DILATE_BIN_MASK_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_dilate_bin_mask_Params *C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr;
                    C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->indata_InArrID1)>INBUF15)||
			((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->msk_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->outdata_OutArrID1)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->cols)%2 != 0)||
                        (((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->cols)*((C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->pitch)-2))%8 !=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_dilate_bin_mask((const unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->outdata_OutArrID1].buf,
					(const	char *)inBufs->descs[C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->msk_InArrID2].buf,
                                        C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->cols,
                                        C6ACCEL_TI_VLIB_dilate_bin_mask_paramPtr->pitch);
                     }
                 break;
                 case (ERODE_BIN_MASK_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_erode_bin_mask_Params *C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr;
                    C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->indata_InArrID1)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->msk_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->outdata_OutArrID1)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->cols)%2 != 0)||
                        (((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->cols)*((C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->pitch)-2))%8 !=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       VLIB_erode_bin_mask((const unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->outdata_OutArrID1].buf,
					(const	char *)inBufs->descs[C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->msk_InArrID2].buf,
                                        C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->cols,
                                        C6ACCEL_TI_VLIB_erode_bin_mask_paramPtr->pitch);
                     }
                 break;
                 case (INITMEANWITHLUMAS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_initMeanWithLumaS16_Params *C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr->runningMean_OutArrID1)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr->lumaFrame_InArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_initMeanWithLumaS16((short *)outBufs->descs[C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr->runningMean_OutArrID1].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr->lumaFrame_InArrID1].buf,
				       	                 C6ACCEL_TI_VLIB_initMeanWithLumaS16_paramPtr->pixelCount);
                    
                     }
                 break;
                 case (INITMEANWITHLUMAS32_FXN_ID):{
                   /* Unmarshal Parameters */
                    VLIB_initMeanWithLumaS32_Params *C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr;
                     

                    C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr->runningMean_OutArrID1)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr->lumaFrame_InArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_initMeanWithLumaS32((int *)outBufs->descs[C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr->runningMean_OutArrID1].buf,
                                                           (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr->lumaFrame_InArrID1].buf,
				       	                   C6ACCEL_TI_VLIB_initMeanWithLumaS32_paramPtr->pixelCount);
                      
                    }
                 break;
                 case (INITVARWITHCONSTS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_initVarWithConstS16_Params *C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr->runningVar_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_initVarWithConstS16((short *)outBufs->descs[C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr->runningVar_OutArrID1].buf,
                                                        (short)C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr->constVar,
				       	                C6ACCEL_TI_VLIB_initVarWithConstS16_paramPtr->pixelCount);
                     
                     }
                 break;
                 case (INITVARWITHCONSTS32_FXN_ID):{
                   /* Unmarshal Parameters */
                    VLIB_initVarWithConstS32_Params *C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr;
                     

                    C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr= pFnArray;
                  
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr->runningVar_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_initVarWithConstS32((int *)outBufs->descs[C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr->runningVar_OutArrID1].buf,
                                                           (int)C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr->constVar,
				       	                   C6ACCEL_TI_VLIB_initVarWithConstS32_paramPtr->pixelCount);
                      
                    }
                 break;
                 case (UPDATEEWRMEANS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateEWRMeanS16_Params *C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->runningMean_InOutArrID1)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->mask32packed_InArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateEWRMeanS16((short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->runningMean_InOutArrID1].buf,
                                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->newLuma_InArrID1].buf,
                                                     (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->mask32packed_InArrID2].buf,
                                                     C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->weight,
				       	             C6ACCEL_TI_VLIB_updateEWRMeanS16_paramPtr->pixelCount);
                     
                     
                     }
                 break;
                 case (UPDATEEWRVARIANCES16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateEWRVarianceS16_Params *C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->runningVar_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->runningMean_InOutArrID2)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->mask32packed_InArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateEWRVarianceS16((short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->runningVar_InOutArrID1].buf,
                                                         (short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->runningMean_InOutArrID2].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->newLuma_InArrID1].buf,
                                                         (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->mask32packed_InArrID2].buf,
                                                         C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->weight,
				       	                 C6ACCEL_TI_VLIB_updateEWRVarianceS16_paramPtr->pixelCount);
                    
                     
                     }
                 break;
                 case (UPDATEEWRMEANS32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateEWRMeanS32_Params *C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->runningMean_InOutArrID1)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->mask32packed_InArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateEWRMeanS32((int *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->runningMean_InOutArrID1].buf,
                                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->newLuma_InArrID1].buf,
                                                     (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->mask32packed_InArrID2].buf,
                                                     C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->weight,
				       	             C6ACCEL_TI_VLIB_updateEWRMeanS32_paramPtr->pixelCount);
                    
                    
                   }
                 break;
                 case (UPDATEEWRVARIANCES32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateEWRVarianceS32_Params *C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->runningVar_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->runningMean_InOutArrID2)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->mask32packed_InArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateEWRVarianceS32((int *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->runningVar_InOutArrID1].buf,
                                                         (int *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->runningMean_InOutArrID2].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->newLuma_InArrID1].buf,
                                                         (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->mask32packed_InArrID2].buf,
                                                         C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->weight,
				       	                 C6ACCEL_TI_VLIB_updateEWRVarianceS32_paramPtr->pixelCount);
                     
                   
                     }
                 break;
                  case (UPDATEUWRMEANS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateUWRMeanS16_Params *C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->updatedMean_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->previousMean_InOutArrID2)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->newestData_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->oldestData_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->newestMask32packed_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->oldestMask32packed_InArrID4)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateUWRMeanS16((short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->updatedMean_InOutArrID1].buf,
                                                     (short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->previousMean_InOutArrID2].buf,
                                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->newestData_InArrID1].buf,
                                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->oldestData_InArrID2].buf,
                                                     (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->newestMask32packed_InArrID3].buf,
                                                     (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->oldestMask32packed_InArrID4].buf,
                                                     C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->pixelCount,
				       	             C6ACCEL_TI_VLIB_updateUWRMeanS16_paramPtr->frameCount);
                    
                   
                     }
                 break;
                 case (UPDATEUWRVARIANCES16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_updateUWRVarianceS16_Params *C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->updatedVar_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->updatedMean_InOutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->previousMean_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->previousVar_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->newestData_InArrID3)>INBUF15)||
		        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->newestMask32packed_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->oldestMask32packed_InArrID5)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_updateUWRVarianceS16((short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->updatedVar_InOutArrID1].buf,
                                                         (short *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->previousVar_InArrID2].buf,
                                                         (short *)inOutBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->updatedMean_InOutArrID2].buf,
                                                         (short *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->previousMean_InArrID1].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->newestData_InArrID3].buf,
                                                         (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->newestMask32packed_InArrID4].buf,
                                                         (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->oldestMask32packed_InArrID5].buf,
                                                         C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->pixelCount,
				       	                 C6ACCEL_TI_VLIB_updateUWRVarianceS16_paramPtr->frameCount);
                     
                    
                     }
                 break;
                 case (SUBTRACTBACKGROUNDS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_subtractBackgroundS16_Params *C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->mask32packed_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->runningMean_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->runningVar_InArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_subtractBackgroundS16((unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->mask32packed_OutArrID1].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->newLuma_InArrID1].buf,
                                                         (short *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->runningMean_InArrID2].buf,
                                                         (short *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->runningVar_InArrID3].buf,
                                                         C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->thresholdGlobal,
                                                         C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->thresholdFactor,
				       	                 C6ACCEL_TI_VLIB_subtractBackgroundS16_paramPtr->PixelCount);
                     
                               
                   
                     }
                 break;
                 case (SUBTRACTBACKGROUNDS32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_subtractBackgroundS32_Params *C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->mask32packed_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->newLuma_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->runningMean_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->runningVar_InArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_subtractBackgroundS32((unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->mask32packed_OutArrID1].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->newLuma_InArrID1].buf,
                                                         (int *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->runningMean_InArrID2].buf,
                                                         (int *)inBufs->descs[C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->runningVar_InArrID3].buf,
                                                         C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->thresholdGlobal,
                                                         C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->thresholdFactor,
				       	                 C6ACCEL_TI_VLIB_subtractBackgroundS32_paramPtr->PixelCount);
                     
                     
                     }
                 break;
                 case (MIXTUREOFGAUSSIANSS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_mixtureOfGaussiansS16_Params *C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr;
                     
                    C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr = pFnArray;
                   

                    if (((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->inputIm_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentWts_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentMeans_InOutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentVars_InOutArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->compIndex_InOutArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->intBuffer_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->fgmask_OutArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                     VLIB_mixtureOfGaussiansS16((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->inputIm_InArrID1].buf,
                                                         ( short *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentWts_InOutArrID1].buf,
                                                         ( short *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentMeans_InOutArrID2].buf,
                                                         ( short *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->currentVars_InOutArrID3].buf,
                                                         (unsigned char *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->compIndex_InOutArrID4].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->intBuffer_InArrID1].buf,
                                                         (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->fgmask_OutArrID2].buf,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->imageSize,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->updateRate1,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->updateRate2,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->mdThreshold,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->bsThreshold,
				       	                 C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->initialWt,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS16_paramPtr->initialVar);
                     
                     }
                 break;
                 case (MIXTUREOFGAUSSIANSS32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_mixtureOfGaussiansS32_Params *C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr;
                     
                    C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr = pFnArray;
                    

                    if (((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->inputIm_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentWts_InOutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentMeans_InOutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentVars_InOutArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->compIndex_InOutArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->intBuffer_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->fgmask_OutArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                     VLIB_mixtureOfGaussiansS32((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->inputIm_InArrID1].buf,
                                                         ( short *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentWts_InOutArrID1].buf,
                                                         ( int *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentMeans_InOutArrID2].buf,
                                                         ( int *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->currentVars_InOutArrID3].buf,
                                                         (unsigned char *)inOutBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->compIndex_InOutArrID4].buf,
                                                         (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->intBuffer_InArrID1].buf,
                                                         (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->fgmask_OutArrID2].buf,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->imageSize,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->updateRate1,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->updateRate2,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->mdThreshold,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->bsThreshold,
				       	                 C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->initialWt,
                                                         C6ACCEL_TI_VLIB_mixtureOfGaussiansS32_paramPtr->initialVar);
                   
                     }
                 break;
                 case (TRACKFEATURESLUCASKANADE_7x7_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_trackFeaturesLucasKanade_7x7_Params *C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr;
                     
                    C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr = pFnArray;
                   
                     if (((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->im1_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->im2_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->gradX_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->gradY_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->x_InArrID5)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->y_InArrID6)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->outx_InOutArrID1)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->outy_InOutArrID2)>OUTBUF15)||
                        ((C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->scratch_InOutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                      VLIB_trackFeaturesLucasKanade_7x7((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->im1_InArrID1].buf,
                                                                  (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->im2_InArrID2].buf,
                                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->gradX_InArrID3].buf,
                                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->gradY_InArrID4].buf,
                                                                  C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->width,
                                                                  C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->height,
                                                                  C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->nfeatures, 
                                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->x_InArrID5].buf,
                                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->y_InArrID6].buf,
                                                                  (short *)inOutBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->outx_InOutArrID1].buf,
                                                                  (short *)inOutBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->outy_InOutArrID2].buf,
                                                                  C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr->max_iters,
                                                                  (unsigned char *)inOutBufs->descs[C6ACCEL_TI_VLIB_trackFeaturesLucasKanade_7x7_paramPtr-> scratch_InOutArrID3].buf);

                     
                     }
                 break;
                 case (XYGRADIENTSANDMAGNITUDE_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_xyGradientsAndMagnitude_Params *C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr;
                     
                    C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr = pFnArray;
                   
                     if (((C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pInBlk_InArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufGradX_OutArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufGradY_OutArrID2)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufMag_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                      VLIB_xyGradientsAndMagnitude((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pInBlk_InArrID1].buf,
                                                              (short *)outBufs->descs[C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufGradX_OutArrID1].buf,
                                                              (short *)outBufs->descs[C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufGradY_OutArrID2].buf,   
                                                              (short *)outBufs->descs[C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->pBufMag_OutArrID3].buf,
                                                              C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->width,
                                                              C6ACCEL_TI_VLIB_xyGradientsAndMagnitude_paramPtr->height);

                    
                     }
                 break;
                 case (NORMALFLOW_16_FXN_ID):{
                    /* Unmarshal Parameters */
                   VLIB_normalFlow_16_Params *C6ACCEL_TI_VLIB_normalFlow_16_paramPtr;
                   
                    C6ACCEL_TI_VLIB_normalFlow_16_paramPtr = pFnArray;
                    
                     if (((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->inDiff_InArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Emag_InArrID2)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Ex_InArrID3)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Ey_InArrID4)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->LUT_InArrID5)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->normalFlowU_OutArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->normalFlowV_OutArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     VLIB_normalFlow_16((short *)inBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->inDiff_InArrID1].buf,
                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Emag_InArrID2].buf,
                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Ex_InArrID3].buf,
                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->Ey_InArrID4].buf,
                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->LUT_InArrID5].buf,
                                                           C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->T,
                                                           C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->numPixels,   
                                                  (short *)outBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->normalFlowU_OutArrID1].buf,
                                                  (short *)outBufs->descs[C6ACCEL_TI_VLIB_normalFlow_16_paramPtr->normalFlowV_OutArrID2].buf);

                    
                     }
                 break;
                 case (HARRISSCORE_7x7_FXN_ID):{
                    /* Unmarshal Parameters */
                   VLIB_harrisScore_7x7_Params *C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr;
                     
                    C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr = pFnArray;
                    
                     if (((C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->gradX_InArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->gradY_InArrID2)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->buffer_InOutArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->harrisscore_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                      VLIB_harrisScore_7x7((short *)inBufs->descs[C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->gradX_InArrID1].buf,
                                                  (short *)inBufs->descs[C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->gradY_InArrID2].buf,
                                                           C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->width,
                                                           C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->height,   
                                                  (short *)outBufs->descs[C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->harrisscore_OutArrID1].buf,
                                                           C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->k,
                                                  (unsigned char *)inOutBufs->descs[C6ACCEL_TI_VLIB_harrisScore_7x7_paramPtr->buffer_InOutArrID1].buf);

                    
                     }
                 break;
                 case (HOUGHLINEFROMLIST_FXN_ID):{
                    /* Unmarshal Parameters */
                   VLIB_houghLineFromList_Params *C6ACCEL_TI_VLIB_houghLineFromList_paramPtr;
                     
                    C6ACCEL_TI_VLIB_houghLineFromList_paramPtr = pFnArray;
                    
                     if (((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pEdgeMapList_InArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pOutHoughSpace_OutArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pSIN_InArrID2)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pCOS_InArrID3)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->ping_InOutArrID1)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pong_InOutArrID2)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pang_InOutArrID3)>INBUF15)||
                         ((C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->peng_InOutArrID4)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     VLIB_houghLineFromList((unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pEdgeMapList_InArrID1].buf,
                                                      (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pOutHoughSpace_OutArrID1].buf,
                                                           C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->outBlkWidth,
                                                           C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->outBlkHeight,
                                                           C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->listSize,
                                                           C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->thetaRange,
                                                           C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->rhoMaxLength,
                                                      (short *)inBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pSIN_InArrID2].buf,
                                                      (short *)inBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pCOS_InArrID3].buf,
                                                      (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->ping_InOutArrID1].buf,
                                                      (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pong_InOutArrID2].buf,
                                                      (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->pang_InOutArrID3].buf,
                                                      (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_houghLineFromList_paramPtr->peng_InOutArrID4].buf);

                     
                     }
                 break;
                 case (IMAGEPYRAMID8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_imagePyramid8_Params *C6ACCEL_TI_VLIB_imagePyramid8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_imagePyramid8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_imagePyramid8(     (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->inCols,
				       	               C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->inRows,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_imagePyramid8_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (IMAGEPYRAMID16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_imagePyramid16_Params *C6ACCEL_TI_VLIB_imagePyramid16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_imagePyramid16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_imagePyramid16(    (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->inCols,
				       	               C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->inRows,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_imagePyramid16_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (LEGENDREMOMENTS_INIT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_legendreMoments_Init_Params *C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr;
                    // 
                    
                    C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->LPoly_OutArrID1)>OUTBUF15)|| 
                        ((C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->Constant_OutArrID2)>OUTBUF15) !=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_legendreMoments_Init((short *)outBufs->descs[C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->LPoly_OutArrID1].buf,
                                                       C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->Order,
				       	               C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->imH,
                                               (short *)outBufs->descs[C6ACCEL_TI_VLIB_legendreMoments_Init_paramPtr->Constant_OutArrID2].buf);
                                          
                     }
                 break;
                 case (LEGENDREMOMENTS_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_legendreMoments_Params *C6ACCEL_TI_VLIB_legendreMoments_paramPtr;
                    // 
                    
                    C6ACCEL_TI_VLIB_legendreMoments_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_legendreMoments_paramPtr->LPoly_InArrID2)>INBUF15)||
                          ((C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Im_InArrID1)>INBUF15)||
                          ((C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Lmoments_OutArrID1)>INBUF15) !=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_legendreMoments((short *)inBufs->descs[C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Im_InArrID1].buf,
                                                       (short  *)outBufs->descs[C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Lmoments_OutArrID1].buf,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_legendreMoments_paramPtr->LPoly_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Order,
				       	               C6ACCEL_TI_VLIB_legendreMoments_paramPtr->imH,
                                                       C6ACCEL_TI_VLIB_legendreMoments_paramPtr->Constant);
                                       
                     }
                 break;  
                 case (GAUSS5X5PYRAMIDKERNEL_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_gauss5x5PyramidKernel_8_Params *C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pB_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_gauss5x5PyramidKernel_8((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pIn_InArrID1].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pB_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->inCols,
                                                       C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pitch,
				       	               C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->inRows,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_8_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (GAUSS5X5PYRAMIDKERNEL_16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_gauss5x5PyramidKernel_16_Params *C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pB_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_gauss5x5PyramidKernel_16((unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pIn_InArrID1].buf,
                                                       (unsigned int *)inBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pB_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->inCols,
                                                       C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pitch,
				       	               C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->inRows,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_gauss5x5PyramidKernel_16_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (GRADIENTH5X5PYRAMIDKERNEL_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_gradientH5x5PyramidKernel_8_Params *C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pB_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_gradientH5x5PyramidKernel_8( (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pIn_InArrID1].buf,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pB_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->inCols,
                                                       C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pitch,
				       	               C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->inRows,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_gradientH5x5PyramidKernel_8_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (GRADIENTV5X5PYRAMIDKERNEL_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_gradientV5x5PyramidKernel_8_Params *C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pB_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_gradientV5x5PyramidKernel_8((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pIn_InArrID1].buf,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pB_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->inCols,
                                                       C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pitch,
				       	               C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->inRows,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_gradientV5x5PyramidKernel_8_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (RECURSIVEFILTERVERT1STORDER_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_recursiveFilterVert1stOrder_Params *C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_recursiveFilterVert1stOrder((unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->pOut_OutArrID1].buf,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->width,
                                                       C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->height,
				       	               C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->weight,
                                                       NULL,
                                                       NULL,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrder_paramPtr->buffer_InArrID2].buf
                                                       );
                     
                     
                     }
                 break;
                 case (RECURSIVEFILTERHORIZ1STORDER_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_recursiveFilterHoriz1stOrder_Params *C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_recursiveFilterHoriz1stOrder((unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->pOut_OutArrID1].buf,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->width,
                                                       C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->height,
				       	               C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->weight,
                                                       NULL,
                                                       NULL,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrder_paramPtr->buffer_InArrID2].buf
                                                       );
                     
                     
                     }
                 break;
                 case (RECURSIVEFILTERVERT1STORDERS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_recursiveFilterVert1stOrderS16_Params *C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_recursiveFilterVert1stOrderS16(( short *)outBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->pOut_OutArrID1].buf,
                                                       ( short *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->width,
                                                       C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->height,
				       	               C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->weight,
                                                       NULL,
                                                       NULL,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterVert1stOrderS16_paramPtr->buffer_InArrID2].buf
                                                       );
                     
                     
                     }
                 break;
                 case (RECURSIVEFILTERHORIZ1STORDERS16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_recursiveFilterHoriz1stOrderS16_Params *C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_recursiveFilterHoriz1stOrderS16((short *)outBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->pOut_OutArrID1].buf,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->width,
                                                       C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->height,
				       	               C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->weight,
                                                       NULL,
                                                       NULL,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_recursiveFilterHoriz1stOrderS16_paramPtr->buffer_InArrID2].buf
                                                       );
                     
                     
                     }
                 break;
                 case (INTEGRALIMAGE8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_integralImage8_Params *C6ACCEL_TI_VLIB_integralImage8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_integralImage8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_integralImage8_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_integralImage8_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_integralImage8(    (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_integralImage8_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_integralImage8_paramPtr->inCols,
				       	               C6ACCEL_TI_VLIB_integralImage8_paramPtr->inRows,
                                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_integralImage8_paramPtr->pLastLine_OutArrID2].buf,
                                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_integralImage8_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (INTEGRALIMAGE16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_integralImage16_Params *C6ACCEL_TI_VLIB_integralImage16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_integralImage16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_integralImage16_paramPtr->pIn_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_integralImage16_paramPtr->pOut_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_integralImage16(    (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_integralImage16_paramPtr->pIn_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_integralImage16_paramPtr->inCols,
				       	               C6ACCEL_TI_VLIB_integralImage16_paramPtr->inRows,
                                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_integralImage16_paramPtr->pLastLine_OutArrID2].buf,
                                                       (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_integralImage16_paramPtr->pOut_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (HISTOGRAM_1D_INIT_U8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_histogram_1D_Init_U8_Params *C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr->binEdges_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr->histArray_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_histogram_1D_Init_U8( (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr->binEdges_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr->numBins,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_Init_U8_paramPtr->histArray_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (HISTOGRAM_1D_INIT_U16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_histogram_1D_Init_U16_Params *C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr->binEdges_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr->histArray_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_histogram_1D_Init_U16( (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr->binEdges_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr->numBins,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_Init_U16_paramPtr->histArray_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (HISTOGRAM_1D_U8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_histogram_1D_U8_Params *C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->histArray_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH1_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH2_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH3_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->H_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_histogram_1D_U8( (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->numBins,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->binWeight,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->histArray_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH1_InArrID3].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH2_InArrID4].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->internalH3_InArrID5].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U8_paramPtr->H_OutArrID1].buf
                                                    );
                    
                     
                     }
                 break;
                 case (WEIGHTEDHISTOGRAM_1D_U8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_weightedHistogram_1D_U8_Params *C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->binWeight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->histArray_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH1_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH2_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH3_InArrID6)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->H_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_weightedHistogram_1D_U8( (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->numBins,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->binWeight_InArrID2].buf,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->histArray_InArrID3].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH1_InArrID4].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH2_InArrID5].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->internalH3_InArrID6].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U8_paramPtr->H_OutArrID1].buf
                                                     );

                    
                     
                     }
                 break;
                 case (HISTOGRAM_1D_U16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_histogram_1D_U16_Params *C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->histArray_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->internalH1_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->H_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_histogram_1D_U16( (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->numBins,
                                                       C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->binWeight,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->histArray_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->internalH1_InArrID3].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_histogram_1D_U16_paramPtr->H_OutArrID1].buf
                                                    );
                    
                     }
                 break;
                 case (WEIGHTEDHISTOGRAM_1D_U16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_weightedHistogram_1D_U16_Params *C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->binWeight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->histArray_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->internalH1_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->H_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_weightedHistogram_1D_U16( (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->numBins,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->binWeight_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->histArray_InArrID3].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->internalH1_InArrID4].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_1D_U16_paramPtr->H_OutArrID1].buf
                                                     );
                    
                     
                     }
                 break;
                 case (HISTOGRAM_ND_U16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_histogram_nD_U16_Params *C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->numBins_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->normVals_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->internalH1_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->internalH2_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->H_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_histogram_nD_U16( (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->dimX,
                                                       C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->binWeight,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->numBins_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->normVals_InArrID3].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->internalH1_InArrID4].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->internalH2_InArrID5].buf,
                                                       (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_histogram_nD_U16_paramPtr->H_InOutArrID1].buf
                                                    );
                     
                     
                     }
                 break;
                 case (WEIGHTEDHISTOGRAM_ND_U16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_weightedHistogram_nD_U16_Params *C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->binWeight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->numBins_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->internalH1_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->internalH2_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->normVals_InArrID6)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->H_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_weightedHistogram_nD_U16( (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->X_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->numX,
                                                       C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->dimX,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->binWeight_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->numBins_InArrID3].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->normVals_InArrID6].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->internalH1_InArrID4].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->internalH2_InArrID5].buf,
                                                       (unsigned short *)inOutBufs->descs[C6ACCEL_TI_VLIB_weightedHistogram_nD_U16_paramPtr->H_InOutArrID1].buf
                                                     );
                     
                     
                     }
                 break;
                 case (BHATTACHARYADISTANCE_U32_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_bhattacharyaDistance_U32_Params *C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr;
                    
                    
                    C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->Y_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->D_OutArrID1)>OUTBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_bhattacharyaDistance_U32(    (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->X_InArrID1].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->Y_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->N,
				       	               (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_bhattacharyaDistance_U32_paramPtr->D_OutArrID1].buf);
                       
                                         
                     }
                 break;
                 case (L1DISTANCES16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_L1DistanceS16_Params *C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr;
                     
                    
                    C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->X_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->Y_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->L1D_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_L1DistanceS16(    (short *)inBufs->descs[C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->X_InArrID1].buf,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->Y_InArrID2].buf,
                                                       C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->N,
				       	               (unsigned int *)outBufs->descs[C6ACCEL_TI_VLIB_L1DistanceS16_paramPtr->L1D_OutArrID1].buf);
                     
                     
                     }
                 break;
                 case (EXTRACTLUMAFROMUYUV_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_extractLumaFromUYUV_Params *C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->inputImage_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->outputImage_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_extractLumaFromUYUV(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->inputImage_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->inputWidth,
				       	               C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->inputPitch,
                                                       C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->inputHeight,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_extractLumaFromUYUV_paramPtr->outputImage_OutArrID1].buf);
                    
                     
                     }
                 break;
                 case (CONVERTUYVYINT_TO_YUVPL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_YUVpl_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->y_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->cr_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->cb_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_YUVpl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->height,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->y_OutArrID1].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->cr_OutArrID2].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUVpl_paramPtr->cb_OutArrID3].buf);
                   
                     
                     }
                 break;
                 case (CONVERTUYVYINT_TO_YUV420PL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_YUV420pl_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->y_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->cr_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->cb_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_YUV420pl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->height,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->y_OutArrID1].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->cr_OutArrID2].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_YUV420pl_paramPtr->cb_OutArrID3].buf);
                    
                     
                     }
                 break;
                 case (CONVERTUYVYINT_TO_HSLPL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_HSLpl_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->coeff_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->div_table_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->H_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->S_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->L_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_HSLpl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->height,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->coeff_InArrID2].buf,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->div_table_InArrID3].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->H_OutArrID1].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->S_OutArrID2].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_HSLpl_paramPtr->L_OutArrID3].buf);
                     
                     
                     }
                 break; 
                 case (CONVERTUYVYINT_TO_LABPL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_LABpl_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr= pFnArray;
                   
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->coeff_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->whitePoint_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->L_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->a_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->b_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_LABpl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->height,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->coeff_InArrID2].buf,
                                                       (float *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->whitePoint_InArrID3].buf,
                                                       (float *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->L_OutArrID1].buf,
                                                       (float *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->a_OutArrID2].buf,
                                                       (float *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_paramPtr->b_OutArrID3].buf);
                     
                     
                     }
                 break;      
                 case (CONVERTUYVYINT_TO_RGBPL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_RGBpl_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->coeff_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->R_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->G_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->B_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_RGBpl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->height,
                                                       (short *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->coeff_InArrID2].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->R_OutArrID1].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->G_OutArrID2].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_RGBpl_paramPtr->B_OutArrID3].buf);
                     
                     
                     }
                 break;      
                 case (CONVERTUYVYINT_TO_LABPL_LUT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYint_to_LABpl_LUT_Params *C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->yc_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->LabLUT_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->L_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->a_OutArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->b_OutArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYint_to_LABpl_LUT(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->yc_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->height,
                                                       C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->d,
                                                       (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->LabLUT_InArrID2].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->L_OutArrID1].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->a_OutArrID2].buf,
                                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYint_to_LABpl_LUT_paramPtr->b_OutArrID3].buf);
                     
                     
                     }
                 break;        
                 case (INITUYVYINT_TO_LABPL_LUT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_initUYVYint_to_LABpl_LUT_Params *C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->coeff_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->whitePoint_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->Lab_OutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_initUYVYint_to_LABpl_LUT( C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->d,
                                                              (short *)inBufs->descs[C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->coeff_InArrID1].buf,
                                                              (float *)inBufs->descs[C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->whitePoint_InArrID2].buf,
                                                              (unsigned short *)outBufs->descs[C6ACCEL_TI_VLIB_initUYVYint_to_LABpl_LUT_paramPtr->Lab_OutArrID1].buf);
                     
                   }
                 break;
                 case (CONVERTUYVYSEMIPL_TO_YUVPL_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_convertUYVYsemipl_to_YUVpl_Params *C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->crcb_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->cr_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->cb_OutArrID2)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYsemipl_to_YUVpl(( unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->crcb_InArrID1].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->height,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->cr_OutArrID1].buf,
                                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYsemipl_to_YUVpl_paramPtr->cb_OutArrID2].buf);
                     
                     
                     }
                 break;     
                 case (CONVERTUYVYPL_TO_YUVINT_FXN_ID):{
                      /* Unmarshal Parameters */
                    VLIB_convertUYVYpl_to_YUVint_Params *C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->yc_OutArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->y_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->cr_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->cb_InArrID3)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_convertUYVYpl_to_YUVint((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->y_InArrID1].buf,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->cr_InArrID2].buf,
                                                       (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->cb_InArrID3].buf,
                                                       C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->width,
				       	               C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->pitch,
                                                       C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->height,
                                                       ( unsigned char *)outBufs->descs[C6ACCEL_TI_VLIB_convertUYVYpl_to_YUVint_paramPtr->yc_OutArrID1].buf);
                     
                     
                     }
                 break;  
                 case (DISPARITY_SAD_FIRSTROW8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_disparity_SAD_firstRow8_Params *C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pLeft_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pRight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pCost_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pMinCost_InArrID4)>INBUF15)||
                        
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pDisparity_OutArrID1)>OUTBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_disparity_SAD_firstRow8((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pLeft_InArrID1].buf,
                                                            (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pRight_InArrID2].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pCost_InArrID3].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pMinCost_InArrID4].buf,
                                                            (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pScratch_InArrID5].buf,
                                                            (char *)inOutBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pDisparity_OutArrID1].buf,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->displacement,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->width,
				       	                    C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->pitch,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow8_paramPtr->windowSize
                                                            );
                     
                     
                     }
                 break;        
                 case (DISPARITY_SAD8_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_disparity_SAD8_Params *C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pLeft_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pRight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pCost_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pMinCost_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pScratch_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pDisparity_OutArrID1)>OUTBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_disparity_SAD8((unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pLeft_InArrID1].buf,
                                                            (unsigned char *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pRight_InArrID2].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pCost_InArrID3].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pMinCost_InArrID4].buf,
                                                            (char *)inOutBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pDisparity_OutArrID1].buf,
                                                            C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->displacement,
                                                            C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->width,
				       	                    C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->pitch,
                                                            C6ACCEL_TI_VLIB_disparity_SAD8_paramPtr->windowSize
                                                            );
                     
                     
                     }
                 break;                        
                 case (DISPARITY_SAD_FIRSTROW16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_disparity_SAD_firstRow16_Params *C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pLeft_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pRight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pCost_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pMinCost_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pScratch_InArrID5)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pDisparity_OutArrID1)>OUTBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_disparity_SAD_firstRow16((unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pLeft_InArrID1].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pRight_InArrID2].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pCost_InArrID3].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pMinCost_InArrID4].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pScratch_InArrID5].buf,
                                                            (char *)outBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pDisparity_OutArrID1].buf,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->displacement,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->width,
				       	                    C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->pitch,
                                                            C6ACCEL_TI_VLIB_disparity_SAD_firstRow16_paramPtr->windowSize
                                                            );
                     
                     
                     }
                 break;        
                 case (DISPARITY_SAD16_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_disparity_SAD16_Params *C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr;
                   
                    
                    C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pLeft_InArrID1)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pRight_InArrID2)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pCost_InArrID3)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pMinCost_InArrID4)>INBUF15)||
                        ((C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pDisparity_OutArrID1)>OUTBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_disparity_SAD16((unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pLeft_InArrID1].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pRight_InArrID2].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pCost_InArrID3].buf,
                                                            (unsigned short *)inBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pMinCost_InArrID4].buf,
                                                            (char *)outBufs->descs[C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pDisparity_OutArrID1].buf,
                                                            C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->displacement,
                                                            C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->width,
				       	                    C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->pitch,
                                                            C6ACCEL_TI_VLIB_disparity_SAD16_paramPtr->windowSize
                                                            );
                     
                     
                     }
                 break; 
                 case (KALMANFILTER_2X4_PREDICT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_kalmanFilter_2x4_Predict_Params *C6ACCEL_TI_VLIB_kalmanFilter_2x4_Predict_paramPtr;
                                       
                    C6ACCEL_TI_VLIB_kalmanFilter_2x4_Predict_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_kalmanFilter_2x4_Predict_paramPtr->KF_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_kalmanFilter_2x4_Predict((VLIB_kalmanFilter_2x4 *)inOutBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_2x4_Predict_paramPtr->KF_InOutArrID1].buf);
                      
                     }
                 break;  
                 case (KALMANFILTER_2X4_CORRECT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_kalmanFilter_2x4_Correct_Params *C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr;
                                       
                    C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr->KF_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_kalmanFilter_2x4_Correct((VLIB_kalmanFilter_2x4 *)  inOutBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr->KF_InOutArrID1].buf,
                                             (short *)inBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr->Z_InArrID1].buf,
                                             (short *)outBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_2x4_Correct_paramPtr->residual_OutArrID1].buf);
                      
                     }
                 break;   
                 case (KALMANFILTER_4X6_PREDICT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_kalmanFilter_4x6_Predict_Params *C6ACCEL_TI_VLIB_kalmanFilter_4x6_Predict_paramPtr;
                                       
                    C6ACCEL_TI_VLIB_kalmanFilter_4x6_Predict_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_kalmanFilter_4x6_Predict_paramPtr->KF_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_kalmanFilter_4x6_Predict((VLIB_kalmanFilter_4x6 *)inOutBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Predict_paramPtr->KF_InOutArrID1].buf);
                      
                     }
                 break;
                 case (KALMANFILTER_4X6_CORRECT_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_kalmanFilter_4x6_Correct_Params *C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr;
                                       
                    C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->KF_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_kalmanFilter_4x6_Correct((VLIB_kalmanFilter_4x6 *)  inOutBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->KF_InOutArrID1].buf,
                                             (short *)inBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->Z_InArrID1].buf,
                                             (short *)outBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->residual_OutArrID1].buf);
                      
                     }
                 break; 
                 case (CALCCONNECTEDCOMPONENTSMAXBUFFERSIZE_FXN_ID):{
                    /* Unmarshal Parameters */
                    VLIB_kalmanFilter_4x6_Correct_Params *C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr;
                                       
                    C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr= pFnArray;
                    
                    /*Parameter check*/
                    if (((C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->KF_InOutArrID1)>INBUF15)!=0){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                     VLIB_kalmanFilter_4x6_Correct((VLIB_kalmanFilter_4x6 *)  inOutBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->KF_InOutArrID1].buf,
                                             (short *)inBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->Z_InArrID1].buf,
                                             (short *)outBufs->descs[C6ACCEL_TI_VLIB_kalmanFilter_4x6_Correct_paramPtr->residual_OutArrID1].buf);
                      
                     }
                 break; 
                                                         
                 default:
                    /*Error caused due to passing of an invalid ID*/
                         return(IUNIVERSAL_EFXNIDFAIL);
          }
          
    return 0;
}



/*=============================================================================*/
/*                  End of File C6accel_ti_imglibFunctionCall.c                */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
