/** ============================================================================
 *  @file   main.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/readwrite/
 *
 *  @desc   Main function that calls SWI or TSK readwrite applications based
 *          on the parameter TSK_MODE.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <sio.h>
#include <tsk.h>
#include <gio.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <hal_interrupt.h>
#endif


#if defined (PCPY_LINK)
#include <buf_pool.h>
#endif /* if defined (PCPY_LINK) */

#if defined (MSGQ_PCPY_LINK)
#include <pcpy_mqt.h>
#endif /* if defined (MSGQ_PCPY_LINK) */

/*  ----------------------------------- Sample Headers              */
#include <readwrite.h>
#include <tskReadwrite.h>
#include <readwrite_config.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID             FID_APP_C

/** ============================================================================
 *  @name   numTransfers
 *
 *  @desc   Iterations of data transfer to be done by the application.
 *  ============================================================================
 */
Uint32 numTransfers ;

#if defined (DSP_BOOTMODE_NOBOOT)
/** ============================================================================
 *  @name   DSPLINK_initFlag
 *
 *  @desc   Flag to indicate to the DSPLINK_init.
 *  ============================================================================
 */
volatile Uint32 DSPLINK_initFlag = 0xBABAC0C0 ;

#if defined (DM6446GEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DM6467GEM )
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (OMAP3530)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (5u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (55u)

#elif defined (OMAP2530)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (5u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (55u)

#elif defined (DM6437)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (56u)

#elif defined (DM648)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (15u)

#elif defined (DRA44XGEM )
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DRX416GEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DA8XXGEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (5u)

#endif

#endif

/** ============================================================================
 *  @name   trace
 *
 *  @desc   trace LOG_Obj used to do LOG_printf
 *  ============================================================================
 */
extern LOG_Obj trace;

/** ============================================================================
 *  @func   atol
 *
 *  @desc   Converts character string to long value.
 *
 *  @arg    str
 *              Pointer to character string.
 *
 *  @ret    <integer value>
 *              Converted integer value.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern long atol(const char *str);


#if defined (DSP_BOOTMODE_NOBOOT)
/** ----------------------------------------------------------------------------
 *  @func   HAL_initIsr
 *
 *  @desc   ISR which is executed to tell DSP, now it is time to rum DSPLINK_init
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static void HAL_initIsr (Ptr arg) ;
#endif


/** ----------------------------------------------------------------------------
 *  @func   tskReadwrite
 *
 *  @desc   Task for TSK based TSKRDWR application.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static Int tskReadwrite ();


/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry function.
 *
 *  @modif  None
 *  ============================================================================
 */
Void main(Int argc, Char *argv[])
{
    Int            status = SYS_OK ;
    /* TSK based readwrite application */
    TSK_Handle     tskReadwriteTask ;

#if defined (DSP_BOOTMODE_NOBOOT)
     /* register the init ISR */
    HAL_intRegister (DSPLINK_INT_ID,
                     DSPLINK_INT_VEC_ID,
                    (Fxn) &HAL_initIsr,
                     0) ;
#endif

#if !defined (DSP_BOOTMODE_NOBOOT)
    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init () ;

    /* Get the number of transfers to be done by the application */
    numTransfers = atol (argv[0]) ;
#else
    /* Get the number of transfers to be done by the application */
    numTransfers = 1000 ;
#endif

    if (status == SYS_OK) {
        /* Creating task for TSKRDWR application */
        tskReadwriteTask = TSK_create(tskReadwrite, NULL, 0);
        if (tskReadwriteTask != NULL) {
            LOG_printf(&trace, "Create TSKRDWR: Success\n");
        }
        else {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
            LOG_printf(&trace, "Create TSKRDWR: Failed.\n");
            return;
        }
    }

    return;
}


/** ----------------------------------------------------------------------------
 *  @func   tskReadwrite
 *
 *  @desc   Task for TSK based TSKRDWR application.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Int tskReadwrite()
{
    Int                     status = SYS_OK;
    TSKRDWR_TransferInfo * info;

#if defined (DSP_BOOTMODE_NOBOOT)
    {
        while (DSPLINK_initFlag != 0xC0C0BABA) ;
    }
    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init () ;
#endif

    /* Create Phase */
    status = TSKRDWR_create (&info);

    /* Execute Phase */
    if (status == SYS_OK) {
        status = TSKRDWR_execute (info);
        if (status != SYS_OK) {
            SET_FAILURE_REASON(status);
        }
    }

    /* Delete Phase */
    status = TSKRDWR_delete (info);
    if (status != SYS_OK) {
        SET_FAILURE_REASON(status);
    }

    return status;
}

#if defined (DSP_BOOTMODE_NOBOOT)
/** ----------------------------------------------------------------------------
 *  @func   HAL_initIsr
 *
 *  @desc   ISR which is executed to tell DSP, now it is time to rum DSPLINK_init
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void HAL_initIsr (Ptr arg)
{
#if defined (DA8XXGEM)
    volatile Uint32 * chipsig_clr = (Uint32 *) 0x1c14178u ;
#endif
    (Void) arg ;
    /* Set the INIT Flag */
    DSPLINK_initFlag = 0xC0C0BABA ;
#if defined (DA8XXGEM)
    *chipsig_clr = 0x4 ;
#endif
}

#endif

