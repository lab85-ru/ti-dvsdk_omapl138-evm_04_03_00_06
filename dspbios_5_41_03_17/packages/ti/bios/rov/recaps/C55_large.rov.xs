/* 
 * Create recap.build.target
 *
 */   
 
this.build = {};
this.build.target = {};
this.build.target['$name'] = unescape('ti.targets.C55_large');
this.build.target['bitsPerChar'] = 16;
this.build.target['isa'] = unescape('55');
this.build.target['model'] = {};
this.build.target['model']['endian'] = 'big';
this.build.target['name'] = unescape('C55_large');
this.build.target['binaryParser'] = "ti.targets.omf.cof.Coff";
this.build.target['stdTypes'] = {};
this.build.target['stdTypes']['t_Char'] =    { size: 1, align: 1 };
this.build.target['stdTypes']['t_Double'] =  { size: 2, align: 2 };
this.build.target['stdTypes']['t_Float'] =   { size: 2, align: 2 };
this.build.target['stdTypes']['t_Fxn'] =     { size: 2, align: 2 };
this.build.target['stdTypes']['t_IArg'] =    { size: 2, align: 2 };
this.build.target['stdTypes']['t_Int'] =     { size: 1, align: 1 };
this.build.target['stdTypes']['t_Int8'] =    { size: 1, align: 1 };
this.build.target['stdTypes']['t_Int16'] =   { size: 1, align: 1 };
this.build.target['stdTypes']['t_Int32'] =   { size: 2, align: 2 };
this.build.target['stdTypes']['t_Int40'] =   { size: 4, align: 2 };
this.build.target['stdTypes']['t_Long'] =    { size: 2, align: 2 };
this.build.target['stdTypes']['t_LDouble'] = { size: 2, align: 2 };
this.build.target['stdTypes']['t_LLong'] =   { size: 4, align: 2 };
this.build.target['stdTypes']['t_Ptr'] =     { size: 2, align: 2 };
this.build.target['stdTypes']['t_Short'] =   { size: 1, align: 1 };
this.build.target['stdTypes']['t_Size'] =    { size: 1, align: 1 };
                                        
/* Load the modules from a separate file */
var mods = xdc.loadCapsule('modules.rov.xs');
this.$modules = mods.$modules;
