/*
 * ata.h
 *
 * ATA Interface header file
 * This file defines the various data structures to be used by various ATA
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 *  \author     Swaminathan S.
 *  \version    10.0
 */

#ifndef _ATA_H
#define _ATA_H

#include "tistdtypes.h"
//#include "pal_os.h"
#include <sem.h>
#include <psp_ata_med.h>
#include <psp_atapi_med.h>
#ifdef BIOS_PWRM_ENABLE
#include "psp_ata_pwrm.h"
#endif

/**
 * \defgroup ATA_ATAPI ATA/ATAPI Interface Definitions
 *
 * All ATA/ATAPI Driver uses these definitions for interacting
 * with the ATA/ATAPI device.
 *
 */
/*@{*/

/**
 * \name ATA/ATAPI Driver Config params
 *
 * The following list of config params are used to alter the support capabilities
 * of the ATA/ATAPI Driver.
 *
 */
/*@{*/

#define ATA_MAX_IDE_INT   		(1u)
/**<  Maximum Number of IDE controller Interfaces (ATA Bus) Supported */
#define ATA_MAX_INT_DEV		(2u)
/**<  Maximum Number of ATA/ATAPI Devices per IDE controller Interfaces (ATA Bus)
 *    Supported.  Should be 2 */
#define ATA_MAX_DEV_QUEUE 	(10u)
/**<  Maximum Number pending request per ATA/ATAPI Device */
#define IDE_MAX_DMA_DISC		(0x100u)
/**< Maximum no. of DMA discriptors per ATA request.  To be incremented in
 *   "MULTIPLES of 4" for ensuring that the DMA Descriptors occupty a complete cache
 *   line.  This will ensure that on a Discpritor Clean the complete Descriptor
 *   cache line is flushed to memory without flusing other data as the decriptors
 *   occupy the complete cache line.
 */

/*@}*/
#define IDE_MAX_DMA_DISC_TRANS_SZ	(0x10000u)
/**< Max Transfer size per DMA descriptor */

/**
 * \brief ATA/ATAPI Driver Reset Definitions
 *
 * Device Reset Definitions.
 *
 */

/* Define ATA Reset */
#define ATA_DEV_RESET		(1u)  /**< Reset the IDE Interface (ATA Bus)    */
#define ATA_DEV_UNRESET	(2u)  /**< Un-Reset the IDE Interface (ATA Bus) */


#define	ATA_IDE_PORT_PM_DOWN	(1u) /**< ATA IDE port power down */
#define	ATA_IDE_PORT_PM_UP		(2u) /**< ATA IDE port powered */


/**
 * \brief ATA/ATAPI Device Command Execution Mode Definitions.
 *
 * List of Device Command Execution Mode Definitions for the ATA/ATAPI Device.
 */
typedef enum {
  READ_PIO = 0,  /**< Use PIO READ for this Device Request  */
  WRITE_PIO,     /**< Use PIO WRITE for this Device Request */
  READ_DMA,      /**< Use DMA READ for this Device Request  */
  WRITE_DMA,     /**< Use DMA WRITE for this Device Request */
  TASKFILE,      /**< No interrupt ack for this Device Request */
  CONTROL        /**< This is a Control request */
} ATA_Req_Mode_t;

/**
 * \brief ATA/ATAPI Device Probe State Definitions.
 *
 * List of Device Probe State Definitions for the ATA/ATAPI Device.
 */
typedef enum {
  ATA_DEV_INIT = 0, /**<  Device is in Init State                    */
  ATA_DEV_NAVAIL,   /**<  Device is not Available                    */
  ATA_DEV_AVAIL,    /**<  Device is Available                        */
  ATA_DEV_REG       /**<  Device is registered by Media Media driver */
} ATA_Dev_Prb_t;

typedef enum {
	ATA_DEV_IO_RESUME = 0,	/**< Device is in Operational state */
	ATA_DEV_IO_SUSPEND		/**< Device is in Suspend state */
}ATA_Dev_State;

/**
 * \brief IDE Controller Interface (ATA Bus) State Definitions.
 *
 * List of IDE Controller Interface (ATA Bus) State Definitions.
 */
typedef enum {
  ATA_IDE_RESET = 0,  /**<  IDE Interface is in Reset  */
  ATA_IDE_BUSY,       /**<  IDE Interface is Busy      */
  ATA_IDE_TO,         /**<  IDE Interface is Timed Out */
  ATA_IDE_FREE        /**<  IDE Interface is Free      */
} ATA_IDE_State_t;

struct Queue_s {
       struct Queue_s *next,*prev;
};
typedef struct Queue_s Queue_t;

/**
 * \brief ATA/ATAPI Device Capabilities
 *
 * This defines the structure for storage of ATA/ATAPI device capabilities.
 */
typedef struct {
  Uint32   MaxLba;        /**< Max User accessible sector cnt */
  Uint16   Cyl;           /**< Number of Cylinders */
  Uint16   Head;          /**< Number of Heads     */
  Uint16   SectperTrack;   /**< Sectors Per Track   */
  Uint16   PioCycle;      /**< PIO Cycle Time       */
  Uint16   PioCycleIordy; /**< PIO Cycle Time IORDY mode */
  Uint16   DmaCycle;      /**< DMA Cycle Time       */
  Uint16   QueueDepth;    /**< Depth of the device request queue */
  Uint16   Pio;           /**< PIO Modes supported  */
  Uint16   Dma;           /**< DMA Modes supported  */
  Uint16   Udma;          /**< UDMA Modes Supported */
  Uint16   SectCnt;       /**< Max Sector Count transferrable on READ/WRITE Multiple */
  Bool     IsAta;         /**< Device is ATA/ATAPI */
  Bool     IsDrRem;       /**< Removable drive ? */
  Bool     IsMedRem;      /**< Removable Media ? */
  Bool     IsMedWp;       /**< Media Write Protected */
  Bool     IsNoMed;       /**< Media Present */
  Bool     RemMedStatus;  /**< Is Removable Media Status Notification supported */
  Bool     IsPowerMan;    /**< Power Management supported */
  Bool     IsDrqInt;      /**< DRQ Assertion Raises interrupt (in ATAPI)?? */
  Uint16   SectSize;      /**< Sector size                                 */
  Uint8    AtapiCmdSize;  /**< Packet command size 12/16                   */
  Uint8    SerialNo[20];  /**< Serial number of device : 20 ASCII characters */
  Uint8    FirmwareRev[8];/**< Firmware revision : 8 ASCII characters */
  Uint8    ModelNo[40];   /**< Model number : 40 ASCII characters */
} ATA_Dev_Cap_t;

/**
 * \brief ATA/ATAPI Device Command Completion Status Definitions
 *
 * This defines the structure for Collection of ATA/ATAPI device Command
 * completion status.
 */
typedef struct {
  Uint8  Error;         /**< Error                */
  Uint8  IntReason;     /**< Bus Release,Cmd/Data */
  Uint8  LbaLow;        /**< LBA Low              */
  Uint8  LbaMid;        /**< LBA Mid              */
  Uint8  LbaHigh;       /**< LBA High             */
  Uint8  Device;        /**< Device head          */
  Uint8  Stat;          /**< Completion status    */
  Uint32 ErrByte;       /**< Number of Bytes tranferred */
} ATA_Dev_Stat_t;

/**
 * \brief ATA/ATAPI Device Command Request handler Definitions
 *
 * On completion of an ATA/ATAPI device request the handler has to handle
 * the completion status.
 *
 * \param Handle  [IN]  Handle for the device
 * \param pStat   [IN]  Command completion Status
 */
typedef Bool (*ATA_ReqHndlr_t) (void const * Handle, ATA_Dev_Stat_t const *pStat);

/**
 * \brief ATA/ATAPI Device Request Definitions
 *
 * This defines the structure for submition of ATA/ATAPI commands from the
 * ATA/ATAPI interface driver to IDE Controller Driver.
 */
typedef struct ATA_Dev_Req {
 // PAL_OsListNodeHeader Link;          /**< links to other queued requests   */
  Queue_t	  link;
  QUE_Elem	  Link;
  Uint8		  PortNum;			  	  /**< Port number to which the device is connected */
  Uint8		  PMP_port;			      /**< PMP Portnumber */
  Uint8		  CmdTag;				  /**< Cmd tag */
  Uint8		  CmdReady;		  		  /**< Cmd is formed in cmdtable & ready */
  ATAPI_Dev_Req_t *pPktCmd;			  /**< Packet command */
  ATA_Dev_Cmd_t   Command;            /**< ATA Command Taskfile             */
  Ptr             Buffer;             /**< Data Buffer                      */
  Uint32          TransferCount;      /**< Total Number of bytes            */
  Uint32          BlockCount;         /**< Number of bytes per block        */
                                      /**< TransferCount = N * BlockCount   */
  Ptr             ExtReqInfo;         /**< Request Information for request  */
                                      /**< Sectors > ATA_MAX_SECT           */
  Uint32          Retry;              /**< Number of request retries        */
  ATA_Req_Mode_t  Mode;               /**< Command Mode                     */
  Ptr             MediaHandle;        /**< Handle for the Media Layer       */
  ATA_DEV_Res_t   Res;                /**< Response info                    */
  ATA_ReqHndlr_t  Handler;            /**< Request Handler                  */
  ATA_ReqHndlr_t  ToHndlr;            /**< Handler for Request Timeout      */
  Uint32          Timeout;            /**< Timeout for this request         */
  Bool            IsPktCmd;           /**< Is it a Packet command (ATAPI)   */
  Bool            IsInternal;         /**< Is this an internal request      */
  ATA_Dev_Stat_t  Status;
}ATA_Dev_Req_t;

/**
 * \brief ATA/ATAPI Device Information Definitions
 *
 * This defines the structure for storage of ATA/ATAPI device information.
 */
typedef struct {

  ATA_Dev_Prb_t        Probe;         /**< Device Availability            */
  ATA_Dev_Req_t       *pReq;          /**< Device request queue           */
//  ATA_Dev_Req_t       *pReqCache;     /**< Available request Tokens       */
  Queue_t	        pReqQueHead;          /**< Device request queue           */
  Queue_t   	     pReqCacheQueHead;     /**< Available request Tokens       */
  Uint8                AvailReq;      /**< Avaibable request counts       */
  Uint8                ActiveReq;     /**< Number of Active Requests      */
  Uint8                ReservReq;     /**< Reserved requests
                                           Avail = TotReq - (ActReq + ResReq)
                                                                          */
  ATA_Dev_Cap_t        Cap;           /**< Drive Capabilities info        */
  struct ATA_IDE_Int  *pIdeInt;       /**< Pointer to the IDE controller  */
  ATA_MED_Callback_t   MedHandler;    /**< Callback for the Media layer   */
  ATA_INT_Ops_t        DevOps;        /**< Drive Operations table         */
  Uint8                Dev;           /**< Device Number                  */
  ATA_DEV_Mode_t       OpMode;        /**< Device Operating Mode          */
  ATA_DEV_PwrMode_t    PwrMode;       /**< Power Mode of the device       */
  Uint8                Level;         /**< Operating Level                */
  Uint32               RetryCnt;      /**< Device Retry Count             */
  Bool                 IsMedStatComp; /**< Is the removable media status
                                       *   enquiry complete               */
  Uint8                OpLevel;       /**< Device Operating Level         */
  ATA_Dev_State		   DevState;	  /**< devState whether device in suspend state */
  Uint8				   port_pm_state; /**< IDE controller port Power management state */
#ifdef BIOS_PWRM_ENABLE
  Uint8				   ioSuspend;
  PWRM_Event		   pwrm_event;
#endif
} ATA_Drive_t;
typedef ATA_Drive_t*  ATA_Drive_Hndl_t;

/**
 * \brief ATA/ATAPI Driver Interrupt Handler Definition
 *
 * On completion of an ATA/ATAPI device request in interrupt mode the ATA/ATAPI
 * interrupt handler has to handle the command completion status.
 *
 * \param IntNum  [IN]  Handle for the IDE Interface
 */
typedef void  (*ATA_IDE_IntHandler_t) (Uint32 IntNum, ATA_Dev_Req_t *pReq);

/**
 * \brief IDE Controller Interface Operation's Definition
 *
 * This defines IDE controller function table to be used by ATA/ATAPI driver.
 */
typedef struct {
  Int32 (*Read)      (Ptr Handle, Ptr pBuffer, Uint32 Count);
  /**< Do a PIO Read of data from the Device using the IDE Controller Interface
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param pBuffer [IN]  Pointer to Buffer transfer data to
   *   \param Count   [IN]  Count of bytes for the transfer  */
  Int32 (*Write)     (Ptr Handle, Ptr pBuffer, Uint32 Count);
  /**< Do a PIO Write of data from the Device using the IDE Controller Interface
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param pBuffer [IN]  Pointer to Buffer transfer data from
   *   \param Count   [IN]  Count of bytes for the transfer  */
  Int32 (*SubmitReq) (Ptr Handle, ATA_Dev_Req_t *pReq);
  /**< Submit a ATA/ATAPI Device request
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param pReq    [IN]  Request pointer */
  Int32 (*CmdStatus) (Ptr Handle, ATA_Dev_Stat_t *pStatus, Bool IsAlt, int portnum);
  /**< Get the Command completion status
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param pStatus [IN]  Status pointer
   *   \param IsAlt   [IN]  Alternate status requested ?
   */
  Int32 (*RegHndlr)  (Ptr Handle, ATA_IDE_IntHandler_t Handler);
  /**< Register a ATA/ATAPI Driver Handler with IDE Controller Interface
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param Handler [IN]  ATA/ATAPI Interface Handler */
  Int32 (*SetDevMode)(Ptr Handle, Uint32 Device, Uint32 CycleTime,
                    ATA_DEV_Mode_t Mode, Uint32 Level);
  /**<
   *   \param Handle    [IN]  Handle of the IDE Controller interface
   *   \param Device    [IN]  ATA/ATAPI Device Number
   *   \param CycleTime [IN]  PIO/DMA cycle time values
   *   \param Mode      [IN]  Device operation Mode (PIO/DMA/UDMA)
   *   \param Level     [IN]  Level in that opmode  */
  Int32 (*EndReq)    (Ptr Handle);
  /**< End a DMA/UDMA request
   *   \param Handle  [IN]  Handle of the IDE Controller interface  */
  Int32 (*StartReq)  (Ptr Handle, ATA_Dev_Req_t *pReq);
  /**< Start a DMA/UDMA request
   *   \param Handle  [IN]  Handle of the IDE Controller interface  */
  Int32 (*Reset) (Ptr Handle, int ResetWidth);
  /**< Reset the IDE bus (Hardware Reset)
   *   \param Handle  [IN]  Handle of the IDE Controller interface
   *   \param ResetWidth [In] Reset High time in Milliseconds       */
  Int32 (*isIdeOnline) (Uint32 drive_num);
  /**< returns ide controller is online or offline
   *   \param drive_num  [IN]  DriveNum */
#ifdef BIOS_PWRM_ENABLE
  Int32 (*isPwrmEnabled) (Ptr Handle);
  /**< whether IDE controller support PWRM 
   *   \param Handle  [IN]  Handle of the IDE Controller interface */
  Int32 (*Is_IDE_Supports_PM)(Ptr Handle, Uint32 port_num);
  Int32 (*GetIDE_PM_state)(Ptr Handle, Uint32 port_num);
  Int32 (*SetIDE_PM_state)(Ptr Handle, Uint32 port_num, Uint32 pm_state);
#endif
}IDE_INT_Ops_t;

typedef struct IDE_INTF_Confg_t {
	Uint32			isSata;
	IDE_INT_Ops_t   *pIdeOps;   
}IDE_INTF_Confg_t;

/**
 * \brief IDE Controller Interface Information Definition
 *
 * This defines IDE controller information storage structure.
 */
typedef struct ATA_IDE_Int {
  IDE_INT_Ops_t   const *pIdeOps;           /**< IDE Controller operations */
  void            *pIntHndl;          /**< Handle for the registered controller */
  Uint32           IntNum;            /**< Interrupt Number of IDE controller */
  ATA_IDE_State_t  BusState;          /**< ATA Interface Availability */
  ATA_Drive_t     *pActiveDrive;      /**< Drive handling the current ATA Command */
  ATA_Drive_t      Drive [ATA_MAX_INT_DEV]; /**< Device Information */
  Ptr              IdeTimeout;        /**< Timeout Info for this IDE controller */
  Ptr              TimeOutData;       /**< External Timeout Data */
  Bool             IsInit;            /**< Initialized ?? */
  Bool			   IsSata;
#ifdef BIOS_PWRM_ENABLE
  Bool			    ide_pwrm_enabled;
  Bool				pwrm_registerd;
  ATA_PWRM_Info		pwrmInfo;	  /**< global ATA Pwrm Info structure */
#endif
} ATA_IDE_Int_t;


extern ATA_IDE_Int_t   Ide_Ctlr_Info [];

/**
 *  \brief ATA_SubmitReq ()       : Submit an IO request on the device.
 *  \param pDevice [IN]           : Pointer to the Drive.
 *  \param pReq   [IN]            : Request Pointer
 *
 *  \return Bool                  : True -> Submitted, False -> Queued
 ******************************************************************************/
Bool ATA_SubmitReq (ATA_Drive_t *pDevice, ATA_Dev_Req_t *pReq);
/**
 *  \brief ATA_SetDevops ()    : Set the Devops in relation to OpMode, PwMode,
 *                               Device Media status.  Runs in Interrupt Context.
 *  \param pDevice [IN]        : Pointer to Device.
 *  \return void
 ******************************************************************************/
void ATA_SetDevops (ATA_Drive_t *pDevice);
/**
 *  \brief ATA_SchedNextReq ()     : Schedule the next request on the ATA Bus.
 *  \param pIdeInt [IN]            : Pointer to the ATA Bus.
 *  \param pReq    [IN]            : Request Pointer
 *
 *  \return void
 ******************************************************************************/
void ATA_SchedNextReq (ATA_IDE_Int_t *pIdeInt, ATA_Dev_Req_t *pReq);
/**
 *  \brief ATA_AllocReq ()  : Allocate a Requrest token from ReqCache of the device.
 *                            If IsResrv is TRUE then allocate out of reserved requests.
 *  \param pDrive [IN]      : Pointer to the Drive.
 *  \param IsResrv [IN]     : Allocate out of reserved requests ??
 *
 *  \return ATA_Dev_Req_t * : Pointer to allocated request
 ******************************************************************************/
ATA_Dev_Req_t* ATA_AllocReq (ATA_Drive_t *pDrive, Bool IsResrv);
/**
 *  \brief ATA_RelReq ()    : Release a Requrest token to ReqCache of the device.
 *  \param pDrive [IN]      : Pointer to the Drive.
 *  \param pReq   [IN]      : Request Pointer
 *
 *  \return ATA_Dev_Req_t * : Next request in Queue
 ******************************************************************************/
ATA_Dev_Req_t* ATA_RelReq (ATA_Drive_t *pDrive, ATA_Dev_Req_t *pReq);
/**
 *  \brief ATA_ResrvReqBlk ()  : Reserve a block of request tokens.  Used when
 *                               breaking single Media Layer requests with (NumSect > 256)
 *                               into multiple ATA Layer requests.
 *  \param pDrive [IN]         : Pointer to the Drive.
 *  \param NumReq [IN]         : Number of Requests
 *
 *  \return Bool               : Reserve Successful ??
 ******************************************************************************/
Bool ATA_ResrvReqBlk (ATA_Drive_t *pDrive, Uint8 NumReq);
/**
 *  \brief ATA_ReadPio ()  : Read PIO handler.  Executes in Interrupt context.
 *  \param Handle [IN]     : Handle to the Device.
 *  \param pStatus [IN]    : Command status
 *  \return Bool           : True
 ******************************************************************************/
Bool ATA_ReadPio (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATA_WritePio ()  : Write PIO handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_WritePio (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATA_ReadDma ()   : Read DMA handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_ReadDma (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATA_WriteDma ()  : Write DMA handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_WriteDma (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATA_Taskfile ()  : Taskfile commmand  handler.  Executes in Interrupt context.
 *                            Misc. handler for Taskfile based request.
 *                            Ex. Power Req.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_Taskfile (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATA_TimeoutCallback ()    : Handle command timeouts on ATA bus.
 *  \param id [IN]                   : Pointer to Device.
 *
 *  \return void
 ******************************************************************************/
//void ATA_DEV_CmdTimeout  (PAL_osTimerCallbackParams const *cbParams);
void ATA_DEV_CmdTimeout  (void *cbParams);
/**
 *  \brief ATA_IDE_Register () : Register a IDE Controller driver with the ATA
 *                               Interface Driver.
 *  \param pIdeOps [IN]        : IDE Controller operations
 *  \param pHandle [IN]        : Handle to be used for future transactions.
 *  \return int                : Success or Failure
 *
 ******************************************************************************/
Int32 ATA_IDE_Register (IDE_INTF_Confg_t const *pIdeOps, void const *pHandle);

/**
 *  \brief Is_ATA_IO_Pending () : check whether any ATA_IO is pending
 *                               
 *  \param 						: None
 *  \return int                : 1 when IO is pending else 0.
 *
 ******************************************************************************/
Int32 Is_ATA_IO_Pending(void);


/**
 *  \brief ATA_getPwrmHandle () : get pwrm handle
 *                               
 *  \param 						: Handle to IDE controller
 *  \return int                : return pwrm handle
 *
 ******************************************************************************/
Ptr ATA_getPwrmHandle(Ptr Handle);

/**
 *  \brief ATA_MED_SuspendIO ()    : Suspend the IO operation on ATA media
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_ResumeIO (Ptr Handle);
/**
 *  \brief ATA_MED_SuspendIO ()    : Resume the IO operation on ATA media
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_SuspendIO (Ptr Handle);
/**
 *  \brief ATA_IDE_PowerDown ()    : power down the ATA IDE controller 
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_PowerDown (Ptr Handle, ATA_DEV_PwrMode_t Mode);
/**
 *  \brief ATA_IDE_PowerUp ()    : power up the ATA IDE controller 
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_PowerUp (Ptr Handle);

/**
 *  \brief Is_ATA_IDE_Online ()    : check controller link status
 *
 *  \param Handle [IN]         : Device Handle
 *  \param drive_num [IN]      : drive num
 *  \return int                :
 ******************************************************************************/
Int32 Is_ATA_IDE_Online (Uint32 drive_num);

/*
 * extern PRD Timer functions defintions
 */
extern void timer0_start(Uint32 timeout);
extern void timer0_stop(void);
extern void timer1_start(Uint32 timeout);
extern void timer1_stop(void);
extern void timer2_start(Uint32 timeout);
extern void timer2_stop(void);
extern void timer3_start(void);
extern void timer3_stop(void);

#define		TIMER_MODE_ONESHOT	 0
#define		TIMER_MODE_PERIODIC	 1

#define	ATA_IO_COMPLETED 		(0)
#define	ATA_IO_PROGRESS			(1) 


typedef struct  {
	SEM_Handle sem;
	Uint32 timeout;
	Char mode;
	Char start;
	Char init;
	Char id;
	void *param;
	void (*fn)(void *);
}TIMER_OBJ;

void m_delay( Uint32 msecDelay);

//#define TRUE 1
//#define FALSE 0
#endif /* _ATA_H */

