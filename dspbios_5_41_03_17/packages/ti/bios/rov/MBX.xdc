/*
 *  ======== MBX.xdc ========
 *
 *! Revision History
 *! ================
 *! 23-Oct-2008  cmcc  Created.
 */
 
package ti.bios.rov;

import xdc.rov.ViewInfo;

/*!
 *  ======== MBX ========
 *  Represents BIOS 5 MBX module in BIOS 5 ROV.
 */
module MBX {
    
    /*!
     *  ======== MBX_Obj ========
     *  BIOS 5 target structure representing a single MBX instance.
     */
     struct MBX_Obj {
        QUE.QUE_Elem    dataQue;	/* message queue */
        QUE.QUE_Elem    freeQue;	/* free queue */
        SEM.SEM_Obj     dataSem;	/* count = number of messages */
        SEM.SEM_Obj     freeSem;	/* count = number of free slots */

        Int     segid;		/* elements are allocated here */
        /* TODO - size_t */
        size_t  size;		/* size of mailbox elements */
        //UInt    size;
        UInt    len;		/* number of elements in mailbox */
        String  name;		/* printable name */
    };

    /*!
     *  ======== InstanceData ========
     *  View of a single MBX instance in BIOS 5 ROV.
     */
    metaonly struct InstanceData {
        String      name;
        Ptr         handle;
        Int         numTasksPending;
        String      tasksPending[];
        Int         numTasksBlockedPosting;
        String      tasksPosting[];
        Int         numMsgs;
        Int         maxMsgs;
        Int         msgSize;
        Int         memSegment;
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
     *  ======== getTskBlockedOn ========
     *  Checks to see if the given TSK is blocked on any of the MBX instances,
     *  and returns the name of the MBX if it finds it.
     */
    metaonly String getTskBlockedOn(Ptr tskAddr);
};