/*
 *  ======== SEM.xdc ========
 *
 *! Revision History
 *! ================
 *! 23-Oct-2008  cmcc  Created.
 */
 
package ti.bios.rov

import xdc.rov.ViewInfo;

/*!
 *  ======== SEM ========
 *  Represents BIOS 5 SEM module in BIOS 5 ROV.
 */
module SEM {

    /*!
     *  ======== SEM_Obj ========
     *  BIOS 5 target structure representing a single SEM instance.
     */
    struct SEM_Obj {
        KNL.KNL_Job     job;    /* handles interrupt-level posting */
        Int             count;  /* semaphore value */
        QUE.QUE_Elem    pendQ;  /* tasks wait here */
        String          name;   /* printable name */
    };

    /*!
     *  ======== InstanceData ========
     *  View of a single SEM instance in BIOS 5 ROV.
     */
    metaonly struct InstanceData {
        String      name;
        Ptr         handle;
        Int         count;
        Int         numTasksPending;
        String      pendQ[];
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
     *  Returns a "nick name" (a name to display to represent the instance) 
     *  for the instance at the given address.
     */
    metaonly String getNickName(Int addr);

    /*!
     *  ======== getTskBlockedOn ========
     *  Checks to see if the given TSK is blocked on any of the SEM instances,
     *  and returns the name of the SEM if it finds it.
     */
    metaonly String getTskBlockedOn(Ptr tskAddr);
}
