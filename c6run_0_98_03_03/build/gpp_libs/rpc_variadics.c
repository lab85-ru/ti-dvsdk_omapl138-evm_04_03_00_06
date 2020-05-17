/*
 * rpc_variadics.c
*/

/************************************************************************
* Procedures for handling stdio-style variadics (printf&scanf) - scan   *
* through given arguments and perform address translations when needed  *
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

#include "debug_common.h"
#include "C6Run_mem.h"
#include "rpc_memory.h"
#include "rpc_server.h"

/*  ----------------------------------- This module's header          */
#include "rpc_variadics.h"



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

/************************************************************
* Global Variable Definitions                               *
************************************************************/

/************************************************************
* Global Function Definitions                               *
************************************************************/

/* Process variadic args given in arg_base into var_buf according to stdio *scanf variadics' format string */
void rpc_process_scanf_variadic_args(char *format_string, char *arg_base)
{
    char *p = format_string;
    char current_char = 0;
    int var_found = 0;
    unsigned int offset_delta = 0;
    unsigned int virt_addr = 0;
    unsigned int phys_addr = 0;

#ifdef VERBOSE
    printf("Processing stdio scanf variadic args, format string is %s \n, arg base is %x \n", format_string, (unsigned int)arg_base);
    FILE *dbg = fopen("rpc_scanf_varargs.dat", "wb");
    fwrite(arg_base, 80, 1, dbg);
    fclose(dbg);
#endif    

    // FIXME need number of vars checking
    while(*p != 0)
    {
        current_char = *p;
        if(var_found == 1)
        {
            // FIXME need to process size modifiers
            if(current_char == 'd' || current_char == 'i' || current_char == 'o' || current_char == 'x' || current_char == 'X' || current_char == 'c' || current_char == 'u' || current_char == 's' || current_char == 'p' || current_char == 'n' || current_char == 'f' || current_char == 'e' || current_char == 'E' || current_char == 'g' || current_char == 'G')
            {
                // pointer argument, needs address translation
                memcpy(&phys_addr, arg_base+offset_delta, sizeof(unsigned int));
                #if (0)
                virt_addr = rpc_handle_physical_address(phys_addr);
                #else
                virt_addr = (unsigned int)C6RUN_MEM_phys2Virt((void *)phys_addr);
                #endif
#ifdef VERBOSE
                printf("found integer-sized pointer argument, offset delta is %u value is %x translated value is %x \n", offset_delta, phys_addr, virt_addr);
#endif                    
                memcpy(arg_base+offset_delta, &virt_addr, sizeof(unsigned int));
                offset_delta += sizeof(unsigned int);
                var_found = 0;
            }
        }
        
        if(current_char == '%')
        {
            if(var_found) var_found = 0;
            else var_found = 1;
        }
        
        p++;
    }
}

/* Process variadic args given in arg_base into var_buf according to stdio *printf variadics' format string */
void rpc_process_printf_variadic_args(char *format_string, char *arg_base)
{
    char *p = format_string;
    char current_char = 0;
    int var_found = 0;
    unsigned int offset_delta = 0;
    unsigned int virt_addr = 0;
    unsigned int phys_addr = 0;

#ifdef VERBOSE
    printf("Processing stdio variadic args, format string is %s \n, arg base is %x \n", format_string, (unsigned int)arg_base);
    FILE *dbg = fopen("rpc_printf_varargs.dat", "wb");
    fwrite(arg_base, 80, 1, dbg);
    fclose(dbg);
#endif    

    // FIXME need number of vars checking
    while(*p != 0)
    {
        current_char = *p;
        if(var_found == 1)
        {
            // FIXME need to process size modifiers
            if(current_char == 'd' || current_char == 'i' || current_char == 'o' || current_char == 'x' || current_char == 'X' || current_char == 'c' || current_char == 'u')
            {
#ifdef VERBOSE            
                memcpy(&phys_addr, arg_base+offset_delta, sizeof(unsigned int));
                printf("found integer-sized value argument, offset delta is %u value is %u \n", offset_delta, phys_addr);
#endif                    
                offset_delta += sizeof(int);
                var_found = 0;
            }
            else if(current_char == 's' || current_char == 'p' || current_char == 'n')
            {
                // pointer argument, needs address translation
                memcpy(&phys_addr, arg_base+offset_delta, sizeof(unsigned int));
                #if (0)
                virt_addr = rpc_handle_physical_address(phys_addr);
                #else
                virt_addr = (unsigned int)C6RUN_MEM_phys2Virt((void *)phys_addr);
                #endif
#ifdef VERBOSE
                printf("found integer-sized pointer argument, offset delta is %u value is %x translated value is %x \n", offset_delta, phys_addr, virt_addr);
#endif                    
                memcpy(arg_base+offset_delta, &virt_addr, sizeof(unsigned int));
                offset_delta += sizeof(unsigned int);
                var_found = 0;
            }
            else if(current_char == 'f' || current_char == 'e' || current_char == 'E' || current_char == 'g' || current_char == 'G')
            {
                /* FIXME print warning message, GPP/DSP alignments are incompatible */
                printf("WARNING: you should avoid using floats/doubles in rpc_*printf functions due to a known alignment problem! \n");
                /* alignment correction sometimes needed for doubles, offset should be aligned to 8 bytes */
                if(offset_delta % 8 == 4)
                    offset_delta += 4;
#ifdef VERBOSE
                {
                  double d_val;
                  memcpy(&d_val, arg_base+offset_delta, sizeof(double));
                  printf("found double-sized value argument, offset delta is %u value is %f \n", offset_delta, d_val);
                }
#endif                    
                offset_delta += sizeof(double);   
                var_found = 0;
            }
        }
        
        if(current_char == '%')
        {
            if(var_found) var_found = 0;
            else var_found = 1;
        }
        
        p++;
    }
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
