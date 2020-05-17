/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== msgq_mqt.h ========
 *  This file is for use by transports.
 *
 */

#ifndef MSGQ_MQT_
#define MSGQ_MQT_

#include <msgq.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal constants used by both MSGQ and the MQTs. */
#define MSGQ_INVALIDPROCID   (Uint16)0xFFFF

/* The different status values for a MSGQ_Obj */ 
#define MSGQ_EMPTY    0      /* Messaging queue not being used    */
#define MSGQ_INUSE    1      /* Messaging queue being used        */

extern MSGQ_Config *MSGQ;

static inline Bool MSGQ_exist(MSGQ_Queue msgqQueue)
{ 
    return ((MSGQ->msgqQueues[(MSGQ_Id) msgqQueue].status == MSGQ_INUSE) ? 
                TRUE : FALSE);
}

/* 
 *  Use the following stub function definitions if a transport
 *  function is not implemented.
 */
#define MSGQ_MQTOPENNOTIMPL    (MSGQ_MqtOpen)SYS_zero
#define MSGQ_MQTCLOSENOTIMPL   (MSGQ_MqtClose)SYS_zero
#define MSGQ_MQTLOCATENOTIMPL  (MSGQ_MqtLocate)SYS_zero
#define MSGQ_MQTRELEASENOTIMPL (MSGQ_MqtRelease)SYS_zero
#define MSGQ_MQTPUTNOTIMPL     (MSGQ_MqtPut)SYS_zero

/* Function prototypes for transport-only MSGQ APIs */
extern Void MSGQ_sendErrorMsg(MSGQ_MqtError errorType, 
                              Uint16 mqtId, 
                              Uint16 parameter);
extern Int  MSGQ_locateLocal(String queueName, MSGQ_Queue *msgqQueue);

/* Jump table handling */
#ifdef _BIOSROM_

extern Void  (*MSGQ_sendErrorMsgPtr)(Uint16 errorType, Uint16 mqtId, Uint16 parameter);
extern Int   (*MSGQ_locateLocalPtr)(String queueName, MSGQ_Queue *msgqQueue);

#define MSGQ_sendErrorMsgJ(errorType, mqtId, parameter)                \
            (MSGQ_sendErrorMsgPtr)(errorType, mqtIdrg1, parameter)
#define MSGQ_locateLocalJ(queueName, msgqQueue)                        \
            (MSGQ_locateLocalPtr)(queueName, msgqQueue)

#else  /* _BIOSROM_ */

#define MSGQ_sendErrorMsgJ(errorType, mqtId, parameter)      \
            MSGQ_sendErrorMsg(errorType, mqtId, parameter)
#define MSGQ_locateLocalJ(queueName, msgqQueue)              \
            MSGQ_locateLocal(queueName, msgqQueue)

#endif /* _BIOSROM_ */

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* MSGQ_MQT_ */
