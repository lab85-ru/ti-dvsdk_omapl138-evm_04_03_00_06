/*
 * rpc_server.h
 */

/************************************************************************
*  Header file for GPP-side RPC definitions                             *
************************************************************************/

#include "rpc_common.h"

#ifndef _RPC_MEMORY_H_
#define _RPC_MEMORY_H_


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/ 

/* defines related to the PROC_read buffer */
#define RPC_PROCREAD_MAXPARAM               64
#define RPC_PROCREAD_BUFSIZE                RPC_PROCREAD_MAXPARAM * RPC_MAX_PARAMS


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

/* GPP-side buffer for copying DSP data into using PROC_read */
extern char *rpc_procread_buf_base;
extern char *rpc_procread_buf;

/***********************************************************
* Global Function Declarations                             *
***********************************************************/

/* Functions related to RPC memory tasks */
#if (0)
extern void rpc_reset_procread_buf();
extern unsigned int rpc_handle_physical_address(unsigned int phys_addr);
extern void rpc_add_mapping(void *virtual_address, void * physical_address, unsigned int size);
extern void rpc_remove_mapping(unsigned int physical_address);
#endif
extern unsigned int rpc_translate_address(unsigned int src_address, unsigned char translation_type);



/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_RPC_SERVER_H_
