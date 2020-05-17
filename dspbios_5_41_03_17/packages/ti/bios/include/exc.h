/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== exc.h ========
 *
 */

#ifndef EXC_
#define EXC_

/* EFR (Exception Flag Register) bits */
#define EXC_EFRSXF      0x00000001
#define EXC_EFRIXF      0x00000002
#define EXC_EFREXF      0x40000000
#define EXC_EFRNXF      0x80000000
/* ECR (Exception Clear Register) bits */
#define EXC_ECRSXF      EXC_EFRSXF
#define EXC_ECRIXF      EXC_EFRIXF
#define EXC_ECREXF      EXC_EFREXF
#define EXC_ECRNXF      EXC_EFRNXF

/* TSR exception enable bits */
#define EXC_TSRGEE      0x00000004
#define EXC_TSRXEN      0x00000008
/* TSR Privilege Mode bits */
#define EXC_TSRCXMMASK  0x000000C0
#define EXC_TSRCXMSHIFT 6
#define EXC_TSRCXMSV    0x00000000
#define EXC_TSRCXMUS    0x00000040

/* IERR (Internal Exception Report Register) bits */
#define EXC_IERRIFX     0x00000001
#define EXC_IERRFPX     0x00000002
#define EXC_IERREPX     0x00000004
#define EXC_IERROPX     0x00000008
#define EXC_IERRRCX     0x00000010
#define EXC_IERRRAX     0x00000020
#define EXC_IERRPRX     0x00000040
#define EXC_IERRLBX     0x00000080
#define EXC_IERRMS      0x00000100

/* MPC CPU Access Memory Protecton Fault Event numbers */
#define EXC_EVTPMCCMPA  120             /* PMC CPU Memory Protection fault */
#define EXC_EVTDMCCMPA  122             /* DMC CPU Memory Protection fault */
#define EXC_EVTUMCCMPA  124             /* UMC CPU Memory Protection fault */
#define EXC_EVTEMCCMPA  126             /* EMC CPU Memory Protection fault */

#define EXC_evtToRegNum(event) ((event) >> 5)
#define EXC_evtToBitNum(event) ((event) & 0x1f)

#define EXC_EVTEVTCLR0          0x01800040
#define EXC_EVTEXPMASK0         0x018000c0
#define EXC_EVTMEXPFLAG0        0x018000e0

#define EXC_evtExpMaskAddr(event) \
    (((volatile Uns *)EXC_EVTEXPMASK0) + EXC_evtToRegNum(event))

#define EXC_evtEvtClrAddr(event) \
    (((volatile Uns *)EXC_EVTEVTCLR0) + EXC_evtToRegNum(event))

#define EXC_evtExpEnable(event) \
    *EXC_evtExpMaskAddr(event) = \
      *EXC_evtExpMaskAddr(event) & ~(1 << EXC_evtToBitNum(event))

#define EXC_evtEvtClear(event) \
    *EXC_evtEvtClrAddr(event) = 1 << EXC_evtToBitNum(event)

#define EXC_SYSCALLPRIV 0
#define EXC_SYSCALLHOOK 1


typedef struct EXC_Status {
    Uint32 efr;
    Uint32 nrp;
    Uint32 ntsr;
    Uint32 ierr;
} EXC_Status;

extern Void (*EXC_userHook)(Void);
extern Void EXC_init(Void);
extern EXC_Status EXC_getLastStatus(Void);
extern Void EXC_clearLastStatus(Void);
extern Ptr EXC_setReturnPtr(Ptr returnPtr);

extern Void (*EXC_exceptionHook)(Void);
extern Void (*EXC_internalHook)(Void);
extern Void (*EXC_externalHook)(Void);
extern Void (*EXC_nmiHook)(Void);


#endif /* !EXC_ */

