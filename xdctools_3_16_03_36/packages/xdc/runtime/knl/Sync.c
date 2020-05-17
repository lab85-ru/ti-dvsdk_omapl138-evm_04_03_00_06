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
 *  ======== Sync.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/knl/ISync.h>

#include "package/internal/Sync.xdc.h"

/*
 *  ======== Sync_query ========
 */
Bool  Sync_query(ISync_Handle sync, Int qual)
{
    return (Sync_Proxy_query(sync, qual));
}

/*
 *  ======== Sync_wait ========
 */
Int Sync_wait(ISync_Handle sync, UInt timeout, Error_Block *eb)
{
    return(Sync_Proxy_wait(sync, timeout, eb));
}

/*
 *  ======== Sync_signal ========
 */
Void Sync_signal(ISync_Handle sync)
{
    Sync_Proxy_signal(sync);
}

/*
 *! Revision History
 *! ================
 *! 07-Feb-2008    nitya    created
 */

/*
 *  @(#) xdc.runtime.knl; 1, 0, 0,46; 2-22-2010 10:33:52; /db/ztree/library/trees/xdc/xdc-u17x/src/packages/
 */

