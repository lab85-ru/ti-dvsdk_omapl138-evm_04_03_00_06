/** ============================================================================
 *  @file   main.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message_multi/
 *
 *  @desc   Main function for multi-application message sample.
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
#include <sys.h>
#include <sem.h>
#include <log.h>
#include <tsk.h>
#include <msgq.h>
#include <pool.h>
#include <hwi.h>
#include <string.h>
#include <stdio.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <notify.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <hal_interrupt.h>
#endif

#if defined (DSP_BOOTMODE_NOBOOT)
#include <zcpy_mqt.h>
#include <dsplinkmsgq.h>
#endif

#if defined (MSGQ_ZCPY_LINK)
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
#include <dma_pool.h>
#else
#include <sma_pool.h>
#endif
#endif /* if defined (MSGQ_ZCPY_LINK) */
/*  ----------------------------------- Sample Headers              */
#include <tskMessage.h>
#include <message_config.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C


/** ============================================================================
 *  @const  MAX_APPS
 *
 *  @desc   Maximum number of applications that can be used with this DSP app.
 *  ============================================================================
 */
#define MAX_APPS  16


/** ============================================================================
 *  @name   numTransfers
 *
 *  @desc   Iterations of message transfer to be done by the application.
 *          A value of 0 in numTransfers implies infinite iterations.
 *  ============================================================================
 */
Uint16 numTransfers ;

#if defined (DSP_BOOTMODE_NOBOOT)

/** ============================================================================
 *  @name   tskMessageSemObj
 *
 *  @desc   Semaphore used for memssage multi.
 *  ============================================================================
 */
SEM_Obj        tskMessageSemObj ;

/** ============================================================================
 *  @name   MESSAGE_MqtParams
 *
 *  @desc   Extern declaration for msgq parameters for the array of pools
 *  ============================================================================
 */
ZCPYMQT_Params  mqtParams ;

/** ============================================================================
 *  @name   flag
 *
 *  @desc   Boolean flag to allow calling DSPLINK_init and POOL_Open for 0 pool
 *          only once
 *  ============================================================================
 */
Uint32 flag = FALSE ;

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
 *  @name   TSKMESSAGE_GppMsgqName
 *
 *  @desc   Base name of GPP message queues.
 *  ============================================================================
 */
static Char TSKMESSAGE_GppMsgqName [DSP_MAX_STRLEN] = "GPPMSGQ" ;

/** ============================================================================
 *  @name   TSKMESSAGE_DspMsgqName
 *
 *  @desc   Base name of DSP message queues.
 *  ============================================================================
 */
static Char TSKMESSAGE_DspMsgqName [DSP_MAX_STRLEN] = "DSPMSGQ" ;

/** ============================================================================
 *  @name   trace
 *
 *  @desc   trace LOG_Obj used to do LOG_printf
 *  ============================================================================
 */
extern LOG_Obj trace ;

#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
/** ============================================================================
 *  @name   MESSAGE_PoolParams
 *
 *  @desc   Extern declaration for pool parameters for the array of pools
 *  ============================================================================
 */
extern DMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] ;

/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
extern DMAPOOL_Params            dmaPoolObj ;
#else
/** ============================================================================
 *  @name   MESSAGE_PoolParams
 *
 *  @desc   Extern declaration for pool parameters for the array of pools
 *  ============================================================================
 */
extern SMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] ;

/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
extern SMAPOOL_Params            smaPoolObj ;
#endif

/** ----------------------------------------------------------------------------
 *  @func   tskMessage
 *
 *  @desc   Task for TSK based TSKMESSAGE application.
 *
 *  @arg    appNum
 *              Sequence number for the application.
 *
 *  @ret    SYS_OK
 *              Success
 *          Error code
 *              Failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static Int tskMessage(Uint32 appNum) ;


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
 *  @func   TSKMESSAGE_notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive the notification about application
 *          number that is in use on GPP-side.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the NOTIFY component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
 *              component. This information is specific to the NOTIFY being
 *              implemented.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
TSKMESSAGE_notify (Uint32 eventNo, Ptr arg, Ptr info) ;


/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Converts character string to integer value.
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
extern int atoi (const char * str) ;


/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry function.
 *
 *  @modif  None
 *  ============================================================================
 */
Void main (Int argc, Char * argv [])
{
    Int          status = SYS_OK ;
    TSK_Handle   tskMessageTask [MAX_APPS] ; /* TSK based application */
    Uint32       i ;

#if defined (DSP_BOOTMODE_NOBOOT)
     /* register the init ISR */
    HAL_intRegister (DSPLINK_INT_ID,
                     DSPLINK_INT_VEC_ID,
                    (Fxn) &HAL_initIsr,
                     0) ;
#endif

#if !defined (DSP_BOOTMODE_NOBOOT)
    /* Get the number of transfers to be done by the application */
    numTransfers = atoi (argv [0]) ;
    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init () ;
#else
    /* Get the number of transfers to be done by the application */
    numTransfers = 10000 ;
#endif

#if defined (DSP_BOOTMODE_NOBOOT)
    SEM_new (&(tskMessageSemObj), 1) ;
#endif

    for (i = 1 ; (i <= MAX_APPS) && (status == SYS_OK) ; i++) {
        /* Creating task for TSKMESSAGE application */
        tskMessageTask [i - 1] = TSK_create (tskMessage, NULL, i) ;
        if (tskMessageTask [i - 1] != NULL) {
            LOG_printf (&trace, "Create TSKMESSAGE: Success\n") ;
        }
        else {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
            LOG_printf (&trace, "Create TSKMESSAGE: Failed.\n") ;
        }
    }
}


/** ----------------------------------------------------------------------------
 *  @func   tskMessage
 *
 *  @desc   Task for TSK based TSKMESSAGE application.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Int tskMessage(Uint32 appNum)
{
    Int                       status    = SYS_OK ;
    Bool                      semStatus = TRUE ;
    TSKMESSAGE_TransferInfo * info ;
    Char                      chAppNum [3] ;

#if defined (DSP_BOOTMODE_NOBOOT)
    POOL_Obj                  poolObj ;
    MSGQ_TransportObj         transport ;

    semStatus = SEM_pend (&(tskMessageSemObj), SYS_FOREVER) ;
    if (semStatus == FALSE) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        status = SYS_OK ;
    }

    {
        while (DSPLINK_initFlag != 0xC0C0BABA) ;
    }

    if (status == SYS_OK) {
        if(flag == FALSE) {

            /* Initialize DSP/BIOS LINK. */

            DSPLINK_init () ;

            smaPoolObj.poolId        = 0    ;
            smaPoolObj.exactMatchReq = TRUE ;
            poolObj.initFxn          = SMAPOOL_init  ;
            poolObj.fxns             = (POOL_Fxns *) &SMAPOOL_FXNS ;
            poolObj.params           = &(smaPoolObj) ;
            status = POOL_open (0, &poolObj) ;

            if (status == SYS_OK) {
                mqtParams.poolId  = 0 ;
                transport.initFxn = ZCPYMQT_init ;         /* Init Function                 */
                transport.fxns    = (MSGQ_TransportFxns *) &ZCPYMQT_FXNS ;     /* Transport interface functions */
                transport.params  = &mqtParams ;   /* Transport params              */
                transport.object  = NULL ;                 /* Filled in by transport        */
                transport.procId  = ID_GPP  ;              /* Processor Id                  */


                status = MSGQ_transportOpen (ID_GPP,
                                             &transport) ;
            }
            flag = TRUE ;
        }
        /* Post the semaphore. */
        SEM_post ((SEM_Handle) &(tskMessageSemObj)) ;
    }
#endif

    /* Allocate TSKMESSAGE_TransferInfo structure that will be initialized
     * and passed to other phases of the application */
    info = MEM_calloc (DSPLINK_SEGID,
                       sizeof (TSKMESSAGE_TransferInfo),
                       DSPLINK_BUF_ALIGN) ;
    if (info == NULL) {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        info->appNum = appNum ;
        sprintf (chAppNum, "%d", appNum) ;
        strcpy (info->dspMsgqName, (const Char *) TSKMESSAGE_DspMsgqName) ;
        strcat (info->dspMsgqName, (const Char *) &chAppNum) ;
        sprintf (chAppNum, "%d", appNum) ;
        strcpy (info->gppMsgqName, (const Char *) TSKMESSAGE_GppMsgqName) ;
        strcat (info->gppMsgqName, (const Char *) &chAppNum) ;

        SEM_new (&(info->notifySemObj), 0) ;
    }

    /* Register and wait for notification indicating if this task is needed. */
    if (status == SYS_OK) {
        status = NOTIFY_register (ID_GPP,
                                  MESSAGE_IPS_ID,
                                  (MESSAGE_IPS_EVENTNO_BASE + appNum),
                                  TSKMESSAGE_notify,
                                  info) ;
        if (status == SYS_OK) {
            semStatus = SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
            if (semStatus == FALSE) {
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    /* Create Phase */
    if (status == SYS_OK) {
        status = TSKMESSAGE_create (info) ;
    }

    /* Execute Phase */
    if (status == SYS_OK) {
        /* Start the execution phase. */
        status = TSKMESSAGE_execute (info) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Delete Phase */
    status = TSKMESSAGE_delete (info) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   TSKMESSAGE_notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive the notification about application
 *          number that is in use on GPP-side.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
TSKMESSAGE_notify (Uint32 eventNo, Ptr arg, Ptr info)
{
    TSKMESSAGE_TransferInfo * messageInfo = (TSKMESSAGE_TransferInfo *) arg ;

    (Void) eventNo ; /* To avoid compiler warning. */

    /* Check the task number for which this notification is meant. */
    if (messageInfo->appNum == (Uint32) info) {
        /* Post the semaphore. */
        SEM_post (&(messageInfo->notifySemObj)) ;
    }
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


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
