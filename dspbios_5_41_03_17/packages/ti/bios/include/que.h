/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== que.h ========
 *
 */

#ifndef QUE_
#define QUE_

#include <fxn.h>
#include <mem.h>

/*
 *
 *
 * QUEUEs
 * ======
 * 
 *      Queues are doubly linked with dummy node to eliminate special
 *      cases for speed.
 *          _______        _______        _______      _______
 *  ,----->|_______|----->|_______|----->|_______|--->|_______|--//---,
 *  | ,----|_______|<-----|_______|<-----|_______|<---|_______|<-//-, |
 *  | |    prev           queue          elem         next          | |
 *  | |_____________________________________________________________| |
 *  |_________________________________________________________________|
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QUE_Elem {
    struct QUE_Elem *next;
    struct QUE_Elem *prev;
} QUE_Elem, *QUE_Handle;

typedef struct QUE_Elem QUE_Obj;

typedef struct QUE_Attrs {
    Int dummy;
} QUE_Attrs;

extern QUE_Attrs QUE_ATTRS;

/*
 *  ======== QUE_create ========
 */
extern QUE_Handle QUE_create(QUE_Attrs *attrs);

/*
 *  ======== QUE_delete ========
 */
#define QUE_delete(queue)       MEM_freeJ(0, (queue), sizeof(QUE_Obj))

/*
 *  ======== QUE_dequeue ========
 *
 *  get elem from front of "queue".
 *  This operation is *NOT* atomic.  External synchronization must
 *  be used to protect this queue from simultaneous access by interrupts
 *  or other tasks. 
 *
 *               _______        _______        _______      _______
 *  Before:     |_______|----->|_______|----->|_______|--->|_______|--->
 *              |_______|<-----|_______|<-----|_______|<---|_______|<---
 *               prev           queue          elem         next
 *
 *
 *               _______        _______        _______
 *  After:      |_______|----->|___*___|----->|_______|--->
 *              |_______|<-----|_______|<-----|___*___|<---
 *               prev           queue          next
 *               _______
 *      elem -->|___x___|       * = modified
 *              |___x___|       x = undefined
 *
 */
static inline Ptr QUE_dequeue(QUE_Handle queue)
{
    QUE_Elem *elem = queue->next;
    QUE_Elem *next = elem->next;

    queue->next = next;
    next->prev = queue;

    return (elem);
}

/*
 *  ======== QUE_empty ========
 */
#define QUE_empty(queue)        ((queue)->next == (queue))

/*
 *  ======== QUE_enqueue ========
 *
 *  put "elem" at end of "queue".
 *  This operation is *NOT* atomic.  External synchronization must
 *  be used to protect this queue from simultaneous access by interrupts
 *  or other tasks. 
 *
 *               _______        _______        _______
 *  Before:     |_______|----->|_______|----->|_______|--->
 *              |_______|<-----|_______|<-----|_______|<---
 *               prev           queue          next
 *               _______
 *      elem -->|___x___|       * = modified
 *              |___x___|       x = undefined
 *
 *               _______        _______        _______      _______
 *  After:      |___*___|----->|___*___|----->|_______|--->|_______|--->
 *              |_______|<-----|___*___|<-----|___*___|<---|_______|<---
 *               prev           elem          queue         next
 *
 */
static inline Void QUE_enqueue(QUE_Handle queue, Ptr elem)
{
    QUE_Elem *prev = queue->prev;

    ((QUE_Elem *)elem)->next = queue;
    ((QUE_Elem *)elem)->prev = prev;
    prev->next = (QUE_Elem *)elem;
    queue->prev = (QUE_Elem *)elem;
}

/*
 *  ======== QUE_get ========
 *  disable interrupts and returns the first element in the queue.
 */
extern Ptr QUE_get(QUE_Handle queue);

/*
 *  ======== QUE_head ========
 */
#define QUE_head(queue)         ((Ptr)((queue)->next))

/*
 *  ======== QUE_init ========
 */
#define QUE_init        FXN_F_nop

/*
 *  ======== QUE_insert ========
 */
#define QUE_insert(qElem, elem)         QUE_enqueue((QUE_Handle)qElem, elem)

/*
 *  ======== QUE_new ========
 */
#define QUE_new(elem)           (elem)->next = (elem)->prev = (elem)

/*
 *  ======== QUE_next ========
 */
#define QUE_next(elem)          ((Ptr)((QUE_Elem *)(elem))->next)

/*
 *  ======== QUE_prev ========
 */
#define QUE_prev(elem)          ((Ptr)((QUE_Elem *)(elem))->prev)

/*
 *  ======== QUE_print ========
 */
extern Void QUE_print(QUE_Handle queue);

/*
 *  ======== QUE_put ========
 *  Disable interrupts and put "elem" at end of "queue".  
 */
extern Void QUE_put(QUE_Handle queue, Ptr elem);

/*
 *  ======== QUE_remove ========
 */
#define QUE_remove(elem) {\
    ((QUE_Elem *)elem)->prev->next = ((QUE_Elem *)elem)->next;  \
    ((QUE_Elem *)elem)->next->prev = ((QUE_Elem *)elem)->prev;  \
    }

#ifdef _BIOSROM_

extern Ptr (*QUE_getPtr)(QUE_Handle queue);
extern Void (*QUE_putPtr)(QUE_Handle queue, Ptr elem);

#define QUE_getJ(queue)         (QUE_getPtr)(queue)
#define QUE_putJ(queue, elem)   (QUE_putPtr)(queue, elem)

#else

#define QUE_getJ(queue)         QUE_get(queue)
#define QUE_putJ(queue, elem)   QUE_put(queue, elem)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* QUE_*/
