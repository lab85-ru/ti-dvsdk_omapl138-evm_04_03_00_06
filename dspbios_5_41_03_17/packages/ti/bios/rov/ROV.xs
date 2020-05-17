/*
 *  ======== fetchGlobal ========
 *  This helper function performs the common BIOS 5 ROV task of reading a
 *  global variable which is a scalar.
 *  
 *  For example:
 *  var numSegments = ROV.fetchGlobal(ScalarStructs.S_Int$fetchDesc,
 *                                              "MEM_D_NUMSEGMENTS");
 */
function fetchGlobal(fetchDesc, symName)
{
    var Program = xdc.useModule('xdc.rov.Program');
    
    /* Retrieve the variable's address */
    var addr = Program.getSymbolValue(symName);
    
    /* Retrieve the variable */
    var scalarStruct = Program.fetchStruct(fetchDesc, addr);
    
    return (scalarStruct.elem);
}

/*
 *  ======== getISA ========
 *  Retrieve the current target's ISA.
 *  Returns '64', '28', etc.
 */
function getISA()
{
    var Model = xdc.useModule('xdc.rov.Model');
    return (Model.$private.recap.build.target.isa);
}

/*
 *  ======== getNickName ========
 *  Formats the name and address into "tsk0 (0x13DC8)"
 */
function getNickName(name, addr)
{
    /* 
     * Convert address to unsigned.
     * The arguments are signed IArgs, so the address may be negative and
     * need to be converted.
     */
    if (addr < 0) {
        addr += Math.pow(2, 32);    
    }
    
    var handle = "(0x" + Number(addr).toString(16) + ")"; 
    if (name == "") {
        return (handle);
    }
    else {
        return (name + " " + handle);
    }
}

/*
 *  ======== getName ========
 *  Returns the string name of the instance (not the nick name).
 */
function getName(modName, addr, hasDynamic, fetchDesc)
{
    /* 
     * Convert address to unsigned.
     * The arguments are signed IArgs, so the address may be negative and
     * need to be converted.
     */
    if (addr < 0) {
        addr += Math.pow(2, 32);    
    }
    
    var Program = xdc.useModule('xdc.rov.Program');
    Program.debugPrint("ROV -- Retrieving name for " + modName + 
                       " instance at 0x" + Number(addr).toString(16));
    
    var data = Program.getPrivateData(modName);
    
    /* Check the module's addrToName map first. */
    if (data.addrToName != undefined) {
        if (data.addrToName[Number(addr)] != undefined) {
            return (data.addrToName[Number(addr)]);
        }
    }
    else {
        data.addrToName = {};
    }
    
    /* Check for a static name */
    var staticName = getStaticName(addr);
    if (!staticName.equals("")) {
        data.addrToName[Number(addr)] = staticName;
        return (staticName);
    }
    
    /* If there's no dynamic name field, we're done. */
    if (!hasDynamic) {
        var name = "0x" + Number(addr).toString(16);
        data.addrToName[Number(addr)] = name;
        return (name);
    }
    
    /* If it has a dynamic name field, check there. */
    var obj = Program.fetchStruct(fetchDesc, addr);
    
    /* If the name field is null, return the address */
    if (obj.name == 0) {
        var name = "0x" + Number(addr).toString(16);
        data.addrToName[Number(addr)] = name;
        return (name);
    }

    /* Fetch the dynamic name string. */
    Program.debugPrint("ROV -- Retrieving dynamic name at 0x" + 
                       Number(obj.name).toString(16));
    var dynamicName = Program.fetchString(obj.name);
    
    /* If the name field is an empty string, return the address. */
    if (dynamicName == "") {
        dynamicName = "0x" + Number(addr).toString(16);
    }
    
    data.addrToName[Number(addr)] = dynamicName;
    return(dynamicName);
}

/*
 *  ======== getStaticName ========
 *  Helper API.
 *  Retrieves the static instance name by looking up the symbols at the given
 *  address.
 *  There may be multiple symbols at the address. To improve the chances of
 *  returning the right symbol:
 *    - Symbols containing TABBEG or TABEND are filtered out
 *    - The shortest symbol is returned
 */
function getStaticName(addr)
{
    var Program = xdc.useModule('xdc.rov.Program');

    /* 
     * Convert address to unsigned.
     * The arguments are signed IArgs, so the address may be negative and
     * need to be converted.
     */
    if (addr < 0) {
        addr += Math.pow(2, 32);    
    }
    
    var jSyms = Program.lookupDataSymbol(Number(addr));
    
    /* Return empty string if no symbols found. */
    if ((jSyms == null) || (jSyms.length == 0)) {
        return ("");
    }
    
    /* Copy the Java String array to a JavaScript array. */
    var symsArr = new Array();
    for each (var sym in jSyms) {
        symsArr[symsArr.length] = sym;
    }

    /* 
     * Sort the array by symbol length, from smallest to largest. In general, 
     * user given names are shorter than generated symbols.
     */
    function compare(a, b) {
        return (a.length - b.length);
    }
    symsArr.sort(compare);
   
    /* 
     * Filter the returned symbols for any containing TABBEG or TABEND.
     * Pick the first symbol which does not contain TABBEG or TABEND.
     */
    var name = "";
    for (var i = 0; i < symsArr.length; i++) {
        if ((symsArr[i].indexOf("_A_TABBEG") == -1) &&
            (symsArr[i].indexOf("_A_TABEND") == -1)) {
        
            name = symsArr[i];
            break;
        }   
    }
    
    /* Strip $obj from the end of the symbol, if it's there. */
    if (name.indexOf('$obj') != -1) {
        name = name.substring(0, name.length - 4);
    }
    
    return (name);
}