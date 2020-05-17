/*
 *  ======== OBJ.xdc ========
 *
 *! Revision History
 *! ================
 *! 22-Oct-2008  cmcc  Created.
 */

package ti.bios.rov;

/*!
 *  ======== OBJ ========
 *  Represents the BIOS 5 OBJ table.
 *
 *  The BIOS 5 OBJ table maintained lists of all of the static and dynamic
 *  instances of particular modules.
 *
 *  Not all modules used this OBJ table, however. Only the TSKs, SEMs, and
 *  MBXs are retrieved from it for BIOS 5 ROV.
 *
 *  Each index in the OBJ table contains a linked list of instances for 
 *  the module at that index. The objType enum specifies the indexes. The
 *  instances are represented by OBJ_* structures which contain a QUE
 *  element for linking, the type of the instance, and Mod_Obj structure
 *  for the instance itself.
 */
module OBJ {
    
    /*!
     *  ======== OBJ_TSK ========
     *  A single TSK on the OBJ TSK list.
     */
    struct OBJ_TSK {
        QUE.QUE_Elem    link;
        UInt            type;
        ti.bios.rov.TSK.TSK_Obj     obj;
    };
    
    /*!
     *  ======== OBJ_TSK_55x ========
     *  A single TSK on the OBJ TSK list.
     */
    struct OBJ_TSK_55x {
        QUE.QUE_Elem    link;
        UInt            type;
        ti.bios.rov.TSK.TSK_Obj_55x     obj;
    };
    
    /*!
     *  ======== OBJ_SEM ========
     *  A single SEM on the OBJ SEM list.
     */
    struct OBJ_SEM {
        QUE.QUE_Elem    link;
        UInt            type;
        ti.bios.rov.SEM.SEM_Obj     obj;
    }
    
    /*!
     *  ======== OBJ_MBX ========
     *  A single MBX on the OBJ MBX list.
     */
    struct OBJ_MBX {
        QUE.QUE_Elem    link;
        UInt            type;
        ti.bios.rov.MBX.MBX_Obj     obj;
    }

    /*!
     *  ======== objType ========
     *  The type of the instances at the corresponding index in the OBJ table.
     */
    enum objType {
        SEM,
        SWI,
        TSK,
        MBX,
        SIO,
        DEV,
        BUF
    };
       
    /*!
     *  ======== getTasks ========
     *  Retrieves all of the OBJ_TSKs from the OBJ table.
     */
    metaonly Any getTasks();
 
    /*!
     *  ======== getSems ========
     *  Retrieves all of the OBJ_SEMs from the OBJ table.
     */
    metaonly Any getSems();
       
    /*!
     *  ======== getMailboxes ========
     *  Retrieves all of the OBJ_MBXs from the OBJ table.
     */
    metaonly Any getMailboxes();
}