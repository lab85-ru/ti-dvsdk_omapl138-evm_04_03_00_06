/*
 *  ======== MEM.xs ========
 *  ROV view code for the MEM module.
 *   
 *  MEM.xs caches the following data in its internal data object from
 *  Program.getPrivateData:
 *    - freeLists: Map of segment ids to free lists. The free list is an array
 *      'Free List' view structures.
 */


/*
 *  ======== viewInitInstances ========
 *  Retrieves all of the MEM instances and displays them in ROV.
 *
 *  The instance view displays data from two sources:
 *    - MEM_segment object provides the base and length of the segment and the
 *      segment's name.
 *    - The segment's free list is processed to determine the current state of
 *      the segment.
 *
 *  The code for reading in the free list is shared between the Instances view,
 *  which processes the free list to determine the largest free block, etc., 
 *  and the Free List view, which simply displays the free list.
 *
 */
function viewInitInstances(view)
{
    var Program = xdc.useModule('xdc.rov.Program');

    /* Retrieve the array of MEM_Segment objects. */
    var segObjs = getSegObjs();
    
    /* Retrieve the free list for each MEM segment. */
    getFreeLists(segObjs.length);
    
    var data = Program.getPrivateData('ti.bios.rov.MEM');
    
    /* Create a view for each segment. */
    Program.debugPrint("MEM -- Processing MEM segment views.");
    for (var segId = 0; segId < segObjs.length; segId++) {
        var seg = segObjs[segId];
        
        Program.debugPrint("MEM -- Processing view for MEM segment 0x" + Number(seg.base));
        
        var viewElem = Program.newViewStruct('ti.bios.rov.MEM', 'Instances');
    
        viewElem.name = getName(seg);

        /* 
         * Display start and end address first, in case the free list 
         * processing errors.
         */
        viewElem.startAddress = "0x" + Number(seg.base).toString(16);
        
        viewElem.endAddress = "0x" + Number(seg.base + seg.len).toString(16);
        
        /* Retrieve the segment's free list. */
        var freeList = data.freeLists[segId];

        /* 
         * Process the freeList and determine the largest free block and 
         * the total amount of free memory in the MEM.
         */
        viewElem.largestFree = 0;
        viewElem.freeMem = 0;
        for each (var elem in freeList) {
            viewElem.freeMem += elem.size;
            
            if (elem.size > viewElem.largestFree) {
                viewElem.largestFree = elem.size;
            }
        }
   
        viewElem.usedMem = Number(seg.len) - Number(viewElem.freeMem);
        
        viewElem.totalSize = seg.len;
                
        view.elements.$add(viewElem);
    }
}

/*
 *  ======== getSegObjs ========
 *  Retrieves the segments array from the global array MEM_memtab.
 *    mem.h:
 *      extern MEM_Segment      MEM_memtab[];
 *
 *  This API is only called by viewInitInstances, so no need to cache the
 *  segments array.
 */
function getSegObjs()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var MEM = xdc.useModule('ti.bios.rov.MEM');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
        
    Program.debugPrint("MEM -- Retrieving segments.");
    
    /*
     *  Retrieve the number of segments from the global variable MEM_D_NUMSEGMENTS.
     *    _mem.h:
     *      extern Int   MEM_D_NUMSEGMENTS;
     */
    var numSegments = ROV.fetchGlobal(ScalarStructs.S_Int$fetchDesc, "MEM_D_NUMSEGMENTS");
    Program.debugPrint("MEM -- Number of segments: " + numSegments);
    
    /* Get the base address of the MEM table. */
    var memTabAddr = Program.getSymbolValue("MEM_memtab");
    
    /* 
     * Get the appropriate fetchDesc for the MEM_Segment and MEM_header based
     * on the ISA.
     */
    var segFetchDesc;
    if (ROV.getISA() == '64') {
        segFetchDesc = MEM.MEM_Segment6x$fetchDesc;
    }
    else if ((ROV.getISA() == '28') || (ROV.getISA() == '55')) {
        segFetchDesc = MEM.MEM_Segment28x55x$fetchDesc;
    }
    else {
        throw (new Error("Unrecognized ISA: " + ROV.getISA()));
    }
    
    /* Fetch all of the MEM_Segments. */
    var segObjs = Program.fetchArray(segFetchDesc, memTabAddr, numSegments);    
    
    return (segObjs);
}

/*
 *  ======== getName ========
 *  Retrieves the name for the given MEM instance.
 */
function getName(seg)
{
    var Program = xdc.useModule('xdc.rov.Program');
    
    /* Check for a symbol at the segment's base address. */
    var syms = Program.lookupDataSymbol(Number(seg.base));

    /* The symbol name should end with '$B' */
    if ((syms != null) && (syms.length != 0) && 
        (syms[0].indexOf('$B') != -1)) {
        var name = syms[0].substring(0, syms[0].length - 2);
    }
    /* If we don't find a name, just use the base address of the segment. */
    else {
        var name = "0x" + Number(seg.base).toString(16);
    }
    
    return (name);
}

/*
 *  ======== getFreeLists ========
 *  Retrieves all of the free lists and caches them in the data object.
 *  Each free list is an array of MEM.FreeListView structures.
 */
function getFreeLists(numSegs)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var MEM = xdc.useModule('ti.bios.rov.MEM');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    
    Program.debugPrint("MEM -- Retrieving free lists.");
    
    /* Get the address of the array of freelist headers. */
    var freeListArrAddr = ROV.fetchGlobal(ScalarStructs.S_Ptr$fetchDesc, "MEM_D_freelist");
    Program.debugPrint("MEM -- Free list root headers: = 0x" + Number(freeListArrAddr).toString(16));
    
    /* Get the fetchDesc for the MEM_Header based on the ISA. */
    var hdrFetchDesc;
    if (ROV.getISA() == '64') {
        hdrFetchDesc = MEM.MEM_Header6x$fetchDesc;
    }
    else if ((ROV.getISA() == '28') || (ROV.getISA() == '55')) {
        hdrFetchDesc = MEM.MEM_Header28x55x$fetchDesc;
    }
    else {
        throw (new Error("Unrecognized ISA: " + ROV.getISA()));
    }

    /* Fetch the freelist header array */
    var freeListHeaders = Program.fetchArray(hdrFetchDesc, freeListArrAddr, numSegs);
    
    /* Cache the free lists, since they will also be referenced by viewInitFreeLists. */
    var data = Program.getPrivateData('ti.bios.rov.MEM');       
    data.freeLists = new Array();
    
    for (var segId = 0; segId < freeListHeaders.length; segId++) {
        var elements = new Array();
        
        /* Start at the header, but don't include the header in the free list. */
        var elem = freeListHeaders[segId];
        
        /* Fetch the rest of the free list. */
        while (elem.next != 0) {
            /* Hang on to the previous address for error reporting. */
            var prevAddr = elem.next;
            
            /* Retrieve the next header. */
            try {
                var elem = fetchHeader(Number(elem.next));
            }
            catch (e) {
                /* Create a view element to display the error. */
                var viewElem = Program.newViewStruct('ti.bios.rov.MEM', 'FreeLists');
                viewElem.address = prevAddr;
                
                /* Associate the error with the address field of the block. */
                var error = "Problem retrieving MEM free list element: " +
                            Program.exToString(e);
                Program.displayError(viewElem, 'address', error);
                
                /* Add the element to the list. */
                elements[elements.length] = viewElem;
                
                /* Store the list in the cache. */
                data.freeLists[Number(baseAddr)] = elements;
                
                /* 
                 * Throw an exception so the instance view can know that something
                 * went wrong.
                 */
                throw (new Error(error));
            }
        
            var viewElem = Program.newViewStruct('ti.bios.rov.MEM', 'FreeLists');
            viewElem.address = elem.$addr;
            viewElem.size = elem.size;
            
            elements[elements.length] = viewElem;    
        }
        
        /* Cache the result. */
        data.freeLists[segId] = elements;
    }
}

/*
 *  ======== fetchHeader ========
 *  Fetches the MEM_Header at the given address.
 *  Helper function to deal with different ISAs. 
 */
function fetchHeader(addr)
{
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var MEM = xdc.useModule('ti.bios.rov.MEM');
    var Program = xdc.useModule('xdc.rov.Program');
    
    Program.debugPrint("MEM -- Fetching MEM_Header at 0x" + Number(addr).toString(16));
    
    if (ROV.getISA() == '64') {
        var elem = Program.fetchStruct(MEM.MEM_Header6x$fetchDesc, addr);
    }
    else if ((ROV.getISA() == '28') || (ROV.getISA() == '55')) {
        var elem = Program.fetchStruct(MEM.MEM_Header28x55x$fetchDesc, addr);
    }
    else {
        throw (new Error("Unrecognized ISA: " + ROV.getISA()));
    }
    
    return (elem);
}

/*
 *  ======== viewInitFreeLists ========
 *  Processes the MEM "Free List" view.
 *
 *  This view relies on the 'Instances' view for the segment names and for 
 *  retrieving the free lists. It first scans the 'Instances' view, then 
 *  accesses the free lists from the internal data object.
 */
function viewInitFreeLists(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');

    var data = Program.getPrivateData('ti.bios.rov.MEM');
    
    /* Retrieve the instance view first to retrieve the instance names. */
    var segViews = Program.scanModuleDataView('ti.bios.rov.MEM', 'Instances').elements;
    
    var instViews = new Array();
    
    /* For each of the segments... */
    for (var segId = 0; segId < segViews.length; segId++) {
        var view = new Program.InstDataView;
        
        /* Get the label from the 'Instances' view */
        view.label = segViews[segId].name;
        
        /* Retrieve the free list for this MEM which was read by 'Instances' */
        view.elements = data.freeLists[segId];
    
        instViews[instViews.length] = view;
    }
    
    return (instViews);
}
