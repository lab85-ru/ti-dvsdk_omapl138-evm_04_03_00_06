/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2007-2012 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#define MAX_U16 65535
#define MAX_S16 32767
#define MAX_U32 4294967295
#define MAX_S32 2147483647
#define MAX_8p8 256
#define MAX_0p16 1
#define MAX_12p4 4095
#define MAX_4p12 16
#define MAX_16p16 65536

#define NUM_PIXELS 32
#define NUM_IMAGES 10

#define L1DBYTES  48*1024
#define L2DBYTES  48*1024
#define BUF0BYTES 48*1024
#define BUF1BYTES 48*1024
#define DDR2HEAP 0 

void VLIB_printErrCode(int status);
float getFractional16(short A, int point, int sign);
float getFractional32(int A, int point, int sign); 
void LegendreMoments_imageReconstruct(float* rIm, int im_h, int im_w, int order, short *Pr, short *Pc, short *L);
int VLIB_readPGMFile(
	char 			* fileName, 
    unsigned char 	* image, 
    unsigned short 	* rows,
    unsigned short  * cols
);

