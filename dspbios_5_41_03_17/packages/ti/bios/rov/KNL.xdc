/*
 *  ======== KNL.xdc ========
 *
 *! Revision History
 *! ================
 *! 23-Oct-2008  cmcc  Created.
 */
 
package ti.bios.rov

import xdc.rov.ViewInfo;

/*!
 *  ======== KNL ========
 *  Represents the BIOS 5 KNL module.
 *
 *  This module provides the TSK module with the currently running TSK
 *  and the priority queues for determining the current state of each
 *  TSK.
 */
module KNL {
    
    /*!
     *  ======== KNL_Obj ========
     *  Object representing a KNL instance.
     *
     *  A KNL_Obj is embedded at the beginning of each TSK instance.
     */
    struct KNL_Obj {
        QUE.QUE_Elem    ready;          /* ready/semaphore queue elem */
        QUE.QUE_Elem    alarm;          /* alarm queue elem */
        QUE.QUE_Elem    setpri;         /* set priority queue elem */
        QUE.QUE_Elem    *queue;          /* task's ready queue */
        Int             priority;      /* task priority */
        UInt            mask;           /* 1 << priority */
        Ptr             sp;             /* current stack pointer */
        UInt            timeout;        /* timeout value */
        Int             mode;           /* KNL_BLOCKED, KNL_READY,... */
        /* 
         * The original structure definition included a pointer to an STS 
         * object. We don't need anything from the STS object, though, so
         * just declare this a Ptr.
         */
        //STS.STS_Obj     *sts;
        Ptr             sts;        
        Bool        signalled;      /* pending task waken up by sem or timeout */
    };
    
    /*!
     *  ======== KNL_Job ========
     *  BIOS 5 target structure embedded in SEM instances.
     */
    struct KNL_Job {
       QUE.QUE_Elem  wListElem;      // KNL_wList queue
       Int           wCount;         // number of posts when on KNL_wList
       Fxn           fxn;            // function that services the job
    };
    
    /*!
     *  ======== SystemView ========
     */
    struct SystemView {
        Int systemTick;
        Ptr stackBase;
        Int stackSize;
        Int stackPeak;
        Ptr sysStackBase;
        Int sysStackSize;
        Int sysStackPeak;
    };
    
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['System', {type: ViewInfo.MODULE_DATA, viewInitFxn: 'viewInitSystem', structName: 'SystemView'}],
            ],
            /* Nothing to show in the raw tab. */
            showRawTab: false
        });

    
    /*!
     *  ======== getCurTaskAddr ========
     *  Returns the address of the currently running TSK.
     */
    metaonly Ptr getCurTaskAddr();

    /*!
     *  ======== getQueues ========
     *  Returns the KNL priority queues used for determining TSK state.
     */
    metaonly Any getQueues();
    
}