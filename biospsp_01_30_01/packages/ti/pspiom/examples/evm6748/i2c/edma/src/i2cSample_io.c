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
 *      1. Blink the LEDS connected to the user LEDs on TCA6416
 *         
 *
 *   Note
 *      1. The sample application is written for C6748 EVM.
 *      2. The only available i2c instance for sample application use is I2c0
 *      3. This instance of I2c is connected to TCA6416 expander on the EVM and
 *         hence application uses this device for demonstration of I2c driver usage.
 *
 *         (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author     PSG
 *
 *  \version    0.1   Created newly for the evm6748 platform
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include "std.h"
#include "stdio.h"
#include "string.h"
#include "log.h"
#include "tsk.h"
#include "ecm.h"
#include "gio.h"
#include "ti/pspiom/i2c/I2c.h"

#include "ti/sdo/edma3/drv/edma3_drv.h"
#include "ti/sdo/edma3/drv/sample/bios_edma3_drv_sample.h"

/* ========================================================================== */
/*                         IMPORTED VARIABLES                                 */
/* ========================================================================== */

extern EDMA3_DRV_Handle hEdma[];

extern LOG_Obj trace;

/* ========================================================================== */
/*                            MACRO DEFINTIONS                                */
/* ========================================================================== */


#define I2C_TRANSFER_SIZE       (4u)

/* Refer to TCA6416 I2C expander dataseet for mode details                    */
#define I2C_SLAVE_ADDR            (0x21u) /* i2c address of TCA6416 expander  */
#define I2C_EXP_CMD_READ_PORT0    (0x00u) /* Cmd to TCA6416 to read from port0*/
#define I2C_EXP_CMD_WRITE_PORT0   (0x02u) /* Cmd to TCA6416 to read from port0*/
#define I2C_EXP_CMD_CONFIG_PORT0  (0x06u) /* Cmd to TCA6416 to configure port0*/
#define I2C_EXP_PORT0_LED_ON      (0x00u) /* This will turn on the LEDs       */
#define I2C_EXP_PORT0_LED_OFF     (0xc0u) /* This will turn off the LEDs     */


/* ========================================================================== */
/*                            GLOBAL VARIABLES                                */
/* ========================================================================== */

/* buffer aligned to cache line is required when working in DMA Mode */
#pragma DATA_ALIGN(wBuffer, 128);
Uint8   wBuffer[I2C_TRANSFER_SIZE];

/* I2C channel handles  */
GIO_Handle      i2c_outHandle;                 /* handle to the TX channel */
GIO_Handle      i2c_inHandle;                  /* handle to the RX channel */

/* Global function prototypes */
void user_i2c_init(void);

/* ========================================================================== */
/*                          LOCAL FUNCTION PROTOTYPE                          */
/* ========================================================================== */

static void ledBlink();
static Int32 configureExpanderPort(void);
static void  i2cExpander_WritePort0(Uint8 *data);
/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */

/* \fn      start_i2c_sample
 *
 * \brief   Function to illustrate usage of I2C driver.
 *
 *  This function will illustrate the usage of I2C driver by writing to the I2c
 *  expander to blink the user LEDs connected to the TCA6416 expander
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

    chanParams.hEdma = hEdma[0];

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
        ledBlink();
    }

    LOG_printf(&trace, "I2C  :End of I2C sample application\n");
}

/* \fn      ledBlink
 *
 * \brief   Function to read and write the I2C expander TCA6416.
 *
 *  This function will illustrate the usage of I2C driver by writing data to the
 *  I2C expander which will in turn blink the User LEDs connected to it.
 *
 * \param   None
 * \return  None
 */
static void ledBlink()
{
    Int32     status = IOM_COMPLETED;
    volatile  Int8 ledBlinkCount = 20;
    Uint8      value;
        
    LOG_printf(&trace, "I2C exapnder/LEDs write/read test started\n");

    status = configureExpanderPort();
    
    if(IOM_COMPLETED == status)
    {
        while(0 != ledBlinkCount)
        {
            value = (Uint8)I2C_EXP_PORT0_LED_ON;
            i2cExpander_WritePort0(&value);                                   
                                                                              
            TSK_sleep(1000);                                                 
                                                
            value = (Uint8)I2C_EXP_PORT0_LED_OFF;                              
            i2cExpander_WritePort0(&value);    

            TSK_sleep(1000); 
            
            ledBlinkCount--;
        }
    }
    
}



/* Perform write operation on port0                                           */
static void  i2cExpander_WritePort0(Uint8 *data)
{
    I2c_DataParam        dataBuffer;
    Int32                status = IOM_COMPLETED;

    /* Assign data into output buffer                                         */
    /* First command word and then the data                                   */
    wBuffer[0] = I2C_EXP_CMD_WRITE_PORT0;
    wBuffer[1] = *data;
    
    /* Prepare the data to be written to the port                             */
    dataBuffer.slaveAddr = I2C_SLAVE_ADDR;
    dataBuffer.buffer    = &wBuffer[0];
    dataBuffer.bufLen    = 2u;
    dataBuffer.flags     = I2c_WRITE | I2c_MASTER | I2c_START | I2c_STOP;
    
    /* Write the data to I2c expander                                         */
    status = GIO_submit(i2c_outHandle,
                        IOM_WRITE, 
                        &dataBuffer,
                        &dataBuffer.bufLen,
                        NULL);
    
    if (IOM_COMPLETED != status)
    {
        LOG_printf(&trace,"I2c Write: Data Write Failed: %d\n",status);        
    }
      
}


/* Configure the port0 of the I2c expander                                    */
Int32 configureExpanderPort(void)
{
    I2c_DataParam        dataBuffer;
    Int32                status = IOM_COMPLETED;
    
    /* 
     *Configure the PORT0, LED connected pins (7th and 8th pin), of the expander 
     * to be an output 
     */
    /* First command word and then the data                                   */
    wBuffer[0] = I2C_EXP_CMD_CONFIG_PORT0;
    wBuffer[1] = 0x3fu;
    
    /* Prepare the data to be written to the expander                         */
    dataBuffer.slaveAddr = I2C_SLAVE_ADDR;
    dataBuffer.buffer    = &wBuffer[0];
    dataBuffer.bufLen    = 2u;
    dataBuffer.flags     = I2c_WRITE | I2c_MASTER | I2c_START | I2c_STOP;

    /* Send the data on the i2c bus                                           */    
    status = GIO_submit(i2c_outHandle,
                        IOM_WRITE, 
                        &dataBuffer,
                        &dataBuffer.bufLen,
                        NULL);

    if(IOM_COMPLETED != status)
    {
        LOG_printf(&trace, "I2C Expander configuration failed : %d!",status);
    }
    
    return status;
}
/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
