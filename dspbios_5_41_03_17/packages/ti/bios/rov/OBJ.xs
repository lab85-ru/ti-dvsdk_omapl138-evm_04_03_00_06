
/*
 *  ======== getObjects ========
 *  Leave it to the modules themselves to decide whether these objects should
 *  be cached.
 */
function getObjects(type, fetchDesc)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    
    /* Get the object table */
    Program.debugPrint("OBJ -- Retrieving the OBJ table.");
    var objTable = getObjTable();
    
    /* Retrieve the QUE of objects for the requested type. */
    var objQ = objTable[Number(type)];
    
    /* Scan the QUE to get the addresses of the OBJ_*s */
    Program.debugPrint("OBJ -- Scanning OBJ_" + String(type) + " queue.");
    var objAddrs = QUE.scanQue(objQ);
    
    /* Fetch each of the OBJ_* structures */
    Program.debugPrint("OBJ -- Fetching each OBJ_" + String(type) + 
                       " structure...");
    var objs = new Array();
    for each (var objAddr in objAddrs) {
        Program.debugPrint("OBJ -- Fetching OBJ_" + String(type) + 
                           " at addr 0x" + Number(objAddr).toString(16));
        objs[objs.length] = Program.fetchStruct(fetchDesc, objAddr);
    }
    
    return (objs);
}

/* 
 *  ======== getObjTable ========
 *  Fetches the OBJ table array.
 *
 *  Returns an array of unscanned QUE objects.
 *
 *  The OBJ table is defined in obj.h:
 *    QUE_Obj   OBJ_table[OBJ_MODULES];
 */
function getObjTable()
{   
    var Program = xdc.useModule('xdc.rov.Program');
    var QUE = xdc.useModule('ti.bios.rov.QUE');
    
    /* 
     * OBJ doesn't have private data since it's not part of the configuration, 
     * so we'll cheat and use TSK's to cache the OBJ table.
     */
    var data = Program.getPrivateData('ti.bios.rov.TSK');

    /* Check if the OBJ table has already been retrieved. */
    if (data.objTable != undefined) {
        return (data.objTable);
    }
    
    /* Get the address of the OBJ table */
    var addr = Program.getSymbolValue('OBJ_table');
    
    Program.debugPrint("OBJ -- Retrieving the OBJ_table at 0x" +
                       Number(addr).toString(16));
    
    /* Fetch the OBJ table. Returns an array of un-scanned QUEs. */
    data.objTable = Program.fetchArray(QUE.QUE_Elem$fetchDesc, addr, 7);
    
    return (data.objTable);
}

/*
 *  ======== getTasks ========
 *  Helper function to retrieve all of the TSK instances.
 */
function getTasks()
{
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    var ROV = xdc.useModule('ti.bios.rov.ROV');
    
    if (ROV.getISA() == '55') {
        return (getObjects(OBJ.TSK, OBJ.OBJ_TSK_55x$fetchDesc));
    }
    else {
        return (getObjects(OBJ.TSK, OBJ.OBJ_TSK$fetchDesc));
    }
}

/*
 *  ======== getSems ========
 *  Helper function to retrieve all of the SEM instances.
 */
function getSems()
{
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    
    return (getObjects(OBJ.SEM, OBJ.OBJ_SEM$fetchDesc));
}

/*
 *  ======== getMailboxes ========
 *  Helper function to retrieve all of the MBX instances.
 */
function getMailboxes()
{
    var OBJ = xdc.useModule('ti.bios.rov.OBJ');
    
    return (getObjects(OBJ.MBX, OBJ.OBJ_MBX$fetchDesc));
}

