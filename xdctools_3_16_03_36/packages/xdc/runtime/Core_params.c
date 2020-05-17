/* --COPYRIGHT--,ESD
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
 *  This program and the accompanying materials are made available under the 
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at 
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * --/COPYRIGHT--*/
/*
 *  ======== Core_params.c ========
 */

#include <string.h>

#include <xdc/runtime/IInstance.h>
#include <xdc/runtime/Types.h>

#include "package/internal/Core.xdc.h"

/*
 *  ======== Core_assignParams ========
 */
Void Core_assignParams(Ptr dstPrms, const Ptr srcPrms, SizeT mpsz, SizeT ipsz)
{
    Types_PrmsHdr *dHdr;
    const Types_PrmsHdr *sHdr;
    IInstance_Params *dIPrms;
    IInstance_Params *sIPrms;
    SizeT sz, sz2, isz2;

    sHdr = (const Types_PrmsHdr *)srcPrms;
    sIPrms = sHdr->instPrms;

    sz = mpsz - ipsz;           /* size of mod params beyond interface */
    isz2 = sIPrms->__size;      
    sz2 = sHdr->size - isz2; 

    memcpy(dstPrms, srcPrms, sz2 < sz ? sz2 : sz);
    dHdr = (Types_PrmsHdr *)dstPrms;
    dHdr->size = mpsz;
    dHdr->self = dstPrms;       /* mark dstParams as initialized */
    dIPrms = dHdr->instPrms = (Char *)dstPrms + sz;

    memcpy(dIPrms, sIPrms, isz2 < ipsz ? isz2 : ipsz);
    dIPrms->__size = ipsz;
 }

/*
 *! Revision History
 *! ================
 *! 10-Mar-2008 sasha	split from Core.c
 */

/*
 *  @(#) xdc.runtime; 2, 0, 0, 0,237; 2-22-2010 10:33:57; /db/ztree/library/trees/xdc/xdc-u17x/src/packages/
 */

