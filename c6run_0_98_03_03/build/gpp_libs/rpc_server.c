/*
 * rpc_server.c
*/

/************************************************************************
* Contains the unmarshaller, stub locator and executor routines for     *
* serving the DSP RPC calls                                             *
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

/*  ----------------------------------- CMEM API                        */
#include <ti/sdo/linuxutils/cmem/include/cmem.h>

#include "debug_common.h"
#include "C6Run_mem.h"
#include "rpc_memory.h"

/*  ----------------------------------- This module's header          */
#include "rpc_server.h"



/************************************************************
* Explicit External Declarations                            *
************************************************************/

/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

unsigned int signature_type_size[255];
/* will hold the parameters */
void **stub_param_buffer = NULL;
/* will hold the result */
void *stub_result_buffer = NULL;
/* will hold the function name */
char *function_name = NULL;
/* will hold the function signature */
char *function_sign = NULL;
/* have the buffers been allocated? */
int buffers_allocated = 0;
/* is unmarshalling done? */
int rpc_unmarshalling_done = 0;
/* is the RPC stub successfully located? */
int rpc_stub_located = 0;
/* will hold the address of the located RPC stub */
void (*rpc_stub)(void **, void *);
/* will hold the handle to the RPC library */
void *rpc_lib_handle = NULL;
/* table init flag */
int rpc_initialized = 0;
/* shared buffer for copying pointer results */
void *rpc_result_shared_buffer = NULL;
unsigned int rpc_result_shared_buffer_size = 0;
unsigned int rpc_result_shared_buffer_phys_addr = 0;

/************************************************************
* Global Variable Definitions                               *
************************************************************/

/* received message buffer */
char rpc_rmessage_buffer[RPC_BUFSZ];

/************************************************************
* Global Function Definitions                               *
************************************************************/

/* FIXME move this function to common files */
/* FIXME find a better way of initializing this table */
/* Initializes the type size table used for signatures, if not already initialized */
void rpc_init_table()
{
    int i;
    
    if(rpc_initialized)
        return;    
    
    /* first, initialize all entries to invalid  */
    for(i = 0; i < 256; i++) signature_type_size[i] = SIG_INVALID_ENTRY;
    
    /* manually set allowed values */
    signature_type_size['v']    =   0;  /* void, 0 bytes    */
    signature_type_size['c']    =   1;  /* 8-bit number, 1 byte    */
    signature_type_size['s']    =   2;  /* 16-bit number, 2 bytes   */
    signature_type_size['i']    =   4;  /* 32-bit number, 4 bytes   */
    signature_type_size['f']    =   4;  /* 32-bit floating point number, 4 bytes   */
    signature_type_size['d']    =   8;  /* 64-bit number, 8 bytes   */
    signature_type_size['@']    =   4;  /* 32-bit pointer with address translation, 4 bytes */
    signature_type_size['a']    =   4;  /* 32-bit pointer without address translation, 4 bytes */
    signature_type_size['$']    =   4;  /* 32-bit pointer, perform manual copy into result buffer, 4 bytes */
    signature_type_size['#']    =   4;  /* 32-bit pointer, perform manual copy into result buffer and then free result buffer, 4 bytes */
    signature_type_size['>']    =   4;  /* 32-bit pointer, stdio printf-style variadic args buffer, 4 bytes */
    signature_type_size['<']    =   4;  /* 32-bit pointer, stdio scanf-style variadic args buffer, 4 bytes */   
    
    rpc_initialized = 1;
}

/* Allocate the buffers used for data exchange with the RPC stubs */
void rpc_allocate_server_buffers()
{
    int i;
    if(!buffers_allocated)
    {
        stub_param_buffer = (void**) malloc(sizeof(void*) * RPC_MAX_PARAMS);
        for(i = 0; i < RPC_MAX_PARAMS; i++)
            stub_param_buffer[i] = malloc(RPC_MAX_PARAM_LEN);
            
        stub_result_buffer = malloc(RPC_RESPSZ);
        
        function_name = (char *) malloc(RPC_MAX_FNAME_LEN);
        function_sign = (char *) malloc(RPC_MAX_FSIGN_LEN);
        
        rpc_result_shared_buffer = C6RUN_MEM_malloc(RPC_RESULT_SHAREDBUF_INITIAL_LEN);
        rpc_result_shared_buffer_size = RPC_RESULT_SHAREDBUF_INITIAL_LEN;
        rpc_result_shared_buffer_phys_addr = (unsigned int ) C6RUN_MEM_virt2Phys(rpc_result_shared_buffer);
        
        rpc_procread_buf_base = (char*) malloc(RPC_PROCREAD_BUFSIZE);
        rpc_procread_buf = rpc_procread_buf_base;
        
        buffers_allocated = 1;
    }
}

/* Free the buffers used for data exchange with the RPC stubs */
void rpc_free_server_buffers()
{
    int i;
    if(buffers_allocated)
    {
        for(i = 0; i < RPC_MAX_PARAMS; i++)
            free(stub_param_buffer[i]);

        free(stub_param_buffer);
        stub_param_buffer = NULL;
        
        free(stub_result_buffer);
        stub_result_buffer = NULL;
        
        free(function_name);
        function_name = NULL;
        
        free(function_sign);
        function_sign = NULL;
        
        C6RUN_MEM_free(rpc_result_shared_buffer);
        rpc_result_shared_buffer = NULL;
        rpc_result_shared_buffer_size = 0;
        rpc_result_shared_buffer_phys_addr = 0;
        
        free(rpc_procread_buf_base);
        rpc_procread_buf = rpc_procread_buf_base = NULL;
        
        buffers_allocated = 0;
    }
}


/* Unpack the given RPC message structure: extract the function name, signature and parameters */
void rpc_unmarshal()
{
/*
    This is how the message buffer is assumed to be structured:
    
         4      NameLen         4          SignatureLen          
    +----------+--------+----------------+-------------+----------+---+
    |  NameLen |  Name  |  SignatureLen  |  Signature  |  Params  | 0 |
    +----------+--------+----------------+-------------+----------+---+
    
*/
    unsigned int name_len = 0, sign_len = 0, i, current_param_size, virt_addr, phys_addr, fmt_str;
    char *p = rpc_rmessage_buffer;
    
    /* reset the position of the PROC_read buffer for the fresh call */
    #if (0)
    rpc_reset_procread_buf();
    #endif
    
    /* FIXME maybe introduce and read message header (for version/capabilities checking?)  */

#ifdef  VERBOSE
    printf("Saving received RPC message contents to rpc_recvd_msg.dat\n");
    FILE *debug_file = fopen("rpc_recvd_msg.dat","wb");
    fwrite(p, RPC_BUFSZ, 1, debug_file);
    fclose(debug_file);
#endif    
    
    rpc_unmarshalling_done = 0;
    
    /* extract function name */
    memcpy(&name_len, p, sizeof(unsigned int));
    p += sizeof(unsigned int);
    memcpy(function_name, p, name_len);
    function_name[name_len] = 0;
    p += name_len;
    
#ifdef  VERBOSE
    printf("Extracted function name: %s\n", function_name);
#endif
        
    /* extract the function signature */
    memcpy(&sign_len, p, sizeof(unsigned int));
    p += sizeof(unsigned int);
    memcpy(function_sign, p, sign_len);
    function_sign[sign_len] = 0;
    p += sign_len;

#ifdef  VERBOSE
    printf("Extracted function signature: %s\n", function_sign);
#endif
    
    for(i = 1; i < sign_len; i++)   /* param index starts from 1, first symbol is return type */
    {
        current_param_size = signature_type_size[(int)function_sign[i]];
        
        #ifdef  VERBOSE
            printf("Processing signature character %c with size %d\n", function_sign[i], current_param_size);
        #endif
        
        /* check for invalid signatures */
        if(current_param_size == SIG_INVALID_ENTRY)
        {
            printf("RPC unmarshalling error: invalid signature character at pos %d : %c \n", i, function_sign[i]);
            return;
        }
        
        if(function_sign[i] == '@')
        {
            /* buffer parameters with physical address, call handler */
            memcpy(&phys_addr, p, sizeof(unsigned int));
            #if (0)
            virt_addr = rpc_handle_physical_address(phys_addr);
            #else
            virt_addr = (unsigned int)C6RUN_MEM_phys2Virt((void *)phys_addr);
            #endif
            memcpy(stub_param_buffer[i-1], &virt_addr, current_param_size);
        }
        else if(function_sign[i] == '>')
        {
            /* variadic printf parameter block - go though it and do address translations where necessary */
            memcpy(&phys_addr, p, sizeof(unsigned int));
            #if (0)
            virt_addr = rpc_handle_physical_address(phys_addr);
            #else
            virt_addr = (unsigned int)C6RUN_MEM_phys2Virt((void *)phys_addr);
            #endif
            memcpy(stub_param_buffer[i-1], &virt_addr, current_param_size);
            memcpy(&fmt_str, stub_param_buffer[i-2], sizeof(unsigned int));
            rpc_process_printf_variadic_args((char*)fmt_str, (char*)virt_addr);
        }
        else if(function_sign[i] == '<')
        {
            /* variadic scanf parameter block - go though it and do address translations for all */
            memcpy(&phys_addr, p, sizeof(unsigned int));
            #if (0)
            virt_addr = rpc_handle_physical_address(phys_addr);
            #else
            virt_addr = (unsigned int)C6RUN_MEM_phys2Virt((void *)phys_addr);
            #endif
            memcpy(stub_param_buffer[i-1], &virt_addr, current_param_size);
            memcpy(&fmt_str, stub_param_buffer[i-2], sizeof(unsigned int));
            rpc_process_scanf_variadic_args((char*)fmt_str, (char*)virt_addr);
        }        
        else
        {
            /* copy current parameter and advance the buffer*/
            memcpy(stub_param_buffer[i-1], p, current_param_size);
        }

        p += current_param_size;

    }
    
    /* we should have arrived at the null message terminator, verify this */
    if(p[0] != 0)
    {
        printf("RPC unmarshalling error: null terminator expected, aborting \n");
        return;
    }
    
    rpc_unmarshalling_done = 1;
}

/* Loads the RPC stubs library */
void rpc_load_lib()
{
    if(!rpc_lib_handle)
    {
        #ifdef  VERBOSE
            printf("rpc_load_lib trying to load %s\n", RPC_STUBS_LIB_PATH);
        #endif    
        rpc_lib_handle = dlopen(RPC_STUBS_LIB_PATH, RTLD_LAZY);
        if (!rpc_lib_handle) 
        {
            printf("Failed to open RPC stub library: %s\n", dlerror());
            return;
        }
    }
}

/* Unloads the RPC stubs library */
void rpc_unload_lib()
{
    if(rpc_lib_handle)
    {
        dlclose(rpc_lib_handle);
        rpc_lib_handle = NULL;
    }
}

/* Locates the RPC stub according to the unmarshalled info */
void rpc_locate_stub()
{   
    char *error_msg = NULL;
    if(rpc_lib_handle)
    {
        rpc_stub_located = 0;
        
        #ifdef  VERBOSE
            printf("rpc_locate_stub is trying to locate %s\n", function_name);
        #endif
        
        rpc_stub = dlsym(rpc_lib_handle, function_name);
        
        if ((error_msg = dlerror()) != NULL)  
        {
            printf("Failed to locate symbol for RPC stub %s: %s\n", function_name, error_msg);
            return;
        }

        #ifdef  VERBOSE
            printf("rpc_locate_stub has located the stub at %x\n", (unsigned int)rpc_stub);
        #endif
       
        rpc_stub_located = 1;
    }
    else
        printf("rpc_locate_stub: RPC library was not loaded \n");
}

/* Executes the located RPC stub with the given parameters */
void rpc_execute_stub()
{
    if(rpc_stub_located && rpc_unmarshalling_done)
    {
        memset(stub_result_buffer, 0xFF, RPC_MAX_PARAM_LEN);

        VERBOSE_PRINT2("rpc_execute_stub: param buffer at %x, result buffer at %x \n", (uint32_t)stub_param_buffer, (uint32_t)stub_result_buffer);
        (*rpc_stub)(stub_param_buffer, stub_result_buffer);
#ifdef  VERBOSE
        printf("Saving RPC result buffer to rpc_result.dat\n");
        FILE *result_output = fopen("rpc_result.dat","wb");
        fwrite(stub_result_buffer, RPC_MAX_PARAM_LEN, 1, result_output);
        fclose(result_output);
#endif
    }
    else
        printf("rpc_execute_stub: RPC stub was not located\n");
}

/* Retrieves the stub result buffer */
void *rpc_get_result_buffer()
{
    if(!rpc_stub_located || !rpc_unmarshalling_done)
        *(int*)stub_result_buffer = RPC_UH_OH;

    return stub_result_buffer;
}

/* Does address-translation and shared-buffer copying on the result, if needed */
void rpc_process_result()
{
    unsigned int virt_addr = 0, phys_addr = 0, size = 0, sign_len = 0, i = 0, ptr = 0;
       
    if(rpc_stub_located && rpc_unmarshalling_done)
    {
#ifdef VERBOSE
        printf("Return type signature is %c, checking if postprocessing is needed... \n", function_sign[0]);
#endif         
        switch(function_sign[0])
        {
            case '@':   // direct address translation is applicable, use RPC virt->phys mapping
                // get virtual address, first 4 bytes of result buffer
                memcpy(&virt_addr, stub_result_buffer, sizeof(unsigned int));
                // use RPC mapping
                phys_addr = rpc_translate_address(virt_addr, RPC_VIRT_TO_PHYS);
#ifdef VERBOSE
                printf("Translating %x -> %x \n", virt_addr, phys_addr);
#endif                
                // write modified result back to result buffer
                memcpy(stub_result_buffer, &phys_addr, sizeof(unsigned int));
                break;
                
            case '$':   // copy returned buffer into the shared buffer
                // get virtual address, first 4 bytes of result buffer
                memcpy(&virt_addr, stub_result_buffer, sizeof(unsigned int));
                // get copy size, second 4 bytes of result buffer
                memcpy(&size, (void*)((unsigned int)stub_result_buffer + sizeof(unsigned int)), sizeof(unsigned int));
                // check the current size of shared buffer and grow it if needed
                if(rpc_result_shared_buffer_size < size)
                {
                    free(rpc_result_shared_buffer);
                    rpc_result_shared_buffer = C6RUN_MEM_malloc(size);
                    rpc_result_shared_buffer_size = size;
                    rpc_result_shared_buffer_phys_addr = (unsigned int) C6RUN_MEM_virt2Phys(rpc_result_shared_buffer);
                }
                // copy returned buffer into shared buffer
                memcpy(rpc_result_shared_buffer, (void*) virt_addr, size);
#ifdef VERBOSE
                printf("Return type signature is $, copying %u bytes and returning shared result buffer %x \n", size, rpc_result_shared_buffer_phys_addr);
#endif                  
                // write physical address of shared result buffer back to result buffer
                memcpy(stub_result_buffer, &rpc_result_shared_buffer_phys_addr, sizeof(unsigned int));                
                break;
                
            case '#':   // copy returned buffer into the result, then free returned buffer
                // get virtual address, first 4 bytes of result buffer
                memcpy(&virt_addr, stub_result_buffer, sizeof(unsigned int));
                // get copy size, second 4 bytes of result buffer
                memcpy(&size, (void*)((unsigned int)stub_result_buffer + sizeof(unsigned int)), sizeof(unsigned int));
#ifdef VERBOSE
                printf("Return type signature is #, copying %u bytes into the result %x \n", size, rpc_result_shared_buffer_phys_addr);
#endif                                  
                // FIXME what to do if the result buffer isn't big enough?
                // copy returned buffer into result
                memcpy(stub_result_buffer, (void*) virt_addr, size);
                // free returned buffer
                free((void*) virt_addr);             
                break;                
        }

        /* ARM cache is enabled, perform writebacks before returning to DSP to ensure coherency */
        sign_len = strlen(function_sign);
        for( i = 0; i < sign_len; i++)
        {
            if(function_sign[i] == '@' || function_sign[i] == '$' || function_sign[i] == '#')
            {
                /* pointer/buffer parameter/return type, writeback is needed */
                if(i == 0)
                    /* process for return type */
                    memcpy(&ptr, stub_result_buffer, sizeof(unsigned int));
                else
                    /* process for parameter */
                    memcpy(&ptr, stub_param_buffer[i-1], sizeof(unsigned int));
                /* the address translation function handles the writebacks */
                rpc_translate_address(ptr, RPC_VIRT_TO_PHYS);
            }
        }
    }
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/

/***********************************************************
* End file                                                 *
***********************************************************/
