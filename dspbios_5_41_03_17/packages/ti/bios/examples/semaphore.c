/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== semaphore.c ========
 *
 *  Use a QUE queue and SEM semaphore to send messages from 
 *  multiple writer() tasks to a single reader() task.  The reader task,
 *  the three writer tasks, queues, and semaphore are created by the
 *  Configuration tool.
 *
 *  The MsgObj's are preallocated in main(), and put on the free queue.
 *  The writer tasks get free message structures from the free queue,
 *  write the message, and then put the message structure onto the message
 *  queue.
 *  This example builds on quetest.c.  The major differences are:
 *    - one reader() and multiple writer() tasks.
 *    - SEM_pend() and SEM_post() are used to synchronize access 
 *      to the message queue.
 *    - 'id' field was added to MsgObj to specify writer() task id.
 *
 *  Unlike a mailbox, a queue can hold an arbitrary number of 
 *  messages or elements.  Each message must, however, be a 
 *  structure with a QUE_Elem as its first field.
 */
 
#include <std.h>

#include <log.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>
#include <tsk.h>
#include <trc.h>

#include "semaphorecfg.h"

extern Int SEG0;

#define NUMMSGS     3    /* number of messages */
#define NUMWRITERS  3    /* number of writer tasks created with Config Tool */

typedef struct MsgObj {
    QUE_Elem    elem;        /* first field for QUE */
    Int         id;          /* writer task id */
    Char        val;         /* message value */
} MsgObj, *Msg;

Void reader();
Void writer(Arg id_arg);

QUE_Obj msgQueue;
QUE_Obj freeQueue;


/*
 *  ======== main ========
 */
Void main()
{
    LOG_printf(&trace, "semaphore example started.\n");
}

/*
 *  ======== initTask ========
 */
Void initTask()
{
    Int         i;
    MsgObj      *msg;
    Uns         mask;

    QUE_new(&msgQueue);
    QUE_new(&freeQueue);

    mask = TRC_LOGTSK | TRC_LOGSWI | TRC_STSSWI | TRC_LOGCLK;
    TRC_enable(TRC_GBLHOST | TRC_GBLTARG | mask);

    msg = (MsgObj *)MEM_alloc(SEG0, NUMMSGS * sizeof(MsgObj), 0);
    if (msg == MEM_ILLEGAL) {
        SYS_abort("Memory allocation failed!\n");
    }

    /* Put all messages on freequeue */
    for (i = 0; i < NUMMSGS; msg++, i++) {
        QUE_put(&freeQueue, msg); 
    }
}

/*
 *  ======== reader ========
 */
Void reader()
{
    Msg         msg;
    Int         i;

    /* initialize time to current time */
    TSK_settime(TSK_self());

    for (i = 0; i < NUMMSGS * NUMWRITERS; i++) {
        /*
         * Wait for semaphore to be posted by writer().
         */
        SEM_pend(&sem, SYS_FOREVER);

        /* dequeue message */
        msg = QUE_get(&msgQueue);
        
        /* print value */
        LOG_printf(&trace, "read '%c' from (%d).", msg->val, msg->id);

        /* free msg */
        QUE_put(&freeQueue, msg);

        /* get task time since made ready to run */
        TSK_deltatime(TSK_self());
    }
    LOG_printf(&trace, "reader done.");
}

/*
 *  ======== writer ========
 */
Void writer(Arg id_arg)
{
    Msg         msg;
    Int         i;
    Int id =    ArgToInt (id_arg);

    /* initialize time to current time */
    TSK_settime(TSK_self());

    for (i = 0; i < NUMMSGS; i++) {
        /*
         *  Get msg from the free queue.  Since reader is higher priority
         *  and only blocks on sem, this queue will never be empty.
         */
        if (QUE_empty(&freeQueue)) {
            SYS_abort("Empty free queue!\n");
        }
        msg = QUE_get(&freeQueue);

        /* fill in value */
        msg->id = id;
        msg->val = (i & 0xf) + 'a';

        LOG_printf(&trace, "(%d) writing '%c' ...", id, msg->val);

        /* enqueue message */
        QUE_put(&msgQueue, msg);

        /* post semaphore */
        SEM_post(&sem);

        /* what happens if you call TSK_yield() here? */
        /* TSK_yield(); */

        /* get task time for this loop */
        TSK_deltatime(TSK_self());
    }

    LOG_printf(&trace, "writer (%d) done.", id);
}
