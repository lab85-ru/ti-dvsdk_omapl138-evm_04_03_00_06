/*******************************************************************************
**+--------------------------------------------------------------------------+**
 *                                                                            **
 * Copyright (c) 2010, Texas Instruments Incorporated                         **
 * All rights reserved.                                                       **
 *                                                                            **
**+--------------------------------------------------------------------------+**
 *           ======== C6ACCEL.c ========                                      **
 *           Universal "C6ACCEL" xdais algorithm.                             **
 *                                                                            **
 *  This file contains an implementation of the C6Accel xdais algorithm using **
 *  IUNIVERSAL interface defined by xDM.                                      **
*******************************************************************************/

/* Include Files */
#include <xdc/std.h>
#include <string.h>
#include <ti/xdais/dm/iuniversal.h>

/* Header Intrinsic to xdais algorithm*/
#include "C6Accel.h"

#ifdef __TI_COMPILER_VERSION__
/* xDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(C6ACCEL_TI_control, ".text:algControl")
#pragma CODE_SECTION(C6ACCEL_TI_process, ".text:algProcess")
#pragma CODE_SECTION(C6ACCEL_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(C6ACCEL_TI_free,    ".text:algFree")
#pragma CODE_SECTION(C6ACCEL_TI_alloc,   ".text:algAlloc")
#endif

/* Application-Algorithm Interface file includes */
#include "iC6Accel_ti.h"
#include "iC6Accel_ti_priv.h"

/* Maintain version string for future releases */
#define C6ACCEL_VERSIONSTRING "1.01.00.03"

/* IALGFXN definition */ 
#define IALGFXNS                                                      \
    &C6ACCEL_TI_IALG,         /* module ID */                         \
    NULL,                     /* activate */                          \
    C6ACCEL_TI_alloc,         /* alloc */                             \
    NULL,                     /* control (NULL => no control ops) */  \
    NULL,                     /* deactivate */                        \
    C6ACCEL_TI_free,          /* free */                              \
    C6ACCEL_TI_initObj,       /* init */                              \
    NULL,                     /* moved */                             \
    NULL                      /* numAlloc (NULL => IALG_MAXMEMRECS) */

/****************************************************************************+
 *           ======== C6ACCEL_TI_I C6ACCEL ========
 *  This structure defines TI's implementation of the IUNIVERSAL interface
 *  for the C6ACCEL_TI module.
 ***************************************************************************/
IUNIVERSAL_Fxns C6ACCEL_TI_IC6ACCEL = {
    {IALGFXNS},
    C6ACCEL_TI_process,
    C6ACCEL_TI_control,
};

/***************************************************************************+
 *           ======== C6ACCEL_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the C6ACCEL_TI module.
 ***************************************************************************/
#ifdef __TI_COMPILER_VERSION__
/* satisfy xDAIS symbol requirement without any overhead */
asm("_C6ACCEL_TI_IALG .set _C6ACCEL_TI_IC6ACCEL");

#else

/***************************************************************************+
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 **************************************************************************/
IALG_Fxns C6ACCEL_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#endif

/*Standard iUniversal params defintion */
IUNIVERSAL_Params UNIVERSAL_TI_PARAMS = {
    sizeof(IUNIVERSAL_Params),       /* size */
};

/**************************************************************************+
 *  ========C6ACCEL_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 **************************************************************************/
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int C6ACCEL_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[]){
   /* Request memory for my object */
    memTab[0].size = sizeof(C6ACCEL_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}

/*************************************************************************+
 *  ======== C6ACCEL_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 *************************************************************************/
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int C6ACCEL_TI_free(IALG_Handle handle, IALG_MemRec memTab[]){
    /*
     * Because our internal object size doesn't depend on create
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */

    return (C6ACCEL_TI_alloc(NULL, NULL, memTab));
}

/**************************************************************************+
 *  ======== C6ACCEL_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 ***************************************************************************/
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int C6ACCEL_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *algParams){
    const IUNIVERSAL_Params *params = (IUNIVERSAL_Params *)algParams;

    if (params == NULL) {
        params = &UNIVERSAL_TI_PARAMS;
    }
    return (IALG_EOK);
}

/**********************************************************************************+
 *  ======== C6ACCEL_TI_process ========
 Process call that contains the Library functions to be executed in the application
 **********************************************************************************/
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
XDAS_Int32 C6ACCEL_TI_process(IUNIVERSAL_Handle handle,
        XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs, XDM1_BufDesc *inOutBufs,
        IUNIVERSAL_InArgs *inArgs, IUNIVERSAL_OutArgs *outArgs)
{
  const IC6Accel_InArgs *CInArgs =(IC6Accel_InArgs *)inArgs;
  void   *pFnArray;
  Int32 fxnID;
  Int32 i,status=0;

  /* Mask Variables to extract information from Function IDs */
  Int32 vendorMsk= VENDOR_MASK;
  Int32 fxntypMsk= FXN_TYP_MASK;
 // Int32 fxnidMsk= FXN_ID_MASK;

   /* Execute the chain of functions wrapped in the process API call  */
   for( i = 0; i < CInArgs->Num_fxns; i++ ){

   /* The pFnArray is at an offset in the inArgs structure and so does not need cache management*/
   pFnArray = (void*)((XDAS_Int8*)CInArgs + CInArgs->fxn[i].Param_ptr_offset);
   fxnID = CInArgs->fxn[i].FxnID;

   /*Check to see if NULL_INDEX has not been passed*/
   if (fxnID == NULL_INDEX){
       return(IUNIVERSAL_EFXNIDFAIL);
    }
   else{
       /* Pick libraries based on vendor */
       switch ((fxnID & vendorMsk)>>VENDOR_TAG_SHIFT){
          case (TI):
          /* Pick the Library function type (eg : IMGLIB function)  */
          switch (( fxnID & fxntypMsk)>>FXNTYP_TAG_SHIFT){
           /* DSPLIB functions */
             case (DSPLIB_FXN_ID):
                status = C6ACCEL_TI_dsplibFunctionCall((void *)pFnArray, fxnID,(XDM1_BufDesc *)inBufs, (XDM1_BufDesc *)outBufs);
                if (status !=0)
                    return status;
                else
                    break;
             case (IMGLIB_FXN_ID):
                /* IMGLIB functions */
                status = C6ACCEL_TI_imglibFunctionCall((void *)pFnArray, fxnID,(XDM1_BufDesc *)inBufs, (XDM1_BufDesc *)outBufs);
                if (status !=0)
                    return status;
                else
                    break;               
             case(MATHLIB_FXN_ID):
                status = C6ACCEL_TI_mathlibFunctionCall((void *)pFnArray, fxnID,(XDM1_BufDesc *)inBufs, (XDM1_BufDesc *)outBufs);
                if (status !=0)
                    return status;
                else
                    break;              
             case (ANALYTICSLIB_FXN_ID):
                    status = C6ACCEL_TI_vlibFunctionCall((void *)pFnArray, fxnID,(XDM1_BufDesc *)inBufs, (XDM1_BufDesc *)outBufs,(XDM1_BufDesc *)inOutBufs);
                if (status !=0)
                    return status;
                else
                    break;  
               /* Reserved for future purposes */ 
              case (MEDICALLIB_FXN_ID):
                   return(IUNIVERSAL_EFXNIDFAIL);
              case(PWRCNTRLLIB_FXN_ID):
                   return(IUNIVERSAL_EFXNIDFAIL);
              case(AUDSPECHLIB_FXN_ID):
                   return(IUNIVERSAL_EFXNIDFAIL);
              default:
                   return(IUNIVERSAL_EFXNIDFAIL);
            }
          break;
          case (OTHER):
              switch (( fxnID & fxntypMsk)>>FXNTYP_TAG_SHIFT){
              /* Non-TI vendors can add their kernels in this section */
                   case (DSPLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case (IMGLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case(MATHLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case (ANALYTICSLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case (MEDICALLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case(PWRCNTRLLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   case(AUDSPECHLIB_FXN_ID):
                        return(IUNIVERSAL_EFXNIDFAIL);
                   default:
                      return(IUNIVERSAL_EFXNIDFAIL);
                }
         default:
                return(IUNIVERSAL_EFXNIDFAIL);
       }
    }
 }

    /* report how we accessed the input buffer */
    for (i=0;i<inBufs->numBufs; i++)
     {
        inBufs->descs[i].accessMask = 0;
        XDM_SETACCESSMODE_READ(inBufs->descs[i].accessMask);
      }
    /* and output buffer */
    for (i=0;i< outBufs->numBufs; i++)
     {
        outBufs->descs[i].accessMask = 0;
        XDM_SETACCESSMODE_WRITE(outBufs->descs[i].accessMask);
      }
    /* and output buffer */
    for (i=0;i< inOutBufs->numBufs; i++)
     {
        inOutBufs->descs[i].accessMask = 0;
        XDM_SETACCESSMODE_WRITE(inOutBufs->descs[i].accessMask);
      }


    return (IUNIVERSAL_EOK);
}


/******************************************************************************
 *  ======== C6Accel_TI_control ========
 *****************************************************************************/
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
XDAS_Int32 C6ACCEL_TI_control(IUNIVERSAL_Handle handle,
    IUNIVERSAL_Cmd id, IUNIVERSAL_DynamicParams *dynParams,
    IUNIVERSAL_Status *status)
{
    XDAS_Int32 retVal;

    /* validate arguments - this codec only supports "base" xDM. */
    if ((dynParams->size != sizeof(*dynParams)) ||
        (status->size != sizeof(*status))) {
        return (IUNIVERSAL_EUNSUPPORTED);
    }

    /* initialize for the general case where we don't access the data buffer */
    XDM_CLEARACCESSMODE_READ(status->data.descs[0].accessMask);
    XDM_CLEARACCESSMODE_WRITE(status->data.descs[0].accessMask);

    switch (id) {
        case XDM_GETVERSION:
            if ((status->data.descs[0].buf != NULL) &&
                (status->data.descs[0].bufSize >=
                    strlen(C6ACCEL_VERSIONSTRING))) {

                strncpy((char *)status->data.descs[0].buf,
                        C6ACCEL_VERSIONSTRING,
                        strlen(C6ACCEL_VERSIONSTRING));

                /* null terminate the string */
                status->data.descs[0].
                buf[strlen(C6ACCEL_VERSIONSTRING)] = '\0';

                /* strncpy wrote to the data buffer */
                XDM_SETACCESSMODE_WRITE(status->data.descs[0].accessMask);
                retVal = IUNIVERSAL_EOK;
            }
            else {
                retVal = IUNIVERSAL_EFAIL;
            }

        break;

        default:
            /* unsupported cmd */
            retVal = IUNIVERSAL_EFAIL;
        break;
    }

    return (retVal);
}

/****************************************************************************
**                        END OF FILE
*****************************************************************************/
/*
 *  @(#) ti.C6accel; 1, 0, 0,10; 5-4-2010 17:59:00;
 */
