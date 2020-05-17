/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <xdc/std.h> 
#include <ti/sdo/linuxutils/edma/include/edma.h>
#include <ti/sdo/linuxutils/vicp/include/vicp.h>
#include <ti/sdo/linuxutils/cmem/include/cmem.h>
#include <ti/sdo/fc/utils/lock.h>

#ifdef xdc_target__os_Linux
#include <ti/sdo/fc/utils/sharedMem.h>
#endif
#include <stdlib.h>

#define EDMA_LOCKID 0xC0FECAFE

typedef struct EDMA_RegInfo {
    int size;
    unsigned int * edmaAddressSpace; 
} EDMA_RegInfo;

typedef struct EDMA_Regs {
    unsigned char registerSpace[0x2000];
    unsigned char paRamSpace[0x800];
} EDMA_Regs;

typedef struct EDMA_State {

    EDMA_RegInfo edma;

    Int paramArr[512];  

    Int edmaArr[64];  
    Int qdmaArr[64];  
    Int tccArr[64];  

    Int symIndex;

    Int qdmaIndex;
    Int tccQdmaIndex;
    Int paramAnyIndex;
    Int refCount; 
    Int sharedMemId;

} EDMA_State;

static EDMA_State * state = NULL; 
#ifdef xdc_target__os_Linux
static EDMA_Regs edmaRegisters; 
#endif

#ifndef xdc_target__os_Linux
static EDMA_State myState;
#endif

static LockMP_Handle EDMA_lock = NULL;

static void setInternalState(void *addr);

int EDMA_init(void)
{
    Int sharedMemId;

    if (NULL == EDMA_lock) {

        EDMA_lock = LockMP_create(EDMA_LOCKID);   

        if (NULL == EDMA_lock) {
            return (0);
        }
    } 

#ifdef xdc_target__os_Linux
    state = ( EDMA_State *)SHM_getObj(EDMA_lock, sizeof(EDMA_State), 
            EDMA_LOCKID, &setInternalState, &sharedMemId); 
#else
    state = &myState; 
    setInternalState(state);
    sharedMemId = 0;
#endif

    state->refCount++; 
    
    if (NULL != state) {
        state->sharedMemId = sharedMemId; 
    }

    return (1);
}

static void setInternalState(void *addr) 
{

    Int i;
    EDMA_State * ptr = (EDMA_State *)addr;

    if (NULL == ptr) {
        return; 
    }

    for (i = 0; i < 64; i++) {
        ptr->paramArr[i] = ptr->qdmaArr[i] = 
                ptr->edmaArr[i] = ptr->tccArr[i] = 0;
    }
    for (i = 64; i < 512; i++) {
        ptr->paramArr[i] = 0;
    }    

    ptr->symIndex = 0;  
    ptr->qdmaIndex = 0;  

    ptr->tccQdmaIndex = 63;  
    ptr->paramAnyIndex = 64;  
    (ptr->edma).size = sizeof(EDMA_RegInfo);
#ifdef xdc_target__os_Linux
    /* For PC BUILD return some fake address */
    (ptr->edma).edmaAddressSpace = (void *)(&edmaRegisters);
#else
    /* For ARM type builds of this module, return actual EDMA address */
    (ptr->edma).edmaAddressSpace = (void *)0x1C00000;
#endif
}

int EDMA_exit(void)
{
    Int refCount = state->refCount;

#ifdef xdc_target__os_Linux
    SHM_freeObj(EDMA_lock, state, state->sharedMemId);
    state = NULL;
#endif
    if (refCount == 0) { 
        LockMP_delete(EDMA_lock);
        EDMA_lock = NULL;
    }
    return (1);
}

EDMA_Status EDMA_mapBaseAddress(void **pvirtAddr)
{
    Int sharedMemId = -1;
    if (NULL == state) {
#ifdef xdc_target__os_Linux
        state = ( EDMA_State *)SHM_getObj(EDMA_lock, sizeof(EDMA_State), 
                EDMA_LOCKID, &setInternalState, &sharedMemId); 
#else
        state = &myState; 
        setInternalState(state);
        sharedMemId = 0;
#endif
        state->sharedMemId = sharedMemId;
    }
    *pvirtAddr = (void **)(state->edma).edmaAddressSpace; 
    return (EDMA_OK);    
}

EDMA_Status EDMA_getResource(int devId, int *tcc, int *channel, int *param,
                             int nParams)
{
    EDMA_Status status = EDMA_ENOCHANNEL;
    Bool found = FALSE;
    Int j = 0;    
    Int temp = -1;
    Int sharedMemId;
    if (NULL == state) {
#ifdef xdc_target__os_Linux
        state = ( EDMA_State *)SHM_getObj(EDMA_lock, sizeof(EDMA_State), 
                EDMA_LOCKID, &setInternalState, &sharedMemId); 
#else
        state = (EDMA_State *)&myState;
        setInternalState(state);
        sharedMemId = 0;
#endif
        state->sharedMemId = sharedMemId;
    }
 
    switch (devId) {

        case EDMA_EDMAANY: 

            while (state->symIndex < 64) {
                if ((state->edmaArr[state->symIndex] == 0) && 
                        (state->paramArr[state->symIndex] == 0) && 
                        (state->tccArr[state->symIndex] == 0)) { 
                    *channel = state->symIndex; 
                    *param   = state->symIndex;
                    *tcc     = state->symIndex; 
                    state->edmaArr[state->symIndex] = 1;        
                    state->paramArr[state->symIndex] = 1;        
                    state->tccArr[state->symIndex] = 1;        
                    status = EDMA_OK;
                    state->symIndex++;
                    break;
                }
                else {
                    state->symIndex++;
                }
           }
           break;
        
       case EDMA_QDMAANY: 

        while (state->tccQdmaIndex != state->symIndex) {

            if (*tcc != EDMA_TCCANY) {
                temp = state->tccQdmaIndex;
                state->tccQdmaIndex = *tcc; 
            }

            if (state->tccArr[state->tccQdmaIndex] == 0) { 

                while ((state->qdmaIndex < 8) && 
                        (state->qdmaArr[state->qdmaIndex] == 1)) {
                    state->qdmaIndex++;
                }

                while ((state->paramAnyIndex < 512) && (state->paramArr[state->paramAnyIndex] == 1)){
                    state->paramAnyIndex++;
                }

                if ((1 == state->qdmaArr[state->qdmaIndex]) || 
                        (1 == state->paramArr[state->paramAnyIndex])) {
                    break;
                }

                *channel = EDMA_QDMA0 + state->qdmaIndex; 
                *param   = state->paramAnyIndex;
                *tcc     = state->tccQdmaIndex; 
                state->qdmaArr[state->qdmaIndex] = 1;        
                state->paramArr[state->paramAnyIndex] = 1;        
                state->tccArr[state->tccQdmaIndex] = 1;        
                state->qdmaIndex++;
                state->tccQdmaIndex--;
                state->paramAnyIndex++;
                status = EDMA_OK;

                if (temp != -1) {
                    state->tccQdmaIndex = temp;
                }
                break;
            }
            else {
                if (*tcc != EDMA_TCCANY) {
                    break; 
                }
                state->tccQdmaIndex--;
            }
       }
       if (temp != -1) {
           state->tccQdmaIndex = temp;
       }
       break;
       case EDMA_PARAMANY: 
            while ((state->paramAnyIndex < 512) && (state->paramArr[state->paramAnyIndex] == 1)) {
                state->paramAnyIndex++;
            }
            if (state->paramAnyIndex + nParams < 512) {
                for (j = 0; j < nParams; j++) {
                    state->paramArr[state->paramAnyIndex+j] = 1;
                }
                *param = state->paramAnyIndex; 
                state->paramAnyIndex += nParams;
                status = EDMA_OK;
                break;
            } 
        break;
       default:
            if (devId < 64) {
                /* Requested and EDMA channel */
                if ((state->edmaArr[devId] == 0) && 
                        (state->paramArr[devId] == 0) && 
                        (state->tccArr[devId] == 0)) { 
                    *channel = devId; 
                    *param   = devId;
                    *tcc     = devId; 
                    state->edmaArr[devId] = 1;        
                    state->paramArr[devId] = 1;        
                    state->tccArr[devId] = 1;        
                    status = EDMA_OK;
                    break;
                }
            }  
            else if (devId < 512) {
                /* Requested a PARAM channel */
                for (j = 0; j < nParams; j++) {
                    if (state->paramArr[devId+j] == 0) {
                        found = TRUE;
                        state->paramArr[devId + j] = 1;
                    }
                    else {
                        found = FALSE;
                        break;
                    }
                }
                if (found) {
                    *param = devId;
                    status = EDMA_OK;
                }
            }
            else if (devId <= EDMA_QDMA7) {
                if (state->qdmaArr[devId-EDMA_QDMA0] == 0) {
                    state->qdmaArr[devId-EDMA_QDMA0] = 1;
                    *channel = devId-EDMA_QDMA0;
                }

                while (state->tccQdmaIndex != state->symIndex) {

                    if (state->tccArr[state->tccQdmaIndex] == 0) { 
    
                        while ((state->paramAnyIndex < 512) && 
                                    (state->paramArr[state->paramAnyIndex] == 1)) {
                                state->paramAnyIndex++;
                        }
        
                        if (1 == state->paramArr[state->paramAnyIndex]) {
                            break;
                        }
    
                        *param   = state->paramAnyIndex;
                        *tcc     = state->tccQdmaIndex; 
                        state->paramArr[state->paramAnyIndex] = 1;        
                        state->tccArr[state->tccQdmaIndex] = 1;        
                        state->tccQdmaIndex--;
                        state->paramAnyIndex++;
                        status = EDMA_OK;
                        break;
                    }
                    else {
                        state->tccQdmaIndex--;
                    }
                }
            }
        }
        return (status);    
}

/* ARGSUSED */
EDMA_Status EDMA_freeResource(int lch, int nParams)
{    
    return (EDMA_OK);
}

/* ARGSUSED */
EDMA_Status EDMA_registerResource(int lch)
{
        return (EDMA_OK);
};

/* ARGSUSED */
EDMA_Status EDMA_unregister(int lch, int nParams)
{
        return (EDMA_OK);
}

int CMEM_init(void) 
{
    return (1);
}

int CMEM_exit(void) 
{
    return (1);
}

/* ARGSUSED */
void *CMEM_alloc(size_t size, CMEM_AllocParams *params)
{
    return malloc(size);
}

/* ARGSUSED */
int CMEM_free(void *ptr, CMEM_AllocParams *params)
{
    free(ptr);
    return (1);
}

unsigned long CMEM_getPhys(void *ptr)
{
   return ((unsigned long)ptr); 
}

int VICP_init()
{
    return (1);
}

int VICP_exit()
{
    return(1);
}

/* ARGSUSED */
VICP_Status VICP_register(VICP_ResourceType resource,
                          VICP_InterruptLine intrLine,
                          VICP_InterruptType intrType)
{
    return (VICP_OK);
}

/* ARGSUSED */
VICP_Status VICP_unregister(VICP_ResourceType resource)
{
    return (VICP_OK);
}

/* ARGSUSED */
VICP_Status VICP_wait(VICP_ResourceType resource)
{
    return (VICP_OK);
}

/* ARGSUSED */
VICP_Status VICP_done(VICP_ResourceType resource, int *done)
{
    return (VICP_OK);
}
/*
 *  @(#) ti.sdo.fc.rman.examples; 1, 0, 0,123; 12-1-2010 17:25:37; /db/atree/library/trees/fc/fc-p01x/src/ xlibrary

 */

