
/*
 *  ======== viewInitInstances ========
 *  Displays all of the TSK instances in BIOS 5 ROV.
 */
function viewInitInstances(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    var KNL = xdc.useModule('ti.bios.rov.KNL');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');

    /* 
     * Retrieve all of the TSK objects (static and dynamic) from 
     * the OBJ table.
     * The objects returned are of type OBJ.OBJ_TSK, so they include
     * the OBJ header, and the TSK object itself is under the 'obj' field.
     */
    try {
        var tsks = getTasks();
    }
    catch (e) {
        throw (new Error("Problem retrieving TSK instances: " +
                         Program.exToString(e)));
    }
    
    var taskStateError = "";
    try {
        /* Retrieve the list of Tasks in the "ready" state */
        var readyTasks = getReadyTasks();
        
        /* Get the address of the "Running" TSK from the KNL module. */
        var curTaskAddr = KNL.getCurTaskAddr();
    }
    catch (e) {
        taskStateError = "Problem retrieving TSK state: " +
                         Program.exToString(e);
    }
    
    /* Process each of the TSKs and display it. */
    for each (var tsk in tsks) {
        Program.debugPrint("Processing TSK 0x" + Number(tsk.$addr).toString(16));
        
        /* Create a new view structure for the TSK. */
        var viewElem = Program.newViewStruct('ti.bios.rov.TSK', 'Instances');
        
        viewElem.name = getName(Number(tsk.obj.$addr));
        
        viewElem.handle = tsk.obj.$addr;
        
        var state = "";
        /* Check if there was an error retrieving the TSK states. */
        if (taskStateError != "") {
            Program.displayError(viewElem, 'state', taskStateError);
        }
        /* Check if this is the running task */
        else if (Number(viewElem.handle) == Number(curTaskAddr)) {
            viewElem.state = "Running";
        }
        /* Otherwise, check if this task is ready or termindated. */
        else {
            try {
                viewElem.state = getTaskState(tsk.obj.$addr, viewElem, 
                                              readyTasks);
            }
            catch (e) {
                Program.displayError(viewElem, 'state', "Problem retrieving " +
                                     "TSK state: " + Program.exToString(e));
            }
        }

        /* 
         * The TSK's priority and timeout values come from the TSK's embedded
         * KNL_Obj object.
         */
        viewElem.priority = tsk.obj.kobj.priority;
        viewElem.timeout = tsk.obj.kobj.timeout;
        
        try {
            /* Get the current 'system' time. */
            var curTime = ROV.fetchGlobal(ScalarStructs.S_Int$fetchDesc,
                                      "KNL_curtime");
        
            /* If the timeout has passed... */
            if (tsk.obj.kobj.timeout < curTime) {
                /* TODO - Error, shouldn't happen? */
                viewElem.timeRemaining = 0;
            }
            else {
                viewElem.timeRemaining = tsk.obj.kobj.timeout - curTime;
            }
        }
        catch (e) {
            Program.displayError(viewElem, 'timeRemaining', "Problem " +
                                 "retrieving TSK time remaining: " + 
                                 Program.exToString(e));
        }
       
        viewElem.stackBase = tsk.obj.stack;
        viewElem.stackSize = tsk.obj.stacksize;

        /* Fetch the TSK stack to calculate the stack peak. */
        try {
            /* 
             * We want fetchArray to return an array of scalars, not scalar 
             * structs, so create the fetch descriptor manually.
             */
            var fetchDesc = {type: 'xdc.rov.support.ScalarStructs.S_UChar',
                             isScalar: true};
            var stackData = Program.fetchArray(fetchDesc, tsk.obj.stack, 
                                               tsk.obj.stacksize);
            
            /* Calculate the stack peak. */
            viewElem.stackPeak = getStackPeak(stackData);
        }
        catch (e) {
            Program.displayError(viewElem, 'stackPeak', "Problem retrieving " +
                                 "TSK stack: " + Program.exToString(e));
        }
        
        /* 
         * The 55x has two stacks to display, all other targets don't need
         * these extra fields.
         */
        if (ROV.getISA() == '55') {
            viewElem.sysStackBase = tsk.obj.sysstack;
            viewElem.sysStackSize = tsk.obj.sysstacksize;
            
            /* Fetch the TSK stack to calculate the stack peak. */
            try {
                /* 
                 * We want fetchArray to return an array of scalars, not scalar
                 * structs, so create the fetch descriptor manually.
                 */
                var fetchDesc = {type: 'xdc.rov.support.ScalarStructs.S_UChar',
                                 isScalar: true};
                var stackData = Program.fetchArray(fetchDesc, tsk.obj.sysstack,
                                                   tsk.obj.sysstacksize);
                
                /* Calculate the sysstack peak. */
                viewElem.sysStackPeak = getStackPeak(stackData, true);
            }
            catch (e) {
                Program.displayError(viewElem, 'sysStackPeak', "Problem " + 
                                     "retrieving TSK sysStack: " + 
                                     Program.exToString(e));
            }
        }
        else {
            viewElem.$status['sysStackBase'] = undefined;
            viewElem.$status['sysStackSize'] = undefined;
            viewElem.$status['sysStackPeak'] = undefined;
        }
        
        view.elements.$add(viewElem);
    }
 }

/*
 *  ======== getReadyTasks ========
 *  Helper function. This function scans the KNL priority queues to determine
 *  which tasks are "ready" and which priority queue they are in.
 */
function getReadyTasks()
{
    var KNL = xdc.useModule('ti.bios.rov.KNL');
    
    /* Retrieve the priority queues from the KNL module. */
    var queues = KNL.getQueues();
    
    var readyTasks = new Array();
    
    /* 
     * There are 20 queues. Queues 0-3 are special, 4-20 are the
     * actual queues. 
     */
    for (var pri = 0; pri < 20; pri++) {
        var queue = queues[pri];
        
        /* Each queue is a QUE_View containing the addresses of all the elements. */
        for each (var addr in queue) {
            readyTasks[readyTasks.length] = {priority: pri, addr: addr};
        }
    }
    
    return (readyTasks);
}

/*
 *  ======== getTasks ========
 *  Helper function. Retrieves the TSK list from the OBJ_table. 
 *  This API handles caching of the OBJ_TSK list.
 */
function getTasks()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    
    /* Cache the TSK list. */
    var data = Program.getPrivateData('ti.bios.rov.TSK');
    
    /* Check if we've already read in the tasks. */
    if (data.objTsks == undefined) {
        data.objTsks = OBJ.getTasks();        
    }
    
    return (data.objTsks);
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
    
    return (ROV.getNickName(name, addr));
}

/*
 *  ======== getName ========
 *  Returns the name for the TSK at the given address.
 */
function getName(addr) {
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var TSK = xdc.useModule('ti.bios.rov.TSK');
    
    if (ROV.getISA() == '55') {
        return (ROV.getName('ti.bios.rov.TSK', Number(addr), true,
                            TSK.TSK_Obj_55x$fetchDesc));
    }
    else {
        return (ROV.getName('ti.bios.rov.TSK', Number(addr), true,
                            TSK.TSK_Obj$fetchDesc));
    }
}
 
/*
 *  ======== getTaskState ========
 *  This function looks through the readyTasks array (returned by 
 *  getReadyTasks) to see if the given task (represented by the
 *  address of it's KNL_Obj) is ready.
 */
function getTaskState(tskObjAddr, viewElem, readyTasks)
{
    var SEM = xdc.useModule('ti.bios.rov.SEM');
    var MBX = xdc.useModule('ti.bios.rov.MBX');
    
    /* 
     * If the TSK is in queue 3, it's inactive, if it's in queue 4, 
     * it's ready. 
     */
    for each (var tsk in readyTasks) {
        if (Number(tsk.addr) == Number(tskObjAddr)) {
            if (tsk.priority == 3) {
                return ("Terminated");
            }
            else if (tsk.priority >= 4) {
                return ("Ready");
            }
        }
    }
    
    /* Check all of the SEMs */
    var semName = SEM.getTskBlockedOn(tskObjAddr);
    
    if (semName != "") {
        viewElem.blockedOn = "SEM: " + semName;
        return("Blocked");
    }
    
    /* Check all of the MBXs */
    var mbxName = MBX.getTskBlockedOn(tskObjAddr);
    
    if (mbxName != "") {
        viewElem.blockedOn = "MBX: " + mbxName;
        return("Blocked");
    }
    
    return ("<unknown>");
}

/*
 *  ======== getStackPeak ========
 *  Calculates the stack peak. The implementation is target-specific.
 *  This code is somewhat redundant with the stack peak code in the KNL module.
 *  However, the system stack fill values are different on the 6x, and the
 *  boundaries are marked by symbols rather than fields in a structure.
 */
function getStackPeak(stackData, isSysStack)
{
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    
    /* For 64x */
    if (ROV.getISA() == '64') {
        var size = stackData.length;
        var index = 0;
        while(stackData[index] == 0xBE) {
            index++;
        }
        return (size - index);
    }
    /* For 28x */
    else if (ROV.getISA() == '28') {
        var index = stackData.length - 1;
        
        while(stackData[index] == 0xBEEF) {
            index--;
        }
        return(index);
    }
    /* For 55x */
    else if (ROV.getISA() == '55') {
        var size = stackData.length;
        var index = 0;

        /* The 55x sysstack is filled with a different value. */
        var filledWith;
        if (isSysStack) {
            filledWith = 0xFEEB;
        }
        else {
            filledWith = 0xBEEF;
        }
        
        while(stackData[index] == filledWith) {
            index++;
        }

        return (size - index);
    }
    else {
        throw (new Error("Stack peak: Unrecognized ISA: " + ROV.getISA()));
    }
}
