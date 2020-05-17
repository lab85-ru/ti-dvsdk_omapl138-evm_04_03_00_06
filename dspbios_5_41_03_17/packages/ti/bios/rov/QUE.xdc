/*
 *  ======== QUE.xdc ========
 *
 *! Revision History
 *! ================
 *! 23-Oct-2008  cmcc  Created.
 */

package ti.bios.rov

/*!
 *  ======== QUE ========
 *  Represents a BIOS 5 QUE.
 *
 *  KOV does not include a view for QUE as does BIOS 6, but this module
 *  provides support for scanning QUEs to retrieve all of their elements.
 */
module QUE {
    
    /*!
     *  ======== QUE_Elem ========
     *  QUE element structure.
     */
    struct QUE_Elem {
        QUE_Elem *next;
        QUE_Elem *prev;
    };
    
    /*!
     *  ======== scanQue ========
     *  Scans a QUE and returns an array of the addresses of the elements
     *  on the QUE.
     */
    metaonly Any scanQue(Any obj);
}
