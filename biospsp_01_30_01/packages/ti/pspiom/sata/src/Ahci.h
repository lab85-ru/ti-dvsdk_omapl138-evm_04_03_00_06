/*
 * ahci.h
 *
 * advanced host controller interface Driver file
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

#include "tistdtypes.h"
#include "ahci_fis.h"
#include <ti/pspiom/sata/psp_sata.h>
#include <ti/pspiom/cslr/cslr_sata.h>

#ifdef BIOS_PWRM_ENABLE
#include "pwrm.h"
#include "pwrm6748.h"
#endif

#define		AHCI_HBA_INITIALIZED			(TRUE)
#define		AHCI_HBA_NOT_INITIALIZED		(FALSE)

/**
 * \brief FIS type definitions
 *
 * Frame Infromation structure (FIS) types
 */
#define	AHCI_H2D_FIS				0x27	/* Host to Device FIS */
#define	AHCI_D2H_FIS				0x34	/* Device to Host FIS */
#define	AHCI_DMA_ACTIVE_FIS			0x39	/* DMA Activate FIS */
#define	AHCI_DMA_SETUP_FIS			0x41	/* DMA setup FIS */
#define	AHCI_DATA_FIS				0x46	/* DATA FIS */
#define	AHCI_BIST_FIS				0x58	/* BIST FIS */
#define	AHCI_PIO_SETUP_FIS			0x5F	/* PIO Setup FIS */
#define	AHCI_SETDEVBITS_FIS			0xA1	/* SetDevice bits FIS */

#define	AHCI_PORT_OFFS				0x100 	/* Port offset from AHCI base */

/** ACHI Port Register bit definition */
/* HOST CAPabilities bits */
#define SATA_HOST_CAP_SSC			(1 << CSL_SATA_CAP_SSC_SHIFT)	/* Slumber capable */
#define SATA_HOST_CAP_CLO			(1 << CSL_SATA_CAP_SCLO_SHIFT)	/* Command List Override support */
#define SATA_HOST_CAP_SSS			(1 << CSL_SATA_CAP_SSS_SHIFT) 	/* Staggered Spin-up */
#define SATA_HOST_CAP_NCQ			(1 << CSL_SATA_CAP_SNCQ_SHIFT)	/* Native Command Queueing */
#define SATA_HOST_CAP_64			(1 << CSL_SATA_CAP_S64A_SHIFT)	/* PCI DAC (64-bit DMA) support */

#define SATA_HOST_AHCI_EN			(1 << CSL_SATA_GHC_AE_SHIFT) 	/* AHCI enabled */
#define SATA_HOST_IRQ_EN			(1 << CSL_SATA_GHC_IE_SHIFT)  	/* global IRQ enable */
#define SATA_HOST_RESET				(1 << CSL_SATA_GHC_HR_SHIFT)  	/* reset controller; self-clear */

/* Port interrupt bit defintions */
#define SATA_PORT_IRQ_COLD_PRES		(1 << CSL_SATA_P0IE_CPDE_SHIFT) /* cold presence detect */
#define SATA_PORT_IRQ_TF_ERR		(1 << CSL_SATA_P0IE_TFEE_SHIFT) /* task file error */
#define SATA_PORT_IRQ_HBUS_ERR		(1 << CSL_SATA_P0IE_HBFE_SHIFT) /* host bus fatal error */
#define SATA_PORT_IRQ_HBUS_DATA_ERR	(1 << CSL_SATA_P0IE_HBDE_SHIFT) /* host bus data error */
#define SATA_PORT_IRQ_IF_ERR		(1 << CSL_SATA_P0IE_IFE_SHIFT)	 /* interface fatal error */
#define SATA_PORT_IRQ_IF_NONFATAL	(1 << CSL_SATA_P0IE_INFE_SHIFT) /* interface non-fatal error */
#define SATA_PORT_IRQ_OVERFLOW		(1 << CSL_SATA_P0IE_OFE_SHIFT)  /* xfer exhausted available S/G */
#define SATA_PORT_IRQ_BAD_PMP		(1 << CSL_SATA_P0IE_IPME_SHIFT) /* incorrect port multiplier */
#define SATA_PORT_IRQ_PHYRDY		(1 << CSL_SATA_P0IE_PRCE_SHIFT) /* PhyRdy changed */
#define SATA_PORT_IRQ_DEV_ILCK		(1 << CSL_SATA_P0IE_DMPE_SHIFT) /* device interlock */
#define SATA_PORT_IRQ_CONNECT		(1 << CSL_SATA_P0IE_PCE_SHIFT)  /* port connect change status */
#define SATA_PORT_IRQ_DP_DONE		(1 << CSL_SATA_P0IE_DPE_SHIFT)  /* descriptor processed */
#define SATA_PORT_IRQ_UNK_FIS		(1 << CSL_SATA_P0IE_UFE_SHIFT)  /* unknown FIS rx'd */
#define SATA_PORT_IRQ_SDB_FIS		(1 << CSL_SATA_P0IE_SDBE_SHIFT) /* Set Device Bits FIS rx'd */
#define SATA_PORT_IRQ_DMAS_FIS		(1 << CSL_SATA_P0IE_DSE_SHIFT)  /* DMA Setup FIS rx'd */
#define SATA_PORT_IRQ_PIOS_FIS		(1 << CSL_SATA_P0IE_PSE_SHIFT)  /* PIO Setup FIS rx'd */
#define SATA_PORT_IRQ_D2H_REG_FIS	(1 << CSL_SATA_P0IE_DHRE_SHIFT) /* D2H Register FIS rx'd */

/* Port CMD  bit defintions */
#define AHCI_PORT_CMD_ICC_MASK      (0xf << CSL_SATA_P0CMD_ICC_SHIFT) /* i/f ICC state mask */
#define AHCI_PORT_CMD_ICC_ACTIVE    (0x1 << CSL_SATA_P0CMD_ICC_SHIFT) /* Put i/f in active state */
#define AHCI_PORT_CMD_ICC_PARTIAL   (0x2 << CSL_SATA_P0CMD_ICC_SHIFT) /* Put i/f in partial state */
#define AHCI_PORT_CMD_ICC_SLUMBER   (0x6 << CSL_SATA_P0CMD_ICC_SHIFT) /* Put i/f in slumber state */
#define AHCI_PORT_CMD_ASP           (1 << CSL_SATA_P0CMD_ASP_SHIFT)   /* Aggressive Slumber/Partial */
#define AHCI_PORT_CMD_ALPE          (1 << CSL_SATA_P0CMD_ALPE_SHIFT)  /* Aggressive Link PM enable */

#define AHCI_PORT_CMD_ATAPI         (1 << CSL_SATA_P0CMD_ATAPI_SHIFT) /* Device is ATAPI */
#define AHCI_PORT_CMD_LIST_ON       (1 << CSL_SATA_P0CMD_CR_SHIFT) 	  /* cmd list DMA engine running */
#define AHCI_PORT_CMD_FIS_ON        (1 << CSL_SATA_P0CMD_FR_SHIFT) 	  /* FIS DMA engine running */
#define AHCI_PORT_CMD_FIS_RX        (1 << CSL_SATA_P0CMD_FRE_SHIFT)   /* Enable FIS receive DMA engine */
#define AHCI_PORT_CMD_CLO           (1 << CSL_SATA_P0CMD_CLO_SHIFT)   /* Command list override */
#define AHCI_PORT_CMD_POWER_ON		(1 << CSL_SATA_P0CMD_POD_SHIFT)   /* Power on Port */
#define AHCI_PORT_CMD_SPIN_UP		(1 << CSL_SATA_P0CMD_SUD_SHIFT)   /* spin up device */
#define AHCI_PORT_CMD_ENABLE		(1 << CSL_SATA_P0CMD_ST_SHIFT)    /* Enable Port */

/* Port controls */
#define AHCI_PORT_CTRL_IPM_DIS_PARTIAL	(1 << CSL_SATA_P0SCTL_IPM_SHIFT)    /* Enable Port */
#define AHCI_PORT_CTRL_IPM_DIS_SLUMBER	(2 << CSL_SATA_P0SCTL_IPM_SHIFT)   /* spin up device */
#define AHCI_PORT_CTRL_IPM_DIS_PM		(3 << CSL_SATA_P0SCTL_IPM_SHIFT)   /* Power on Port */

/* Port control defintions */
#define AHCI_HOST_RESET               (1 << CSL_SATA_GHC_HR_SHIFT)   	/* reset controller; self-clear */
#define AHCI_HOST_IRQ_EN              (1 << CSL_SATA_GHC_IE_SHIFT)  	/* global IRQ enable */
#define AHCI_HOST_AHCI_EN             (1 << CSL_SATA_GHC_AE_SHIFT) 		/* AHCI enabled */

/* AHCI capabilities defintions */
#define AHCI_HOST_CAP_EMS             (1 << CSL_SATA_CAP_EMS_SHIFT)  	/* Enclosure Management support */
#define AHCI_HOST_CAP_SSC             (1 << CSL_SATA_CAP_SSC_SHIFT) 	/* Slumber capable */
#define AHCI_HOST_CAP_PMP             (1 << CSL_SATA_CAP_SPM_SHIFT) 	/* Port Multiplier support */
#define AHCI_HOST_CAP_CLO             (1 << CSL_SATA_CAP_SCLO_SHIFT) 	/* Command List Override support */
#define AHCI_HOST_CAP_ALPM            (1 << CSL_SATA_CAP_SALP_SHIFT) 	/* Aggressive Link PM support */
#define AHCI_HOST_CAP_SSS             (1 << CSL_SATA_CAP_SSS_SHIFT) 	/* Staggered Spin-up */
#define AHCI_HOST_CAP_SMPS            (1 << CSL_SATA_CAP_SMPS_SHIFT) 	/* Mechanical switch presence */
#define AHCI_HOST_CAP_SNTF            (1 << CSL_SATA_CAP_SSNTF_SHIFT) 	/* SNotification register */
#define AHCI_HOST_CAP_NCQ             (1 << CSL_SATA_CAP_SNCQ_SHIFT) 	/* Native Command Queueing */
#define AHCI_HOST_CAP_64              (1 << CSL_SATA_CAP_S64A_SHIFT) 	/* PCI DAC (64-bit DMA) support */

/* port interrupt status bits */
#define AHCI_PORT_IRQ_COLD_PRES       (1 << CSL_SATA_P0IS_CPDS_SHIFT) 	/* cold presence detect */
#define AHCI_PORT_IRQ_TF_ERR          (1 << CSL_SATA_P0IS_TFES_SHIFT) 	/* task file error */
#define AHCI_PORT_IRQ_HBUS_ERR        (1 << CSL_SATA_P0IS_HBFS_SHIFT) 	/* host bus fatal error */
#define AHCI_PORT_IRQ_HBUS_DATA_ERR   (1 << CSL_SATA_P0IS_HBDS_SHIFT) 	/* host bus data error */
#define AHCI_PORT_IRQ_IF_ERR          (1 << CSL_SATA_P0IS_IFS_SHIFT) 	/* interface fatal error */
#define AHCI_PORT_IRQ_IF_NONFATAL     (1 << CSL_SATA_P0IS_INFS_SHIFT) 	/* interface non-fatal error */
#define AHCI_PORT_IRQ_OVERFLOW        (1 << CSL_SATA_P0IS_OFS_SHIFT) 	/* overflow  */
#define AHCI_PORT_IRQ_BAD_PMP         (1 << CSL_SATA_P0IS_IPMS_SHIFT) 	/* incorrect port multiplier */
#define AHCI_PORT_IRQ_PHYRDY          (1 << CSL_SATA_P0IS_PRCS_SHIFT) 	/* PhyRdy changed */
#define AHCI_PORT_IRQ_DEV_ILCK        (1 << CSL_SATA_P0IS_DMPS_SHIFT) 	/* device interlock */
#define AHCI_PORT_IRQ_CONNECT         (1 << CSL_SATA_P0IS_PCS_SHIFT) 	/* port connect change status */
#define AHCI_PORT_IRQ_SG_DONE         (1 << CSL_SATA_P0IS_DPS_SHIFT) 	/* descriptor processed */
#define AHCI_PORT_IRQ_UNK_FIS         (1 << CSL_SATA_P0IS_UFS_SHIFT) 	/* rcvd unknown FIS */
#define AHCI_PORT_IRQ_SDB_FIS         (1 << CSL_SATA_P0IS_SDBS_SHIFT) 	/* rcvd Set Device Bits FIS*/
#define AHCI_PORT_IRQ_DMAS_FIS        (1 << CSL_SATA_P0IS_DSS_SHIFT) 	/* rcvd DMA Setup FIS */
#define AHCI_PORT_IRQ_PIOS_FIS        (1 << CSL_SATA_P0IS_PSS_SHIFT) 	/* rcvd PIO Setup FIS  */
#define AHCI_PORT_IRQ_D2H_REG_FIS     (1 << CSL_SATA_P0IS_DHRS_SHIFT) 	/* rcvd D2H Register FIS */

/* Port errors */
#define AHCI_PORT_IRQ_FREEZE		(AHCI_PORT_IRQ_HBUS_ERR | AHCI_PORT_IRQ_IF_ERR | AHCI_PORT_IRQ_CONNECT | AHCI_PORT_IRQ_PHYRDY | AHCI_PORT_IRQ_UNK_FIS | AHCI_PORT_IRQ_BAD_PMP)
#define AHCI_PORT_IRQ_ERROR			(AHCI_PORT_IRQ_FREEZE | AHCI_PORT_IRQ_TF_ERR | AHCI_PORT_IRQ_HBUS_DATA_ERR)
#define AHCI_DEF_PORT_IRQ			(AHCI_PORT_IRQ_ERROR | AHCI_PORT_IRQ_SG_DONE | AHCI_PORT_IRQ_SDB_FIS | AHCI_PORT_IRQ_DMAS_FIS | AHCI_PORT_IRQ_PIOS_FIS | AHCI_PORT_IRQ_D2H_REG_FIS)

/* ahci reset types */
#define	AHCI_HBA_RESET				0
#define	AHCI_PORT_RESET				1
#define	AHCI_SOFT_RESET				2

/** Cmd header options flags */
#define	AHCI_CMD_ATAPI				(1 << 5)
#define	AHCI_CMD_WRITE				(1 << 6)
#define	AHCI_CMD_PREFETCH			(1 << 7)
#define	AHCI_CMD_RESET				(1 << 8)
#define	AHCI_CMD_CLR_BUSY			(1 << 10)


/** TODO to be moved to ata.h*/
/* bits in ATA command block registers */
#define	ATA_HOB          	        (1 << 7)     /* LBA48 selector */
#define	ATA_NIEN            	    (1 << 1)     /* disable-irq flag */
#define	ATA_LBA                 	(1 << 6)     /* LBA28 selector */
#define	ATA_DEV1					(1 << 4)     /* Select Device 1 (slave) */
#define	ATA_DEVICE_OBS				(1 << 7)| (1 << 5) /* obs bits in dev reg */
#define	ATA_DEVCTL_OBS				(1 << 3)     /* obsolete bit in devctl reg */
#define	ATA_BUSY					(1 << 7)     /* BSY status bit */
#define	ATA_DRDY					(1 << 6)     /* device ready */
#define	ATA_DF						(1 << 5)     /* device fault */
#define	ATA_DRQ						(1 << 3)     /* data request i/o */
#define	ATA_ERR						(1 << 0)     /* have an error */
#define	ATA_SRST					(1 << 2)     /* software reset */
#define	ATA_ICRC					(1 << 7)     /* interface CRC error */
#define	ATA_UNC						(1 << 6)     /* uncorrectable media error */
#define	ATA_IDNF					(1 << 4)     /* ID not found */
#define	ATA_ABORTED					(1 << 2)    

/* ahci power management defines */
enum port_link_pm{
	NOT_AVAILABLE = 0,
	MIN_POWER,
	MEDIUM_POWER,
	MAX_PERFORMANCE
};

/* SATA power management state */
#define AHCI_PM_ACTIVE				(1)
#define AHCI_PM_PARTIAL				(2)
#define AHCI_PM_SLUMBER				(3)

/**
 * \brief	SATA AHCI Port Registers
 */
typedef struct {
	
	volatile Uint32	PxCLB;			/** 00: Port command List Base Address */
	volatile Uint32	PxCLBU_reserved;/** 04: Port Command List base Address upper 32 bits */
	volatile Uint32	PxFB;			/** 08: Port FIS Base Address */
	volatile Uint32	PxFBU_reserved;	/** 0c: Port FIS Base Address upper 32 bits*/
	volatile Uint32	PxIS;			/** 10: Port Interrupt Status */
	volatile Uint32	PxIE;			/** 14: Port Interrupt Enable */
	volatile Uint32	PxCMD;			/** 18: Port command and status */
	volatile Uint32	Reserved1;		/** 1C: Reserved */
	volatile Uint32	PxTFD;			/** 20: Port Task File Data */
	volatile Uint32	PxSIG;			/** 24: Port Signature */
	volatile Uint32	PxSSTS;			/** 28: Port serial ATA Status (SCR0:SStatus)*/
	volatile Uint32	PxSCTL;			/** 2C: Port Serial ATA Control (SCR2:SControl) */
	volatile Uint32	PxSERR;			/** 30: Port Serial ATA Error (SCR1:SError) */
	volatile Uint32	PxSACT;			/** 34: Port Serial ATA Active (SCR3:SActive) */
	volatile Uint32	PxCI;			/** 38: Port command Issue */
	volatile Uint32	PxSNTF;			/** 3c: Port SNotification */
	volatile Uint32	Reserved2[12];	/** 40-6C: Reserved for FIS based Switching Definition*/
	/* Vendor specific Registers*/
	volatile Uint32	PxDMACR;		/** 70: Port DMA Control */
	volatile Uint32	Reserved3;		/** 74: Reserved */
	volatile Uint32	PxPHYCR;		/** 78: Port DMA phy control */
	volatile Uint32	PxPHYSR;		/** 7c: Port DMA PHY status */
}AhciPortRegs;

/**
 * \brief AhciCmdHeader
 *
 * SATA AHCI Comamnd header
 */
typedef struct {
    
    Uint32 options;		/** DW0 : Description information */
    Uint32 prdByteCount;	/** DW1 : status indicates current byte count transfered on device write/reads */
	Uint32 cmdTblAdr;		/** DW2 : command table base address */
	Uint32 cmdTblAdr_Hi;	/** DW3 : command table address high */	
	Uint32 reserved[4];	/** DW4:DW7 reserved */
} AhciCmdHeader;

/**
 * \brief AhciPrdTable
 *
 * Physical Region Descriptor Table or PRDT entry
 */
typedef struct {
    
	Uint32 dataBaseAdr;		/** DW0 : address */
	Uint32 dataBaseAdr_Hi;	/** DW1 : address high */
	Uint32 reserved;			/** DW2 : reservered */
	Uint32 flagsDataByteCnt;	/** DW3 : flag and data byte count, maximum lenght of 4MB, bit0 is always 1 for
								even byte count */
}AhciPrdTable;


/**
* \brief AhciCmdTable
*
* Command Table Data structure
*/
typedef struct{
	
	RegFis	CmdFis;			/** 00:3F Command FIS */
	Uint32	reserved1[11];	
	Uint8	atapi_cmd[16];	/** 40:4F ATAPI Command */
	Uint32	reserved2[12];	/* 50:7F reserved */
	AhciPrdTable prdTable[SATA_CONFIG_MAX_NUM_PRD_ENTRIES];	/** 80: PRD table entries upto 65535 */
}AhciCmdTable;

/**
* \brief AhciPortData
*
* Port Data structure
*/
typedef struct {
	
	AhciCmdHeader	cmdHeader[SATA_CONFIG_MAX_NUM_AHCI_CMDSLOT];	/** pointer to command header */
	AhciRxFis 		rxFis;											/** receive fis */											
	AhciCmdTable	cmdTableBase[SATA_CONFIG_MAX_NUM_AHCI_CMDSLOT];	/** pointer to command Table */
	Uint32  			portNum;										/* Port Number */
	Uint32  			flags;											/* port flags */
	Uint8 			pmpEnable;		  	  							/* whether PMP is enable */
	Uint32  			maxNumCmdSlot;									/* maximum number of cmd allowed */
	Uint32  			speed;											/* Operating Speed of Port, 1 for 1.5Gbps, 2 for 3.0Gbps */
	void 			*ahciHost;										/* Pointer to AHCI HBA ctrlr */
	CSL_SataRegs 	*ahciRegs;										/* Pointer to GHC register */
	AhciPortRegs	*regs;											/* Pointer to port register */
	Uint32 			maxNumPrdtLen;									/** Maximum num of PRDT entries */
	Uint32 			qActCmdIssue;									/** Number of active commands */
	Uint8			ncqEnable;										/* Native Command Queue Enabled or not */
	Uint32 			intrMask;										/** interrupt mask */
	Uint32 			sActive;
	Uint32			curActCmdIssued; 								/* current active issued commands */
	Uint32    		IntrStatus;
	Uint8			linkOnline;										/* port link status */
	Uint8			cfgLinkSpeed;
	Uint8			pm_enabled;
	Uint8			pm_state;
	/* current active device io requests */
	void 			*curActDevReq[SATA_CONFIG_MAX_NUM_AHCI_CMDSLOT];
}AhciPortData;

/**
 * \brief ATA/ATAPI Driver Interrupt Handler Definition
 *
 * On completion of an ATA/ATAPI device request in interrupt mode the ATA/ATAPI
 * interrupt handler has to handle the command completion status.
 *
 * \param IntNum  [IN]  Handle for the IDE Interface
 */
typedef Uint32  (*Ahci_IntHandler_t) (void *handle);

/**
* \brief AhciHostCtrl;
*
* ahci host controller object data structure
*/
typedef struct{
	AhciPortData		port[SATA_CONFIG_MAX_NUM_AHCI_PORTS]; 		/* port data structure */
	CSL_SataRegs		*reg;										/** ahci regs */
	AhciPortRegs		*portreg[SATA_CONFIG_MAX_NUM_AHCI_PORTS];	/* Port registers, port offset = 100 * PortNum*0x80 */
	Uint32				numPorts;									/* number of ports */
	Uint32				irqEventNum;								/* irq number */
	Uint32				irqEventGroup;
	Ahci_IntHandler_t	ahciIntrHandler; 							/* ahci interrupt handler */
	PlatformResource	*plat_res;			   						/* pointer to platform resource */
	Uint32 				privFlags;									 /* flags to enb/dis the supported features*/
	Uint32		 		cap;
	Uint32		 		origCap;   									/* saved orignial cap */
	Uint32		 		NumPort;
	Uint32		 		initDone;
	Uint32		 		hbaNum;

	/* sata power management */
	Uint32				is_pwr_mgmt_enabled;								/* whether Power management support is enabled */
#ifdef BIOS_PWRM_ENABLE
	PWRM_Resource		lpsc_id;									/* sata lpsc module id */
	Uint32				pwrm_event;									
	PWRM_Event			pwrm_event_type;
	Fxn					pwrm_callback_fn;
#else
	Uint32				lpsc_id;									/* sata lpsc module id */
#endif
	Uint32 				pm_mode;									/* sata pm mode 
																	 * bit 0 : 1/0 - Self PM enabled/disabled
																	 * bit 1 : 1/0 - DVFS PM enabled/disabled
																	 * bit 2 : 1/0 - Sleep PM enabled/disabled
																	 */
	Uint32				pm_state;									/* SATA power state's */
	Uint32				pm_error;
	Uint32				pm_on_cnt;
	Uint32				pm_off_cnt;
}AhciHostCtrl;

/**
 *	\brief	Enable the operation of port, this will enable port dma
 *
 *	\param	port[IN] pointer to ahci controller port object
 *
 *	\return	none
 */
void AhciEnablePort(AhciPortData *port);
/**
 *	\brief	Disable the operation of port, this will disable port dma
 *
 *	\param	port[IN] pointer to ahci controller port object
 *
 *	\return IOM_COMPLETED on success else return IOM_EBADIO
 */
Int32 AhciDisablePort(AhciPortData *port);

/**
 * \brief ahciInitSata
 *
 * This function initializes the AHCI compliance Serial ATA subsystem.
 *
 * @param hba_index 	[IN] AHCI compliance host bus adaptor index
 * @param base_address  [IN] base address of HBA
 * @param irq_num		[IN] irq number of HBA
 * @param ahci_handle   [OUT] handle for ahci HBA
 *
 * @return  none
 */
Ptr ahciInitSata (Uint32 hbaIndex, PlatformResource *plat_res);

/**
 *	\brief	This function de-initializes the AHCI compliance Serial ATA subsystem.
 *
 *	\param	hbaIndex	[IN] AHCI compliance host bus adaptor index ( there
 *                          can be multiple ahci host controller)
 *
 *	\return  none
 */
void ahciDeInitSata (Uint32 hbaIndex);

/**
 * \brief getAhciHandle
 *
 * This function returns the ahci handle of hbaIndex
 *
 * @param hba_index 	[IN] AHCI compliance host bus adaptor index
 */
AhciHostCtrl *getAhciHandle(Uint32 hbaIndex);

/**
 *	\brief this function set power management states of the interface
 *
 *	\param	port			[IN] pointer to ahci controller port object
 *	\param	port_link_state	[IN] power management state
 *
 *	\return	IOM_COMPLETED on success
 */
Int32 AhciPort_SetPowerState(AhciPortData *port, Uint32 port_link_state);
/**
 *	\brief	perform ahci Controller reset or HBA Reset
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *	\param	portNum 	[IN] port number
 *
 *	\return IOM_COMPLETED on success
 */
Int32 ahciReset(AhciHostCtrl *pAhciHost, Uint32 portNum, Uint32 resetType);

