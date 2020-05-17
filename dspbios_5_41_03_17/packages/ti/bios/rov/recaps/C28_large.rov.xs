/* 
 * Create recap.build.target
 *
 */    
this.build = {};
this.build.target = {};
this.build.target['$name'] = unescape('ti.targets.C28_large');
this.build.target['bitsPerChar'] = 16;
this.build.target['isa'] = unescape('28');
this.build.target['model'] = {};
this.build.target['model']['endian'] = 'little';
this.build.target['name'] = unescape('C28_large');
this.build.target['binaryParser'] = "ti.targets.omf.cof.Coff";
this.build.target['stdTypes'] = {};
this.build.target['stdTypes']['t_Char'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_Double'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Float'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Fxn'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_IArg'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Int'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_Int16'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_Int32'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Int40'] = {};
this.build.target['stdTypes']['t_Int64'] = {align: 2, size: 4};
this.build.target['stdTypes']['t_Int8'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_LDouble'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_LLong'] = {align: 2, size: 4};
this.build.target['stdTypes']['t_Long'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Ptr'] = {align: 2, size: 2};
this.build.target['stdTypes']['t_Short'] = {align: 1, size: 1};
this.build.target['stdTypes']['t_Size'] = {align: 2, size: 2};

/* Load the modules from a separate file */
var mods = xdc.loadCapsule('modules.rov.xs');
this.$modules = mods.$modules;
