/*
 * i2cSample_io.c
 *
 * This file contains the test / demo code to demonstrate the I2C driver functi- 
 * nality in DMA-interrupt mode of operation on DSP/BIOS 5.
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

/** \file   i2csample_io.c
 *
 *  \brief  This file contains sample code to demonstrate the I2C driver
 *          implemented for DSP/BIOS
 *
 *   This file performs tests on the following:
 *
 *      1. Sequential Read/Write test on CAT24WC256.(From known address)
 *         
 *
 *   Note
 *      1. The sample application is written for 6747 EVM.
 *      2. The only available i2c instance for sample application use is I2c0
 *      3. This instance of I2c is connected to CAT24WC256 EEPROM on the EVM hence the
 *         application uses these devices for the demonstration of I2c Driver usage.
 *
 *         (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author     Vishwanath K
 *
 *  \version    0.1   Created newly for the evm6747 platform
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <stdio.h>
#include <string.h>
#include <log.h>
#include <tsk.h>
#include <ecm.h>
#include <gio.h>
#include <ti/pspiom/i2c/I2c.h>

#include <ti/sdo/edma3/drv/edma3_drv.h>
#include <ti/sdo/edma3/drv/sample/bios_edma3_drv_sample.h>

/* ========================================================================== */
/*                         IMPORTED VARIABLES                                 */
/* ========================================================================== */

extern EDMA3_DRV_Handle hEdma;

extern LOG_Obj trace;

/* ========================================================================== */
/*                            MACRO DEFINTIONS                                */
/* ========================================================================== */


#define I2C_TRANSFER_SIZE       (128u)

/* Refer to CAT24WC256 datasheet Pg: 5 , */

#define I2C_SLAVE_ADDR          (0x50u)         /* i2c address of CAT24WC256 EEPROM */
#define CAT_REG_ADDR_HIGH       (0x01u)         /* CAT24WC256 EEPROM memory start address(high) for read/write test*/
#define CAT_REG_ADDR_LOW        (0x00u)         /* CAT24WC256 EEPROM memory start address(low) for read/write test i.e address = 0x0100*/
#define I2C_SLAVE_TRANSFER_SIZE (66u)           /* CAT24WC256 page size */


/* ========================================================================== */
/*                            GLOBAL VARIABLES                                */
/* ========================================================================== */

/* buffer aligned to cache line is required when working in DMA Mode */
#pragma DATA_ALIGN(wBuffer, 128);
Uint8   wBuffer[I2C_TRANSFER_SIZE];

#pragma DATA_ALIGN(rBuffer, 128);
Uint8   rBuffer[I2C_TRANSFER_SIZE];

/* I2C channel handles  */
GIO_Handle      i2c_outHandle;                 /* handle to the TX channel */
GIO_Handle      i2c_inHandle;                  /* handle to the RX channel */

/* Global function prototypes */
void user_i2c_init(void);

/* ========================================================================== */
/*                          LOCAL FUNCTION PROTOTYPE                          */
/* ========================================================================== */

static void i2cReadWrite();
static Void  CAT24WC256_i2cRead(Uint32  deviceId, Uint8 *data);

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */

/* \fn      start_i2c_sample
 *
 * \brief   Function to illustrate usage of I2C driver.
 *
 *  This function will illustrate the usage of I2C driver by writing 64 bytes of data to the
 *  memory locations of the CAT24WC256 EEPROM and then reading the data from the same address
 *  ( 64 bytes of len) and comparing the read data with the wriitten data.
 *
 * \param   None
 * \return  None
 */

Void start_i2c_sample(Void)
{
    GIO_Attrs gioAttrs = GIO_ATTRS;
    I2c_ChanParams     chanParams;
    Int status = 340;

    EDMA3_DRV_Result edmaResult      = 0;
   
    /* Initialize EDMA */
    edmaResult = edma3init();
    
    if (edmaResult != EDMA3_DRV_SOK)
    {
        LOG_printf(&trace, "EDMA INIT FAILED\n");
    }
    else
    {
        LOG_printf(&trace, "\r\nEDMA3 : edma3init() passed\r\n");
    }

    /*
     * Initialize channel attributes.
     */
    gioAttrs.nPackets = 2;

    chanParams.hEdma = hEdma;

    chanParams.masterOrSlave = I2c_CommMode_MASTER;

    LOG_printf(&trace, "I2C  :Start of I2C sample application\n");

    /* Create the I2C Channels for the TX and RX communication                */
    i2c_outHandle = GIO_create("/I2C0", IOM_OUTPUT, &status, &chanParams, &gioAttrs);
    LOG_printf(&trace, "GIO_create(outHandle) returned status = %d\n", status);
    i2c_inHandle  = GIO_create("/I2C0", IOM_INPUT, &status, &chanParams, &gioAttrs);
    LOG_printf(&trace, "GIO_create(inHandle) returned status = %d\n", status);

    if ((NULL == i2c_outHandle) || (NULL == i2c_inHandle) )
    {
       LOG_printf(&trace, "I2C : I2C Handle Create Failed.......!\n");
    }
    else
    {
        /* Call function for the I2c read write test                          */
        i2cReadWrite();
    }

    LOG_printf(&trace, "I2C  :End of I2C sample application\n");
}

/* \fn      i2cReadWrite
 *
 * \brief   Function to read write to CAT24WC256 EEPROM.
 *
 *  This function will illustrate the usage of I2C driver by writing 64 bytes of data to the
 *  memory locations of the CAT24WC256 EEPROM and then reading the data from the same address
 *  (64 bytes of len) and comparing the read data with the written data.
 *
 * \param   None
 * \return  None
 */
static void i2cReadWrite()
{
    I2c_DataParam        buf;
    Uint8 i = 0; 

    LOG_printf(&trace, "I2C CAT24WC256 EEPROM write/read test started\n");

    wBuffer[0] = CAT_REG_ADDR_HIGH;
    wBuffer[1] = CAT_REG_ADDR_LOW;

    for(i=2; i < I2C_SLAVE_TRANSFER_SIZE ; i++) {
        wBuffer[i]= i;
    } 

    buf.buffer    = &wBuffer[0];
    buf.bufLen    = I2C_SLAVE_TRANSFER_SIZE;
    buf.flags     = I2c_DEFAULT_WRITE;
    buf.param     = NULL;
    buf.slaveAddr = I2C_SLAVE_ADDR;

    GIO_submit(i2c_outHandle, IOM_WRITE, &buf, &buf.bufLen, NULL);

    TSK_sleep(10u);

   
    for(i=0; i < I2C_SLAVE_TRANSFER_SIZE ; i++) {
        rBuffer[i]= 0;
    }

    rBuffer[0] = CAT_REG_ADDR_HIGH;
    rBuffer[1] = CAT_REG_ADDR_LOW;    
    
    CAT24WC256_i2cRead(I2C_SLAVE_ADDR, rBuffer);


    /* comparing written value and read data                                  */
    
    for(i=0; i<(I2C_SLAVE_TRANSFER_SIZE - 2) ;i++) {
        if ( (rBuffer[i] != wBuffer[i+2]) )  /* First two bytes of wBuffer contains the read/write address */
        {
            LOG_printf(&trace, "\nI2C CAT24WC256 EEPROM Read Failed\n");
            return;
        }
    }

    LOG_printf(&trace, "\nI2C CAT24WC256 EEPROM Read/write test passed\n");

    return;
}


/**
 *  \brief CAT24WC256_i2cRead
 *
 *  This function perfroms a  read(s) from the CAT24WC256 EEPROM memory using the
 *  I2C control bus. This function first writes the read address without
 *  issuing a I2C stop. It then issues a I2C restart and reads
 *  the value(s) from the EEPROM memory.
 */
static Void  CAT24WC256_i2cRead(Uint32 deviceId, Uint8 *data)
{
    I2c_DataParam        dataBuffer;

    /* The following the procedure to read data from CAT24WC256 EEPROM

       Step A: Write the address of the EEPROM memory to be read. Note: The I2C
               stop should not be issued.
       Step B: Issue I2C restart.
       Step C: Read the value(s) of the EEPROM memory.                                */

    /* Prepare the data to be written to the CAT24WC256 EEPROM                      */
    dataBuffer.slaveAddr = deviceId;
    dataBuffer.buffer    = data;
    dataBuffer.bufLen    = 2; /* 2 bytes of address */
    dataBuffer.flags     = I2c_WRITE | I2c_MASTER | I2c_START;

    GIO_submit(i2c_outHandle, IOM_WRITE, &dataBuffer,
            &dataBuffer.bufLen, NULL);

    TSK_sleep(10u);

    /* If the write command is complete, then read the data from EEPROM.     */
    dataBuffer.buffer    = data;
    
    dataBuffer.flags = I2c_READ | I2c_MASTER | I2c_STOP | I2c_RESTART;
    dataBuffer.bufLen  = (I2C_SLAVE_TRANSFER_SIZE - 2) ; /* read 64 bytes of data from the slave */
    
   
    GIO_submit(i2c_inHandle, IOM_READ, &dataBuffer,
            &dataBuffer.bufLen, NULL);

}
/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
