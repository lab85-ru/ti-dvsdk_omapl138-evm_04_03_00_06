/** ============================================================================
 *  @file   dra44xgem_map.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DRA44XGEM/
 *
 *  @desc   Defines the configuration mapping information for the DSP/BIOS LINK
 *          driver.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (POOL_COMPONENT)
#include <pooldefs.h>
#include <sma_pool.h>
#endif /* if defined (POOL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

#include <cfgmap.h>
#include <kfile.h>
#include <ips.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   DRA44XGEM_DspObjects
 *
 *  @desc   Array of configuration mapping objects for the DSPs in the system.
 *  ============================================================================
 */
extern DSP_Interface DRA44XGEM_Interface ;
CONST CFGMAP_Dsp DRA44XGEM_DspObjects [] =
{
    {
        "DRA44XGEM",                     /* NAME      : Name of the DSP */
        &DRA44XGEM_Interface             /* INTERFACE : DSP interface table */
    }
} ;

/** ============================================================================
 *  @name   DRA44XGEM_LoaderObjects
 *
 *  @desc   Array of configuration mapping objects for the DSP executable
 *          loaders in the system.
 *  ============================================================================
 */
EXTERN LOADER_Interface COFFFILE_Interface ;
EXTERN LOADER_Interface COFFMEM_Interface ;
EXTERN LOADER_Interface COFFSHM_Interface ;
EXTERN LOADER_Interface NOLOADER_Interface ;
EXTERN KFILE_Interface KFILEPSEUDO_Interface ;
#if defined (KFILE_DEFAULT)
EXTERN KFILE_Interface KFILEDEF_Interface ;
#endif /* if defined (KFILE_DEFAULT) */
#if defined (OS_PROS)
extern LOADER_Interface BINLOADER_Interface ;
extern LOADER_Interface STATICLOADER_Interface ;
#endif /* if defined (OS_PROS) */
CONST CFGMAP_Loader  DRA44XGEM_LoaderObjects [] =
{
    {
        "COFF",                        /* NAME           : Name of the loader */
        &COFFFILE_Interface,           /* LOADERINTF     : Loader interface table */
#if defined (KFILE_DEFAULT)
        &KFILEDEF_Interface            /* KFILEINTF      : KFILE interface table */
#else /* if defined (KFILE_DEFAULT) */
#if defined (OS_PROS)
#if defined (KFILE_PSEUDO)
        &KFILEPSEUDO_Interface         /* KFILEINTF      : KFILE interface table */
#endif /* if defined (KFILE_PSEUDO) */
#else /* if defined (OS_PROS) */
        #error Psuedo file system not supported for this configuration
#endif /* if defined (OS_PROS) */
#endif /* if defined (KFILE_DEFAULT) */
    },
    {
        "COFFSHM",                     /* NAME           : Name of the loader */
        &COFFSHM_Interface,           /* LOADERINTF     : Loader interface table */
#if defined (KFILE_DEFAULT)
        &KFILEDEF_Interface            /* KFILEINTF      : KFILE interface table */
#else /* if defined (KFILE_DEFAULT) */
#if defined (OS_PROS)
#if defined (KFILE_PSEUDO)
        &KFILEPSEUDO_Interface         /* KFILEINTF      : KFILE interface table */
#endif /* if defined (KFILE_PSEUDO) */
#else /* if defined (OS_PROS) */
        #error Psuedo file system not supported for this configuration
#endif /* if defined (OS_PROS) */
#endif /* if defined (KFILE_DEFAULT) */
    },
    {
        "COFFMEM",                     /* NAME           : Name of the loader */
        &COFFMEM_Interface,            /* LOADERINTF     : Loader interface table */
        &KFILEPSEUDO_Interface         /* KFILEINTF      : KFILE interface table */
    },
    {
        "NOLOADER",                    /* NAME           : Name of the loader */
        &NOLOADER_Interface,           /* LOADERINTF     : Loader interface table */
        NULL                           /* KFILEINTF      : KFILE interface table */
    },
#if defined (OS_PROS)
    {
        "BINARY",                      /* NAME           : Name of the loader */
        &BINLOADER_Interface,          /* LOADERINTF     : Loader interface table */
        NULL                           /* KFILEINTF      : KFILE interface table */
    },
    {
        "STATIC",                      /* NAME           : Name of the loader */
        &STATICLOADER_Interface,       /* LOADERINTF     : Loader interface table */
        NULL                           /* KFILEINTF      : KFILE interface table */
    }
#endif /* if defined (OS_PROS) */
} ;

#if (!defined (ONLY_PROC_COMPONENT))
/** ============================================================================
 *  @name   DRA44XGEM_LinkDrvObjects
 *
 *  @desc   Array of configuration mapping objects for the link drivers in the
 *          system.
 *  ============================================================================
 */
extern DRV_Interface SHMDRV_Interface ;
CONST CFGMAP_LinkDrv  DRA44XGEM_LinkDrvObjects [] =
{
    {
        "SHMDRV",                      /* NAME           : Name of the link driver */
        &SHMDRV_Interface              /* INTERFACE      : Link driver interface table */
    }
} ;

/** ============================================================================
 *  @name   DRA44XGEM_IpsObjects
 *
 *  @desc   Array of configuration mapping objects for the IPS components in the
 *          system.
 *  ============================================================================
 */
CONST CFGMAP_Ips  DRA44XGEM_IpsObjects [] =
{
    {
        "IPS",                          /* NAME           : Name of the IPS */
        (FnIpsInit) (&IPS_init),        /* FXN_INIT       : Init function for the IPS */
        (FnIpsExit) (&IPS_exit),        /* FXN_EXIT       : Exit function for the IPS */
#if defined (DDSP_DEBUG)
        (FnIpsDebug) &IPS_debug        /* FXN_DEBUG      : Debug function for the IPS */
#endif /* if defined (DDSP_DEBUG) */
    }
} ;
#endif /* if (!defined (ONLY_PROC_COMPONENT)) */


#if defined (POOL_COMPONENT)
/** ============================================================================
 *  @name   DRA44XGEM_PoolObjects
 *
 *  @desc   Array of configuration mapping objects for the POOLs in the system.
 *  ============================================================================
 */
extern POOL_Interface   SMAPOOL_Interface ;

#if defined (PCPY_LINK)
extern POOL_Interface   BUFPOOL_Interface ;
#endif /* if defined (PCPY_LINK) */
CONST CFGMAP_Pool  DRA44XGEM_PoolObjects [] =
{
    {
        "SMAPOOL",                     /* NAME           : Name of the pool */
        (FnPoolInit) (&SMAPOOL_init),  /* FXN_INIT       : Init function for the pool */
        (FnPoolExit) (&SMAPOOL_exit),  /* FXN_EXIT       : Exit function for the pool */
        &SMAPOOL_Interface             /* INTERFACE      : Pool interface table */
    },
#if defined (PCPY_LINK)
    {
        "BUFPOOL",                     /* NAME           : Name of the pool */
        (FnPoolInit) &BUFPOOL_init,    /* FXN_INIT       : Init function for the pool */
        (FnPoolExit) &BUFPOOL_exit,    /* FXN_EXIT       : Exit function for the pool */
        &BUFPOOL_Interface             /* INTERFACE      : Pool interface table */
    }
#endif /* if defined (PCPY_LINK) */
} ;
#endif /* if defined (POOL_COMPONENT) */

#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @name   DRA44XGEM_DataDrvObjects
 *
 *  @desc   Array of configuration mapping objects for the Data drivers in the
 *          system.
 *  ============================================================================
 */
extern DATA_Interface ZCPYDATA_Interface ;
CONST CFGMAP_DataDrv  DRA44XGEM_DataDrvObjects [] =
{
    {
        "ZCPYDATA",                /* NAME           : Name of the data driver */
        &ZCPYDATA_Interface        /* INTERFACE      : Data transfer interface table */
    }
} ;
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
/** ============================================================================
 *  @name   DRA44XGEM_MqtObjects
 *
 *  @desc   Array of configuration mapping objects for the Message Queue
 *          Transports in the system.
 *  ============================================================================
 */
extern MQT_Interface ZCPYMQT_Interface ;
CONST CFGMAP_Mqt  DRA44XGEM_MqtObjects [] =
{
    {
        "ZCPYMQT",                     /* NAME           : Name of the Message Queue Transport */
        &ZCPYMQT_Interface             /* INTERFACE      : MQT Interface table */
    }
} ;
#endif /* if defined (MSGQ_COMPONENT) */

/** ============================================================================
 *  @name   DRA44XGEM_Config
 *
 *  @desc   DSP/BIOS LINK configuration mapping structure.
 *  ============================================================================
 */
CONST CFGMAP_Object  DRA44XGEMMAP_Config = {
    1u,                                          /* NUMDSPS        : Number of types of DSPs */
    (CFGMAP_Dsp *)     DRA44XGEM_DspObjects,     /* DSPOBJECTS     : Array of DSP configuration mapping objects */
#if defined (OS_PROS)
    6,                                        /* NUMLOADERS     : Number of types of DSP executable loaders */
#else /* if defined (OS_PROS) */
    4,                                        /* NUMLOADERS     : Number of types of DSP executable loaders */
#endif /* if defined (OS_PROS) */
    (CFGMAP_Loader *)  DRA44XGEM_LoaderObjects,  /* LOADERS        : Array of DSP executable loader configuration mapping objects */

#if (!defined (ONLY_PROC_COMPONENT))
    1u,                                          /* NUMLINKDRVS    : Number of types of link drivers */
    (CFGMAP_LinkDrv *) DRA44XGEM_LinkDrvObjects, /* LINKDRVOBJECTS : Array of Link Driver configuration mapping objects */
    1u,                                          /* NUMIPS         : Number of types of IPS */
    (CFGMAP_Ips *)     DRA44XGEM_IpsObjects,     /* IPSOBJECTS     : Array of IPS configuration mapping objects */
#else
    0,                                              /* NUMLINKDRVS    : Number of types of link drivers */
    NULL,                                           /* LINKDRVOBJECTS : Array of Link Driver configuration mapping objects */
    0,                                              /* NUMIPS         : Number of types of IPS */
    NULL,                                           /* IPSOBJECTS     : Array of IPS configuration mapping objects */
#endif /* if (!defined (ONLY_PROC_COMPONENT)) */

#if defined (POOL_COMPONENT)
    1u,                                          /* NUMPOOLS       : Number of types of POOLs */
    (CFGMAP_Pool *)    DRA44XGEM_PoolObjects,    /* POOLOBJECTS    : Array of POOL configuration mapping objects */
#endif /* if defined (POOL_COMPONENT) */
#if defined (CHNL_COMPONENT)
    1u,                                          /* NUMDATADRIVERS : Number of types of Data drivers */
    (CFGMAP_DataDrv *) DRA44XGEM_DataDrvObjects, /* DATADRIVERS    : Array of Data driver configuration mapping objects */
#endif /* if defined (CHNL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
    1u,                                          /* NUMMQTS        : Number of types of MQTs */
    (CFGMAP_Mqt *)     DRA44XGEM_MqtObjects      /* MQTOBJECTS     : Array of MQT configuration mapping objects */
#endif /* if defined (MSGQ_COMPONENT) */
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
