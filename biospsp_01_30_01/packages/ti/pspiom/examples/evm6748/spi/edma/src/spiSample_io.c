/*
 * spiSample_io.c
 *
 * This file contains the test / demo code to demonstrate the SPI driver functi-
 * onality in DMA-interrupt mode of operation on DSP/BIOS 5.
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
 *  \file   spiSample_io.c
 *
 *  \brief  sample application to demostrate the usage of SPI driver.
 *
 *  (C) Copyright 2010, Texas Instruments, Inc
 *
 *  \author     PSG
 *
 *  \version    0.1    Created newly for C6748 platform.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <std.h>
#include <log.h>
#include <sys.h>
#include <tsk.h>
#include <idl.h>
#include <ecm.h>
#include <iom.h>
#include <gio.h>
#include <pwrm.h>
#include <pwrm6748.h>
#include "ti/pspiom/spi/Spi.h"
#include "ti/sdo/edma3/drv/edma3_drv.h"

/* SPI HW conig macro's */
#define CS_DEFAULT_SPI                      0x000000FFu
#define CHIP_NUMBER                         1u
#define SPI_DATA_CHAR_LENGTH                8u

extern EDMA3_DRV_Handle hEdma[];
extern EDMA3_DRV_Result edma3init();
extern LOG_Obj trace;

/* ========================================================================== */
/*                          LOCAL FUNCTION PROTOTYPES                         */
/* ========================================================================== */

static Int spi_Test();
static void Spi_Flash_WriteEnable(Spi_DataParam *dataparam);
static Bool Spi_Flash_IsBusy(Spi_DataParam *dataparam);

/* ========================================================================== */
/*                            MACRO DEFINITONS                                */
/* ========================================================================== */

#define  SPI_TEST_STRING_LEN       0x100

/* chip select for the spi flash                          */
#define  SPI_CHIPSELECT_SPIFLASH   0x01

/* flash address where the data will be written and read  */
#define  SPI_FLASH_ADDR_MSB1       0x0A
#define  SPI_FLASH_ADDR_MSB0       0x00
#define  SPI_FLASH_ADDR_LSB        0x00


#define  SPI_MAX_CMD_LEN           0x04

/* SPI flash opcode */
/* flash page write                                       */
#define  SPI_FLASH_PAGE_WRITE      0x02

/* flash data read                                        */
#define  SPI_FLASH_READ            0x03

/* read the status register                               */
#define  SPI_FLASH_READ_STATUS_REG 0x05

/* write enable opcode                                    */
#define  SPI_FLASH_WRITE_ENABLE    0x06

/* sectore erase command                                  */
#define SPI_FLASH_SECTOR_ERASE     0xD8

/* ========================================================================== */
/*                              GLOBAL VARIABLES                              */
/* ========================================================================== */

/* Buffer alignement is required when working in DMA Mode */
/* max command length is 4 (opcode + 3 bytes address) + max trnsfer size )    */
#pragma DATA_ALIGN(loopWrite, 128);
Uint8  loopWrite[SPI_TEST_STRING_LEN + SPI_MAX_CMD_LEN];

/* Buffer alignement is required when working in DMA Mode */
#pragma DATA_ALIGN(loopRead, 128);
Uint8  loopRead[SPI_TEST_STRING_LEN + SPI_MAX_CMD_LEN];

/* handle to the input and output streams                 */
GIO_Handle  spiHandle  = NULL;

/* Global SPI init config data structure */
Spi_Params       spiParams;


/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */

void spi_initParams(void)
{

    spiParams = Spi_PARAMS;

    spiParams.hwiNumber = 8;
    spiParams.spiHWCfgData.intrLevel = FALSE;
    spiParams.opMode = Spi_OpMode_DMAINTERRUPT;

    spiParams.outputClkFreq     = 20000000;
    spiParams.loopbackEnabled   = FALSE;
    spiParams.edmaHandle        = NULL;

    spiParams.spiHWCfgData.configDatafmt[0].charLength   = 8;
    spiParams.spiHWCfgData.configDatafmt[1].charLength   = 16;
    spiParams.spiHWCfgData.configDatafmt[0].clkHigh      = TRUE ;
    spiParams.spiHWCfgData.configDatafmt[0].lsbFirst     = FALSE;
    spiParams.spiHWCfgData.configDatafmt[0].oddParity    = FALSE;
    spiParams.spiHWCfgData.configDatafmt[0].parityEnable = FALSE ;
    spiParams.spiHWCfgData.configDatafmt[0].phaseIn      = FALSE ;
    spiParams.spiHWCfgData.configDatafmt[0].waitEnable   = FALSE;
    //spiParams.spiHWCfgData.configDatafmt[0].wdelay       = 0;
    spiParams.spiHWCfgData.intrLevel     = TRUE;

    /* enable the EDMA in the PSC module  */
    PWRM_setDependency(PWRM_RSRC_EDMA3_CC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_1);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_2);

    PWRM_setDependency(PWRM_RSRC_GPIO);
}


/*
 *  \fn    void start_spi_sample(void)
 *
 *  \brief function which creates the inout and output channels for the SPI
 *         device.
 *
 *   This function opens two channels for the transaction. The channel handles
 *   returned are used to transact with the PSI device.
 *
 *  \param  eb    [OUT]  error block given by the upper layers
 *
 *  \return None
 */
void start_spi_sample()
{

    Int                 devStatus;
    GIO_Attrs           gioAttrs            = GIO_ATTRS;
    Spi_ChanParams      chanParams;
    /* if the edma support is required then we need to configure edma         */
    EDMA3_DRV_Result    edmaResult      = 0;

    if (NULL == hEdma[0])
    {
        edmaResult = edma3init();
        if (EDMA3_DRV_SOK != edmaResult)
        {
            /* Error in configuring the edma driver                       */
            LOG_printf(&trace,"\r\nEDMA3 : edma3init() failed\r\n");
        }
        else
        {
            LOG_printf(&trace,"\r\nEDMA3 : edma3init() passed\r\n");
        }
    }

    chanParams.hEdma = hEdma[0];

    /* create SPI channel for transmission */
    spiHandle =  GIO_create("/Spi0",IOM_INOUT,NULL,&chanParams,&gioAttrs);

    if (NULL != spiHandle)
    {
        devStatus = spi_Test();
        if(IOM_COMPLETED != devStatus)
        {
            LOG_printf(&trace,"\r\nBIOS SPI:Error in SPI sample Test \n");
        }
        else
        {
            LOG_printf(&trace,"\r\nBIOS SPI:SPI sample transceive ended succesfully \n");
        }

        GIO_delete(spiHandle);
    }
    else
    {
        LOG_printf(&trace,"\r\n SPI Driver Handle creation Failed ");
    }

    return;
}

/*
 *  \fn    static Int spi_Test(Error_block *eb)
 *
 *  \brief function That will test the SPI device by reading and writing from the
 *         SPI flash device.
 *
 *  \param  eb    [OUT]  error block given by the upper layers
 *
 *  \return
 *          DriverTypes_Completed - if sucessful
 *          Error code            - in case of error
 */
static Int spi_Test()
{
    Uint32              ui32Count   = 0;
    Int                 status      = IOM_COMPLETED;
    Spi_DataParam       dataparam;
    size_t              size = 0;

    /* clear the input and output buffers                                     */
    memset(loopWrite, 0x00, sizeof(loopWrite));
    memset(loopRead , 0x00, sizeof(loopRead));

    /* clear the spi params data structure                                    */
    memset(&dataparam,0x00, sizeof(Spi_DataParam));

    /* enable the write to SPI flash                                          */
    Spi_Flash_WriteEnable(&dataparam);

    /* erase a sector of the flash                                            */
    /* write the data to the SPI flash                                        */
    loopWrite[0] = SPI_FLASH_SECTOR_ERASE;
    loopWrite[1] = SPI_FLASH_ADDR_MSB1;
    loopWrite[2] = SPI_FLASH_ADDR_MSB0;
    loopWrite[3] = SPI_FLASH_ADDR_LSB;

    dataparam.bufLen       = SPI_MAX_CMD_LEN;
    dataparam.inBuffer     = NULL;
    dataparam.outBuffer    = &loopWrite[0];
    dataparam.flags        = Spi_CSHOLD;
    dataparam.dataFormat   = Spi_DataFormat_0;

    size = dataparam.bufLen;

    /* write command to FLASH                                                 */
    GIO_write(spiHandle, &dataparam, &size);

    /* wait till the flash has completed the erase                            */
    while (TRUE == Spi_Flash_IsBusy(&dataparam))
    {
        TSK_sleep(1000);
    }


    /* enable the write to SPI flash                                          */
    Spi_Flash_WriteEnable(&dataparam);


    /* initialise the data to write to the flash                              */
    for (ui32Count = 0; ui32Count < SPI_TEST_STRING_LEN; ui32Count++)
    {
        /* first 4 bytes will accomodate the command                          */
        loopWrite[SPI_MAX_CMD_LEN + ui32Count] = ui32Count;

        /* clear the input buffer                                             */
        loopRead[SPI_MAX_CMD_LEN + ui32Count] = 0x00;
    }

    /* write a page of data to the SPI flash                                  */

    /* write the data to the SPI flash                                        */
    loopWrite[0] = SPI_FLASH_PAGE_WRITE;
    loopWrite[1] = SPI_FLASH_ADDR_MSB1;
    loopWrite[2] = SPI_FLASH_ADDR_MSB0;
    loopWrite[3] = SPI_FLASH_ADDR_LSB;

    dataparam.bufLen       = SPI_TEST_STRING_LEN + SPI_MAX_CMD_LEN;
    dataparam.inBuffer     = NULL;
    dataparam.outBuffer    = &loopWrite[0];
    dataparam.flags        = Spi_CSHOLD;
    dataparam.dataFormat   = Spi_DataFormat_0;

    size = dataparam.bufLen;

    /* write data to FLASH */
    GIO_write(spiHandle, &dataparam, &size);

    /* wait till the flash has completed the write                            */
    while (TRUE == Spi_Flash_IsBusy(&dataparam))
    {
        TSK_sleep(1000);
    }

    /* read the data from the Flash                                           */
    loopWrite[0] = SPI_FLASH_READ;
    loopWrite[1] = SPI_FLASH_ADDR_MSB1;
    loopWrite[2] = SPI_FLASH_ADDR_MSB0;
    loopWrite[3] = SPI_FLASH_ADDR_LSB;

    dataparam.flags      = Spi_CSHOLD;
    dataparam.dataFormat  = Spi_DataFormat_0;
    /* ( Opcode (1byte) + address (3bytes) + (data to be read)                */
    dataparam.bufLen      = SPI_TEST_STRING_LEN + SPI_MAX_CMD_LEN;
    dataparam.outBuffer   = &loopWrite[0];
    dataparam.inBuffer    = &loopRead[0];

    size = dataparam.bufLen;

    /* read from Flash                                                        */
    GIO_write(spiHandle, &dataparam, &size);

    /* compare the data and check if the data read is OK                      */
    for (ui32Count = 0; ui32Count < SPI_TEST_STRING_LEN; ui32Count++)
    {
        if (loopRead[ui32Count + SPI_MAX_CMD_LEN] != ui32Count)
        {
            LOG_printf(&trace,"Data compare Failed SENT %d RECV %d \n",ui32Count,loopRead[ui32Count+SPI_MAX_CMD_LEN]);
            status = IOM_EBADIO;
            break;
        }
    }

    return status;
}

/*
 *  \fn    static void Spi_Flash_WriteEnable(Spi_DataParam *dataparam,Error_Block *eb)
 *
 *  \brief function That will test the SPI device by reading and writing from the
 *         SPI flash device.
 *
 *  \param  eb    [OUT]  error block given by the upper layers
 *
 *  \return
 *          DriverTypes_Completed - if sucessful
 *          Error code            - in case of error
 */
static void Spi_Flash_WriteEnable(Spi_DataParam *dataparam)
{
    size_t    size = 0;

    /* Enable the write for the SPI flash                                     */

    /* send the command for enabling the write to SPI flash                   */
    loopWrite[0]            = SPI_FLASH_WRITE_ENABLE;
    dataparam->inBuffer     = NULL;
    dataparam->outBuffer    = (Uint8*)&loopWrite[0];
    dataparam->chipSelect   = SPI_CHIPSELECT_SPIFLASH;
    dataparam->dataFormat   = Spi_DataFormat_0;
    dataparam->flags        = 0;
    dataparam->bufLen       = 1u;

    size = dataparam->bufLen;

    /* SPI Data Write OPCODE Transfer                                         */
    GIO_write (spiHandle, dataparam, &size);

    /* now read the status register                                           */
    loopWrite[0]            = SPI_FLASH_READ_STATUS_REG;
    dataparam->inBuffer     = loopRead;
    dataparam->outBuffer    = loopWrite;
    dataparam->bufLen       = 2u;
    dataparam->dataFormat   = Spi_DataFormat_0;
    dataparam->flags        = Spi_CSHOLD;

    size = dataparam->bufLen;

    /* read the register                                                      */
    GIO_write(spiHandle,dataparam, &size);

    /* now check if the write enable latch is set to 1 (write enabled)        */
    if (loopRead[1] & 0x02)
    {
        LOG_printf(&trace,"write is Enabled \n");
    }
    else
    {
        LOG_printf(&trace,"Error unable to enable write \n");
    }
}
/*
 *  \fn      static Bool Spi_Flash_IsBusy(Spi_DataParam *dataparam,Error_Block *eb)
 *
 *  \brief   This function tests whether the flash  is busy executing a command
 *           or is it free to receive the next command
 *
 *  \param   dataparam - pointer to the SPI data params structure
 *
 *  \return  TRUE   - if flash is busy executing command
 *           FALSE  - if flash up
 */
static Bool Spi_Flash_IsBusy(Spi_DataParam *dataparam)
{
    size_t    size = 0;

    /* now read the status register                                           */
    loopWrite[0]            = SPI_FLASH_READ_STATUS_REG;
    dataparam->inBuffer     = loopRead;
    dataparam->outBuffer    = loopWrite;
    dataparam->bufLen       = 2u;
    dataparam->dataFormat   = Spi_DataFormat_0;
    dataparam->chipSelect   = SPI_CHIPSELECT_SPIFLASH;
    dataparam->flags        = Spi_CSHOLD;

    size = dataparam->bufLen;

    /* read the register                                                      */
    GIO_write (spiHandle,dataparam, &size);

    /* now check if the write enable latch is set to 1 (write enabled)        *
     * The first byte will contain junk data hence check the second byte      */
    if (loopRead[1] & 0x01)
    {
        /* flash is busy     */
        return TRUE;
    }
    else
    {
        /* flash is not busy */
        return FALSE;
    }
}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */



