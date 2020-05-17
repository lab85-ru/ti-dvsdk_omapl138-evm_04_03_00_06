/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== clk.h ========
 *
 */

#ifndef CLK_
#define CLK_

#ifdef __cplusplus
extern "C" {
#endif

extern LgUns CLK_countspms(Void);
extern LgUns CLK_gethtime(Void);
extern LgUns CLK_getltime(Void);
extern LgUns CLK_getprd(Void);
extern LgUns CLK_itime(Void);
extern Bool CLK_reconfig(Void);
extern Float CLK_cpuCyclesPerHtime(Void);
extern Float CLK_cpuCyclesPerLtime(Void);

#if defined(_55_) || defined(_6x_) || defined(_28_)
extern Void CLK_stop(Void);
extern Void CLK_start(Void);
#endif

/* Only available on C5505 platforms */
extern Void CLK_setTimerFunc(Uns id, Void (*func)(Arg), Arg arg);

#ifdef _BIOSROM_
extern LgUns (*CLK_countspmsPtr)(Void);
extern LgUns (*CLK_gethtimePtr)(Void);
extern LgUns (*CLK_getltimePtr)(Void);
extern LgUns (*CLK_getprdPtr)(Void);
extern LgUns (*CLK_itimePtr)(Void);
extern Bool (*CLK_reconfigPtr)(Void);

#define CLK_countspmsJ()        (CLK_countspmsPtr)()
#define CLK_gethtimeJ()         (CLK_gethtimePtr)()
#define CLK_getltimeJ()         (CLK_getltimePtr)()
#define CLK_getprdJ()           (CLK_getprdPtr)()
#define CLK_itimeJ()            (CLK_itimePtr)()
#define CLK_reconfigJ()         (CLK_reconfigPtr)()

#else

#define CLK_countspmsJ()        CLK_countspms()
#define CLK_gethtimeJ()         CLK_gethtime()
#define CLK_getltimeJ()         CLK_getltime()
#define CLK_getprdJ()           CLK_getprd()
#define CLK_itimeJ()            CLK_itime()
#define CLK_reconfigJ()         CLK_reconfig()

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* CLK_ */
