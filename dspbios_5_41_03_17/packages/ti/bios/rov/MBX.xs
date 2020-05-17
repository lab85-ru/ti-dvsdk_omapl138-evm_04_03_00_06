/*
 *  ======== viewInitInstances ========
 *  View function for displaying all of the MBX instances.
 */
function viewInitInstances(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    var TSK = xdc.useModule('ti.bios.rov.TSK');
    
    /* Retrieve all of the OBJ_MBX objects. */
    try {
        var mbxs = OBJ.getMailboxes();
    }
    catch (e) {
        throw (new Error("Problem retrieving MBX instances: " +
                         Program.exToString(e)));
    }
    
    /* Store an array of all of the tasks pended on MBXs */
    var data = Program.getPrivateData('ti.bios.rov.MBX');
    data.pendedTasks = new Array();
       
    /* Process each of the OBJ_MBX instances */
    for each (var mbx in mbxs) {

        var viewElem = Program.newViewStruct('ti.bios.rov.MBX', 'Instances');
        
        viewElem.name = getName(Number(mbx.obj.$addr));
            
        viewElem.handle = mbx.obj.$addr;
        
        /* Scan the dataSem to find all of the tasks blocked pending. */
        try {
            var tskAddrs = QUE.scanQue(mbx.obj.dataSem.pendQ);
            
            viewElem.numTasksPending = tskAddrs.length;
        
            /* Create the que view */
            for each (var tskAddr in tskAddrs) {

                viewElem.tasksPending.$add(TSK.getNickName(Number(tskAddr)));
            
                data.pendedTasks[data.pendedTasks.length] = 
                    {tskAddr: Number(tskAddr), mbx: viewElem.name};
            }
        }
        catch (e) {
            Program.displayError(viewElem, 'tasksPending', "Problem " + 
                                 "retrieving list of pended TSKs: " + 
                                 Program.exToString(e));
        }
        
        /* Scan the freeSem to find all of the tasks blocked posting. */
        try {
            var tskAddrs = QUE.scanQue(mbx.obj.freeSem.pendQ);
        
            viewElem.numTasksBlockedPosting = tskAddrs.length;

            /* Create the que view */
            for each (var tskAddr in tskAddrs) {
                viewElem.tasksPosting.$add(TSK.getNickName(Number(tskAddr)));
            
                data.pendedTasks[data.pendedTasks.length] = 
                    {tskAddr: Number(tskAddr), mbx: viewElem.name};
            }
        }
        catch (e) {
            Program.displayError(viewElem, 'tasksPending', "Problem " + 
                                 "retrieving list of TSKs posting: " + 
                                 Program.exToString(e));
        }
        
        /* Scan the data que to find the current number of messages. */
        try {
            var dataQue = QUE.scanQue(mbx.obj.dataQue);
            viewElem.numMsgs = dataQue.length;
        }
        catch (e) {
            Program.displayError(viewElem, 'numMsgs', "Problem retrieving " +
                                 "list of messages: " + Program.exToString(e));
        }
        
        viewElem.maxMsgs = mbx.obj.len;
        viewElem.msgSize = mbx.obj.size;
        viewElem.memSegment = mbx.obj.segid;
        
        view.elements.$add(viewElem);
    }    
}

/*
 *  ======== getTskBlockedOn ========
 *  Checks all of the MBX pendQs for the given TSK, and returns the MBX
 *  name if it finds the TSK.
 */
function getTskBlockedOn(tskAddr)
{
    var Program = xdc.useModule('xdc.rov.Program');
    
    var data = Program.getPrivateData('ti.bios.rov.MBX');
    
    /* Scan the module data view to populate the list of pended tasks. */
    if (data.pendedTasks == undefined) {
        Program.scanModuleDataView('ti.bios.rov.MBX', 'Instances');
    }
    
    /* Check the list of pended TSKs for the given TSK. */
    for each (var pendedTask in data.pendedTasks) {
        if (Number(pendedTask.tskAddr) == Number(tskAddr)) {
            return (pendedTask.mbx);
        }
    }

    /* Return "" if we didn't find the TSK. */
    return ("");
}

/*
 *  ======== getNickName ========
 *  Returns the sem name used by other views such as TSK.
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
 *  Returns the name for the SEM at the given address.
 */
function getName(addr) {
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    var MBX = xdc.useModule('ti.bios.rov.MBX');
    
    return (ROV.getName('ti.bios.rov.MBX', Number(addr), true,
                        MBX.MBX_Obj$fetchDesc));
}