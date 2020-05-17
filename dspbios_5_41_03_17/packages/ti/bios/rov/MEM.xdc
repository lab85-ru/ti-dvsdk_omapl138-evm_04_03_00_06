/*
 *  ======== MEM.xdc ========
 *
 *! Revision History
 *! ================
 *! 23-Oct-2008  cmcc  Created.
 */

package ti.bios.rov

import xdc.rov.ViewInfo;

/*!
 *  ======== MEM ========
 *  Represents BIOS 5 MEM module in BIOS 5 ROV.
 */
module MEM {

    /*!
     *  ======== MEM_Segment6x ========
     *  BIOS 5 target structure representing a MEM_Segment on 6x devices.
     *
     *  MEM_sizep is defined:
     *    55L, 55H, 28  -   unsigned long
     *    6x            -   unsigned int
     */
    struct MEM_Segment6x {
        Ptr        base;            /* Base of the segment */
        UInt       len; //MEM_sizep /* Size of the segment */
        Int        space;           /* x, y or p memory */
    };

    /*!
     *  ======== MEM_Segment28x55x ========
     *  BIOS 5 target structure representing a MEM_Segment on 28x and 55x 
     *  devices.
     *
     *  MEM_sizep is defined:
     *    55L, 55H, 28  -   unsigned long
     *    6x            -   unsigned int
     */    
    struct MEM_Segment28x55x {
        Ptr        base;            /* Base of the segment */
        ULong      len; //MEM_sizep /* Size of the segment */
        Int        space;           /* x, y or p memory */
    };
    
    /*!
     *  ======== MEM_Header6x ========
     *  BIOS 5 target structure representing a MEM_Header on 6x devices.
     *
     *  MEM_sizep is defined:
     *    55L, 55H, 28  -   unsigned long
     *    6x            -   unsigned int
     */ 
    struct MEM_Header6x {
        MEM_Header6x    *next;   /* form a free memory link list */
        UInt             size; //MEM_sizep  /* size of the free memory */
    };
    
    /*!
     *  ======== MEM_Header28x55x ========
     *  BIOS 5 target structure representing a MEM_Header on 28x and 55x 
     *  devices.
     *
     *  MEM_sizep is defined:
     *    55L, 55H, 28  -   unsigned long
     *    6x            -   unsigned int
     */ 
    struct MEM_Header28x55x {
        MEM_Header6x    *next;   /* form a free memory link list */
        ULong            size; //MEM_sizep  /* size of the free memory */
    };

    /*!
     *  ======== InstanceData ========
     *  View of a single MEM instance in BIOS 5 ROV.
     */
    metaonly struct InstanceData {
        String      name;
        Int         largestFree;
        Int         freeMem;
        Int         usedMem;
        Int         totalSize;
        Ptr         startAddress;
        Ptr         endAddress;        
    };
    
    /*!
     *  ======== FreeListElement ========
     *  View of a single block of memory in the MEM free list in BIOS 5 ROV.
     */
    metaonly struct FreeListElement {
        Ptr     address;
        Int     size;
    }
   
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['Instances', {type: ViewInfo.MODULE_DATA, viewInitFxn: 'viewInitInstances', structName: 'InstanceData'}],
                ['FreeLists', {type: ViewInfo.INSTANCE_DATA, viewInitFxn: 'viewInitFreeLists', structName: 'FreeListElement'}],
            ],
            /* Nothing to show in the raw tab. */
            showRawTab: false
        });


}
