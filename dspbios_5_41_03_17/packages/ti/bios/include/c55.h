/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== c55.h ========
 *  C55 header file
 *
 */

#ifndef C55_
#define C55_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Interrupt Vector Id
 */
#define C55_RESET       0       /* Hardware RESET */
#define C55_NMI         1       /* NMI */
#define C55_INT2        2       /* S/w or H/w interrupts */
#define C55_INT3        3
#define C55_INT4        4
#define C55_INT5        5
#define C55_INT6        6
#define C55_INT7        7
#define C55_INT8        8
#define C55_INT9        9
#define C55_INT10       10
#define C55_INT11       11
#define C55_INT12       12
#define C55_INT13       13
#define C55_INT14       14
#define C55_INT15       15
#define C55_INT16       16
#define C55_INT17       17
#define C55_INT18       18
#define C55_INT19       19
#define C55_INT20       20
#define C55_INT21       21
#define C55_INT22       22
#define C55_INT23       23
#define C55_BERR        24      /* BERR Bus Error interrupt */
#define C55_DLOG        25      /* DLOG Emulation interrupt */
#define C55_RTOS        26      /* RTOS Emulation interrupt */
#define C55_INT27       27      /* Software interrupts */
#define C55_INT28       28
#define C55_INT29       29
#define C55_INT30       30
#define C55_TINT        31


/* interrupt mask regist31ers, interrupt flag registers */
#define C55_IMR0        (Uns *)0x0
#define C55_IFR0        (Uns *)0x1
#define C55_IMR1        (Uns *)0x45
#define C55_IFR1        (Uns *)0x46

/* IMR0 Register masks */
#define C55_IEN02       (1 << C55_INT2)
#define C55_IEN03       (1 << C55_INT3)
#define C55_IEN04       (1 << C55_INT4)
#define C55_IEN05       (1 << C55_INT5)
#define C55_IEN06       (1 << C55_INT6)
#define C55_IEN07       (1 << C55_INT7)
#define C55_IEN08       (1 << C55_INT8)
#define C55_IEN09       (1 << C55_INT9)
#define C55_IEN010      (1 << C55_INT10)
#define C55_IEN011      (1 << C55_INT11)
#define C55_IEN012      (1 << C55_INT12)
#define C55_IEN013      (1 << C55_INT13)
#define C55_IEN014      (1 << C55_INT14)
#define C55_IEN015      (1 << C55_INT15)

/* IMR1 Register masks */
#define C55_IEN016      (1 << (C55_INT16 - 16))
#define C55_IEN017      (1 << (C55_INT17 - 16))
#define C55_IEN018      (1 << (C55_INT18 - 16))
#define C55_IEN019      (1 << (C55_INT19 - 16))
#define C55_IEN020      (1 << (C55_INT20 - 16))
#define C55_IEN021      (1 << (C55_INT21 - 16))
#define C55_IEN022      (1 << (C55_INT22 - 16))
#define C55_IEN023      (1 << (C55_INT23 - 16))
#define C55_IEBERR      (1 << (C55_BERR - 16))
#define C55_IEDLOG      (1 << (C55_DLOG - 16))
#define C55_IERTOS      (1 << (C55_RTOS - 16))

/* IFR0 Register masks */
#define C55_IFG02       (1 << C55_INT2)
#define C55_IFG03       (1 << C55_INT3)
#define C55_IFG04       (1 << C55_INT4)
#define C55_IFG05       (1 << C55_INT5)
#define C55_IFG06       (1 << C55_INT6)
#define C55_IFG07       (1 << C55_INT7)
#define C55_IFG08       (1 << C55_INT8)
#define C55_IFG09       (1 << C55_INT9)
#define C55_IFG010      (1 << C55_INT10)
#define C55_IFG011      (1 << C55_INT11)
#define C55_IFG012      (1 << C55_INT12)
#define C55_IFG013      (1 << C55_INT13)
#define C55_IFG014      (1 << C55_INT14)
#define C55_IFG015      (1 << C55_INT15)

/* IFR1 Register masks */
#define C55_IFG016      (1 << (C55_INT16 - 16))
#define C55_IFG017      (1 << (C55_INT17 - 16))
#define C55_IFG018      (1 << (C55_INT18 - 16))
#define C55_IFG019      (1 << (C55_INT19 - 16))
#define C55_IFG020      (1 << (C55_INT20 - 16))
#define C55_IFG021      (1 << (C55_INT21 - 16))
#define C55_IFG022      (1 << (C55_INT22 - 16))
#define C55_IFG023      (1 << (C55_INT23 - 16))
#define C55_IBERR       (1 << (C55_BERR - 16))
#define C55_IDLOG       (1 << (C55_DLOG - 16))
#define C55_IRTOS       (1 << (C55_RTOS - 16))

/*
 *  ======== C55_plug ========
 */ 
extern Void C55_plug(Int, Fxn); /* plug interrupt vector */

/*
 *  ======== C55_disableIMR0 ========
 */
extern Uns C55_disableIMR0( Uns mask );
#define C55_disableIER0(mask)  C55_disableIMR0(mask) 

/*
 *  ======== C55_disableIMR1 ========
 */
extern Uns C55_disableIMR1( Uns mask ); 
#define C55_disableIER1(mask)  C55_disableIMR1(mask) 

/*
 *  ======== C55_enableIMR0 ========
 */
extern Void C55_enableIMR0( Uns mask );
#define C55_enableIER0(mask)  C55_enableIMR0(mask) 

/*
 *  ======== C55_enableIMR1 ========
 */
extern Void C55_enableIMR1( Uns mask );
#define C55_enableIER1(mask)  C55_enableIMR1(mask) 

/*
 *  ======== C55_enableInt ========
 *  enable a single interrupt
 */
extern Void C55_enableInt(Uns vecid);

/*
 *  ======== C55_disableInt ========
 *  disable a single interrupt
 */
extern Void C55_disableInt(Uns vecid);

/*
 *  ======== C55_clearIFR0 ========
 *  Clear bits in ifr0.
 */
extern Void C55_clearIFR0( Uns mask );

/*
 *  ======== C55_clearIFR1 ========
 *  Clear bits in ifr1.
 */
extern Void C55_clearIFR1( Uns mask );

/*
 * Below defs & prototypes new with omap2420 L2 Interrupt Manager
 */

/*
 *  OMAP 24xx L2 Interrupt Vector Ids
 *  These are logical IDs
 */

#define C55_L2_INT0     32
#define C55_L2_INT1     33
#define C55_L2_INT2     34
#define C55_L2_INT3     35
#define C55_L2_INT4     36
#define C55_L2_INT5     37
#define C55_L2_INT6     38
#define C55_L2_INT7     39
#define C55_L2_INT8     40
#define C55_L2_INT9     41
#define C55_L2_INT10    42
#define C55_L2_INT11    43
#define C55_L2_INT12    44
#define C55_L2_INT13    45
#define C55_L2_INT14    46
#define C55_L2_INT15    47
#define C55_L2_INT16    48
#define C55_L2_INT17    49
#define C55_L2_INT18    50
#define C55_L2_INT19    51
#define C55_L2_INT20    52
#define C55_L2_INT21    53
#define C55_L2_INT22    54
#define C55_L2_INT23    55
#define C55_L2_INT24    56
#define C55_L2_INT25    57
#define C55_L2_INT26    58
#define C55_L2_INT27    59
#define C55_L2_INT28    60
#define C55_L2_INT29    61
#define C55_L2_INT30    62
#define C55_L2_INT31    63

/*
 *  Additional OMAP 23xx L2 Interrupt Vector Ids
 */

#define C55_L2_INT32    64
#define C55_L2_INT33    65
#define C55_L2_INT34    66
#define C55_L2_INT35    67
#define C55_L2_INT36    68
#define C55_L2_INT37    69
#define C55_L2_INT38    70
#define C55_L2_INT39    71
#define C55_L2_INT40    72
#define C55_L2_INT41    73
#define C55_L2_INT42    74
#define C55_L2_INT43    75
#define C55_L2_INT44    76
#define C55_L2_INT45    77
#define C55_L2_INT46    78
#define C55_L2_INT47    79
#define C55_L2_INT48    80
#define C55_L2_INT49    81
#define C55_L2_INT50    82
#define C55_L2_INT51    83
#define C55_L2_INT52    84
#define C55_L2_INT53    85
#define C55_L2_INT54    86
#define C55_L2_INT55    87
#define C55_L2_INT56    88
#define C55_L2_INT57    89
#define C55_L2_INT58    90
#define C55_L2_INT59    91
#define C55_L2_INT60    92
#define C55_L2_INT61    93
#define C55_L2_INT62    94
#define C55_L2_INT63    95

/*
 *  ======== C55_l2SetIntPriority ========
 *  set an individual interrupt priority (0 - 31)
 */
extern Void C55_l2SetIntPriority(Uns vecid, Uns priority);

/*
 *  ======== C55_l2EnableMIR ========
 *  enable a block of L2 interrupts
 *  (ie mask is written to MIR_CLEAR)
 */
extern Void C55_l2EnableMIR(LgUns mask);

/*
 *  ======== C55_l2DisableMIR ========
 *  disable a block of L2 interrupts
 *  (ie mask is written to MIR_SET)
 */
extern Void C55_l2DisableMIR(LgUns mask);

/*
 *  ======== C55_l2EnableMIR1 ========
 *  enable a block of L2 interrupts
 *  (ie mask is written to MIR_CLEAR1)
 */
extern Void C55_l2EnableMIR1(LgUns mask);

/*
 *  ======== C55_l2DisableMIR1 ========
 *  disable a block of L2 interrupts
 *  (ie mask is written to MIR_SET1)
 */
extern Void C55_l2DisableMIR1(LgUns mask);

/*
 *  ======== C55_l2AckInt ========
 *  Acknowledge an L2 interrupt by latching new FIQ agreement.
 *  Use in non-dispatched C interrupt routines.
 */
extern LgUns C55_INTC_CONTROL;

#define C55_l2AckInt() (C55_INTC_CONTROL=2)

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* C55_ */
