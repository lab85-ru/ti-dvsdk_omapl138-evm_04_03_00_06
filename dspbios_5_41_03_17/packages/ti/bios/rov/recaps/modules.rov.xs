/* ======== ti.bios.rov ======== */
var pkg = {};
pkg.$name = 'ti.bios.rov';    

/* ======== Modules ======== */
var KNL = {};
KNL['$package'] = pkg;
KNL['$name'] = 'ti.bios.rov.KNL';
KNL['common$'] = {};
KNL['common$']['namedInstance'] = false;
KNL['rovViewInfo'] = {};
KNL['rovViewInfo']['showRawTab'] = false;
KNL['rovViewInfo']['viewMap'] = {};
KNL['rovViewInfo']['viewMap']['$keys'] = [];
KNL['rovViewInfo']['viewMap']['$keys'].push('System');
KNL['rovViewInfo']['viewMap']['System'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitSystem', 
                                              structName: 'SystemView'};

var TSK = {};
TSK['$package'] = pkg;
TSK['$name'] = 'ti.bios.rov.TSK';
TSK['common$'] = {};
TSK['common$']['namedInstance'] = false;
TSK['rovViewInfo'] = {};
TSK['rovViewInfo']['showRawTab'] = false;
TSK['rovViewInfo']['viewMap'] = {};
TSK['rovViewInfo']['viewMap']['$keys'] = [];
TSK['rovViewInfo']['viewMap']['$keys'].push('Instances');
TSK['rovViewInfo']['viewMap']['Instances'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitInstances', 
                                              structName: 'InstanceData'};

var SWI = {};
SWI['$package'] = pkg;
SWI['$name'] = 'ti.bios.rov.SWI';
SWI['common$'] = {};
SWI['common$']['namedInstance'] = false;
SWI['rovViewInfo'] = {};
SWI['rovViewInfo']['showRawTab'] = false;
SWI['rovViewInfo']['viewMap'] = {};
SWI['rovViewInfo']['viewMap']['$keys'] = [];
SWI['rovViewInfo']['viewMap']['$keys'].push('Instances');
SWI['rovViewInfo']['viewMap']['Instances'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitInstances', 
                                              structName: 'InstanceData'};

var SEM = {};
SEM['$package'] = pkg;
SEM['$name'] = 'ti.bios.rov.SEM';
SEM['common$'] = {};
SEM['common$']['namedInstance'] = false;
SEM['rovViewInfo'] = {};
SEM['rovViewInfo']['showRawTab'] = false;
SEM['rovViewInfo']['viewMap'] = {};
SEM['rovViewInfo']['viewMap']['$keys'] = [];
SEM['rovViewInfo']['viewMap']['$keys'].push('Instances');
SEM['rovViewInfo']['viewMap']['Instances'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitInstances', 
                                              structName: 'InstanceData'};

var MBX = {};
MBX['$package'] = pkg;
MBX['$name'] = 'ti.bios.rov.MBX';
MBX['common$'] = {};
MBX['common$']['namedInstance'] = false;
MBX['rovViewInfo'] = {};
MBX['rovViewInfo']['showRawTab'] = false;
MBX['rovViewInfo']['viewMap'] = {};
MBX['rovViewInfo']['viewMap']['$keys'] = [];
MBX['rovViewInfo']['viewMap']['$keys'].push('Instances');
MBX['rovViewInfo']['viewMap']['Instances'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitInstances', 
                                              structName: 'InstanceData'};

var MEM = {};
MEM['$package'] = pkg;
MEM['$name'] = 'ti.bios.rov.MEM';
MEM['common$'] = {};
MEM['common$']['namedInstance'] = false;
MEM['rovViewInfo'] = {};
MEM['rovViewInfo']['showRawTab'] = false;
MEM['rovViewInfo']['viewMap'] = {};
MEM['rovViewInfo']['viewMap']['$keys'] = [];
MEM['rovViewInfo']['viewMap']['$keys'].push('Instances');
MEM['rovViewInfo']['viewMap']['Instances'] = {type: 'xdc.rov.ViewInfo.MODULE_DATA', 
                                              viewInitFxn: 'viewInitInstances', 
                                              structName: 'InstanceData'};
MEM['rovViewInfo']['viewMap']['$keys'].push('FreeLists');
MEM['rovViewInfo']['viewMap']['FreeLists'] = {type: 'xdc.rov.ViewInfo.INSTANCE_DATA', 
                                              viewInitFxn: 'viewInitFreeLists',
                                              structName: 'FreeListElement'};

var LOG = {};
LOG['$package'] = pkg;
LOG['$name'] = 'ti.bios.rov.LOG';
LOG['common$'] = {};
LOG['common$']['namedInstance'] = false;
LOG['rovViewInfo'] = {};
LOG['rovViewInfo']['showRawTab'] = false;
LOG['rovViewInfo']['viewMap'] = {};
LOG['rovViewInfo']['viewMap']['$keys'] = [];
LOG['rovViewInfo']['viewMap']['$keys'].push('Logs');
LOG['rovViewInfo']['viewMap']['Logs'] = {type: 'xdc.rov.ViewInfo.INSTANCE_DATA',
                                         viewInitFxn: 'viewInitLogs',
                                         structName: 'LogRecordView'};

/* Module list */
this.$modules = {};
this.$modules['ti.bios.rov.KNL'] = KNL;
this.$modules['ti.bios.rov.TSK'] = TSK;
this.$modules['ti.bios.rov.SWI'] = SWI;
this.$modules['ti.bios.rov.SEM'] = SEM;
this.$modules['ti.bios.rov.MBX'] = MBX;
this.$modules['ti.bios.rov.MEM'] = MEM;
this.$modules['ti.bios.rov.LOG'] = LOG;
