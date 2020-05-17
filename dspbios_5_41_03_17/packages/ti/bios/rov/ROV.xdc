/*
 *  ======== ROV.xdc ========
 *
 *! Revision History
 *! ================
 *! 22-Oct-2008  cmcc  Created.
 */
 
package ti.bios.rov

/*!
 *  ======== ROV ========
 *  Support module for BIOS 5 ROV.
 *
 *  This module provides helper functions for common tasks in implementing
 *  BIOS 5 ROV. 
 */
module ROV {
    
    /*!
     *  ======== fetchGlobal ========
     *  Retrieves the specified global variable.
     *
     *  The type of the variable is specified by fetchDesc, which should 
     *  be the fetchDesc for one of the scalar structs in 
     *  xdc.rov.support.ScalarStructs.
     */
    metaonly Any fetchGlobal(Any fetchDesc, String symbolName);
    
    /*!
     *  ======== getISA ========
     *  Returns the target's ISA as a string: '55', '64', etc.
     */
    metaonly String getISA();
    
    /*!
     *  ======== getNickName ========
     *  Format name into a "nick name".
     *
     *  Formats the given name and handle into a nick name, such as 
     *  "tsk0 (0x13DC8)".
     */
    metaonly String getNickName(String name, Ptr addr);
    
    /*!
     *  ======== getName ========
     *  Returns the name for the instance at the given address.
     *
     *  This API will check for a static name first, then if hasDynamic is
     *  true, it will check for a dynamic name.
     *
     *  This API maintains a map of addresses to names in the given module's
     *  private data structure so that the names are cached.
     */
    metaonly String getName(String modName, Ptr addr, Bool hasDynamic,
                            Any fetchDesc);
    
    /*!
     *  ======== getStaticName ========
     *  Returns the object name at the given address.
     *
     *  This is a helper function for getName.
     *
     *  It is also used by the LOG module to get instance names for modules 
     *  such as PRD that have no other ROV representation.
     */
    metaonly String getStaticName(Ptr addr);
    
}