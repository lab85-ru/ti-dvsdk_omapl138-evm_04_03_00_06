/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== _mpc.h ========
 *
 */

#ifndef __MPC_H_
#define __MPC_H_

#include <mpc.h>

/*
 *  The following values correspond to chip HW configuration inputs,
 *  as specified in GEM design specs.  They should be the only thing
 *  that needs modifying to port to a new GEM chip design.
 */
/*
 * BADDR - "Megabyte" #, e.g., 0x0000000E -> 0x00E00000
 *
 * Each MPC controls a megabyte range, starting on a megabyte boundary.
 *
 * All current C64+ implementations use these same base addresses.
 */
#define PMC_BADDR       0x0000000E
#define DMC_BADDR       0x0000000F
#define UMC_P0_BADDR    0x00000008
#define UMC_P1_BADDR    0x00000001

/*
 * PMC_R?_SIZE -
 * 0x1000 * 2 ^ n, e.g., 0x00000003 -> 0x1000 * 2 ^ 3 (8) = 0x8000
 *
 * Refer to GEM Program Memory Controller v1.02.1, section 2.2.2 L1P Regions,
 * tables 3 & 4
 */
#if defined(_DM420_) || defined(_F761990_)

#define PMC_R0_SIZE     0x00000003      /* 32KB ROM */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_2430_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6482_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6486_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6488_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_DM6467_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_6747_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_6748_)

#define PMC_R0_SIZE     0x00000000      /* 0KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_DRA442_)

#define PMC_R0_SIZE     0x00000003      /* 32KB */
#define PMC_R1_SIZE     0x00000003      /* 32KB */

#else

#error MPC not supported for this device

#endif

/*
 * PMC_R0_ADJ is not a real chip input, but is here to allow for a hole
 * at the end of the region to cover unimplemented memory
 */
#if defined(_DM420_) || defined(_F761990_)

#define PMC_R0_ADJ      0x00000001      /* 16KB hole at 0xe04000 on DaVinci */
#define PMC_R1_ADJ      0x00000000

#elif defined(_2430_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_TCI6482_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_TCI6486_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_TCI6488_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_DM6467_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_6747_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_6748_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#elif defined(_DRA442_)

#define PMC_R0_ADJ      0x00000000
#define PMC_R1_ADJ      0x00000000

#else

#error MPC not supported for this device

#endif

/*
 * DMC_R?_SIZE -
 * 0x1000 * 2 ^ n, e.g., 0x00000004 -> 0x1000 * 2 ^ 4 (16) = 0x10000
 *
 * Refer to GEM Data Memory Controller v1.02.1, section 2.2.2 L1D Regions,
 * tables 3 & 4
 */
#if defined(_DM420_) || defined(_F761990_)

#define DMC_R0_SIZE     0x00000004      /* 64KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_2430_)

#define DMC_R0_SIZE     0x00000004      /* 64KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6482_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6486_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_TCI6488_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_DM6467_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_6747_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_6748_)

#define DMC_R0_SIZE     0x00000000      /* 0KB */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#elif defined(_DRA442_)

#define DMC_R0_SIZE     0x00000004      /* 64KB but only 48KB implemented */
#define DMC_R1_SIZE     0x00000003      /* 32KB */

#else

#error MPC not supported for this device

#endif

/*
 * DMC_R0_ADJ is not a real chip input, but is here to allow for a hole
 * at the end of the region to cover unimplemented memory
 */
#define DMC_R0_ADJ      0x00000000
#define DMC_R1_ADJ      0x00000000

/* UMC_P?_SIZE -
 * 0x0400 * 2 ^ n, e.g., 0x00000006 -> 0x0400 * 2 ^ 6 (64) = 0x10000
 *
 * Refer to GEM Unified Memory Controller v1.02.0, section 2.2.6 L2 Memory Map,
 * table 4, 5, & 6.
 */
#if defined(_DM420_) || defined(_F761990_)

#define UMC_P0_SIZE     0x00000006      /* 64KB */
#define UMC_P1_SIZE     0x00000007      /* 128KB */
#define UMC_P0_ADJ      0x00000000
#define UMC_P1_ADJ      0x00000000

#elif defined(_2430_)

#define UMC_P0_SIZE     0x00000006      /* 64KB */
#define UMC_P1_SIZE     0x00000007      /* 128KB */
#define UMC_P0_ADJ      0x00000000
#define UMC_P1_ADJ      0x00000000

#elif defined(_TCI6482_)

#define UMC_P0_SIZE     0x0000000B      /* 2048KB (2MB) */
#define UMC_P1_SIZE     0x00000006      /* 16KB ROM (but 64KB is minimum) */
#define UMC_P0_ADJ      0x00000000
#define UMC_P1_ADJ      0x00000000

#elif defined(_TCI6486_)

#define UMC_P0_SIZE     0x0000000A      /* 1024KB (1MB) */
#define UMC_P1_SIZE     0x0000000B      /* 2048KB (2MB) */
#define UMC_P0_ADJ      0x0000001A      /* 16KB * 26 */
#define UMC_P1_ADJ      0x00000000

#elif defined(_TCI6488_)

#define UMC_P0_SIZE     0x0000000A      /* 1024KB (1MB) */
#define UMC_P1_SIZE     0x00000006      /* 0KB but minimum is 64KB */
#define UMC_P0_ADJ      0x00000000
#define UMC_P1_ADJ      0x00000000

#elif defined(_DM6467_)

#define UMC_P0_SIZE     0x00000008      /* 256KB */
#define UMC_P1_SIZE     0x0000000C      /* 4096KB (4MB) */
#define UMC_P0_ADJ      0x00000003      /* 32KB */
#define UMC_P1_ADJ      0x00000000      /* 0KB */

#elif defined(_6747_)

#define UMC_P0_SIZE     0x00000008      /* 256KB */
#define UMC_P1_SIZE     0x0000000A      /* 1024KB (1MB) ROM */
#define UMC_P0_ADJ      0x00000000      /* 0KB */
#define UMC_P1_ADJ      0x00000000      /* 0KB */

#elif defined(_6748_)

#define UMC_P0_SIZE     0x00000008      /* 256KB */
#define UMC_P1_SIZE     0x0000000A      /* 1024KB (1MB) ROM */
#define UMC_P0_ADJ      0x00000000      /* 0KB */
#define UMC_P1_ADJ      0x00000000      /* 0KB */

#elif defined(_DRA442_)

#define UMC_P0_SIZE     0x00000007      /* 128KB because of DRA447 */
#define UMC_P1_SIZE     0x00000008      /* 256KB ROM because of DRA447 */
#define UMC_P0_ADJ      0x00000000      /* 0KB */
#define UMC_P1_ADJ      0x00000000      /* 0KB */

#else

#error MPC not supported for this device

#endif

/*
 * Each MPC needs to implement MPLCK.
 * DMC/PMC/UMA all conform to the struct below.
 */
typedef struct _MPC_Lock {
volatile        Uint32  mpLk0;          /* lock bits 31-0 */
volatile        Uint32  mpLk1;          /* lock bits 63-32 */
volatile        Uint32  mpLk2;          /* lock bits 95-64 */
volatile        Uint32  mpLk3;          /* lock bits 127-96 */
volatile        Uint32  mpLkCmd;        /* lock command register */
volatile        Uint32  mpLkStat;       /* lock status register */
} _MPC_Lock;

typedef struct _MPC_Fault {
volatile        Uint32  mpFar;          /* MPC Fault Address Register */
volatile        Uint32  mpFsr;          /* MPC Fault Status Register */
volatile        Uint32  mpFcr;          /* MPC Fault Clear Register */
} _MPC_Fault;

/*
 *  ======== _MPC_Key ========
 *  _MPC_Key represents the 128-bit key used for locking
 *  and unlocking a controller's permission attribute entries.
 *  It is used for writing to the MPLK0-MPLK3 registers.
 */
typedef struct _MPC_Key {
        Uint32  key0;                   /* bits 0-31 of 128-bit key */
        Uint32  key1;                   /* bits 32-63 of 128-bit key */
        Uint32  key2;                   /* bits 64-95 of 128-bit key */
        Uint32  key3;                   /* bits 96-127 of 128-bit key */
} _MPC_Key;

/*
 *  The following match the values returned from _MPC_decodeRegion.
 *  They also relate to indexing into the _MPC_controllers array, although
 *  1 must be subtracted before using them as indexes into the 0-based
 *  array (_MCP_NONE won't ever be used to index this array).
 */
#define _MPC_NONE       0
#define _MPC_UMAP0      1
#define _MPC_UMAP1      2
#define _MPC_DMC        3
#define _MPC_PMC        4
#define _MPC_UMC        _MPC_UMAP0

/* Memory Protection Controller */
typedef struct _MPC_Controller {
        _MPC_Fault      *mpFault;       /* addr of MP Fault registers */
        _MPC_Lock       *mpLck;         /* addr of MP Lock registers */
        MPC_Perm        *mppaTab;       /* addr of base of MP PA table */
        Uns             *mpCfg;         /* addr of MP Configuration register */
        Uns             nPages;         /* # pages in the MPC */
        Uns             evtNum;         /* Event number for controller */
} _MPC_Controller;

extern _MPC_Controller *_MPC_controllers[];

#define _MPC_MPLCKUNLOCK        0x00000001
#define _MPC_MPLCKLOCK          0x00000002
#define _MPC_MPLCKKEYR          0x00000004

#define _MPC_MPLKSTATLK         0x00000001

#define _MPC_MPFSR_UXE          MPC_MPPA_UX
#define _MPC_MPFSR_UWE          MPC_MPPA_UW
#define _MPC_MPFSR_URE          MPC_MPPA_UR
#define _MPC_MPFSR_SXE          MPC_MPPA_SX
#define _MPC_MPFSR_SWE          MPC_MPPA_SW
#define _MPC_MPFSR_SRE          MPC_MPPA_SR
#define _MPC_MPFSR_SECE         0x00000080
#define _MPC_MPFSR_FIDMASK      0x0000FF00
#define _MPC_MPFSR_FIDSHIFT     8

Bool _MPC_makeLocal(Uns *addr, Uns *space);
Uns _MPC_getDeviceNum(Void);
Void _MPC_lock(_MPC_Controller *ctrl, _MPC_Key *key);
Void _MPC_unlock(_MPC_Controller *ctrl, _MPC_Key *key);
Void _MPC_writePA(_MPC_Controller *ctrl, Uns page, MPC_Perm perm);
MPC_Perm _MPC_readPA(_MPC_Controller *ctrl, Uns page);
Void _MPC_writeMPCPA(_MPC_Controller *ctrl, MPC_Perm perm);
_MPC_Controller *_MPC_getMPC(Ptr addr);
Int _MPC_getPageSize(Ptr addr);
Int _MPC_decodeRegion(Uns begAddr, Uns endAddr, Uns *begPage, Uns *endPage);

Void            _MPC_init(Void);
Void            _MPC_exceptionHandler(Void);
Void            _MPC_internalHandler(Void);
Void            _MPC_externalHandler(Void);
Void            _MPC_nmiHandler(Void);
Void            _MPC_decodeException(Uns id);
Uint32          _MPC_getLastMPFAR(Uns id);
Uint32          _MPC_getLastMPFSR(Uns id);

extern Void (*_MPC_userHook)(Void);

#endif /* !__MPC_H_ */

