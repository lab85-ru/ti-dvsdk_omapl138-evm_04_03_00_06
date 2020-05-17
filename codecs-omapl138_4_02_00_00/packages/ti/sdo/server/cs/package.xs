/*
 *  ======== package.xs ========
 */

function validate() {

    // Each codec is validated with the following call:
    //    validate_one_codec(<package name>, <module name>);

         validate_one_codec( "ti.c6accel", "C6ACCEL" );
         validate_one_codec( "ti.sdo.codecs.aachedec", "AACHEDEC" );
         validate_one_codec( "ti.sdo.codecs.g711dec", "G711DEC" );
         validate_one_codec( "ti.sdo.codecs.g711enc", "G711ENC" );
         validate_one_codec( "ti.sdo.codecs.h264dec", "H264DEC" );
         validate_one_codec( "ti.sdo.codecs.h264enc", "H264ENC" );
         validate_one_codec( "ti.sdo.codecs.jpegdec", "JPEGDEC" );
         validate_one_codec( "ti.sdo.codecs.jpegenc", "JPEGENC" );
         validate_one_codec( "ti.sdo.codecs.mpeg2dec", "MPEG2DEC" );
         validate_one_codec( "ti.sdo.codecs.mpeg4dec", "MPEG4DEC" );
         validate_one_codec( "ti.sdo.codecs.mpeg4enc", "MPEG4ENC" );
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
