// The DSP is processor id 0
var multiProcDSPDevice = "DSP";
var multiProcDeviceArray  = [multiProcDSPDevice, "VIDEO-M3", "VPSS-M3", "HOST"];


/* Set the BIOS timer frequency  to 20MHz so as to match with the default timer
 * frequency on Linux
 */
var BIOS         = xdc.useModule('ti.sysbios.BIOS');
BIOS.cpuFreq.hi  = 0;
BIOS.cpuFreq.lo  = 500000000; 
 
var Timer        = xdc.useModule('ti.sysbios.timers.dmtimer.Timer');
Timer.intFreq.hi = 0;
Timer.intFreq.lo = 20000000;

var Clock        = xdc.useModule('ti.sysbios.knl.Clock');
Clock.timerId    = 0; /* Maps to GPTimer3 in the system */


/* Set DSP cache settings */
var Cache = xdc.useModule('ti.sysbios.family.c64p.Cache');
Cache.MAR128_159 = 0xffffffff;
Cache.MAR160_191 = 0xffffffff;
Cache.MAR192_223 = 0xffffffff;
Cache.MAR224_255 = 0xffffffff;
