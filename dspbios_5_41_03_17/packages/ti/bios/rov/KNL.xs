/*
 *  ======== viewInitSystem ========
 *  Initializes the KNL "System" view, which provides system stack information.
 */
function viewInitSystem(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    
    var viewElem = Program.newViewStruct('ti.bios.rov.KNL', 'System');
    
    /* 
     * Get the currenty system tick count.
     * The system tick is a UInt stored at the symbol 'KNL_curtime'
     */
    var sysTickAddr = Program.getSymbolValue('KNL_curtime');
    var sysTick = Program.fetchStruct(ScalarStructs.S_UInt$fetchDesc, 
                                      sysTickAddr);
    
    viewElem.systemTick = sysTick.elem;
    
    /* 
     * The 55x has two stacks to display, all other targets don't need these
     * extra fields.
     */
    if (ROV.getISA() != '55') {
        viewElem.$status['sysStackBase'] = undefined;
        viewElem.$status['sysStackSize'] = undefined;
        viewElem.$status['sysStackPeak'] = undefined;
    }
    
    /* Retrieve the system stack information based on the ISA */
    
    /* 64 */
    if (ROV.getISA() == '64') {
        /* The 64x stack grows upward and is filled with 0x00C0FFEE */
        var stackView = getStackView('GBL_stackbeg', 'GBL_stackend', false, 
                                     0x00C0FFEE);
        viewElem.stackBase = stackView.stackBase;
        viewElem.stackSize = stackView.stackSize;
        viewElem.stackPeak = stackView.stackPeak;
    }
    /* 28 */
    else if (ROV.getISA() == '28') {
        /* The 28x stack grows down and is filled with 0xBEEF */
        var stackView = getStackView('GBL_stackbeg', 'GBL_stackend', true, 
                                     0xBEEF);
        viewElem.stackBase = stackView.stackBase;
        viewElem.stackSize = stackView.stackSize;
        viewElem.stackPeak = stackView.stackPeak;
    }    
    /* The 55x has multiple stack regions... */
    else if (ROV.getISA() == '55') {
        /* The 55x stack grows up and is filled with 0xBEEF */
        var stackView = getStackView('GBL_stackbeg', 'GBL_stackend', false, 
                                     0xBEEF);
        viewElem.stackBase = stackView.stackBase;
        viewElem.stackSize = stackView.stackSize;
        viewElem.stackPeak = stackView.stackPeak;

        /* The 55x sysstack grows up and is filled with 0xFEEB */
        var stackView = getStackView('GBL_sysstackbeg', 'GBL_sysstackend', false, 
                                     0xFEEB);
        viewElem.sysStackBase = stackView.stackBase;
        viewElem.sysStackSize = stackView.stackSize;
        viewElem.sysStackPeak = stackView.stackPeak;        
    }
    else {
        throw (new Error("Unrecognized ISA: " + ROV.getISA()));
    }
        
    view.elements.$add(viewElem);
}

/*
 *  ======== getStackView ========
 *  Calculates the stack information for the stack within the specified
 *  begin and end symbols.
 *  'growsDown' should be true if the stack grows from lower to higher
 *  addresses. 
 *  'filledWith' is the value that an empty stack is filled with; this
 *  is used to find the stack peak.
 */
function getStackView(stackBegSym, stackEndSym, growsDown, filledWith)
{
    var stackView = {};
    
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    
    /* Get the address of the beginning of the stack. */
    var stackBase = Program.getSymbolValue(stackBegSym);
    stackView.stackBase = "0x" + Number(stackBase).toString(16);
    
    /* Get the address of the end of the stack. */
    var stackEnd = Program.getSymbolValue(stackEndSym);
    
    /* Calculate the length of the stack in MAUs */
    stackView.stackSize = stackEnd - stackBase + 1;
    
    /* 
     * Determine the number of int values in the stack. 
     * The stackSize is in MAUs, and $sizeof returns the size in MAUs.
     */
    var numValues = stackView.stackSize / ScalarStructs.S_UInt.$sizeof();
    
    /* 
     * Read in the stack. The stack may be outside of a valid section, so
     * turn off address check.
     */
    var stackData = Program.fetchArray(ScalarStructs.S_UInt$fetchDesc, 
                                       stackBase, 
                                       numValues,
                                       false);
    
    /* Get the peak stack usage */
    stackView.stackPeak = getStackPeak(stackData, growsDown, filledWith);
    
    return (stackView);
}
 
/*
 *  ======== getStackPeak ========
 *  Returns the peak stack usage in MAUs of the given stack.
 *  'stackData' should be an array of UInts.
 */
function getStackPeak(stackData, growsDown, filledWith)
{
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    
    /* If the stack grows down (grows from low address to high) */
    if (growsDown) {
        var index = stackData.length - 1;
        
        /* Go through the stack until we find real data. */
        while(stackData[index].elem == filledWith) {
            index--;
        }

        /* Report the stackPeak in MAUs */
        return((index + 1) * ScalarStructs.S_UInt.$sizeof());
    }
    /* If the stack grows up (grows from high address to low) */
    else {
        var index = 0;
        
        /* Go through the stack until we find real data. */
        while(stackData[index].elem == filledWith) {
            index++;
        }

        /* Report the stack peak in MAUs */
        return((stackData.length - index) * ScalarStructs.S_UInt.$sizeof());
    }
}
 
/*
 *  ======== getQueues ========
 *  This function retrieves all of the KNL priority QUEs.
 *
 *  The number of queues is 20. 5 - 20 are the priority queues (there are
 *  always 16 priorities).
 *
 *  knl.c:
 *    QUE_Obj KNL_queues[KNL_NUMQUEUES]
 */
function getQueues()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    
    /* Retrieve the address of the priority queues. */
    var addr = Program.getSymbolValue('KNL_queues');

    Program.debugPrint("KNL -- Fetching array of KNL priority queues " +
                       "(KNL_queues) at 0x" + Number(addr).toString(16));
    
    /* Retrieve the array of priority queues. */
    var queueObjs = Program.fetchArray(QUE.QUE_Elem$fetchDesc, addr, 20);

    /* Scan each of the queues, and return an array of the scanned queues. */
    var queues = new Array();
    
    for each (var qObj in queueObjs) {
        queues[queues.length] = QUE.scanQue(qObj);
    }
    
    return (queues);
}

/*
 *  ======== getCurTaskAddr ========
 *  Gets the address of the current TSK for determining which TSK is running.
 */
function getCurTaskAddr()
{
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    
    /* Retrieve the Ptr at the address specified by KNL_curtask. */
    var addr = ROV.fetchGlobal(ScalarStructs.S_Ptr$fetchDesc, 'KNL_curtask');
    
    return (addr);   
}
 