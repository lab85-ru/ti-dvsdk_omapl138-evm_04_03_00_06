/*
 * spi_example.c
 *
 * This file contains the test / demo code to demonstrate basic SPI 
 * operations using the Regsiter CSL macros.
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

/** ============================================================================
 *   @brief A simple example to demonstrate CSL  SPI register layer usage.
 *
 *   @file  spi_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
     This example sets up the SPI in loopback mode.  Characters are
     transmitted and received using software polling. The test will transmit
     8 characters and receive 8 characters and verify the results are correct.
     A pass\fail result will be returned from main.
     @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
     1. Configure the CCS setup to work with the emulator being used
     2. Please refer CCS manual for setup configuration and loading
        proper GEL file
     3. Launch CCS window
     4. Open project spi_example.pjt
     5. Build the project and load the .out file of the project.
     @endverbatim
 *
 * =============================================================================
 **/


/*
================================================================================
                            INLCUDE FILES
================================================================================
*/

#include <stdio.h>
#include <ti/pspiom/cslr/soc_C6747.h>
#include <ti/pspiom/cslr/cslr_spi.h>



#define SPI_NUM_OF_TXBITS   8
#define SPI_NEW_DATA_NO   0x01

CSL_SpiRegsOvly  spiRegs=(CSL_SpiRegsOvly)CSL_SPI_0_REGS;


/*
================================================================================
                         LOCAL FUNCTION PROTOTYPES
================================================================================
*/

static void Spi_dev_init();
static void Spi_test();



/*------------------------------------------------------------------------------
* void main(void)
------------------------------------------------------------------------------*/
/*
*
*  @Param1 : None
*  @RetVal : None
*  @see    : None
*
*  @Description : This function is the main function for the SPI loop back test
*
*/
void main(void)
{
    /* This function will set up the serial port in the loopback mode and     *
     * write a test string for checking the SPI interface                     */

    printf("\nSPI loopback test started.\n");

    /* now initialise the SPI controller with appropriate settings            */
    Spi_dev_init();

    /* test the SPI interface by sending the data                             */
    Spi_test();

}
/*------------------------------------------------------------------------------
* static void Spi_dev_init(void)
------------------------------------------------------------------------------*/
/*
*
*  @Param1 : None
*  @RetVal : None
*  @see    : refer main()
*
*  @Description :This function initialises the SPI interface in the loopback
*                mode with 8 bit transmission and all timer settings as default.
*/
static void Spi_dev_init(void)
{
    /* First reset the SPI chip                                               */
    spiRegs->SPIGCR0 = CSL_FMK(SPI_SPIGCR0_RESET,
                                            CSL_SPI_SPIGCR0_RESET_IN_RESET);

    /* now bring the chip out of reset state                                  */
    spiRegs->SPIGCR0 = CSL_FMK(SPI_SPIGCR0_RESET,
                                            CSL_SPI_SPIGCR0_RESET_OUT_OF_RESET);

    /* enable the CLKMOD and MASTER bits in the SPI global control reg        */
    spiRegs->SPIGCR1 |=  CSL_FMK( SPI_SPIGCR1_MASTER,0x01)
                                  | CSL_FMK(SPI_SPIGCR1_CLKMOD,0x01);

     /* enable the pins so that they are used for the SPI interface(Multiplex) */
    spiRegs->SPIPC0  = CSL_FMK(SPI_SPIPC0_CLKFUN ,0x01)
                                  | CSL_FMK(SPI_SPIPC0_SOMIFUN ,0x01)
                                  | CSL_FMK(SPI_SPIPC0_SIMOFUN ,0x01);


    /* configure the data format in SPIFMT                                    */
    spiRegs->SPIFMT[0] = CSL_FMK(SPI_SPIFMT_CHARLEN,SPI_NUM_OF_TXBITS)
                                  | CSL_FMK(SPI_SPIFMT_PRESCALE,0x02);

    /* set the preconfigure data format as 0 which is already set above       */
    spiRegs->SPIDAT1 = CSL_FMKT(SPI_SPIDAT1_DFSEL,FORMAT0);

    /* dont use any interrupts hence disable them                             */
    spiRegs->SPIINT0 = CSL_FMKT(SPI_SPIINT0_RXINTENA,DISABLE);

    spiRegs->SPIGCR1 |=  CSL_FMK(SPI_SPIGCR1_ENABLE,0x01)
                                  | CSL_FMK(SPI_SPIGCR1_LOOPBACK,0x01);

}
/*------------------------------------------------------------------------------
* static void Spi_test(void)
------------------------------------------------------------------------------*/
/*
*
*  @Param1 : None
*  @RetVal : None
*  @see    : refer main()
*
*  @Description : This function tests the SPI loop back interface by sending a
*                 string and comparing with the data recieved.
*
*/

static void Spi_test(void)
{
    char *data="loopback"; /* test string to transmit*/
    char test='n';

    while (*data)
    {

        /* write the data to the transmit buffer                              */
        CSL_FINS(spiRegs->SPIDAT1,SPI_SPIDAT1_TXDATA,*data);

        /* check if data is recieved                                          */
        while ((CSL_FEXT(spiRegs->SPIBUF,SPI_SPIBUF_RXEMPTY))
                                                    == SPI_NEW_DATA_NO);

        test = (CSL_FEXT(spiRegs->SPIBUF,SPI_SPIBUF_RXDATA));

        if (test == *data)
        {
            printf("sent %c recv %c\n",*data,test);
        }
        else
        {
            printf("ERROR : sent %c recv %c\n",*data,test);
            printf("\nSPI Loopback test Failed.\n");
            return;
        }
        data++;
    }
    printf("SPI Loopback test success.\n");
}

/*
================================================================================
                            END OF FILE
================================================================================
*/

