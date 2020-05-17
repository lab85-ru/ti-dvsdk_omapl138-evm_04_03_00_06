/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== c62.h ========
 *  'C62xx Peripheral Register Definitions
 *
 */

#ifndef C62_
#define C62_

#include <hwi.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Interrupt vector id
 */
#define C62_NMI     1
#define C62_INT4    4
#define C62_INT5    5
#define C62_INT6    6
#define C62_INT7    7
#define C62_INT8    8
#define C62_INT9    9
#define C62_INT10   10
#define C62_INT11   11
#define C62_INT12   12
#define C62_INT13   13
#define C62_INT14   14
#define C62_INT15   15

/*
 *  Interrupt vector mask bits
 */
#define C62_EINT4   (1 << C62_INT4)
#define C62_EINT5   (1 << C62_INT5)
#define C62_EINT6   (1 << C62_INT6)
#define C62_EINT7   (1 << C62_INT7)
#define C62_EINT8   (1 << C62_INT8)
#define C62_EINT9   (1 << C62_INT9)
#define C62_EINT10  (1 << C62_INT10)
#define C62_EINT11  (1 << C62_INT11)
#define C62_EINT12  (1 << C62_INT12)
#define C62_EINT13  (1 << C62_INT13)
#define C62_EINT14  (1 << C62_INT14)
#define C62_EINT15  (1 << C62_INT15)

/*
 *  Timer Controller Registers 
 */
typedef volatile struct {
    Uns       control;        /* Global Control Register              */
    Uns       period;         /* Timer Period                         */
    Uns       counter;        /* Timer Counter                        */
} C62_TRegs;

/*
 * Timer Global Control Register Bits
 */
#define C62_FUNC        0x001   /* Configure TCLK as a timer            */
#define C62_INVOUT      0x002   /* TOUT inverter control.               */
#define C62_DATOUT      0x004   /* TCLK output value                    */
#define C62_DATIN       0x008   /* TCLK input value                     */
#define C62_GO          0x040   /* Reset and start timer                */
#define C62_HLDNOT      0x080   /* Don't hold the timer                 */
#define C62_CP          0x100   /* Clock/Pulse mode control             */
#define C62_CLKSRC      0x200   /* Use internal clock                   */
#define C62_INVINP      0x400   /* TINP inverter control bit            */
#define C62_TSTAT       0x800   /* Timer status                         */
 
/*
 * Peripheral Register Addresses
 */
#define C62_TREGS0      ((C62_TRegs *)0x01940000) /* Timer 0     */
#define C62_TREGS1      ((C62_TRegs *)0x01980000) /* Timer 1     */

/*
 * Preserved for backwards compatibility
 */
#define C62_disable(mask) C62_disableIER(mask)
#define C62_enable(mask) C62_enableIER(mask)
#define C62_disableGIE() HWI_disableJ()
#define C62_enableGIE(mask) HWI_restoreJ(mask)

/*
 *  ======== C62_disableIER ========
 *  disable interrupts specified in mask and return old mask
 */
extern Uns  C62_disableIER( Uns mask );

/*
 *  ======== C62_enableIER ========
 *  enable interrupts specified by mask
 */
extern Void C62_enableIER( Uns mask );

/*
 *  ======== C62_clearIFR ========
 *  clear pending interrupts specified by mask
 */
extern Void C62_clearIFR( Uns mask );

/*
 *  ======== C62_mask ========
 */
#define C62_mask(devid) ((Uns)(1 << devid-1))

/*
 *  ======== C62_plug ========
 */
extern Void C62_plug( Int devid, Fxn isr, Int dmachan );

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* C62_ */
