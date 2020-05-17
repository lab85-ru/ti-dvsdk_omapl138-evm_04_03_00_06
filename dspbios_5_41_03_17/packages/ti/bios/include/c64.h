/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== c64.h ========
 *  'C64xx Peripheral Register Definitions
 *
 */

#ifndef C64_
#define C64_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Interrupt vector id
 */
#define C64_NMI     1
#define C64_INT4    4
#define C64_INT5    5
#define C64_INT6    6
#define C64_INT7    7
#define C64_INT8    8
#define C64_INT9    9
#define C64_INT10   10
#define C64_INT11   11
#define C64_INT12   12
#define C64_INT13   13
#define C64_INT14   14
#define C64_INT15   15

/*
 *  Interrupt vector mask bits
 */
#define C64_EINT4   (1 << C64_INT4)
#define C64_EINT5   (1 << C64_INT5)
#define C64_EINT6   (1 << C64_INT6)
#define C64_EINT7   (1 << C64_INT7)
#define C64_EINT8   (1 << C64_INT8)
#define C64_EINT9   (1 << C64_INT9)
#define C64_EINT10  (1 << C64_INT10)
#define C64_EINT11  (1 << C64_INT11)
#define C64_EINT12  (1 << C64_INT12)
#define C64_EINT13  (1 << C64_INT13)
#define C64_EINT14  (1 << C64_INT14)
#define C64_EINT15  (1 << C64_INT15)

/*
 *  Timer Controller Registers 
 */
typedef volatile struct {
    Uns       control;        /* Global Control Register              */
    Uns       period;         /* Timer Period                         */
    Uns       counter;        /* Timer Counter                        */
} C64_TRegs;

/*
 * Timer Global Control Register Bits
 */
#define C64_FUNC        0x001   /* Configure TCLK as a timer            */
#define C64_INVOUT      0x002   /* TOUT inverter control.               */
#define C64_DATOUT      0x004   /* TCLK output value                    */
#define C64_DATIN       0x008   /* TCLK input value                     */
#define C64_GO          0x040   /* Reset and start timer                */
#define C64_HLDNOT      0x080   /* Don't hold the timer                 */
#define C64_CP          0x100   /* Clock/Pulse mode control             */
#define C64_CLKSRC      0x200   /* Use internal clock                   */
#define C64_INVINP      0x400   /* TINP inverter control bit            */
#define C64_TSTAT       0x800   /* Timer status                         */
 
/*
 * Peripheral Register Addresses
 */
#define C64_TREGS0      ((C64_TRegs *)0x01940000) /* Timer 0     */
#define C64_TREGS1      ((C64_TRegs *)0x01980000) /* Timer 1     */

/*
 * Preserved for backwards compatibility
 */
#define C64_disable(mask) C64_disableIER(mask)
#define C64_enable(mask) C64_enableIER(mask)
#define C64_disableGIE() HWI_disableJ()
#define C64_enableGIE(mask) HWI_restoreJ(mask)

/*
 *  ======== C64_disableIER ========
 *  disable interrupts specified in mask and return old mask
 */
extern Uns  C64_disableIER( Uns mask );

/*
 *  ======== C64_enableIER ========
 *  enable interrupts specified by mask
 */
extern Void C64_enableIER( Uns mask );

/*
 *  ======== C64_clearIFR ========
 *  clear pending interrupts specified by mask
 */
extern Void C64_clearIFR( Uns mask );

/*
 *  ======== C64_mask ========
 */
#define C64_mask(devid) ((Uns)(1 << devid-1))

/*
 *  ======== C64_plug ========
 */
extern Void C64_plug( Int devid, Fxn isr, Int dmachan );

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* C64_ */
