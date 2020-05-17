/*
 * common.c
 *
 * Source file having common functions and data srtuctures for the demo
 * application.
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

#include "common.h"

/* Flag variable to check transfer completion on channel 1 */
volatile short irqRaised1 = 0;
/* Flag variable to check transfer completion on channel 2 */
volatile short irqRaised2 = 0;
/* Flag variable to check transfer completion on channel 3 */
volatile short irqRaised3 = 0;


/* Cache line aligned source buffer 1. */
#ifdef EDMA3_ENABLE_DCACHE
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_srcBuff1, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _srcBuff1[MAX_BUFFER_SIZE];


/* Cache line aligned destination buffer 1. */
#ifdef EDMA3_ENABLE_DCACHE
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_dstBuff1, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _dstBuff1[MAX_BUFFER_SIZE];

signed char *srcBuff1;
signed char *dstBuff1;



/* Cache line aligned source buffer 2. */
#ifdef EDMA3_ENABLE_DCACHE
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_srcBuff2, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _srcBuff2[MAX_BUFFER_SIZE];


#ifdef EDMA3_ENABLE_DCACHE
/* Cache line aligned destination buffer 2. */
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_dstBuff2, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _dstBuff2[MAX_BUFFER_SIZE];

signed char *srcBuff2;
signed char *dstBuff2;


/* Cache line aligned source buffer 3. */
#ifdef EDMA3_ENABLE_DCACHE
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_srcBuff3, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _srcBuff3[MAX_BUFFER_SIZE];


/* Cache line aligned destination buffer 3. */
#ifdef EDMA3_ENABLE_DCACHE
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_dstBuff3, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
signed char   _dstBuff3[MAX_BUFFER_SIZE];

signed char *srcBuff3;
signed char *dstBuff3;


/* Callback function 1 */
void callback1 (unsigned int tcc, EDMA3_RM_TccStatus status,
                        void *appData)
    {
    (void)tcc;
    (void)appData;

    switch (status)
        {
        case EDMA3_RM_XFER_COMPLETE:
            /* Transfer completed successfully */
            irqRaised1 = 1;
            break;

        case EDMA3_RM_E_CC_DMA_EVT_MISS:
            /* Transfer resulted in DMA event miss error. */
            irqRaised1 = -1;

#ifdef IS_MULTI_DSP_DEVICE
			/** Clear the event in the Chip Interrupt Controller (CIC)
			 * Event Flag Register by writing to the Event Clear Register.
			 */
			EDMA3_CIC_EVTCLR1(dsp_num) = 1u << EDMA3_CC_ERROR_EVENT_SHIFT;
#endif

            break;

        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /* Transfer resulted in QDMA event miss error. */
            irqRaised1 = -2;
            break;

        default:
            break;
        }
    }



/* Callback function 2 */
void callback2 (unsigned int tcc, EDMA3_RM_TccStatus status,
                        void *appData)
    {
    (void)tcc;
    (void)appData;

    switch (status)
        {
        case EDMA3_RM_XFER_COMPLETE:
            /* Transfer completed successfully */
            irqRaised2 = 1;
            break;

        case EDMA3_RM_E_CC_DMA_EVT_MISS:
            /* Transfer resulted in DMA event miss error. */
            irqRaised2 = -1;

#ifdef IS_MULTI_DSP_DEVICE
			/** Clear the event in the Chip Interrupt Controller (CIC)
			 * Event Flag Register by writing to the Event Clear Register.
			 */
			EDMA3_CIC_EVTCLR1(dsp_num) = 1u << EDMA3_CC_ERROR_EVENT_SHIFT;
#endif

            break;

        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /* Transfer resulted in QDMA event miss error. */
            irqRaised2 = -2;
            break;

        default:
            break;
        }
    }

