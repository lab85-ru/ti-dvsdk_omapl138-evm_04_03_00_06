/*
 * rpc_common.h
 */

/*****************************************************************************
*  This file contains common RPC definitions used on DSP and GPP sides       *
*****************************************************************************/

#ifndef _RPC_COMMON_H_
#define _RPC_COMMON_H_

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/* Maximum number of parameters allowed in RPC */
#define RPC_MAX_PARAMS      10
/* Maximum length of the function name */
#define RPC_MAX_FNAME_LEN   20
/* Maximum length of the function signature */
#define RPC_MAX_FSIGN_LEN   RPC_MAX_PARAMS + 1
/* Maximum possible length of a parameter in bytes */
#define RPC_MAX_PARAM_LEN   8

/* Size of buffer for RPC request operations */
#define RPC_BUFSZ           RPC_MAX_FNAME_LEN + RPC_MAX_FSIGN_LEN + 8 + (RPC_MAX_PARAMS * RPC_MAX_PARAM_LEN)
/* Size of buffer for RPC response */
#define RPC_RESPSZ          64

/* Value for invalid entries in the signature table */
#define SIG_INVALID_ENTRY   0xFFFFFFFF

/* Value returned to the DSP in case something goes wrong */
#define RPC_UH_OH           0xDEADBEEF

/* Operator to get next parameter from stack for the marshaller */
/* FIXME define this operator according to calling conventions? */
#define NEXT_PARAM_OP       +

/* Message identifiers for RPC-related messages */
#define RPC_MSG_REQUEST     0x01
#define RPC_MSG_RESPONSE    0x02
#define RPC_MSG_MALLOC      0x03
#define RPC_MSG_FREE        0x04
#define RPC_MSG_TRANSLATE   0x05

/* Unpack result from buffer by casting it to desired type */
#define RPC_UNPACK_RESULT(target, type) *((type *)target)

/* Types of address translation */
#define RPC_PHYS_TO_VIRT   (1)
#define RPC_VIRT_TO_PHYS   (2)

#define RPC_RESULT_SHAREDBUF_INITIAL_LEN    500

/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif // _RPC_COMMON_H_
