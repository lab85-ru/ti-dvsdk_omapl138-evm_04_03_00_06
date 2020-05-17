/*
 *  ======== viewInitInstances ========
 *  Retrieves all of the SEM instances and displays them in ROV.
 */
function viewInitInstances(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    var TSK = xdc.useModule('ti.bios.rov.TSK');
    
    /* Retrieve all of the OBJ_SEMs */
    try {
        var sems = OBJ.getSems();
    }
    catch (e) {
        throw (new Error("Problem retrieving SEM instances: " + 
                         Program.exToString(e)));
    }
    
    /* 
     * Cache the list of pended Tasks so TSK can later check what SEM its
     * blocked on.
     */
    var data = Program.getPrivateData('ti.bios.rov.SEM');
    data.pendedTasks = new Array();
    
    /* Process each of the OBJ_SEMs */
    for each (var sem in sems) {
        var viewElem = Program.newViewStruct('ti.bios.rov.SEM', 'Instances');
        
        /* Retrieve the SEM's name. */
        try {
            viewElem.name = getName(Number(sem.obj.$addr));
        }
        catch (e) {
            Program.displayError(viewElem, 'name', "Problem retrieving name " +
                                 "for SEM: " + Program.exToString(e));
        }
        
        viewElem.handle = sem.obj.$addr;
        
        viewElem.count = sem.obj.count;
        
        /* Scan the pendQ */
        try {
            var tskAddrs = QUE.scanQue(sem.obj.pendQ);
        
            viewElem.numTasksPending = tskAddrs.length;
            
            for each (var addr in tskAddrs) {
                /* Add the TSK to the pendQ */
                viewElem.pendQ.$add(TSK.getNickName(Number(addr)));

                /* 
                 * Add this task to the list of all pended tasks. TSK uses this
                 * to find blocked tasks.
                 */
                data.pendedTasks[data.pendedTasks.length] = 
                    {tskAddr: Number(addr), sem: viewElem.name};
            }
        
        }
        /* Catch problems scanning queue or retrieving nick names. */
        catch (e) {
            Program.displayError(viewElem, 'pendQ', "Problem retrieving " + 
                                 "list of pended TSKs: " + 
                                 Program.exToString(e));
            continue;
        }

        
        /* Add the SEM instance to the list. */
        view.elements.$add(viewElem);
    }
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
    var SEM = xdc.useModule('ti.bios.rov.SEM');
    
    return (ROV.getName('ti.bios.rov.SEM', Number(addr), true,
                        SEM.SEM_Obj$fetchDesc));
}

/*
 *  ======== getTskBlockedOn ========
 *  Checks all of the SEM pendQs for the given TSK, and returns the SEM
 *  name if it finds the TSK.
 */
function getTskBlockedOn(tskAddr)
{
    var Program = xdc.useModule('xdc.rov.Program');
    
    var data = Program.getPrivateData('ti.bios.rov.SEM');
    
    /* Scan the module data view to populate the list of pended tasks. */
    if (data.pendedTasks == undefined) {
        Program.scanModuleDataView('ti.bios.rov.SEM', 'Instances');
    }
    
    /* Check the list of pended TSKs for the given TSK. */
    for each (var pendedTask in data.pendedTasks) {
        if (Number(pendedTask.tskAddr) == Number(tskAddr)) {
            return (pendedTask.sem);
        }
    }
    
    /* Return "" if we didn't find the TSK. */
    return ("");
}