/*
 * bios_edma3_drv_sample_c6472_int_reg.c
 *
 * Platform specific interrupt registration and un-registration routines.
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <ecm.h>
#include <hwi.h>

#include <ti/sdo/edma3/drv/sample/bios_edma3_drv_sample.h>

/**
 * Variables which will be used internally for referring the hardware interrupt
 * for various EDMA3 interrupts. Specified in the sample configuration file.
 */
extern unsigned int hwIntXferComp[];
extern unsigned int hwIntCcErr;

/*
 *  ======== C64_enableIER ========
 *  enable interrupts specified by mask
 */
extern Void C64_enableIER( Uns mask );

/**
 * DSP instance number on which the executable is running. Its value is
 * determined by reading the processor specific register DNUM.
 */
extern unsigned int dsp_num;

/**
 * Variable array which will be used internally for referring to
 * transfer completion interrupts for different DSPs.
 */
extern unsigned int ccXferCompInt[];
/**
 * Variable which will be used internally for referring to CC error interrupt.
 * It will be used to enable CC error interrupt in ECM. Specified in the sample configuration file.
 */
extern unsigned int ccErrorInt;


/**  To Register the ISRs with the underlying OS, if required. */
void registerEdma3Interrupts (void)
    {
    unsigned int intState;
    ECM_Attrs ecmattrs = ECM_ATTRS;
    /* Disabling the global interrupts */
    intState = HWI_disable();

    /* Enable the Xfer Completion Event Interrupt, according the DSP number */
    ecmattrs.unmask = 1u;
	ecmattrs.arg = (Arg)0u;

    ECM_dispatchPlug (ccXferCompInt[dsp_num], (ECM_Fxn)(&lisrEdma3ComplHandler0),
                        &ecmattrs);
    ECM_enableEvent(ccXferCompInt[dsp_num]);

    ECM_enableEvent(ccErrorInt);

   /**
    * Enabling the HWI_ID.
    * EDMA3 interrupts (transfer completion, CC error etc.)
    * correspond to different ECM events (SoC specific). These ECM events come
    * under ECM block XXX (handling those specific ECM events). Normally, block
    * 0 handles events 4-31 (events 0-3 are reserved), block 1 handles events
    * 32-63 and so on. This ECM block XXX (or interrupt selection number XXX)
    * is mapped to a specific HWI_INT YYY in the tcf file. So to enable this
    * mapped HWI_INT YYY, one should use the corresponding bitmask in the
    * API C64_enableIER(), in which the YYY bit is SET.
    */
    /* To enable EDMA3 transfer completion interrupt */
	C64_enableIER(1u << hwIntXferComp[dsp_num]);
	/* To enable EDMA3 CC Error interrupt */
	C64_enableIER(1u << hwIntCcErr);

    /* Restore interrupts */
    HWI_restore(intState);
    }


/**  To Unregister the ISRs with the underlying OS, if previously registered. */
void unregisterEdma3Interrupts (void)
    {
    unsigned int intState;

    /* Disabling the global interrupts */
    intState = HWI_disable();

    /* Disable the Xfer Completion Event Interrupt */
    ECM_disableEvent(ccXferCompInt[dsp_num]);
    /* Disable the CC Error Event Interrupt */
    ECM_disableEvent(ccErrorInt);
    /* Restore interrupts */
    HWI_restore(intState);
    }
/* End of File */

