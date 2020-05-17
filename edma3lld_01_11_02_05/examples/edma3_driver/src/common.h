/*
 * common.h
 *
 * Header file for the Demo application for the EDMA3 Driver.
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

#ifndef _COMMON_H_
#define _COMMON_H_

#include <std.h>
#include <stdio.h>
#include <tsk.h>

/* Include EDMA3 Driver */
#include <ti/sdo/edma3/drv/edma3_drv.h>

#ifdef __cplusplus
extern "C" {
#endif


/* To include linking or chaining test case. */
#define QDMA_TEST_WITH_LINKING      (1u)
/* #undef QDMA_TEST_WITH_LINKING */

#define EDMA3_TEST_WITH_CHAINING    (1u)
/* #undef EDMA3_TEST_WITH_CHAINING */

/* To include Poll mode tests */
#define EDMA3_POLL_MODE_TEST        (1u)
/* #undef EDMA3_POLL_MODE_TEST */

/* To include ping-pong buffer tests */
#define EDMA3_PING_PONG_TEST        (1u)
/* #undef EDMA3_PING_PONG_TEST */


/* MAX ACOUNT */
#define MAX_ACOUNT					(512u)

/* MAX BCOUNT */
#define MAX_BCOUNT					(32u)

/* MAX CCOUNT */
#define MAX_CCOUNT					(8u)


/**
 * Buffers (src and dest) are needed for mem-2-mem data transfers.
 * This define is for the MAXIMUM size and hence the maximum data
 * which could be transferred using the sample test cases below.
 */
#define MAX_BUFFER_SIZE                     (MAX_ACOUNT * MAX_BCOUNT * MAX_CCOUNT)

/**
 * Cache line size on the underlying SoC. It needs to be modified
 * for different cache line sizes, if the Cache is Enabled.
 */
#define EDMA3_CACHE_LINE_SIZE_IN_BYTES      (128u)

/* To enable/disable the cache .*/
#define EDMA3_ENABLE_DCACHE                 (1u)

/* OPT Field specific defines */
#define OPT_SYNCDIM_SHIFT                   (0x00000002u)
#define OPT_TCC_MASK                        (0x0003F000u)
#define OPT_TCC_SHIFT                       (0x0000000Cu)
#define OPT_ITCINTEN_SHIFT                  (0x00000015u)
#define OPT_TCINTEN_SHIFT                   (0x00000014u)


/* Error returned in case of data mismatch */
#define EDMA3_DATA_MISMATCH_ERROR           (-1)

/* Defines for devices having multiple DSPs like 6474/6488. */
#ifdef IS_MULTI_DSP_DEVICE
#define EDMA3_CIC_REGS(num)                      		(0x02880000u + num*0x100u)
#define EDMA3_CIC_0_REGS                      			(EDMA3_CIC_REGS(0))
#define EDMA3_CIC_1_REGS                      			(EDMA3_CIC_REGS(1))
#define EDMA3_CIC_2_REGS                      			(EDMA3_CIC_REGS(2))

#define EDMA3_CIC_EVTFLAG0(num)		*((volatile unsigned int *)(EDMA3_CIC_REGS(num) + 0x00))
#define EDMA3_CIC_EVTFLAG1(num)		*((volatile unsigned int *)(EDMA3_CIC_REGS(num) + 0x04))

#define EDMA3_CIC_EVTCLR0(num)		*((volatile unsigned int *)(EDMA3_CIC_REGS(num) + 0x20))
#define EDMA3_CIC_EVTCLR1(num)		*((volatile unsigned int *)(EDMA3_CIC_REGS(num) + 0x24))

#define EDMA3_CC_ERROR_EVENT                            (36u)
#define EDMA3_CC_ERROR_EVENT_SHIFT                     	(4u)

extern unsigned int dsp_num;
#endif		/* #ifdef IS_MULTI_DSP_DEVICE */


extern void callback1 (unsigned int tcc, EDMA3_RM_TccStatus status,
                        void *appData);

extern void callback2 (unsigned int tcc, EDMA3_RM_TccStatus status,
                        void *appData);

extern signed char*  getGlobalAddr(signed char* addr);
/* Flag variable to check transfer completion on channel 1 */
extern volatile short irqRaised1;
/* Flag variable to check transfer completion on channel 2 */
extern volatile short irqRaised2;

#define GLOBAL_ADDR(addr) (getGlobalAddr(addr))

/**
 * \brief   EDMA3 Initialization (single DSP devices)
 *
 * This function initializes the EDMA3 Driver and registers the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Result edma3init (void);

/**
 * \brief   EDMA3 De-initialization (single DSP devices)
 *
 * This function removes the EDMA3 RM Instance and unregisters the
 * interrupt handlers. It also deletes the RM  Object.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Result edma3deinit (void);


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
EDMA3_DRV_Result edma3_test(
					EDMA3_DRV_Handle hEdma,
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
EDMA3_DRV_Result edma3_test_with_link(
					EDMA3_DRV_Handle hEdma,
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
EDMA3_DRV_Result qdma_test(
					EDMA3_DRV_Handle hEdma,
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);

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
EDMA3_DRV_Result qdma_test_with_link(
					EDMA3_DRV_Handle hEdma,
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);


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
EDMA3_DRV_Result edma3_test_with_chaining(
					EDMA3_DRV_Handle hEdma,
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);


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
EDMA3_DRV_Result edma3_test_poll_mode(
					EDMA3_DRV_Handle hEdma,
                    unsigned int acnt,
                    unsigned int bcnt,
                    unsigned int ccnt,
                    EDMA3_DRV_SyncType syncType);


/**
 *  \brief   EDMA3 ping-pong based data copy test case, using a DMA and
 *              a link channel.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3_test_ping_pong_mode(EDMA3_DRV_Handle hEdma);


/**
 *  \brief   EDMA3 misc test cases.
 *              This test case will read/write to some CC registers.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3_misc_test(EDMA3_DRV_Handle hEdma);


EDMA3_DRV_Result edma3_test_multi_edma(unsigned int edma3InstId,
								EDMA3_DRV_Handle hEdma,
                                unsigned int acnt,
                                unsigned int bcnt,
                                unsigned int ccnt,
                                EDMA3_DRV_SyncType syncType);



/**
 *  \brief   EDMA3 Cache Invalidate
 *
 *  This function invalidates the D cache.
 *
 *  \param  mem_start_ptr [IN]      Starting adress of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheInvalidate(unsigned int mem_start_ptr,
                           unsigned int num_bytes);



/**
 * \brief   EDMA3 Cache Flush
 *
 *  This function flushes (cleans) the Cache
 *
 *  \param  mem_start_ptr [IN]      Starting adress of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheFlush(unsigned int mem_start_ptr,
                      unsigned int num_bytes);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* _COMMON_H_ */

