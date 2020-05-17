/* 
 * Create recap.build.target
 *
 */    
this.build = {};
this.build.target = {};
this.build.target['$name'] = unescape('ti.targets.C64');
this.build.target['bitsPerChar'] = 8;
this.build.target['isa'] = unescape('64');
this.build.target['model'] = {};
this.build.target['model']['endian'] = 'little';
this.build.target['name'] = unescape('C64');
this.build.target['binaryParser'] = "ti.targets.omf.cof.Coff";
this.build.target['stdTypes'] = {};
this.build.target['stdTypes']['t_Char'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_Double'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_Float'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_Fxn'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_IArg'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_Int'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_Int16'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Int32'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_Int40'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_Int64'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_Int8'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_LDouble'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_LLong'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_Long'] = {align: 8, size: 8};
this.build.target['stdTypes']['t_Ptr'] = {align: 4, size: 4};
this.build.target['stdTypes']['t_Short'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Size'] = {align: 4, size: 4};

/* Load the modules from a separate file */
var mods = xdc.loadCapsule('modules.rov.xs');
this.$modules = mods.$modules;
