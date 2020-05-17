/*
 *  ======== viewInitInstances ========
 *  Processes the views for all of the SWI instances.
 */
function viewInitInstances(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    
    var data = Program.getPrivateData('ti.bios.rov.SWI');
    
    /* Retrieve all of the SWI instances and cache them. */
    if (data.objects == undefined) {
        try {
            data.objects = getSwis();
        }
        catch (e) {
            throw (new Error("Problem retrieving SWI instances: " +
                             Program.exToString(e)));
        }
    }
    
    /* Get the fxn address of the currently running SWI. */
    var curFxnError = "";
    try {
        var curFxn = getCurFxn();
    }
    catch (e) {
        var curFxnError = "Problem retrieving currently running SWI fxn: " + 
                          Program.exToString(e);
    }
    
    /* Process each of the swi instances */
    for each (var swi in data.objects) {
        var viewElem = Program.newViewStruct('ti.bios.rov.SWI', 'Instances');
        
        viewElem.name = getName(Number(swi.$addr));
        
        viewElem.handle = "0x" + Number(swi.$addr).toString(16);
        
        /* Get the SWI's state. */
        if (curFxnError != "") {
            Program.displayError(viewElem, 'state', curFxnError);
        }
        else if (Number(swi.fxn) == Number(curFxn)) {
            viewElem.state = "Running";
        }
        else {
            viewElem.state = "Inactive";
        }
        
        /* Calculate the SWI's priority from its mask. */
        for (var i = 0; i < 16; i++) {
            if (Number(swi.mask) == (1 << i)) {
                viewElem.priority = i;
            }
        }
        
        viewElem.mailboxVal = swi.mailbox;
        
        var fxn = Program.lookupFuncName(Number(swi.fxn));
        viewElem.fxn = String(fxn[0]);
        
        viewElem.arg0 = swi.arg0;
        viewElem.arg1 = swi.arg1;
        viewElem.fxnAddr = swi.fxn;
        
        view.elements.$add(viewElem);
    }    
}

/*
 *  ======== getSwis ========
 *  Retrieve all of the swi objects from the SWI table.
 */
function getSwis()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var SWI = xdc.useModule('ti.bios.rov.SWI');
    
    /* Get the base address of the SWI table. */
    var tabBegAddr = Program.getSymbolValue('SWI_A_TABBEG');
    Program.debugPrint("SWI -- tabBegAddr = 0x" + Number(tabBegAddr).toString(16));
    
    /* Get the number of SWIs */
    var tabLen = Program.getSymbolValue('SWI_A_TABLEN');
    Program.debugPrint("SWI -- tabLen = " + tabLen);
    
    /* Retrieve all of the SWI instances. */
    var swis = Program.fetchArray(SWI.SWI_Obj$fetchDesc, tabBegAddr, tabLen);
   
    return (swis);
}

/*
 *  ======== getNickName ========
 *  Returns the task name used by other views such as LOG and SEM.
 */
function getNickName(addr) {
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    
    /* 
     * Convert address to unsigned.
     * The arguments are signed IArgs, so the address may be negative and
     * need to be converted.
     */
    if (addr < 0) {
        addr += Math.pow(2, 32);    
    }
    
    var name = getName(addr);
    
    /* 
     * If this is the KNL_swi, indicate to the user that 'KNL_swi' 
     * is the TSK scheduler.
     */
    if (name.equals("KNL_swi")) {
        name = name + " (TSK scheduler)";
    }
    
    return (ROV.getNickName(name, addr));
}

/*
 *  ======== getName ========
 *  Returns the name for the TSK at the given address.
 */
function getName(addr) {
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var SWI = xdc.useModule('ti.bios.rov.SWI');
    
    return (ROV.getName('ti.bios.rov.SWI', Number(addr), false, null));
}

/* 
 *  ======== getCurFxn ========
 *  SWI_D_curfxn points to the field in the currently running SWI which holds
 *  the SWI's function address.
 *  Dereference this pointer to get the address of the current running
 *  function.
 */
function getCurFxn() {
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    var Program = xdc.useModule('xdc.rov.Program');

    /* The symbol points to a field in the SWI_data structure. */
    var curFxnPtr = ROV.fetchGlobal(ScalarStructs.S_Ptr$fetchDesc, 'SWI_D_curfxn');
    
    /* 
     * If there's no currently running SWI, return null. 
     * On the 55x, this value may equal 0xDEAD.
     */
    if ((Number(curFxnPtr) == 0) ||
        ((ROV.getISA() == '55') && (Number(curFxnPtr == 0xDEAD)))) {
        return (null);
    }
        
    /* The SWI_data curFxn field points to the fxn field of the current SWI. */
    var curFxn = Program.fetchStruct(ScalarStructs.S_Ptr$fetchDesc, curFxnPtr).elem;

    return (curFxn);
}

/*
 *  ======== fxnToHandle ========
 *  The SWI LOG events log the SWI's fxn address rather than its handle. This
 *  function returns the handle for the given swi fxn.
 */
function fxnToHandle(fxn) {
    var Program = xdc.useModule('xdc.rov.Program');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
    
    var data = Program.getPrivateData('ti.bios.rov.SWI');
    
    /* Convert the value to unsigned, if necessary. */
    if (fxn < 0) {
        fxn += Math.pow(2, 32);    
    }
    
    /* First, check the fxnToHandle map to see if it has already been computed. */
    if (data.fxnToHandle != undefined) {
        if (data.fxnToHandle[fxn] != undefined) {
            return (data.fxnToHandle[fxn]);
        }
    }
    else {
        data.fxnToHandle = {};
    }
    
    /* 
     * 'fxn' points to a field in the SWI_Obj structure. Retrieve the actual
     * function address.
     */
    var fxnAddr = Program.fetchStruct(ScalarStructs.S_Ptr$fetchDesc, fxn).elem;
    
    /* Read in all of the swi objects. */
    if (data.objects == undefined) {
        data.objects = getSwis();
    }
    
    /* Look through all of the SWI objects and find the matching fxn. */
    for each (var swi in data.objects) {
        if (Number(fxnAddr) == Number(swi.fxn)) {
            data.fxnToHandle[fxn] = Number(swi.$addr);
            return (swi.$addr);
        }
    }
    
    throw (new Error("The specified address 0x" + Number(fxn).toString(16) + 
                     " does not match the function address of any known SWI " +
                     "instance."));
}