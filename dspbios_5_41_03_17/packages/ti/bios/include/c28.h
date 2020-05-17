/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== c28.h ========
 *  C28 header file
 *
 */

#ifndef C28_
#define C28_

#ifdef __cplusplus
extern "C" {
#endif

/*=====================C28_plug====================
 *
 */ 
extern Void C28_plug(Int, Fxn); /* plug interrupt vector */

/*
 *====================C28_disableIER==============
 */
extern Uns C28_disableIER( Uns mask );
#define C28_disableIER(mask)  C28_disableIER(mask) 
/*
 *==================C28_enableIER==============
 */
extern Void C28_enableIER( Uns mask );
#define C28_enableIER(mask)  C28_enableIER(mask) 


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* C28_ */
