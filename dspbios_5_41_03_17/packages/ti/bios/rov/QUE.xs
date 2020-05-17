/*
 *  ======== scanQue ========
 */
function scanQue(obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    
    /* 
     * To check for loops, store each address as a key in a map,
     * then with each new address check first if it is already in
     * the map.
     */
    var addrs = {};
    
    var elements = [];
    
    /* Check for an uninitialized QUE. */
    if (Number(obj.next) == 0) {
        throw (new Error("Unitialized QUE. QUE.next = 0x0"));
    }
    
    var elem = obj;
    
    /* Loop through the queue until we get back to the 'firstElem' */
    while ((Number(elem.next) != Number(obj.$addr))) {
        
        /* Before fetching the next element, verify we're not in a loop. */
        if (elem.next in addrs) {
            throw (new Error("Detected a loop while scanning QUE 0x" + 
                             Number(obj.$addr).toString(16) + 
                             ". Encountered 0x" +
                             Number(elem.next).toString(16) + " twice."));
        }
        
        /* Fetch the next element and add it to the array of elements. */
        elem = Program.fetchStruct(QUE.QUE_Elem$fetchDesc, elem.next);
            
        elements[elements.length] = elem.$addr;
        
        /* 
         * Add the address to a map so we can check for loops. 
         * The value 'true' is just a placeholder to make sure the address
         * is in the map.
         */
        addrs[elem.$addr] = true;
    }

    return (elements);
}