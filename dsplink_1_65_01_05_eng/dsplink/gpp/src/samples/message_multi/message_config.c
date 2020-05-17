/** ============================================================================
 *  @file   message_config.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multi/
 *
 *  @desc   Configuration file for message_multi sample application.
 *          Contains the dynamic configuration that needs to be different for
 *          the message_multi sample application.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <procdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LINKCFG_messageMultiLinkDrvObjects
 *
 *  @desc   Array of Link driver objects in the system.
 *  ============================================================================
 */
LINKCFG_LinkDrv  LINKCFG_messageMultiLinkDrvObjects ;


/** ============================================================================
 *  @name   LINKCFG_poolTable_00
 *
 *  @desc   Pool table ID 0. Dummy Pool table to hold 17 Pools information That
 *          gets updated  and passed  to PROC_attach at runtime.
 *  ============================================================================
 */
LINKCFG_Pool  LINKCFG_messageMultiPoolTable_00 [] =
{
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    },
    {
        "",               /* NAME           : Name of the pool */
        (Uint32) 0,       /* MEMENTRY       : Memory entry ID (-1 if not needed) */
        (Uint32) 0,  /* POOLSIZE       : Size of the pool (-1 if not needed) */
        (Uint32) -1,      /* IPSID          : ID of the IPS used */
        (Uint32) -1,      /* IPSEVENTNO     : IPS Event number associated with POOL */
        0,                /* POOLMEMENTRY   : Pool memory region section ID  */
        0,                /* ARGUMENT1      : First Pool-specific argument */
        0x0               /* ARGUMENT2      : Second Pool-specific argument */
    }
} ;

/** ============================================================================
 *  @name   LINKCFG_poolTables
 *
 *  @desc   Array of Pool tables in the system.
 *  ============================================================================
 */
LINKCFG_Pool * LINKCFG_messageMultiPoolTables [] =
{
    LINKCFG_messageMultiPoolTable_00       /* Pool Table 0 */
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
