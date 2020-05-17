/*
 *  ======== package.xs ========
 */

function validate() {

    // Each codec is validated with the following call:
    //    validate_one_codec(<package name>, <module name>);

         validate_one_codec( "ti.c6accel", "C6ACCEL" );
}

function validate_one_codec( packageName, moduleName ) {
    var baseMod = xdc.module( packageName + '.' + moduleName);
    
    var spec = baseMod.$spec;

    var configNames = [];
    
    var declList = spec.getDecls();
    var decls = {};
    var declsByKind = {};
        
    for (var i = 0; i < declList.size(); i++) {
        var decl = declList.get(i);
        var name = decl.getName()+'';
        var kind = decl.getXmlTag()+'';
        if (kind=="config") { configNames.push( name ); }
    }
    
    var msg = "Package " + packageName + " has config params: " 
        + configNames + ". At least one of these params is undefined in " 
        + "codec.cfg. Read ti/sdo/ce/wizards/genserver/readme.txt for more " 
        + "information on how to edit codec.cfg."; 
}
