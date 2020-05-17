// The DSP is processor id 0
var multiProcDSPDevice = "DSP";
var multiProcDeviceArray  = [multiProcDSPDevice, "HOST"];

var Cache = xdc.useModule('ti.sysbios.family.c64p.Cache');
Cache.MAR128_159 = 0xffffffff;
Cache.MAR160_191 = 0xffffffff;
Cache.MAR192_223 = 0xffffffff;
Cache.MAR224_255 = 0xffffffff;
