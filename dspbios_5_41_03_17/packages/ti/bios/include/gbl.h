/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gbl.h ========
 *
 */

#ifndef GBL_
#define GBL_

#ifdef __cplusplus
extern "C" {
#endif
#include <std.h>


/* external references */
extern Uint16 GBL_procId;
extern Uint32 GBL_clkIn;
extern Uint32 GBL_freq;


/*
 * ======== GBL_getProcId ========
 *
 * Returns the Uint16 static configured processor ID required by MSGQ
 *
 */
static inline Uint16 GBL_getProcId(Void)
{
    return (GBL_procId);
}

/*
 * ======== GBL_setProcId ========
 *
 * Sets the Uint16 static configured processor ID required by MSGQ
 * Must only be called in GBL User Init Function (bios.GBL.USERINITFXN).
 *
 */
static inline Void GBL_setProcId(Uint16 newProcId)
{
    GBL_procId = newProcId;
}

/*
 * ======== GBL_getClkIn ========
 *
 * Returns the static configured value of the input clock to the board.
 * This value needs to be specified in KHz as an Uint32
 */
static inline Uint32 GBL_getClkIn(Void)
{
    return (GBL_clkIn);
}

/*
 * ======== GBL_getFrequency ========
 *
 * Returns the current value of cpu frequency in KHz as an Uint32.
 *
 */
static inline Uint32 GBL_getFrequency(Void)
{
    return (GBL_freq);
}

/*
 * ======== GBL_setFrequency ========
 *
 * Sets the current frequency of CPU frequency.Frequency needs to be specified 
 * as an Uint32 value in Khz. This API can only be called from main.
 * This API does NOT configure the PLL. 
 * It only updates an internal BIOS variable.
 */
static inline Bool GBL_setFrequency(Uint32 frequency)
{
    GBL_freq = frequency;

    return (TRUE);
}

/*
 * ======== GBL_getVersion ========
 *
 * Returns the MSRR version number.
 */
extern Uint16  GBL_getVersion(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* GBL_ */
