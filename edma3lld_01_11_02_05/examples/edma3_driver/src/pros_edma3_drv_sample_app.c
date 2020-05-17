/*
 * pros_edma3_drv_sample_app.c
 *
 * Sample application for the EDMA3 Driver on PrOS.
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

#include <ti/sdo/edma3/drv/sample/pros_edma3_drv_sample.h>
extern void edma3OsWaitMsecs(unsigned int mSecs);
#define TSK_sleep(a) edma3OsWaitMsecs(10*a)

/* Flag variable to check transfer completion on channel 1 */
static volatile short irqRaised1 = 0;
/* Flag variable to check transfer completion on channel 2 */
static volatile short irqRaised2 = 0;

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
static signed char   _srcBuff1[MAX_BUFFER_SIZE];


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
static signed char   _dstBuff1[MAX_BUFFER_SIZE];


static signed char *srcBuff1;
static signed char *dstBuff1;



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
static signed char   _srcBuff2[MAX_BUFFER_SIZE];


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
static signed char   _dstBuff2[MAX_BUFFER_SIZE];



static signed char *srcBuff2;
static signed char *dstBuff2;


#ifdef EDMA3_PING_PONG_TEST
/** Test Case Description **/
/**
 * There are two big buffers of size (PING_PONG_NUM_COLUMNS * PING_PONG_NUM_ROWS).
 * Both are present in DDR and are known as pingpongSrcBuf and pingpongDestBuf.
 * There are two small buffers of size (PING_PONG_L1D_BUFFER_SIZE). They are known as
 * ping buffer and pong buffer.
 * The pingpongSrcBuf is divided into chunks, each having size of
 * PING_PONG_L1D_BUFFER_SIZE. Data is being transferred from pingpongSrcBuf
 * to either ping or pong buffers, using EDMA3. Logic behind using two ping pong
 * buffers is that one can be processed by DSP while the other is used by EDMA3
 * for data movement. So ping and pong are alternately used by EDMA3 and DSP.
 * Also, to simulate the real world scenario, as a part of DSP processing,
 * I am copying data from ping/pong buffers to pingpongDestBuf.
 * In the end, I compare pingpongSrcBuf and pingpongDestBuf to check whether
 * the algorithm has worked fine.
 */
/**
 * Number of columns in the bigger source buffer.
 */
#define PING_PONG_NUM_COLUMNS           (720u)

/**
 * Number of columns in the bigger source buffer.
 */
#define PING_PONG_NUM_ROWS                (480u)

/* ACNT is equal to number of columns. */
#define PING_PONG_ACNT                          PING_PONG_NUM_COLUMNS
/* BCNT is equal to number of rows which will be transferred in one shot. */
#define PING_PONG_BCNT                          (8u)
/* CCNT is equal to 1. */
#define PING_PONG_CCNT                          (1u)

/* Number of times DMA will be triggered. */
#define PING_PONG_NUM_TRIGGERS           (PING_PONG_NUM_ROWS/PING_PONG_BCNT)

/* Size of bigger buffers in DDR. */
#define PING_PONG_DDR_BUFFER_SIZE     (PING_PONG_NUM_COLUMNS*PING_PONG_NUM_ROWS)
/* Size of smaller buffers in IRAM. */
#define PING_PONG_L1D_BUFFER_SIZE     (PING_PONG_ACNT*PING_PONG_BCNT)

/* Ping pong source buffer */
#ifdef EDMA3_ENABLE_DCACHE
/* Cache line aligned big source buffer. */
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_pingpongSrcBuf, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
static signed char _pingpongSrcBuf[PING_PONG_DDR_BUFFER_SIZE];


/**
 * Ping pong destination buffer.
 * It will be used to copy data from L1D ping/pong buffers to check the
 * validity.
 */
#ifdef EDMA3_ENABLE_DCACHE
/* Cache line aligned big destination buffer. */
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_pingpongDestBuf, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
static signed char _pingpongDestBuf[PING_PONG_DDR_BUFFER_SIZE];


/* These destination buffers are in IRAM. */
#ifdef EDMA3_ENABLE_DCACHE
/* Cache line aligned destination buffer 1 i.e. Ping buffer. */
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_dstL1DBuff1, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
#pragma DATA_SECTION(_dstL1DBuff1, "my_sect");
static signed char _dstL1DBuff1[PING_PONG_L1D_BUFFER_SIZE];

#ifdef EDMA3_ENABLE_DCACHE
/* Cache line aligned destination buffer 2 i.e. Pong buffer. */
/**
 * The DATA_ALIGN pragma aligns the symbol to an alignment boundary. The
 * alignment boundary is the maximum of the symbol’s default alignment value
 * or the value of the constant in bytes. The constant must be a power of 2.
 * The syntax of the pragma in C is:
 * #pragma DATA_ALIGN (symbol, constant);
 */
#pragma DATA_ALIGN(_dstL1DBuff2, EDMA3_CACHE_LINE_SIZE_IN_BYTES);
#endif  /* #ifdef EDMA3_ENABLE_DCACHE */
#pragma DATA_SECTION(_dstL1DBuff2, "my_sect");
static signed char _dstL1DBuff2[PING_PONG_L1D_BUFFER_SIZE];

/* Pointers for all those buffers */
static signed char *pingpongSrcBuf;
static signed char *pingpongDestBuf;
static signed char *pingpongSrcBufCopy;
static signed char *pingpongDestBufCopy;

static signed char *dstL1DBuff1;
static signed char *dstL1DBuff2;

#endif  /* #ifdef EDMA3_PING_PONG_TEST */


/**
 * EDMA3 Driver Handle, which is used to call all the Driver APIs.
 * It gets initialized during EDMA3 Initialization.
 */
extern EDMA3_DRV_Handle hEdma;



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a DMA channel.
 *
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using two DMA
 *              channels, linked to each other.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_with_link(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a QDMA channel.
 *
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result qdma_test(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);



/**
 *  \brief   EDMA3 misc test cases.
 *              This test case will read/write to some CC registers.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_misc_test();



#ifdef QDMA_TEST_WITH_LINKING
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a QDMA channel,
 *              linked to another LINK channel.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result qdma_test_with_link(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);
#endif  /* #ifdef QDMA_TEST_WITH_LINKING */



#ifdef EDMA3_TEST_WITH_CHAINING
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using two DMA channels,
 *              chained to each other.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_with_chaining(
                                unsigned int acnt,
                                unsigned int bcnt,
                                unsigned int ccnt,
                                EDMA3_DRV_SyncType syncType);
#endif  /* #ifdef EDMA3_TEST_WITH_CHAINING */



#ifdef EDMA3_POLL_MODE_TEST
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a DMA channel.
 *              This test case doesnot rely on the callback mechanism.
 *              Instead, it Polls the IPR register to check the transfer
 *              completion status.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_poll_mode(
                                unsigned int acnt,
                                unsigned int bcnt,
                                unsigned int ccnt,
                                EDMA3_DRV_SyncType syncType);
#endif  /* #ifdef EDMA3_POLL_MODE_TEST */



#ifdef EDMA3_PING_PONG_TEST

/**
 *  \brief   EDMA3 ping-pong based data copy test case, using a DMA and
 *              a link channel.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_ping_pong_mode();

#endif  /* #ifdef EDMA3_PING_PONG_TEST */



/* Callback function 1 */
static void callback1 (unsigned int tcc, EDMA3_RM_TccStatus status,
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
static void callback2 (unsigned int tcc, EDMA3_RM_TccStatus status,
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
            break;
        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /* Transfer resulted in QDMA event miss error. */
            irqRaised2 = -2;
            break;
        default:
            break;
        }
    }


/** Local MemCpy function */
extern void edma3MemCpy(void *dst, const void *src, unsigned int len);

#ifdef EDMA3_PING_PONG_TEST
static EDMA3_DRV_Result process_ping_pong_buffer(unsigned short buff_id)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;

    /**
     * Copy the L1D ping-pong buffers from L1D to DDR using CPU.
     * This is kind of dummy processing routine.
     */
    if (buff_id == 1u)
        {
        /* Copy pong buffer */

        /* Invalidate first if cache is enabled, otherwise CPU will take from cache. */
        /**
         * Since the ping/pong buffers are in IRAM, there is no need of invalidating
         * them. If they are in DDR, invalidate them.
         */
#ifdef EDMA3_ENABLE_DCACHE
        /*
        if (result == EDMA3_DRV_SOK)
            {
            result = Edma3_CacheInvalidate((unsigned int)dstL1DBuff2,
                                                PING_PONG_L1D_BUFFER_SIZE);
            }
        */
#endif  /* EDMA3_ENABLE_DCACHE */

        if (result == EDMA3_DRV_SOK)
            {
            edma3MemCpy((void *)(pingpongDestBufCopy),
                                (const void *)(dstL1DBuff2),
                                PING_PONG_L1D_BUFFER_SIZE);
            }
        }
    else
        {
        /* Copy ping buffer */

        /* Invalidate first if cache is enabled, otherwise CPU will take from cache. */
#ifdef EDMA3_ENABLE_DCACHE
        /*
        if (result == EDMA3_DRV_SOK)
            {
            result = Edma3_CacheInvalidate((unsigned int)dstL1DBuff1,
                                                PING_PONG_L1D_BUFFER_SIZE);
            }
        */
#endif  /* EDMA3_ENABLE_DCACHE */

        if (result == EDMA3_DRV_SOK)
            {
            edma3MemCpy((void *)(pingpongDestBufCopy),
                                (const void *)(dstL1DBuff1),
                                PING_PONG_L1D_BUFFER_SIZE);
            }
        }

    /* Adjust the pointer. */
    pingpongDestBufCopy += PING_PONG_L1D_BUFFER_SIZE;

    return result;
}
#endif


/**
 *  \brief   Main sample test case which will call other EDMA3 test cases.
 *              If one wants to call Edma3 test cases, include this main
 *              test case only.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3MemToMemCpytest ()
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;


    if (hEdma == NULL)
        {
        result = EDMA3_DRV_E_INVALID_PARAM;
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Edma test without linking, async, incr mode */
        result = edma3_test(512u, 32u, 8u, EDMA3_DRV_SYNC_A);
        }


    if (result == EDMA3_DRV_SOK)
        {
#ifdef EDMA3_DEBUG_PRINT
        EDMA3_DEBUG_PRINTF ("edma3_test (without linking) Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */

        /* Edma test with linking, async, incr mode */
        result = edma3_test_with_link(512u, 32u, 8u, EDMA3_DRV_SYNC_A);
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("edma3_test (without linking) Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }


    if (result == EDMA3_DRV_SOK)
        {
#ifdef EDMA3_DEBUG_PRINT
        EDMA3_DEBUG_PRINTF ("edma3_test_with_link (with linking) Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */

        /* Qdma test, async, incr mode */
        result = qdma_test(512u, 32u, 8u, EDMA3_DRV_SYNC_A);
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("edma3_test_with_link (with linking) Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }


    if (result == EDMA3_DRV_SOK)
        {
#ifdef EDMA3_DEBUG_PRINT
        EDMA3_DEBUG_PRINTF ("qdma_test (without linking) Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */

        /* EDMA3 misc test */
        result = edma3_misc_test();
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("qdma_test (without linking) Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }


    if (result == EDMA3_DRV_SOK)
        {
#ifdef EDMA3_DEBUG_PRINT
        EDMA3_DEBUG_PRINTF ("edma3_misc_test Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("edma3_misc_test Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }


#ifdef QDMA_TEST_WITH_LINKING
    if (result == EDMA3_DRV_SOK)
        {
        result = qdma_test_with_link(512u, 32u, 8u, EDMA3_DRV_SYNC_A);

        if (result == EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("qdma_test_with_link Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("qdma_test_with_link Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
            }
#endif  /* QDMA_TEST_WITH_LINKING */
        }



#ifdef EDMA3_TEST_WITH_CHAINING
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_with_chaining(512u, 32u, 8u, EDMA3_DRV_SYNC_A);

        if (result == EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("edma3_test_with_chaining Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("edma3_test_with_chaining Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
            }
        }
#endif  /* EDMA3_TEST_WITH_CHAINING */



#ifdef EDMA3_POLL_MODE_TEST
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_poll_mode(512u, 32u, 8u, EDMA3_DRV_SYNC_A);

        if (result == EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("edma3_test_poll_mode Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("edma3_test_poll_mode Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
            }
        }
#endif  /* EDMA3_POLL_MODE_TEST */


#ifdef EDMA3_PING_PONG_TEST
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_ping_pong_mode();

        if (result == EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("edma3_test_ping_pong_mode Passed\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("edma3_test_ping_pong_mode Failed\r\n");
#endif  /* EDMA3_DRV_DEBUG */
            }
        }
#endif  /* EDMA3_PING_PONG_TEST */

    return result;
}



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a DMA channel.
 *
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test(
                                    unsigned int acnt,
                                    unsigned int bcnt,
                                    unsigned int ccnt,
                                    EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_PaRAMRegs paramSet = {0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int chId = 0;
    unsigned int tcc = 0;
    int i;
    unsigned int count;
    unsigned int Istestpassed = 0u;
    unsigned int numenabled = 0;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;


    srcBuff1 = (signed char*)_srcBuff1;
    dstBuff1 = (signed char*)_dstBuff1;


    /* Initalize source and destination buffers */
    for (count = 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        */
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    /* Set B count reload as B count. */
    BRCnt = bcnt;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Setup for Channel 1*/
    tcc = EDMA3_DRV_TCC_ANY;
    chId = EDMA3_DRV_DMA_CHANNEL_ANY;

    /* Request any DMA channel and any TCC */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &chId, &tcc,
                                        (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Fill the PaRAM Set with transfer specific information */
        paramSet.srcAddr    = (unsigned int)(srcBuff1);
        paramSet.destAddr   = (unsigned int)(dstBuff1);

        /**
         * Be Careful !!!
         * Valid values for SRCBIDX/DSTBIDX are between –32768 and 32767
         * Valid values for SRCCIDX/DSTCIDX are between –32768 and 32767
         */
        paramSet.srcBIdx    = srcbidx;
        paramSet.destBIdx   = desbidx;
        paramSet.srcCIdx    = srccidx;
        paramSet.destCIdx   = descidx;

        /**
         * Be Careful !!!
         * Valid values for ACNT/BCNT/CCNT are between 0 and 65535.
         * ACNT/BCNT/CCNT must be greater than or equal to 1.
         * Maximum number of bytes in an array (ACNT) is 65535 bytes
         * Maximum number of arrays in a frame (BCNT) is 65535
         * Maximum number of frames in a block (CCNT) is 65535
         */
        paramSet.aCnt       = acnt;
        paramSet.bCnt       = bcnt;
        paramSet.cCnt       = ccnt;

        /* For AB-synchronized transfers, BCNTRLD is not used. */
        paramSet.bCntReload = BRCnt;

        paramSet.linkAddr   = 0xFFFFu;

        /* Src & Dest are in INCR modes */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= ((tcc << OPT_TCC_SHIFT) & OPT_TCC_MASK);

        /* Enable Intermediate & Final transfer completion interrupt */
        paramSet.opt |= (1 << OPT_ITCINTEN_SHIFT);
        paramSet.opt |= (1 << OPT_TCINTEN_SHIFT);

        if (syncType == EDMA3_DRV_SYNC_A)
            {
            paramSet.opt &= 0xFFFFFFFBu;
            }
        else
            {
            /* AB Sync Transfer Mode */
            paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
            }

        /* Now, write the PaRAM Set. */
        result = EDMA3_DRV_setPaRAM(hEdma, chId, &paramSet);
        }


    /*
     * There is another way to program the PaRAM Set using specific APIs
     * for different PaRAM set entries. It gives user more control and easier
     * to use interface. User can use any of the methods.
     * Below is the alternative way to program the PaRAM Set.
     */

    /*
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, chId, (unsigned int)(srcBuff1),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, chId, (unsigned int)(dstBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, chId, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, chId, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, chId, acnt, bcnt, ccnt,
                                                BRCnt, EDMA3_DRV_SYNC_A);
            }
        else
            {
            result = EDMA3_DRV_setTransferParams (hEdma, chId, acnt, bcnt, ccnt,
                                                BRCnt, EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, chId,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, chId,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    */


    /*
     * Since the transfer is going to happen in Manual mode of EDMA3
     * operation, we have to 'Enable the Transfer' multiple times.
     * Number of times depends upon the Mode (A/AB Sync)
     * and the different counts.
     */
    if (result == EDMA3_DRV_SOK)
        {
        /*Need to activate next param*/
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            numenabled = bcnt * ccnt;
            }
        else
            {
            /* AB Sync Transfer Mode */
            numenabled = ccnt;
            }

        for (i = 0; i < numenabled; i++)
            {
            irqRaised1 = 0;

            /*
             * Now enable the transfer as many times as calculated above.
             */
            result = EDMA3_DRV_enableTransfer (hEdma, chId,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test: EDMA3_DRV_enableTransfer " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }

            /* Wait for the Completion ISR. */
            while (irqRaised1 == 0u)
                {
                TSK_sleep (1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nedma3_test: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, chId);
                break;
                }
            }
        }


    /* Match the Source and Destination Buffers. */
    if (EDMA3_DRV_SOK == result)
        {
        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff1[i] != dstBuff1[i])
                {
                Istestpassed = 0u;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test: Data write-read matching" \
                                     "FAILED at i = %d\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed = 1u;
            }


        /* Free the previously allocated channel. */
        result = EDMA3_DRV_freeChannel (hEdma, chId);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3_test: EDMA3_DRV_freeChannel() FAILED, " \
                                "error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        }


    if(Istestpassed == 1u)
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
}



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using two DMA
 *              channels, linked to each other.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_with_link(
                                    unsigned int acnt,
                                    unsigned int bcnt,
                                    unsigned int ccnt,
                                    EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_PaRAMRegs paramSet = {0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int ch1Id = 0;
    unsigned int ch2Id = 0;
    unsigned int tcc1 = 0;
    unsigned int tcc2 = 0;
    int i;
    unsigned int count;
    unsigned int Istestpassed1 = 0u;
    unsigned int Istestpassed2 = 0u;
    unsigned int numenabled = 0;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;


    srcBuff1 = (signed char*) _srcBuff1;
    dstBuff1 = (signed char*) _dstBuff1;
    srcBuff2 = (signed char*) _srcBuff2;
    dstBuff2 = (signed char*) _dstBuff2;


    /* Initalize source and destination buffers */
    for (count = 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count+1;
        srcBuff2[count] = (int)count+1;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        dstBuff2[count] = initval;
        */
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff2, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff2, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    irqRaised1 = 0;
    irqRaised2 = 0;

    /* Set B count reload as B count. */
    BRCnt = bcnt;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Setup for Channel 1*/
    tcc1 = EDMA3_DRV_TCC_ANY;
    ch1Id = EDMA3_DRV_DMA_CHANNEL_ANY;

    /* Request any DMA channel and any TCC */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &ch1Id, &tcc1,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Fill the PaRAM Set with transfer specific information */
        paramSet.srcAddr    = (unsigned int)(srcBuff1);
        paramSet.destAddr   = (unsigned int)(dstBuff1);

        /**
         * Be Careful !!!
         * Valid values for SRCBIDX/DSTBIDX are between –32768 and 32767
         * Valid values for SRCCIDX/DSTCIDX are between –32768 and 32767
         */
        paramSet.srcBIdx    = srcbidx;
        paramSet.destBIdx   = desbidx;
        paramSet.srcCIdx    = srccidx;
        paramSet.destCIdx   = descidx;

        /**
         * Be Careful !!!
         * Valid values for ACNT/BCNT/CCNT are between 0 and 65535.
         * ACNT/BCNT/CCNT must be greater than or equal to 1.
         * Maximum number of bytes in an array (ACNT) is 65535 bytes
         * Maximum number of arrays in a frame (BCNT) is 65535
         * Maximum number of frames in a block (CCNT) is 65535
         */
        paramSet.aCnt       = acnt;
        paramSet.bCnt       = bcnt;
        paramSet.cCnt       = ccnt;

        /* For AB-synchronized transfers, BCNTRLD is not used. */
        paramSet.bCntReload = BRCnt;

        paramSet.linkAddr   = 0xFFFFu;

        /* Src & Dest are in INCR modes */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= ((tcc1 << OPT_TCC_SHIFT) & OPT_TCC_MASK);

        /* Enable Intermediate & Final transfer completion interrupt */
        paramSet.opt |= (1 << OPT_ITCINTEN_SHIFT);
        paramSet.opt |= (1 << OPT_TCINTEN_SHIFT);

        if (syncType == EDMA3_DRV_SYNC_A)
            {
            paramSet.opt &= 0xFFFFFFFBu;
            }
        else
            {
            /* AB Sync Transfer Mode */
            paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
            }

        /* Now, write the PaRAM Set. */
        result = EDMA3_DRV_setPaRAM (hEdma, ch1Id, &paramSet);
        }


    /*
     * There is another way to program the PaRAM Set using specific APIs
     * for different PaRAM set entries. It gives user more control and easier
     * to use interface. User can use any of the methods.
     * Below is the alternative way to program the PaRAM Set.
     */

    /*

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, ch1Id, (unsigned int)(srcBuff1),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, ch1Id,
                                            (unsigned int)(dstBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, ch1Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, ch1Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch1Id, acnt, bcnt,
                                                ccnt, BRCnt,
                                                EDMA3_DRV_SYNC_A);
            }
        else
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch1Id, acnt, bcnt,
                                                ccnt, BRCnt,
                                                EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, ch1Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, ch1Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    */


    /* Request any LINK channel and any TCC */
    if (result == EDMA3_DRV_SOK)
        {
        /* Setup for Channel 2 */
        ch2Id   = EDMA3_DRV_LINK_CHANNEL;
        tcc2    = EDMA3_DRV_TCC_ANY;

        result = EDMA3_DRV_requestChannel (hEdma, &ch2Id, &tcc2,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /*
         * Fill the PaRAM Set for the LINK channel
         * with transfer specific information.
         */
        paramSet.srcAddr    = (unsigned int)(srcBuff2);
        paramSet.destAddr   = (unsigned int)(dstBuff2);

        /**
         * Be Careful !!!
         * Valid values for SRCBIDX/DSTBIDX are between –32768 and 32767
         * Valid values for SRCCIDX/DSTCIDX are between –32768 and 32767
         */
        paramSet.srcBIdx    = srcbidx;
        paramSet.destBIdx   = desbidx;
        paramSet.srcCIdx    = srccidx;
        paramSet.destCIdx   = descidx;

        /**
         * Be Careful !!!
         * Valid values for ACNT/BCNT/CCNT are between 0 and 65535.
         * ACNT/BCNT/CCNT must be greater than or equal to 1.
         * Maximum number of bytes in an array (ACNT) is 65535 bytes
         * Maximum number of arrays in a frame (BCNT) is 65535
         * Maximum number of frames in a block (CCNT) is 65535
         */
        paramSet.aCnt       = acnt;
        paramSet.bCnt       = bcnt;
        paramSet.cCnt       = ccnt;

        /* For AB-synchronized transfers, BCNTRLD is not used. */
        paramSet.bCntReload = BRCnt;

        paramSet.linkAddr   = 0xFFFFu;

        /* Reset opt field first */
        paramSet.opt = 0x0u;
        /* Src & Dest are in INCR modes */
        paramSet.opt &= 0xFFFFFFFCu;

        /* Enable Intermediate & Final transfer completion interrupt */
        paramSet.opt |= (1 << OPT_ITCINTEN_SHIFT);
        paramSet.opt |= (1 << OPT_TCINTEN_SHIFT);

        if (syncType == EDMA3_DRV_SYNC_A)
            {
            paramSet.opt &= 0xFFFFFFFBu;
            }
        else
            {
            /* AB Sync Transfer Mode */
            paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
            }

        /* Now, write the PaRAM Set. */
        result = EDMA3_DRV_setPaRAM(hEdma, ch2Id, &paramSet);
        }


    /*
     * There is another way to program the PaRAM Set using specific APIs
     * for different PaRAM set entries. It gives user more control and easier
     * to use interface. User can use any of the methods.
     * Below is the alternative way to program the PaRAM Set.
     */

    /*

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, ch2Id, (unsigned int)(srcBuff2),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, ch2Id,
                                        (unsigned int)(dstBuff2),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, ch2Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, ch2Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch2Id, acnt, bcnt,
                                                    ccnt,
                                                    BRCnt,EDMA3_DRV_SYNC_A);
            }
        else
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch2Id, acnt, bcnt,
                                                    ccnt,
                                                    BRCnt,EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, ch2Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, ch2Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    */


    /* Link both the channels. */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_linkChannel (hEdma, ch1Id, ch2Id);
        }


    /*
     * Since the transfer is going to happen in Manual mode of EDMA3
     * operation, we have to 'Enable the Transfer' multiple times.
     * Number of times depends upon the Mode (A/AB Sync)
     * and the different counts.
     */
    if (result == EDMA3_DRV_SOK)
        {
        /*Need to activate next param*/
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            numenabled = bcnt * ccnt;
            }
        else
            {
            /* AB Sync Transfer Mode */
            numenabled = ccnt;
            }

        for (i = 0; i < numenabled; i++)
            {
            irqRaised1 = 0;

            /*
             * Now enable the transfer for Master channel as many times
             * as calculated above.
             */
            result = EDMA3_DRV_enableTransfer (hEdma, ch1Id,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from edma3_test_with_link\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }

            while (irqRaised1 == 0)
                {
                /* Wait for the Completion ISR on Master Channel. */
                TSK_sleep(1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nedma3_test_with_link: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, ch1Id);

                break;
                }
            }
        }


    /**
     * Now the transfer on Master channel is finished.
     * Trigger next (LINK) param.
     */
    if (EDMA3_DRV_SOK == result)
        {
        for (i = 0; i < numenabled; i++)
            {
            irqRaised1 = 0;

            /*
             * Enable the transfer for LINK channel as many times
             * as calculated above.
             */
            result = EDMA3_DRV_enableTransfer (hEdma, ch1Id,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from edma3_test_with_link\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }

            while (irqRaised1 == 0)
                {
                /* Wait for the Completion ISR on the Link Channel. */
                TSK_sleep(1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nedma3_test_with_link: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, ch2Id);

                break;
                }
            }
        }



    /* Match the Source and Destination Buffers. */
    if (EDMA3_DRV_SOK == result)
        {
        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff1[i] != dstBuff1[i])
                {
                Istestpassed1 = 0u;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_with_link: Data write-read " \
                                "matching FAILED at i = %d " \
                                "(srcBuff1 -> dstBuff1)\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed1 = 1u;
            }


        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff2[i] != dstBuff2[i])
                {
                Istestpassed2 = 0;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_with_link: Data write-read " \
                            "matching FAILED at i = %d " \
                            "(srcBuff2 -> dstBuff2)\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed2 = 1u;
            }


        /* Free the previously allocated channels. */
        result = EDMA3_DRV_freeChannel (hEdma, ch1Id);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3_test_with_link: EDMA3_DRV_freeChannel() " \
                                "for ch1 FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        else
            {
            result = EDMA3_DRV_freeChannel (hEdma, ch2Id);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_with_link: " \
                                "EDMA3_DRV_freeChannel() for ch 2 FAILED, " \
                                "error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                }
            }
        }


    if((Istestpassed1 == 1u) && (Istestpassed2 == 1u))
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_with_link PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_with_link FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
}



/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a QDMA channel.
 *
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result qdma_test(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    unsigned int i;
    unsigned int count;
    unsigned int Istestpassed = 0u;
    unsigned int numenabled = 0;
    unsigned int qCh1Id=0;
    unsigned int qTcc1 = 0;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;
    static signed char* tmpSrcBuff1 = NULL;
    static signed char* tmpDstBuff1 = NULL;

    srcBuff1 = (signed char*) _srcBuff1;
    dstBuff1 = (signed char*) _dstBuff1;

    tmpSrcBuff1 = srcBuff1;
    tmpDstBuff1 = dstBuff1;

    /* Initalize source and destination buffers */
    for (count = 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count+2;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        */
        }

#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    irqRaised1 = 0;

    /* Set B count reload as B count. */
    BRCnt = bcnt;
    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;

    if (syncType == EDMA3_DRV_SYNC_A)
        {
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Setup for any QDMA Channel */
    qCh1Id = EDMA3_DRV_QDMA_CHANNEL_ANY;
    qTcc1 = EDMA3_DRV_TCC_ANY;

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &qCh1Id, &qTcc1,
                                        (EDMA3_RM_EventQueue)0, &callback1,
                                        NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Set QDMA Trigger Word as Destination Address */
        result =  EDMA3_DRV_setQdmaTrigWord (hEdma, qCh1Id,
                                            EDMA3_RM_QDMA_TRIG_DST);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, qCh1Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, qCh1Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, qCh1Id, acnt, bcnt,
                                                ccnt, BRCnt, EDMA3_DRV_SYNC_A);
            }
        else
            {
            /* AB Sync Transfer Mode */
            result = EDMA3_DRV_setTransferParams (hEdma, qCh1Id, acnt, bcnt,
                                                ccnt, BRCnt, EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Enable Transfer Completion Interrupt */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Enable Intermediate Transfer Completion Interrupt */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Set Source Transfer Mode as Increment Mode. */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id, EDMA3_DRV_OPT_FIELD_SAM,
                                        EDMA3_DRV_ADDR_MODE_INCR);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Set Destination Transfer Mode as Increment Mode. */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id, EDMA3_DRV_OPT_FIELD_DAM,
                                        EDMA3_DRV_ADDR_MODE_INCR);
        }


    /*
     * Since the transfer is going to happen in QDMA mode of EDMA3
     * operation, we have to "Trigger" the transfer multiple times.
     * Number of times depends upon the Mode (A/AB Sync)
     * and the different counts.
     */
    if (result == EDMA3_DRV_SOK)
        {
        /*Need to activate next param*/
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            numenabled = bcnt * ccnt;
            }
        else
            {
            /* AB Sync Transfer Mode */
            numenabled = ccnt;
            }

        for (i = 0u; i < numenabled; i++)
            {
            irqRaised1 = 0u;

            if (i == (numenabled-1u))
                {
                /**
                 * Since OPT.STATIC field should be SET for isolated QDMA
                 * transfers or for the final transfer in a linked list of QDMA
                 * transfers, do the needful for the last request.
                 */
                result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_STATIC, 1u);
                }

            /* Write to the Source Address */
            result = EDMA3_DRV_setSrcParams (hEdma, qCh1Id,
                                            (unsigned int)(srcBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
            /*
             * Now trigger the QDMA channel by writing to the Trigger
             * Word which is set as Destination Address.
             */
            if (result == EDMA3_DRV_SOK)
                {
                result = EDMA3_DRV_setPaRAMEntry (hEdma, qCh1Id,
                                                    EDMA3_DRV_PARAM_ENTRY_DST,
                                                    (unsigned int)(dstBuff1));
                if (result != EDMA3_DRV_SOK)
                    {
#ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("error from qdma_test\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                    break;
                    }
                }

            /* Wait for the Completion ISR. */
            while (irqRaised1 == 0)
                {
                /* Wait for the Completion ISR. */
                TSK_sleep (1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nqdma_test: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, qCh1Id);

                break;
                }

            /**
             * Now, update the source and destination addresses for next
             * "Trigger".
             */
            srcBuff1 += srccidx;
            dstBuff1 += descidx;
            }
        }


    if (result == EDMA3_DRV_SOK)
        {
        /* Restore the src and dest buffers */
        srcBuff1 = tmpSrcBuff1;
        dstBuff1 = tmpDstBuff1;

        /* Match the Source and Destination Buffers. */
        for (i = 0u; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff1[i] != dstBuff1[i])
                {
                Istestpassed = 0u;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("qdma_test: Data write-read matching FAILED" \
                                " at i = %d\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed = 1u;
            }


        /* Free the previously allocated channel. */
        result = EDMA3_DRV_freeChannel (hEdma, qCh1Id);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("qdma_test: EDMA3_DRV_freeChannel() FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        }


    if(Istestpassed == 1u)
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("qdma_test PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("qdma_test FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
    }



/**
 *  \brief   EDMA3 misc test cases.
 *              This test case will read/write to some CC registers.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_misc_test()
    {
    EDMA3_DRV_Result drvResult = EDMA3_DRV_SOK;
    unsigned int ccRegOffset = 0u;
    unsigned int ccRegVal = 0u;
    unsigned int newRegVal = 0u;
    unsigned int origRegVal = 0u;


    /**
     *1. Try fetching some CC registers value.
     * It should PASS.
     */
    /* PID Register */
    ccRegOffset = 0x0u;
    ccRegVal = 0;
    drvResult = EDMA3_DRV_getCCRegister(hEdma, ccRegOffset, &ccRegVal);

    if (drvResult != EDMA3_DRV_SOK)
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Failed, test FAILED\r\n", ccRegOffset);
#endif
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Passed, test PASSED\r\n", ccRegOffset);
#endif

#ifdef EDMA3_DEBUG_PRINT
        EDMA3_DEBUG_PRINTF ("Fetched CC Register at Offset 0X%x, Value = 0X%x\r\n", ccRegOffset, ccRegVal);
#endif  /* EDMA3_DEBUG_PRINT */
        }


    if (drvResult == EDMA3_DRV_SOK)
        {
        /* Fetch DRAE1 Register */
        ccRegOffset = 0x0348u;
        ccRegVal = 0;
        drvResult = EDMA3_DRV_getCCRegister(hEdma, ccRegOffset, &ccRegVal);

        if (drvResult != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Failed, test FAILED\r\n", ccRegOffset);
#endif
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Passed, test PASSED\r\n", ccRegOffset);
#endif

#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("Fetched CC Register at Offset 0X%x, Value = 0X%x\r\n", ccRegOffset, ccRegVal);
#endif  /* EDMA3_DEBUG_PRINT */
            }
        }


    if (drvResult == EDMA3_DRV_SOK)
        {
        /* Fetch QWMTHRA Register */
        ccRegOffset = 0x0620u;
        ccRegVal = 0;
        drvResult = EDMA3_DRV_getCCRegister(hEdma, ccRegOffset, &ccRegVal);

        if (drvResult != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Failed, test FAILED\r\n", ccRegOffset);
#endif
            return drvResult;
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Passed, test PASSED\r\n", ccRegOffset);
#endif

#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("Fetched CC Register at Offset 0X%x, Value = 0X%x\r\n", ccRegOffset, ccRegVal);
#endif  /* EDMA3_DEBUG_PRINT */
            }
        }



    /**
     * 2. Try fetching some CC registers value, whose offset is not 4-bytes
     * aligned. It should FAIL.
     */
    if (drvResult == EDMA3_DRV_SOK)
        {
        ccRegOffset = 0x1002u;
        ccRegVal = 0x0u;
        drvResult = EDMA3_DRV_getCCRegister(hEdma, ccRegOffset, &ccRegVal);

        if (drvResult == EDMA3_DRV_E_INVALID_PARAM)
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("Fetching Invalid CC Register (offset 0X%x) Failed, test PASSED\r\n", ccRegOffset);
#endif  /* EDMA3_DEBUG_PRINT */
            drvResult = EDMA3_DRV_SOK;
            }
        else
            {
#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("Fetching Invalid CC Register (offset 0X%x) Passed, test FAILED\r\n", ccRegOffset);
#endif  /* EDMA3_DEBUG_PRINT */
            }
        }



    /**
     * 3. Read CC Register QWMTHRA. Store the current value. Write a different
     * value on it. Read it back. Write the original value again. Read it back to
     * cross-check. It should PASS.
     */
    if (drvResult == EDMA3_DRV_SOK)
        {
        ccRegOffset = 0x0620u;
        origRegVal = 0x0u;
        drvResult = EDMA3_DRV_getCCRegister(hEdma, ccRegOffset, &origRegVal);

        if (drvResult != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Failed, test FAILED\r\n", ccRegOffset);
#endif
            }
        else
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("Fetching CC Register (offset 0X%x) Passed, test PASSED\r\n", ccRegOffset);
#endif

#ifdef EDMA3_DEBUG_PRINT
            EDMA3_DEBUG_PRINTF ("Fetched CC Register at Offset 0X%x, Value = 0X%x\r\n", ccRegOffset, origRegVal);
#endif  /* EDMA3_DEBUG_PRINT */

            /* Find the new value to be written, it depends on the no of event queues */
            switch (origRegVal)
                {
                /* Write a new value on the same register */
                case 0x10:
                    /* 1 TC */
                    newRegVal = 0x0Fu;
                    break;

                case 0x1010:
                    /* 2 TC */
                    newRegVal = 0x0F0Fu;
                    break;

                case 0x101010:
                    /* 3 TC */
                    newRegVal = 0x0F0F0Fu;
                    break;

                case 0x10101010:
                    /* 4 TC */
                    newRegVal = 0x0F0F0F0Fu;
                    break;

                default:
                    newRegVal = origRegVal;
                    break;
                }

            drvResult = EDMA3_DRV_setCCRegister (hEdma, ccRegOffset, newRegVal);
            if (drvResult == EDMA3_DRV_SOK)
                {
                /* If write is successful, read it back to check */
                ccRegVal = 0x0u;

                drvResult = EDMA3_DRV_getCCRegister (hEdma, ccRegOffset, &ccRegVal);
                if (drvResult == EDMA3_DRV_SOK)
                    {
                    /* Check with the value which we have written */
                    if (newRegVal == ccRegVal)
                        {
#ifdef EDMA3_DEBUG_PRINT
                        EDMA3_DEBUG_PRINTF ("Value written successfully \r\n");
#endif  /* EDMA3_DEBUG_PRINT */
                        }
                    else
                        {
#ifdef EDMA3_DRV_DEBUG
                        EDMA3_DRV_PRINTF ("QWMTHRA write FAILED \r\n");
#endif
                        drvResult = EDMA3_DRV_E_INVALID_PARAM;
                        }
                    }

                /* Restore the original value */
                if (drvResult == EDMA3_DRV_SOK)
                    {
                    drvResult = EDMA3_DRV_setCCRegister (hEdma, ccRegOffset, origRegVal);
                    if (drvResult != EDMA3_DRV_SOK)
                        {
#ifdef EDMA3_DEBUG_PRINT
                        EDMA3_DEBUG_PRINTF ("QWMTHRA Restore FAILED\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
                        }
                    else
                        {
#ifdef EDMA3_DEBUG_PRINT
                        EDMA3_DEBUG_PRINTF ("QWMTHRA Restore Successful\r\n");
#endif  /* EDMA3_DEBUG_PRINT */
                        }
                    }
                }
            }
        }


    return drvResult;
    }



#ifdef QDMA_TEST_WITH_LINKING
/**
   * Test case demonstrating the usgae of QDMA channel for transferring
   * data between two memory locations. QDMA channel is linked to a LINK
   * channel.
   */
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a QDMA channel,
 *              linked to another LINK channel.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result qdma_test_with_link(
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    unsigned int i;
    unsigned int count;
    unsigned int Istestpassed1 = 0u;
    unsigned int Istestpassed2 = 0u;
    unsigned int qCh1Id=0;
    unsigned int qTcc1 = 0;
    unsigned int qCh2Id=0;
    unsigned int qTcc2 = 0;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;
    unsigned int numenabled = 0;
    static signed char* tmpSrcBuff1 = NULL;
    static signed char* tmpDstBuff1 = NULL;
    static signed char* tmpSrcBuff2 = NULL;
    static signed char* tmpDstBuff2 = NULL;
    unsigned int abCNT = 0;
    unsigned int bcntReloadLinkField = 0x0u;


    srcBuff1 = (signed char*) _srcBuff1;
    dstBuff1 = (signed char*) _dstBuff1;
    srcBuff2 = (signed char*) _srcBuff2;
    dstBuff2 = (signed char*) _dstBuff2;


    /* Store the original pointers for future usage. */
    tmpSrcBuff1 = srcBuff1;
    tmpDstBuff1 = dstBuff1;
    tmpSrcBuff2 = srcBuff2;
    tmpDstBuff2 = dstBuff2;


    /* Initalize source and destination buffers */
    for (count= 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count+3;
        srcBuff2[count] = (int)count+3;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        dstBuff2[count] = initval;
        */
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff2, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff2, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    /* Set B count reload as B count. */
    BRCnt = bcnt;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;

    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Setup for any QDMA Channel */
    qCh1Id = EDMA3_DRV_QDMA_CHANNEL_ANY;
    qTcc1 = EDMA3_DRV_TCC_ANY;

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &qCh1Id, &qTcc1,
                                        (EDMA3_RM_EventQueue)0,
                                        &callback1, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Setup for Channel 2 (Link Channel) */
        qCh2Id = EDMA3_DRV_LINK_CHANNEL;
        qTcc2 = EDMA3_DRV_TCC_ANY;

        result = EDMA3_DRV_requestChannel (hEdma, &qCh2Id, &qTcc2,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    /* Configure the Link Channel first */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, qCh2Id,
                                        (unsigned int)(srcBuff2),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, qCh2Id,
                                            (unsigned int)(dstBuff2),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, qCh2Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestIndex (hEdma, qCh2Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, qCh2Id, acnt, bcnt,
                                                    ccnt, BRCnt,
                                                    EDMA3_DRV_SYNC_A);
            }
        else
            {
            /* AB Sync Transfer Mode */
            result = EDMA3_DRV_setTransferParams (hEdma, qCh2Id, acnt, bcnt,
                                                    ccnt, BRCnt,
                                                    EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Enable the Transfer Completion Interrupt on Link Channel */
        result = EDMA3_DRV_setOptField (hEdma, qCh2Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /**
         * Enable the Intermediate Transfer Completion Interrupt on Link
         * Channel.
         */
        result = EDMA3_DRV_setOptField (hEdma, qCh2Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Link both the channels. */
        result = EDMA3_DRV_linkChannel (hEdma, qCh1Id, qCh2Id);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /**
         * Now configure the QDMA channel. Here lies the trick. Since QDMA
         * channel is linked to another DMA channel, as soon as transfer on
         * QDMA channel is finished, static field being NOT SET, the associated
         * PaRAM Set will be reloaded with the Linked PaRAM Set. Now, as the
         * reload occurs, the QDMA channel will be triggered due to the write
         * on a specific Trigger Word. We want the trigger to happen immediately
         * after the write, so the trigger word should be chosen in such a way
         * that it should trigger after the COMPLETE PaRAM Set will get copied
         * onto the QDMA Channel PaRAM Set. In that case, only ONE option is
         * there to choose the CCNT as the trigger word. All other trigger
         * words will cause the trigger happen in-between the PaRAM Set is
         * loading. So Set the trigger word as CCNT.
         */
        result = EDMA3_DRV_setQdmaTrigWord (hEdma, qCh1Id,
                                            EDMA3_RM_QDMA_TRIG_CCNT);
        }


    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, qCh1Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestIndex (hEdma, qCh1Id, desbidx, descidx);
        }

    abCNT = acnt | ((bcnt & 0xFFFFu) << 16u);

    /* Write ACNT and BCNT */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setPaRAMEntry(hEdma, qCh1Id,
                                            EDMA3_DRV_PARAM_ENTRY_ACNT_BCNT,
                                            abCNT);
        }

    /* Set the SYNC Mode (A/AB Sync) */
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_SYNCDIM, 0u);
        }
    else
        {
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_SYNCDIM, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Enable Transfer Completion Interrupt */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Enable Intermediate Transfer Completion Interrupt */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Set Source Transfer Mode as Increment Mode. */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id, EDMA3_DRV_OPT_FIELD_SAM,
                                        EDMA3_DRV_ADDR_MODE_INCR);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Set Destination Transfer Mode as Increment Mode. */
        result = EDMA3_DRV_setOptField (hEdma, qCh1Id, EDMA3_DRV_OPT_FIELD_DAM,
                                        EDMA3_DRV_ADDR_MODE_INCR);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /* Get Link Address. */
        result = EDMA3_DRV_getPaRAMField(hEdma, qCh1Id,
                                        EDMA3_DRV_PARAM_FIELD_LINKADDR,
                                        &bcntReloadLinkField);
        }

    bcntReloadLinkField = (bcntReloadLinkField | (BRCnt << 16));

    if (result == EDMA3_DRV_SOK)
        {
        /* Set B Count Reload & Link Address. */
        result = EDMA3_DRV_setPaRAMEntry(hEdma, qCh1Id,
                                        EDMA3_DRV_PARAM_ENTRY_LINK_BCNTRLD,
                                        bcntReloadLinkField);
        }


    /*Need to activate next param*/
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        numenabled = bcnt * ccnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        numenabled = ccnt;
        }


    if (numenabled == 1u)
        {
        /**
         * If only one Sync event is required, make the PaRAM Set associated
         * with the LINK channel as Static.
         */
        if (result == EDMA3_DRV_SOK)
            {
            result = EDMA3_DRV_setOptField (hEdma, qCh2Id,
                                            EDMA3_DRV_OPT_FIELD_STATIC, 1u);
            }

            /**
             * Be Careful!!!
             * PaRAM Set associated with the Master Channel should NOT be
             * set as Static, otherwise the reload will not occur.
             */
        }



    /*
     * Since the transfer is going to happen in QDMA mode of EDMA3
     * operation, we have to "Trigger" the transfer multiple times.
     * Number of times depends upon the Mode (A/AB Sync)
     * and the different counts.
     */
    if (result == EDMA3_DRV_SOK)
        {
        for (i = 0u; i < numenabled; i++)
            {
            irqRaised1 = 0u;

            /* Write to the Source Address */
            result = EDMA3_DRV_setSrcParams (hEdma, qCh1Id,
                                            (unsigned int)(srcBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                return result;
                }


            /* Write to the Destination Address */
            result = EDMA3_DRV_setDestParams(hEdma, qCh1Id,
                                                (unsigned int)(dstBuff1),
                                                EDMA3_DRV_ADDR_MODE_INCR,
                                                EDMA3_DRV_W8BIT);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                return result;
                }


            /* Now write to the Trigger Word i.e. CCNT. */
            result = EDMA3_DRV_setPaRAMEntry(hEdma, qCh1Id, EDMA3_DRV_PARAM_ENTRY_CCNT, ccnt);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                return result;
                }


            /* After this, transfer will start. */
            while (irqRaised1 == 0)
                {
                /* Wait for the Completion ISR for the Master QDMA Channel. */
                TSK_sleep(1u);
                }


            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nqdma_test_with_link: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, qCh1Id);

                break;
                }


            /**
             * Now, update the source and destination addresses for next
             * "Trigger".
             */
            srcBuff1 += srccidx;
            dstBuff1 += descidx;


            /**
             * Read the current C Count from the PaRAM Set and write it back.
             * In this way, we would write the correct CCNT every time and
             * trigger the transfer also. Since CC will decrement the CCNT
             * after every (ACNT * BCNT) bytes of data transfer, we can use
             * that decremented value to trigger the next transfer.
             * Another option is to take count of CCNT manually (in your code)
             * and write that value.
             * First option seems less error prone.
             */
            result = EDMA3_DRV_getPaRAMField(hEdma, qCh1Id, EDMA3_DRV_PARAM_FIELD_CCNT, &ccnt);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
#endif  /* EDMA3_DRV_DEBUG */
                return result;
                }
            }
        }


    /**
     * Transfer on the QDMA channel has finished and Link
     * PaRAM Set is loaded on the QDMA channel PaRAM Set.
     * Now for the transfers on the LINK channel,
     * if only one "TRIGGER" is required,
     * that has already been provided by the PaRAM Set
     * upload.
     * For other triggers, we will take care.
     */
    if (result == EDMA3_DRV_SOK)
        {
        /**
         * One trigger has been provided already, so first wait for
         * that transfer to complete.
         */
        while (irqRaised1 == 0)
            {
            /* Wait for the Completion ISR for the Master QDMA Channel. */
            TSK_sleep(1u);
            }

        /* Check the status of the completed transfer */
        if (irqRaised1 < 0)
            {
            /* Some error occured, clear the error bits. */
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF ("\r\nqdma_test_with_link: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

            /* Clear the error bits first */
            result = EDMA3_DRV_clearErrorBits (hEdma, qCh1Id);
            }


        if (numenabled == 1u)
            {
            /**
             * Only 1 trigger was required which has been provided
             * already. No need to do anything.
             */
            }
        else
            {
            /**
             * One trigger has been provided already, so take that into account.
             */
            numenabled -= 1u;

            for (i = 0u; i < numenabled; i++)
                {
                irqRaised1 = 0u;

                if (i == (numenabled - 1u))
                    {
                    /**
                     * Before providing the last trigger,
                     * make the PaRAM Set static.
                     */
                    result = EDMA3_DRV_setOptField (hEdma, qCh1Id,
                                            EDMA3_DRV_OPT_FIELD_STATIC, 1u);
                    }

                /**
                 * Now, update the source and destination addresses for next
                 * "Trigger".
                 */
                srcBuff2 += srccidx;
                dstBuff2 += descidx;


                /**
                 * Read the current C Count from the PaRAM Set and write it back.
                 * In this way, we would write the correct CCNT every time and
                 * trigger the transfer also.
                 */
                result = EDMA3_DRV_getPaRAMField(hEdma, qCh1Id, EDMA3_DRV_PARAM_FIELD_CCNT, &ccnt);
                if (result != EDMA3_DRV_SOK)
                    {
    #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
    #endif  /* EDMA3_DRV_DEBUG */
                    return result;
                    }


                /* Write to the Source Address */
                result = EDMA3_DRV_setSrcParams (hEdma, qCh1Id,
                                                (unsigned int)(srcBuff2),
                                                EDMA3_DRV_ADDR_MODE_INCR,
                                                EDMA3_DRV_W8BIT);
                if (result != EDMA3_DRV_SOK)
                    {
    #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
    #endif  /* EDMA3_DRV_DEBUG */
                    return result;
                    }


                /* Write to the Destination Address */
                result = EDMA3_DRV_setDestParams(hEdma, qCh1Id,
                                                    (unsigned int)(dstBuff2),
                                                    EDMA3_DRV_ADDR_MODE_INCR,
                                                    EDMA3_DRV_W8BIT);
                if (result != EDMA3_DRV_SOK)
                    {
    #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
    #endif  /* EDMA3_DRV_DEBUG */
                    return result;
                    }


                /* Now write to the Trigger Word i.e. CCNT. */
                result = EDMA3_DRV_setPaRAMEntry(hEdma, qCh1Id, EDMA3_DRV_PARAM_ENTRY_CCNT, ccnt);
                if (result != EDMA3_DRV_SOK)
                    {
    #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("error from qdma_test_with_link\r\n\r\n");
    #endif  /* EDMA3_DRV_DEBUG */
                    return result;
                    }


                /* After this, transfer will start. */
                while (irqRaised1 == 0)
                    {
                    /* Wait for the Completion ISR for the Link Channel. */
                    TSK_sleep(1u);
                    }


                /* Check the status of the completed transfer */
                if (irqRaised1 < 0)
                    {
                    /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("\r\nqdma_test_with_link: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                    /* Clear the error bits first */
                    result = EDMA3_DRV_clearErrorBits (hEdma, qCh1Id);

                    break;
                    }
                }
            }
        }


    /* Restore the src and dest buffers */
    srcBuff1 = tmpSrcBuff1;
    dstBuff1 = tmpDstBuff1;
    srcBuff2 = tmpSrcBuff2;
    dstBuff2 = tmpDstBuff2;


    /* Match the Source and Destination Buffers. */
    if (EDMA3_DRV_SOK == result)
        {
        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff1[i] != dstBuff1[i])
                {
                Istestpassed1 = 0;
    #ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("qdma_test_with_link: Data write-read " \
                                "matching FAILED at i = %d " \
                                "(srcBuff1 -> dstBuff1)\r\r\n", i);
    #endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed1 = 1u;
            }


        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff2[i] != dstBuff2[i])
                {
                Istestpassed2 = 0;
    #ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("qdma_test_with_link: Data write-read " \
                                "matching FAILED at i = %d " \
                                "(srcBuff2 -> dstBuff2)\r\n", i);
    #endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed2 = 1u;
            }
        }


    if (EDMA3_DRV_SOK == result)
        {
        /* Free the previously allocated channels. */
        result = EDMA3_DRV_freeChannel (hEdma, qCh1Id);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("qdma_test_with_link: EDMA3_DRV_freeChannel() for qCh1Id FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        else
            {
            result = EDMA3_DRV_freeChannel (hEdma, qCh2Id);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("qdma_test_with_link: EDMA3_DRV_freeChannel() for qCh2Id FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                }
            }
        }


    if((Istestpassed1 == 1u) && (Istestpassed2 == 1u))
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("qdma_test_with_link PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("qdma_test_with_link FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
    }

#endif  /* QDMA_TEST_WITH_LINKING */



#ifdef EDMA3_TEST_WITH_CHAINING
/**
   * Test case demonstrating the usgae of DMA channels for transferring
   * data between two memory locations. The two DMA channels are chained
   * to each other.
   */
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using two DMA channels,
 *              chained to each other.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_with_chaining(
                                unsigned int acnt,
                                unsigned int bcnt,
                                unsigned int ccnt,
                                EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_ChainOptions chain = {EDMA3_DRV_TCCHEN_DIS,
                                    EDMA3_DRV_ITCCHEN_DIS,
                                    EDMA3_DRV_TCINTEN_DIS,
                                    EDMA3_DRV_ITCINTEN_DIS};
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;
    unsigned int ch1Id = 0;
    unsigned int ch2Id = 0;
    unsigned int tcc1 = 0;
    unsigned int tcc2 = 0;
    int i;
    unsigned int numenabled = 0;
    unsigned int count;
    unsigned int Istestpassed1 = 0u;
    unsigned int Istestpassed2 = 0u;


    srcBuff1 = (signed char*) _srcBuff1;
    dstBuff1 = (signed char*) _dstBuff1;
    srcBuff2 = (signed char*) _srcBuff2;
    dstBuff2 = (signed char*) _dstBuff2;


    /* Initalize source and destination buffers */
    for (count = 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count+4;
        srcBuff2[count] = (int)count+4;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        dstBuff2[count] = initval;
        */
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff2, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff2, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    /* Set B count reload as B count. */
    BRCnt = bcnt;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;

    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Transfer complete chaining enable. */
    chain.tcchEn = EDMA3_DRV_TCCHEN_EN;
    /* Intermediate transfer complete chaining enable. */
    chain.itcchEn = EDMA3_DRV_ITCCHEN_EN;
    /* Transfer complete interrupt is enabled. */
    chain.tcintEn = EDMA3_DRV_TCINTEN_EN;
    /* Intermediate transfer complete interrupt is disabled. */
    chain.itcintEn = EDMA3_DRV_ITCINTEN_DIS;


    /* Setup for Channel 1*/
    tcc1 = EDMA3_DRV_TCC_ANY;
    ch1Id = EDMA3_DRV_DMA_CHANNEL_ANY;

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &ch1Id, &tcc1,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, ch1Id,
                                        (unsigned int)(srcBuff1),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, ch1Id,
                                            (unsigned int)(dstBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, ch1Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestIndex (hEdma, ch1Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch1Id, acnt, bcnt,
                                                    ccnt,BRCnt,
                                                    EDMA3_DRV_SYNC_A);
            }
        else
            {
            /* AB Sync Transfer Mode */
            result = EDMA3_DRV_setTransferParams (hEdma, ch1Id, acnt, bcnt,
                                                    ccnt, BRCnt,
                                                    EDMA3_DRV_SYNC_AB);
            }
        }


    if (result == EDMA3_DRV_SOK)
        {
        /* Setup for Channel 2 */
        tcc2 = EDMA3_DRV_TCC_ANY;
        ch2Id = EDMA3_DRV_DMA_CHANNEL_ANY;

        result = EDMA3_DRV_requestChannel (hEdma, &ch2Id, &tcc2,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback2, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, ch2Id, (unsigned int)(srcBuff2),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, ch2Id,
                                            (unsigned int)(dstBuff2),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, ch2Id, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, ch2Id, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, ch2Id, acnt, bcnt,
                                                    ccnt, BRCnt,
                                                    EDMA3_DRV_SYNC_A);
            }
        else
            {
            /* AB Sync Transfer Mode */
            result = EDMA3_DRV_setTransferParams (hEdma, ch2Id, acnt, bcnt,
                                                    ccnt, BRCnt,
                                                    EDMA3_DRV_SYNC_AB);
            }
        }


    if (result == EDMA3_DRV_SOK)
        {
        /**
         * Enable the Transfer Completion Interrupt on the Chained Channel
         * (Ch 2).
         */
        result = EDMA3_DRV_setOptField (hEdma, ch2Id,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        /**
         * Enable the Intermediate Transfer Completion Interrupt on the
         * Chained Channel (Ch 2).
         */
        result = EDMA3_DRV_setOptField (hEdma, ch2Id,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }


    if (result == EDMA3_DRV_SOK)
        {
        /* Now chain the two channels together. */
        result = EDMA3_DRV_chainChannel(hEdma, ch1Id, ch2Id,
                                    (EDMA3_DRV_ChainOptions *)&chain);
        }


    /*Need to activate next param*/
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        numenabled = bcnt * ccnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        numenabled = ccnt;
        }


    if (result == EDMA3_DRV_SOK)
        {
        for (i = 0; i < numenabled; i++)
            {
            irqRaised2 = 0;

            /*
             * Now enable the transfer for Master channel as many times
             * as calculated above.
             */
            result = EDMA3_DRV_enableTransfer (hEdma, ch1Id,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);
            if (result != EDMA3_DRV_SOK)
                {
    #ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("error from edma3_test_with_chaining\n\r\n");
    #endif  /* EDMA3_DRV_DEBUG */
                break;
                }


            /**
             * Transfer on the master channel (ch1Id) will finish after some
             * time.
             * Now, because of the enabling of intermediate chaining on channel
             * 1, after the transfer gets over, a sync event will be sent
             * to channel 2, which will trigger the transfer on it.
             * Also, Final and Intermediate Transfer Complete
             * Interrupts are enabled on channel 2, so we should wait for the
             * completion ISR on channel 2 first, before proceeding
             * ahead.
             */
            while (irqRaised2 == 0)
                {
                /* Wait for the Completion ISR on channel 2. */
                TSK_sleep(1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised2 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nedma3_test_with_chaining: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, ch1Id);

                break;
                }
            }
        }


    if (result == EDMA3_DRV_SOK)
        {
        /* Match the Source and Destination Buffers. */
        if (result == EDMA3_DRV_SOK)
            {
            for (i = 0; i < (acnt*bcnt*ccnt); i++)
                {
                if (srcBuff1[i] != dstBuff1[i])
                    {
                    Istestpassed1 = 0;

        #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF("edma3_test_with_chaining: Data write-read " \
                                    "matching FAILED at i = %d " \
                                    "(srcBuff1 -> dstBuff1)\r\n", i);
        #endif  /* EDMA3_DRV_DEBUG */
                    break;
                    }
                }
            if (i == (acnt*bcnt*ccnt))
                {
                Istestpassed1 = 1u;
                }


            for (i = 0; i < (acnt*bcnt*ccnt); i++)
                {
                if (srcBuff2[i] != dstBuff2[i])
                    {
                    Istestpassed2 = 0;

        #ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF("edma3_test_with_chaining: Data write-read " \
                                    "matching FAILED at i = %d " \
                                    "(srcBuff2 -> dstBuff2)\r\n", i);
        #endif  /* EDMA3_DRV_DEBUG */
                    break;
                    }
                }
            if (i == (acnt*bcnt*ccnt))
                {
                Istestpassed2 = 1u;
                }
            }


    if (result == EDMA3_DRV_SOK)
        {
        /* Free the previously allocated channels. */
        result = EDMA3_DRV_freeChannel (hEdma, ch1Id);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3_test_with_chaining: EDMA3_DRV_freeChannel() for ch 1 FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        else
            {
            result = EDMA3_DRV_freeChannel (hEdma, ch2Id);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_with_chaining: EDMA3_DRV_freeChannel() for ch 2 FAILED, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                }
            }
        }
    }


    if((Istestpassed1 == 1u) && (Istestpassed2 == 1u))
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_with_chaining PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_with_chaining FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
    }

#endif  /* EDMA3_TEST_WITH_CHAINING */



#ifdef EDMA3_POLL_MODE_TEST
/**
   * Test case demonstrating the poll mode scenario.
   * A user has requested a data transfer without giving any
   * callback function. After programming and enabling the channel,
   * he uses different APIs (meant to be used in poll mode)
   * to check the status of ongoing transfer.
   * Interrupt will NOT occur in this case.
   */
/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a DMA channel.
 *              This test case doesnot rely on the callback mechanism.
 *              Instead, it Polls the IPR register to check the transfer
 *              completion status.
 *
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_poll_mode(
                                    unsigned int acnt,
                                    unsigned int bcnt,
                                    unsigned int ccnt,
                                    EDMA3_DRV_SyncType syncType)
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    unsigned int chId = 0;
    unsigned int tcc = 0;
    int i;
    unsigned int count;
    unsigned int Istestpassed = 0u;
    unsigned int numenabled = 0;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;


    srcBuff1 = (signed char*)_srcBuff1;
    dstBuff1 = (signed char*)_dstBuff1;


    /* Initalize source and destination buffers */
    for (count = 0u; count < (acnt*bcnt*ccnt); count++)
        {
        srcBuff1[count] = (int)count+5;
        /**
         * No need to initialize the destination buffer as it is being invalidated.
        dstBuff1[count] = initval;
        */
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)srcBuff1, (acnt*bcnt*ccnt));
        }

    /* Invalidate the Destination Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstBuff1, (acnt*bcnt*ccnt));
        }
#endif  /* EDMA3_ENABLE_DCACHE */


    /* Set B count reload as B count. */
    BRCnt = bcnt;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;

    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }


    /* Setup for Channel 1*/
    tcc = EDMA3_DRV_TCC_ANY;
    chId = EDMA3_DRV_DMA_CHANNEL_ANY;


    /* Request any DMA channel and any TCC */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &chId, &tcc,
                                            (EDMA3_RM_EventQueue)0,
                                            NULL, NULL);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcParams (hEdma, chId, (unsigned int)(srcBuff1),
                                        EDMA3_DRV_ADDR_MODE_INCR,
                                        EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setDestParams (hEdma, chId, (unsigned int)(dstBuff1),
                                            EDMA3_DRV_ADDR_MODE_INCR,
                                            EDMA3_DRV_W8BIT);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setSrcIndex (hEdma, chId, srcbidx, srccidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result =  EDMA3_DRV_setDestIndex (hEdma, chId, desbidx, descidx);
        }

    if (result == EDMA3_DRV_SOK)
        {
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            result = EDMA3_DRV_setTransferParams (hEdma, chId, acnt, bcnt, ccnt,
                                                BRCnt, EDMA3_DRV_SYNC_A);
            }
        else
            {
            result = EDMA3_DRV_setTransferParams (hEdma, chId, acnt, bcnt, ccnt,
                                                BRCnt, EDMA3_DRV_SYNC_AB);
            }
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, chId,
                                        EDMA3_DRV_OPT_FIELD_TCINTEN, 1u);
        }

    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setOptField (hEdma, chId,
                                        EDMA3_DRV_OPT_FIELD_ITCINTEN, 1u);
        }

    /*
     * Since the transfer is going to happen in Manual mode of EDMA3
     * operation, we have to 'Enable the Transfer' multiple times.
     * Number of times depends upon the Mode (A/AB Sync)
     * and the different counts.
     */
    if (result == EDMA3_DRV_SOK)
        {
        /*Need to activate next param*/
        if (syncType == EDMA3_DRV_SYNC_A)
            {
            numenabled = bcnt * ccnt;
            }
        else
            {
            /* AB Sync Transfer Mode */
            numenabled = ccnt;
            }


        for (i = 0; i < numenabled; i++)
            {
            /*
             * Now enable the transfer as many times as calculated above.
             */
            result = EDMA3_DRV_enableTransfer (hEdma, chId,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test_poll_mode: EDMA3_DRV_enableTransfer " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }


            /* Wait for the Completion Bit to be SET in the IPR/IPRH register. */
            result = EDMA3_DRV_waitAndClearTcc (hEdma, tcc);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test_poll_mode: EDMA3_DRV_waitAndClearTcc " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        }


    /* Match the Source and Destination Buffers. */
    if (EDMA3_DRV_SOK == result)
        {
        for (i = 0; i < (acnt*bcnt*ccnt); i++)
            {
            if (srcBuff1[i] != dstBuff1[i])
                {
                Istestpassed = 0u;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_poll_mode: Data write-read matching" \
                                     "FAILED at i = %d\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == (acnt*bcnt*ccnt))
            {
            Istestpassed = 1u;
            }


        /* Free the previously allocated channel. */
        result = EDMA3_DRV_freeChannel (hEdma, chId);
        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3_test_poll_mode: EDMA3_DRV_freeChannel() FAILED, " \
                                "error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        }


    if(Istestpassed == 1u)
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_poll_mode PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_poll_mode FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
}
#endif  /* #ifdef EDMA3_POLL_MODE_TEST */



#ifdef EDMA3_PING_PONG_TEST
/**
 *  \brief   EDMA3 ping-pong based data copy test case, using a DMA and
 *              a link channel.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
static EDMA3_DRV_Result edma3_test_ping_pong_mode()
    {
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;
    EDMA3_DRV_PaRAMRegs paramSet = {0,0,0,0,0,0,0,0,0,0,0,0};
    /* One master channel */
    unsigned int chId = 0;
    /* Two link channels */
    unsigned int lChId1 = 0;
    unsigned int lChId2 = 0;
    unsigned int tcc = 0;
    int i;
    unsigned int count;
    unsigned int Istestpassed = 0u;
    unsigned int BRCnt = 0;
    int srcbidx = 0, desbidx = 0;
    int srccidx = 0, descidx = 0;
    /* PaRAM Set handle */
    unsigned int phyaddress = 0;
    EDMA3_DRV_ParamentryRegs *param_handle = NULL;
    /* Number of triggers for EDMA3. */
    unsigned int numenabled = PING_PONG_NUM_TRIGGERS;

    pingpongSrcBuf = (signed char*)_pingpongSrcBuf;
    pingpongDestBuf = (signed char*)_pingpongDestBuf;
    pingpongSrcBufCopy = pingpongSrcBuf;
    pingpongDestBufCopy = pingpongDestBuf;
    dstL1DBuff1 = (signed char*)_dstL1DBuff1;
    dstL1DBuff2 = (signed char*)_dstL1DBuff2;


    /* Initalize source buffer for PING_PONG_DDR_BUFFER_SIZE bytes of data */
    for (count = 0u; count < PING_PONG_DDR_BUFFER_SIZE; count++)
        {
        pingpongSrcBuf[count] = (count % 0xFF);
        }


#ifdef EDMA3_ENABLE_DCACHE
    /*
    * Note: These functions are required if the buffer is in DDR.
    * For other cases, where buffer is NOT in DDR, user
    * may or may not require the below functions.
    */
    /* Flush the Source Buffer */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheFlush((unsigned int)pingpongSrcBuf, PING_PONG_DDR_BUFFER_SIZE);
        }

    /* Invalidate the Destination Buffers */
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)pingpongDestBuf, PING_PONG_DDR_BUFFER_SIZE);
        }

    /**
     * Since the ping/pong buffers are in IRAM, there is no need of invalidating
     * them. If they are in DDR, invalidate them.
     */

    /*
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstL1DBuff1, PING_PONG_L1D_BUFFER_SIZE);
        }
    if (result == EDMA3_DRV_SOK)
        {
        result = Edma3_CacheInvalidate((unsigned int)dstL1DBuff2, PING_PONG_L1D_BUFFER_SIZE);
        }
    */
#endif  /* EDMA3_ENABLE_DCACHE */


    /* Set B count reload as B count. */
    BRCnt = PING_PONG_BCNT;

    /* Setting up the SRC/DES Index */
    srcbidx = (int)PING_PONG_ACNT;
    desbidx = (int)PING_PONG_ACNT;

    /* AB Sync Transfer Mode */
    srccidx = ((int)PING_PONG_ACNT * (int)PING_PONG_BCNT);
    descidx = ((int)PING_PONG_ACNT * (int)PING_PONG_BCNT);

    /* Setup for DMA Channel 1*/
    tcc = EDMA3_DRV_TCC_ANY;
    chId = EDMA3_DRV_DMA_CHANNEL_ANY;

    /* Request any DMA channel and any TCC */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_requestChannel (hEdma, &chId, &tcc,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL);
        }

    /* If successful, allocate the two link channels. */
    if (result == EDMA3_DRV_SOK)
        {
        lChId1 = EDMA3_DRV_LINK_CHANNEL;
        lChId2 = EDMA3_DRV_LINK_CHANNEL;

        result = (
                    (EDMA3_DRV_requestChannel (hEdma, &lChId1, NULL,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL))
                    ||
                    (EDMA3_DRV_requestChannel (hEdma, &lChId2, NULL,
                                            (EDMA3_RM_EventQueue)0,
                                            &callback1, NULL))
                    );
        }


    /**
     * Fill the PaRAM Sets associated with all these channels with transfer
     * specific information.
     */
    if (result == EDMA3_DRV_SOK)
        {
        paramSet.srcBIdx    = srcbidx;
        paramSet.destBIdx   = desbidx;
        paramSet.srcCIdx    = srccidx;
        paramSet.destCIdx   = descidx;

        paramSet.aCnt       = PING_PONG_ACNT;
        paramSet.bCnt       = PING_PONG_BCNT;
        paramSet.cCnt       = PING_PONG_CCNT;

        /* For AB-synchronized transfers, BCNTRLD is not used. */
        paramSet.bCntReload = BRCnt;

        /* Src & Dest are in INCR modes */
        paramSet.opt &= 0xFFFFFFFCu;
        /* Program the TCC */
        paramSet.opt |= ((tcc << OPT_TCC_SHIFT) & OPT_TCC_MASK);

        /* Enable Intermediate & Final transfer completion interrupt */
        paramSet.opt |= (1 << OPT_ITCINTEN_SHIFT);
        paramSet.opt |= (1 << OPT_TCINTEN_SHIFT);

        /* AB Sync Transfer Mode */
        paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);


        /* Program the source and dest addresses for master DMA channel */
        paramSet.srcAddr    = (unsigned int)(pingpongSrcBuf);
        paramSet.destAddr   = (unsigned int)(dstL1DBuff1);


        /* Write to the master DMA channel first. */
        result = EDMA3_DRV_setPaRAM(hEdma, chId, &paramSet);
        }


    /* If write is successful, write the same thing to first link channel. */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_setPaRAM(hEdma, lChId1, &paramSet);
        }


    /**
     * Now modify the dest addresses and write the param set to the
     * second link channel.
     */
    if (result == EDMA3_DRV_SOK)
        {
        paramSet.destAddr   = (unsigned int)(dstL1DBuff2);

        result = EDMA3_DRV_setPaRAM(hEdma, lChId2, &paramSet);
        }



    /**
     * Do the linking now.
     * Master DMA channel is linked to IInd Link channel.
     * IInd Link channel is linked to Ist Link channel.
     * Ist Link channel is again linked to IInd Link channel.
     */
    if (result == EDMA3_DRV_SOK)
        {
        result = (
                    (EDMA3_DRV_linkChannel (hEdma, chId, lChId2))
                    ||
                    (EDMA3_DRV_linkChannel (hEdma, lChId2, lChId1))
                    ||
                    (EDMA3_DRV_linkChannel (hEdma, lChId1, lChId2))
                    );
        }

    /**
     * Save the handle to the master dma channel param set.
     * It will be used later to modify the source address quickly.
     */
    if (result == EDMA3_DRV_SOK)
        {
        result = EDMA3_DRV_getPaRAMPhyAddr(hEdma, chId, &phyaddress);
        }

    /*
    - Algorithm used in the ping pong copy:
    1. Application starts EDMA of first image stripe into ping buffer in L1D.
    2. Application waits for ping EDMA to finish.
    3. Application starts EDMA of next image stripe into pong buffer in L1D.
    4. Application starts processing ping buffer.
    5. Application waits for pong EDMA to finish.
    6. Application starts EDMA of next image stripe into ping buffer in L1D.
    7. Application starts processing pong buffer.
    8. Repeat from step 3, until image exhausted.
    - EDMA re-programming should be minimized to reduce overhead (PaRAM
        accesses via slow config bus), i.e. use 2 reload PaRAM entries, and
        only change src address fields.
    */

    if (result == EDMA3_DRV_SOK)
        {
        /* Param address successfully fetched. */
        param_handle = (EDMA3_DRV_ParamentryRegs *)phyaddress;

        /* Step 1 */
        result = EDMA3_DRV_enableTransfer (hEdma, chId,
                                            EDMA3_DRV_TRIG_MODE_MANUAL);
        /**
         * Every time a transfer is triggered, numenabled is decremented.
         */
        numenabled--;

        /**
         * Every time a transfer is triggered, pingpongSrcBufCopy is
         * incremented to point it to correct source address.
         */
        pingpongSrcBufCopy += PING_PONG_L1D_BUFFER_SIZE;
        }


    if (result == EDMA3_DRV_SOK)
        {
        /* Need to activate next param till numenabled is exhausted. */
        for (i = 0; numenabled; i++)
            {
            /* Step 2 */
            /* Wait for the Completion ISR. */
            while (irqRaised1 == 0u)
                {
                TSK_sleep (1u);
                }

            irqRaised1 = 0;

            /*
             * Now modify the source buffer in the param set
             * loaded to the master dma channel and trigger
             * the transfer again..
             */
            param_handle->SRC = (unsigned int)pingpongSrcBufCopy;

            /* Step 3 */
            result = EDMA3_DRV_enableTransfer (hEdma, chId,
                                                EDMA3_DRV_TRIG_MODE_MANUAL);

            /**
             * Every time a transfer is triggered, numenabled is decremented.
             */
            numenabled--;

            /**
             * Every time a transfer is triggered, pingpongSrcBufCopy is
             * incremented to point it to correct source address.
             */
            pingpongSrcBufCopy += PING_PONG_L1D_BUFFER_SIZE;

            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test_ping_pong_mode: EDMA3_DRV_enableTransfer " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }

            /**
             * Step 4, copy the ping buffer to the dest buffer in
             * DDR (using CPU), as a part of processing.
             */
            result = process_ping_pong_buffer(0u);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test_ping_pong_mode: process_ping_pong_buffer " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }


            /* Step 5 */
            /* Wait for the Completion ISR. */
            while (irqRaised1 == 0u)
                {
                TSK_sleep (1u);
                }

            /* Check the status of the completed transfer */
            if (irqRaised1 < 0)
                {
                /* Some error occured, break from the FOR loop. */
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("\r\nedma3_test: Event Miss Occured!!!\r\n");
#endif  /* EDMA3_DRV_DEBUG */

                /* Clear the error bits first */
                result = EDMA3_DRV_clearErrorBits (hEdma, chId);
                break;
                }


            /**
             * Last row will be transferred by the Pong buffer.
             * So this step should be jumped over.
             * Check for that...
             */
            if (numenabled)
                {
                irqRaised1 = 0;

                /* Step 6 */
                /*
                 * Now modify the source buffer in the param set
                 * again and trigger the transfer...
                 */
                param_handle->SRC = (unsigned int)pingpongSrcBufCopy;

                result = EDMA3_DRV_enableTransfer (hEdma, chId,
                                                    EDMA3_DRV_TRIG_MODE_MANUAL);
                /**
                 * Every time a transfer is triggered, numenabled is decremented.
                 */
                numenabled--;

                /**
                 * Every time a transfer is triggered, pingpongSrcBufCopy is
                 * incremented to point it to correct source address.
                 */
                pingpongSrcBufCopy += PING_PONG_L1D_BUFFER_SIZE;

                if (result != EDMA3_DRV_SOK)
                    {
#ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF ("edma3_test_ping_pong_mode: EDMA3_DRV_enableTransfer " \
                                        "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                    break;
                    }
                }

            /**
             * Step 7, copy the pong buffer to the dest buffer in
             * DDR (using CPU), as a part of processing.
             */
            result = process_ping_pong_buffer(1u);
            if (result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF ("edma3_test_ping_pong_mode: process_ping_pong_buffer " \
                                    "Failed, error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        }



    if (EDMA3_DRV_SOK == result)
        {
        /* Match the Source and Destination Buffers. */
        for (i = 0; i < PING_PONG_DDR_BUFFER_SIZE; i++)
            {
            if (pingpongSrcBuf[i] != pingpongDestBuf[i])
                {
                Istestpassed = 0u;
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3_test_ping_pong_mode: Data write-read matching" \
                                     "FAILED at i = %d\r\n", i);
#endif  /* EDMA3_DRV_DEBUG */
                break;
                }
            }
        if (i == PING_PONG_DDR_BUFFER_SIZE)
            {
            Istestpassed = 1u;
            }



        /* Free the previously allocated channels. */
        result = (
                    (EDMA3_DRV_freeChannel (hEdma, chId))
                    ||
                    (EDMA3_DRV_freeChannel (hEdma, lChId1))
                    ||
                    (EDMA3_DRV_freeChannel (hEdma, lChId2))
                    );

        if (result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3_test_ping_pong_mode: EDMA3_DRV_freeChannel() FAILED, " \
                                "error code: %d\r\n", result);
#endif  /* EDMA3_DRV_DEBUG */
            }
        }


    if(Istestpassed == 1u)
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_ping_pong_mode PASSED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        }
    else
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3_test_ping_pong_mode FAILED\r\n");
#endif  /* EDMA3_DRV_DEBUG */
        result = ((EDMA3_DRV_SOK == result) ?
                                EDMA3_DATA_MISMATCH_ERROR : result);
        }


    return result;
    }
#endif  /*#ifdef EDMA3_PING_PONG_TEST */

