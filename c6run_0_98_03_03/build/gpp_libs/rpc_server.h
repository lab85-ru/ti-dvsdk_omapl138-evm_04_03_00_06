/*
 * rpc_server.h
 */

/************************************************************************
*  Header file for GPP-side RPC definitions                             *
************************************************************************/

#include "rpc_common.h"

#ifndef _RPC_SERVER_H_
#define _RPC_SERVER_H_


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/ 

/* path to RPC stubs library */
#define RPC_STUBS_LIB_PATH  "./librpcstubs.so"


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

/* received message buffer */
extern char rpc_rmessage_buffer[RPC_BUFSZ];
/* GPP-side buffer for copying DSP data into using PROC_read */
extern char *rpc_procread_buf_base;
extern char *rpc_procread_buf;

/***********************************************************
* Global Function Declarations                             *
***********************************************************/

/* Functions related to initialization of the RPC subsystem */
void rpc_init_table();
void rpc_allocate_server_buffers();
void rpc_free_server_buffers();

/* Functions related to RPC general flow */
void rpc_unmarshal();
void rpc_load_lib();
void rpc_unload_lib();
void rpc_locate_stub();
void rpc_execute_stub();
void *rpc_get_result_buffer();
void rpc_process_result();

/* Functions related to stdio variadics handling */
void rpc_process_printf_variadic_args(char *format_string, char *arg_base);
void rpc_process_scanf_variadic_args(char *format_string, char *arg_base);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_RPC_SERVER_H_
