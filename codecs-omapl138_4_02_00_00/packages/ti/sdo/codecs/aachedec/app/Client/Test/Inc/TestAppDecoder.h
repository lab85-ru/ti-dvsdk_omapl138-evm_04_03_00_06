/** ==================================================================
* @file TestAppDecoder.h
*
*
* =====================================================================
* Copyright (c) Texas Instruments Inc 2004-2006
*
* Use of this software is controlled by the terms and conditions found
*in the license agreement under which this software has been supplied *
=====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdas.h>

#include "iauddec1.h"
#include "iaudio.h"
#include "xdm.h"


typedef XDAS_Int8  Word8;
typedef XDAS_UInt8 UWord8;
typedef XDAS_Int16  Word16;
typedef XDAS_UInt16 UWord16;
typedef XDAS_Int32  Word32;
typedef XDAS_UInt32 UWord32;



#define INPUT_BUFFER_SIZE  (3840)
#define OUTPUT_BUFFER_SIZE (4096)
#define FILE_STRING_SIZE 150
#define NO_OF_IO_BUFFERS 1

void WriteToFile(FILE *outputFile,XDAS_Int8 *out, IAUDDEC1_Status* status);

XDAS_Int16 CompareOutputData(FILE *outputFile,UWord16 *out, Word16 nch,Word32 nSamples);

