/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/***********************************************************************
* The RTDX Target Library Event User Interface
************************************************************************/
#ifndef __RTDX_EVT_H
#define __RTDX_EVT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>             /* NULL, size_t                         */
#include <RTDX_access.h>        /* RTDX_CODE, RTDX_DATA                 */

/*------------------------------------------------------------------------
* Data Types
*
* Events are represented by statically declared structures.  
* There are classes of events.  
* Each event is a member of one or more classes.  
* The class(s) in which an event belongs is represented by a bit set in 
* the event object.
*
* There is a maximum of 16 different possible events.
* RTDX reserves (the top) two of them for its own purposes.
*-----------------------------------------------------------------------*/

/* --- Event Type --- */
typedef unsigned RTDX_e_class_set;  /* event class bit mask             */

typedef struct
{   /* private */
    RTDX_e_class_set e_class;   /* mask class event belongs to          */
} RTDX_event;

/* Predefined Event Classes                                             */
#define RTDX_ALL_EVENTS   0xFFFF        /* defines all possible events  */
#define RTDX_MAX_RT       0x8000        /* Maximum Runtime Reporting    */
#define RTDX_MIN_RT       0x4000        /* Minimum Runtime Reporting    */

/* Following variable contains all currently enabled event classes              */
extern RTDX_DATA RTDX_e_class_set RTDX_Enabled_Event_Classes;

/*************************************************************************
* Procedure:
*   RTDX_Enable_Event_Class
*   RTDX_Disable_Event_Class
* Parameters:
*   eclass  set of event classes to be enabled/disabled
*
* Initially, all event classes except the predefined ones are enabled.
* A call to the enable/disable routine causes the specified event class
* to be enabled/disabled respecively.  Normally - user code will not call
* these functions. Enabling/Disabling is typically done from the debugger.
*************************************************************************/
extern RTDX_CODE void RTDX_Enable_Event_Class( RTDX_e_class_set eclass );
extern RTDX_CODE void RTDX_Disable_Event_Class( RTDX_e_class_set eclass );

/* Define macros for use where possible */
#define RTDX_Enable_Event_Class(eclass) \
                        (       RTDX_Enabled_Event_Classes |= (eclass))

#define RTDX_Disable_Event_Class(eclass) \
                        ( RTDX_Enabled_Event_Classes &= ~(eclass))

/*************************************************************************
* Procedure:
*   RTDX_Log_Event
*   RTDX_Log_Event_Data
* Parameters:
*   e       Event to be logged
*   buffer  a pointer to a buffer of related data to be recorded
*   bsize   the size of the buffer in address units
* Returns:
*   int     Success ( non-zero ) / Fail (0)
*
* If the specified event blongs to an enabled class, a call to this
* routine will cause the event and any related arguments to be recorded.
* If the event is not enabled - the call is a NO-OP.
*
* The call to Log_Event will record that the event occurred.
* Log_Event_Data records the events occurence and logs associated data.
* The Log_Event function is typically used as a macro.
*
* Note: This function will return SUCCESS if the channel is NOT enabled.
* This is so that a user application which uses the return status to
* indicate a Data_Write as successful will not block if channel is
* disabled from the debugger.
*************************************************************************/
extern RTDX_CODE int RTDX_Log_Event( RTDX_event *e );
#define RTDX_Log_Event( e )     RTDX_Log_Event_Data(e,NULL,0)

extern RTDX_CODE int RTDX_Log_Event_Data( 
 RTDX_event *e,                 /* event to write to                    */
 void *buffer,                  /* address of data to write             */
 size_t bsize);                 /* number of bytes to be written        */

#ifdef __cplusplus
}
#endif

#endif  /* __RTDX_EVT_H */
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

