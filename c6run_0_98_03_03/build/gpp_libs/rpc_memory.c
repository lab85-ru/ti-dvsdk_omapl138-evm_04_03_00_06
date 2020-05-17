/*
 * rpc_memory.c
*/

/************************************************************************
* Contains the management routines for shared CMEM memory utilized in   *
* RPC buffer parameters eg add/remove mappings and perform translations *
************************************************************************/


/*  ----------------------------------- Standard Headers              */
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <procdefs.h>

#include "rpc_server.h"
#include "C6Run_mem.h"

/*  ----------------------------------- This module's header          */
#include "rpc_memory.h"

/************************************************************
* Explicit External Declarations                            *
************************************************************/

/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

typedef struct _RPC_memmap_
{
    unsigned int phys_addr;
    unsigned int virt_addr;
    unsigned int size;
    struct _RPC_memmap_ * next_entry;
    struct _RPC_memmap_ * prev_entry;
}
RPC_memmap;

/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

/* virt<->phy memory mapping */
RPC_memmap *rpc_memmap = NULL;
/* GPP-side buffer for copying DSP data into using PROC_read */
char *rpc_procread_buf_base = NULL;
char *rpc_procread_buf = NULL;

/************************************************************
* Global Variable Definitions                               *
************************************************************/


/************************************************************
* Global Function Definitions                               *
************************************************************/

#if (0)
/* Reset the position of the PROC_read buffer to make space for new readings*/
void rpc_reset_procread_buf()
{
    rpc_procread_buf = rpc_procread_buf_base;
}
#endif

/* Handle the passed pointer with physical address, either by translation or manual copying */
#if (0)
unsigned int rpc_handle_physical_address(unsigned int phys_addr)
{
    unsigned int virt_addr = rpc_translate_address(phys_addr, RPC_PHYS_TO_VIRT);
    
    if(virt_addr == phys_addr)
    {
        /* when the translation fails, the function returns the same value */
        /* in this case, we perform manual copying using PROC_read, to read a fixed number of bytes */
        /* from the DSP stack/heap, into the GPP-side buffer allocated for this purpose, and pass   */
        /* the address of this buffer as the parameter instead */
        // FIXME how to determine the size of buffer to be read?
        // FIXME is reading past the end of memory an issue?
        if(rpc_procread_buf - rpc_procread_buf_base == RPC_PROCREAD_BUFSIZE)
        {
            /* no space left in the procread buffer */
            printf("GPP buffer is full, cannot perform PROC_read \n");
            return virt_addr;
        }
        if(PROC_read(DSP0, virt_addr, RPC_PROCREAD_MAXPARAM, rpc_procread_buf) == DSP_SOK)
        {
            // FIXME forcible null termination needed?
            rpc_procread_buf[RPC_PROCREAD_MAXPARAM-1] = 0;
            #ifdef VERBOSE
            printf("Read from DSP memory: %s \n", rpc_procread_buf);
            #endif
            virt_addr = (unsigned int) rpc_procread_buf;
            /* increment the buffer to the next free slot */
            rpc_procread_buf += RPC_PROCREAD_MAXPARAM;
        }
        else
        {
            printf("Failed to read from DSP mem using PROC_read, returning NULL pointer \n");
            return 0;
        }
    }

    return virt_addr;
}
#endif

#if (0)
/* Add map entry for given CMEM block */
void rpc_add_mapping(void *virtual_address, void * physical_address, unsigned int size)
{
    RPC_memmap *new_entry = (RPC_memmap *) malloc(sizeof(RPC_memmap));
    
    new_entry->phys_addr = (unsigned int) physical_address;
    new_entry->virt_addr = (unsigned int) virtual_address;
    new_entry->size = size;
    new_entry->next_entry = NULL;
    new_entry->prev_entry = NULL;

#ifdef VERBOSE
    printf("rpc_add_mapping: adding map entry with virt=%x phys=%x size=%u\n", (unsigned int)virtual_address, physical_address, size);
#endif
    
    // insert new entry to the beginning of the linked list
    
    if(rpc_memmap)
    {
        rpc_memmap->prev_entry = new_entry;
        new_entry->next_entry = rpc_memmap;
    }
    
    rpc_memmap = new_entry;
    
}
#endif

/* Do address translation using the present map entries */
unsigned int rpc_translate_address(unsigned int src_address, unsigned char translation_type)
{    
    if(translation_type == RPC_PHYS_TO_VIRT)
    {
      #if (1)
        return ((unsigned int) C6RUN_MEM_phys2Virt((void *)src_address));
      #else
        RPC_memmap *current_entry = rpc_memmap;
        unsigned int result = 0;
        while(current_entry != NULL)
        {
            if((src_address >= current_entry->phys_addr) && (src_address <= (current_entry->phys_addr + current_entry->size)))
            {
                result = current_entry->virt_addr + (src_address - current_entry->phys_addr);
                VERBOSE_PRINT2("Physical address %x is mapped to virtual address %x\n", src_address, result);

                /* ARM cache is enabled, need to perform invalidation to ensure cache coherency */
                /* since we have a physical->virtual transition (coming from the DSP) */
                C6RUN_MEM_inv( (void *) current_entry->virt_addr, current_entry->size );                

                return result;
            }
            current_entry = current_entry->next_entry;
        }
        VERBOSE_PRINT1("Physical address %x couldn't be mapped to virtual, returning  itself \n", src_address);
      #endif
    }
    else if(translation_type == RPC_VIRT_TO_PHYS)
    {
      #if (1)
        return ((unsigned int) C6RUN_MEM_virt2Phys((void *)src_address));
      #else
        RPC_memmap *current_entry = rpc_memmap;
        unsigned int result = 0;
        while(current_entry != NULL)
        {
            if((src_address >= current_entry->virt_addr) && (src_address <= (current_entry->virt_addr + current_entry->size)))
            {
                result = current_entry->phys_addr + (src_address - current_entry->virt_addr);
                VERBOSE_PRINT2("Virtual address %x is mapped to physical address %x\n", src_address, result);

                /* ARM cache is enabled, need to perform writeback to ensure cache coherency */
                /* since we have a virtual->physical transition (going back to the DSP) */
                C6RUN_MEM_wb( (void *) current_entry->virt_addr, current_entry->size );              

                return result;
            }
            current_entry = current_entry->next_entry;
        }
        VERBOSE_PRINT1("Virtual address %x couldn't be mapped to physical, returning itself \n", src_address);  
      #endif
    }
    
    return src_address;
}

/* Remove given mapping */
#if (0)
void rpc_remove_mapping(unsigned int physical_address)
{
    RPC_memmap *current_entry = rpc_memmap;
    
    if(rpc_memmap->phys_addr == physical_address)
    {
        rpc_memmap = rpc_memmap->next_entry;
        if(rpc_memmap) rpc_memmap->prev_entry = NULL;
        free(current_entry);
        return;
    }

    while(current_entry != NULL)
    {
        if(current_entry->phys_addr == physical_address)
        {
            current_entry->prev_entry->next_entry = current_entry->next_entry;
            if(current_entry->next_entry) 
                current_entry->next_entry->prev_entry = current_entry->prev_entry;
            free(current_entry);
            return;
        }
        current_entry = current_entry->next_entry;
    }
}
#endif
/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
