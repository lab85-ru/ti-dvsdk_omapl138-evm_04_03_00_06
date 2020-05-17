import xdc.rov.ISymbolTable;

metaonly module Recap {

    /*! 
     *  ======== locateRecap ========
     *  Returns the path to the appropriate pre-fabricated BIOS 5 recap file.
     *  
     *  The ISymbolTable instance is used to retrieve the ISA of the current
     *  program. There is one recap file for each ISA.
     */
    String locateRecap(ISymbolTable.Instance symTab);
   
}
