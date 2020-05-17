/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== mpc.h ========
 *
 */

#ifndef _MPC_H_
#define _MPC_H_

#include <exc.h>

/*
 * MPPA Permission Attribute Register bitmasks, as defined by
 * PA register spec
 */
#define MPC_MPPA_UX     0x00000001      /* User eXecute */
#define MPC_MPPA_UW     0x00000002      /* User Write */
#define MPC_MPPA_UR     0x00000004      /* User Read */
#define MPC_MPPA_SX     0x00000008      /* Supervisor eXecute */
#define MPC_MPPA_SW     0x00000010      /* Supervisor Write */
#define MPC_MPPA_SR     0x00000020      /* Supervisor Read */
#define MPC_MPPA_LOCAL  0x00000100      /* LOCAL CPU access */

/*
 * Type for holding PAs
 */
typedef Uint32 MPC_Perm;

#define MPC_US  ((EXC_TSRCXMUS & EXC_TSRCXMMASK) >> EXC_TSRCXMSHIFT)
#define MPC_SV  ((EXC_TSRCXMSV & EXC_TSRCXMMASK) >> EXC_TSRCXMSHIFT)

Int MPC_setPA(Ptr addr, Int space, MPC_Perm perm);
Int MPC_getPA(Ptr addr, Int space, MPC_Perm *perm);
Int MPC_getPageSize(Ptr addr, Int space, Uns *pageSize);
Int MPC_setBufferPA(Ptr baseAddr, Uns size, Int space, MPC_Perm perm);
Void MPC_setPrivMode(Uns mode);
Uns MPC_getPrivMode(Void);

#endif /* !_MPC_H_ */

