/*
 *  ======== TSK.xdc ========
 *
 *! Revision History
 *! ================
 *! 16-Oct-2008 cmcc Created.
 */
package ti.bios.rov;

import xdc.rov.ViewInfo;

/*!
 *  ======== TSK ========
 *  This module provides support for the TSK module in BIOS 5 ROV.
 *
 *  It defines the target state structures used by the TSK module, and provides
 *  ROV views for displaying them.
 */
module TSK {
    
    /*!
     *  ======== TSK_Obj ========
     *  BIOS 5 target structure representing a single TSK instance.
     *
     *  This is the TSK_Obj object defined in tsk.h.
     */
    struct TSK_Obj {
        KNL.KNL_Obj kobj;           /* kernel object */
        Char        stack[];        /* used in TSK_delete() */
        size_t      stacksize;      /* ditto */
        Int         stackseg;       /* segment to allocate stack from */
        String      name;           /* printable name */
        Ptr         environ;        /* environment pointer */
        Int         errno;          /* TSK_seterr()/TSK_geterr() */
        Bool        exitflag;       /* FALSE for server tasks */
    };
    
    /*!
     *  ======== TSK_Obj_55x ========
     *  Structure representing a single TSK instance on 55x targets.
     *
     *  This is the TSK_Obj object defined in tsk.h, with extra fields to
     *  support 55x targets.
     */
    struct TSK_Obj_55x {
        KNL.KNL_Obj kobj;           /* kernel object */
        Char        stack[];        /* used in TSK_delete() */
        Ptr         sysstack;       /* system stack for 55x, Unused in _55Pb_ */
        size_t      stacksize;      /* ditto */
        size_t      sysstacksize;   /* system stackszie for 55x Unused in _55Pb_ */
        Int         stackseg;       /* segment to allocate stack from */
        String      name;           /* printable name */
        Ptr         environ;        /* environment pointer */
        Int         errno;          /* TSK_seterr()/TSK_geterr() */
        Bool        exitflag;       /* FALSE for server tasks */
    }

    /*!
     *  ======== InstanceData ========
     *  ROV view representation of a single TSK instance.
     */
    metaonly struct InstanceData {
        String      name;
        Ptr         handle;
        String      state;
        Int         priority;
        Int         timeout;
        Int         timeRemaining;
        String      blockedOn;
        Ptr         stackBase;
        Int         stackSize;
        Int         stackPeak;
        Ptr         sysStackBase;
        Int         sysStackSize;
        Int         sysStackPeak;
    };
        
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['Instances', {type: ViewInfo.MODULE_DATA, viewInitFxn: 'viewInitInstances', structName: 'InstanceData'}]
            ],
            /* Nothing to show in the raw tab. */
            showRawTab: false
        });

    /*!
     *  ======== getNickName ========
     *  Returns the TSK's "nick name".
     *  
     *  The "nick name" is a name which represents this instance and is for 
     *  other modules to use in displaying this instance as part of their view.
     */
    metaonly String getNickName(Int tskObjAddr);
    
    /*!
     *  ======== getBlockedTask ========
     *  Returns what SEM or MBX instance the TSK is blocked on.
     *
     *  Looks at all the SEMs and MBXs to try and find the given TSK.
     */
    metaonly String getBlockedTask(Int tskObjAddr);
}