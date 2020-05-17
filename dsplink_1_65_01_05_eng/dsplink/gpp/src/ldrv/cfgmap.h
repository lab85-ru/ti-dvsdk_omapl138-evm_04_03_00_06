/** ============================================================================
 *  @file   cfgmap.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines constants and interfaces for configuration mapping
 *          information for the DSP/BIOS LINK driver.
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


#if !defined (CFGMAP_H)
#define CFGMAP_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#include <ldrv_mqt.h>
#endif /* if defined (MSGQ_COMPONENT) */


#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   CFGMAP_Dsp
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          DSP.
 *
 *  @field  name
 *              Name of DSP processor.
 *  @field  interface
 *              Function pointer interface table for accessing the DSP.
 *  ============================================================================
 */
typedef struct CFGMAP_Dsp_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    DSP_Interface *          interface ;
} CFGMAP_Dsp ;

/** ============================================================================
 *  @name   CFGMAP_Loader
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          DSP executable loader.
 *
 *  @field  name
 *              Name of DSP executable loader.
 *  @field  interface
 *              Function pointer interface table for the DSP executable loader.
 *  @field  kfileIntf
 *              Function pointer interface table for the KFILE module
 *  ============================================================================
 */
typedef struct CFGMAP_Loader_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    LOADER_Interface *       interface ;
    KFILE_Interface *        kfileIntf ;
} CFGMAP_Loader ;

/** ============================================================================
 *  @name   CFGMAP_LinkDrv
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          link driver.
 *
 *  @field  name
 *              Name of link driver.
 *  @field  interface
 *              Function pointer interface table for the link driver.
 *  ============================================================================
 */
typedef struct CFGMAP_LinkDrv_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    DRV_Interface *          interface ;
} CFGMAP_LinkDrv ;

/** ============================================================================
 *  @name   CFGMAP_Ips
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          IPS component.
 *
 *  @field  name
 *              Name of link driver.
 *  @field  init
 *              Function pointer for the init function.
 *  @field  exit
 *              Function pointer for the exit function.
 *  @field  debug
 *              Function pointer for the debug function.
 *              Defined only if DDSP_DEBUG is enabled.
 *  ============================================================================
 */
typedef struct CFGMAP_Ips_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    FnIpsInit                init ;
    FnIpsExit                exit ;
#if defined (DDSP_DEBUG)
    FnIpsDebug               debug ;
#endif /* if defined (DDSP_DEBUG) */
} CFGMAP_Ips ;

#if defined (POOL_COMPONENT)
/** ============================================================================
 *  @name   CFGMAP_Pool
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          POOL component.
 *
 *  @field  name
 *              Name of the pool.
 *  @field  init
 *              Initialization function for the pool.
 *  @field  exit
 *              Finalization function for the pool.
 *  @field  interface
 *              Function pointer interface table for the pool.
 *  ============================================================================
 */
typedef struct CFGMAP_Pool_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    FnPoolInit               init ;
    FnPoolExit               exit ;
    POOL_Interface *         interface ;
} CFGMAP_Pool ;
#endif /* if defined (POOL_COMPONENT) */

#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @name   CFGMAP_DataDrv
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          DATA driver component.
 *
 *  @field  name
 *              Name of the data transfer driver.
 *  @field  interface
 *              Function pointer interface table for the data driver.
 *  ============================================================================
 */
typedef struct CFGMAP_DataDrv_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    DATA_Interface *         interface ;
} CFGMAP_DataDrv ;
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
/** ============================================================================
 *  @name   CFGMAP_Mqt
 *
 *  @desc   This structure defines the configuration mapping structure for the
 *          MQT component.
 *
 *  @field  name
 *              Name of the MQT.
 *  @field  interface
 *              Function pointer interface table for the MQT.
 *  ============================================================================
 */
typedef struct CFGMAP_Mqt_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    MQT_Interface *          interface ;
} CFGMAP_Mqt ;
#endif /* if defined (MSGQ_COMPONENT) */


/** ============================================================================
 *  @name   CFGMAP_Object
 *
 *  @desc   Defines the object containing all configuration mapping information
 *          for DSP/BIOS LINK.
 *
 *  @field  numDsps
 *              Number of DSPs supported for this platform.
 *  @field  dspObjects
 *              Pointer to the array of DSP configuration mapping objects.
 *  @field  numLoaders
 *              Number of DSP executable loaders supported for this platform.
 *  @field  loaders
 *              Pointer to the array of DSP executable loader configuration
 *              mapping objects.
 *  @field  numLinkDrvs
 *              Number of types of link drivers supported for this platform.
 *  @field  linkDrvObjects
 *              Pointer to the array of link driver configuration mapping
 *              objects.
 *  @field  numPools
 *              Number of different types of POOLs supported for this platform.
 *              Only defined if POOL_COMPONENT is used.
 *  @field  poolObjects
 *              Pointer to the array of POOL configuration mapping objects.
 *              Only defined if POOL_COMPONENT is used.
 *  @field  numDataDrivers
 *              Number of different types of data drivers supported for this
 *              platform.
 *              Only defined if CHNL_COMPONENT is used.
 *  @field  dataObjects
 *              Pointer to the array of data driver configuration mapping
 *              objects.
 *              Only defined if CHNL_COMPONENT is used.
 *  @field  numMqts
 *              Number of different types of MQTs supported for this platform.
 *              Only defined if MSGQ_COMPONENT is used.
 *  @field  mqtObjects
 *              Pointer to the array of MQT configuration mapping objects.
 *              Only defined if MSGQ_COMPONENT is used.
 *  ============================================================================
 */
typedef struct CFGMAP_Object_tag {
    Uint32                   numDsps ;
    CFGMAP_Dsp *             dspObjects ;

    Uint32                   numLoaders ;
    CFGMAP_Loader *          loaders ;

    Uint32                   numLinkDrvs ;
    CFGMAP_LinkDrv *         linkDrvObjects ;

    Uint32                   numIps ;
    CFGMAP_Ips *             ipsObjects ;

#if defined (POOL_COMPONENT)
    Uint32                   numPools ;
    CFGMAP_Pool *            poolObjects ;
#endif /* if defined (POOL_COMPONENT) */

#if defined (CHNL_COMPONENT)
    Uint32                   numDataDrivers ;
    CFGMAP_DataDrv *         dataObjects ;
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
    Uint32                   numMqts ;
    CFGMAP_Mqt *             mqtObjects ;
#endif /* if defined (MSGQ_COMPONENT) */
} CFGMAP_Object ;


/** ============================================================================
 *  @name   CFGMAP_ObjDB
 *
 *  @desc   Defines an array containing a database of configuration mapping
 *          information for all DSPs configured in the DSP/BIOS LINK. LDRV_init
 *          plugs the correct CFGMAP object after checking with this object.
 *
 *  @field  dspName
 *              Name of the DSP.
 *  @field  obj
 *              CFGMAP object associated with the DSP.
 *  ============================================================================
 */
typedef struct CFGMAP_ObjDB_tag {
    Char8 *         dspName ;
    CFGMAP_Object * obj     ;
} CFGMAP_ObjDB ;


/** ============================================================================
 *  @func   CFGMAP_attachObject
 *
 *  @desc   Plugs the CFGMAP object at correct place in CFGMAP_Config array.
 *
 *  @arg    procId
 *              Processor ID.
 *  @arg    dspName
 *              Name of the DSP for which CFGMAP object has to be plugged.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ECONFIG
 *              Configuartion error.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CFGMAP_attachObject (IN ProcessorId procId, IN Char8 * dspName) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (CFGMAP_H) */
