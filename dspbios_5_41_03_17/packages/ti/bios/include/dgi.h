/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 * ======== dgi.h ========
 * compatibility file for .cfg files that use DGI.  DGN now works for
 * both integer and floating point DSPs.
 *
 */

#include <dgn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DGI     DGN

#define DGI_Params DGN_IParams

#define DGI_CONST       DGN_CONST
#define DGI_RAND        DGN_RAND
#define DGI_SINE        DGN_SINE
#define DGI_USER        DGN_USER

#ifdef __cplusplus
}
#endif /* extern "C" */

