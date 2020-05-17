/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Use of this software is controlled by the terms and conditions found in
 * the New BSD Software License Agreement, which is reproduced in its
 * entirity below.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== evmdm6437init.c ========
 */

#include <stdio.h>
#include <netmain.h>
#include <evmdm6437.h>
#include <evmdm6437_eeprom.h>

// MDIO phy access functions
uint MDIO_phyRegWrite( uint phyIdx, uint phyReg, Uint16 data );

// This string array corresponds to link state as defined in dm6437_mdio.h
static char *LinkStr[] = { "No Link",
                           "10Mb/s Half Duplex",
                           "10Mb/s Full Duplex",
                           "100Mb/s Half Duplex",
                           "100Mb/s Full Duplex"};

static UINT8 bMacAddr[8];

//
// Main Thread
//
// We changed our TCF file to point call this private init
// function. Here we initialize our board and read in our
// MAC address.
//
void dm6437_init()
{
    // Init the board
    EVMDM6437_init();

    // Read the MAC Address from EEPROM
    EVMDM6437_EEPROM_read (0x00007F00, (Uint32) bMacAddr, 8);
}

//
// DM64LCEMAC_getConfig()
//
// This is a callback from the Ethernet driver. This function
// is used by the driver to get its 6 byte MAC address, and
// to determine which DSP interrupt the EMAC should be mapped to.
//
void DM64LCEMAC_getConfig( UINT8 *pMacAddr, uint *pIntVector )
{
    printf("Using MAC Address: %02x-%02x-%02x-%02x-%02x-%02x\n",
            bMacAddr[0], bMacAddr[1], bMacAddr[2],
            bMacAddr[3], bMacAddr[4], bMacAddr[5]);

    // We fill in the two pointers here. We'll use int 5 for EMAC
    mmCopy( pMacAddr, bMacAddr, 6 );
    *pIntVector = 5;
}

//
// DM64LCEMAC_linkStatus()
//
// This is a callback from the Ethernet driver. This function
// is called whenever there is a change in link state. The
// current PHY and current link state are passed as parameters.
//
void DM64LCEMAC_linkStatus( uint phy, uint linkStatus )
{
    printf("Link Status: %s on PHY %d\n",LinkStr[linkStatus],phy);

#if  0
    /* <TODO> Add the write function call to program LEDs for Micrel */
     
    // Program the LEDs for the Micrel PHY 
    MDIO_phyRegWrite( phy, 0x14, 0xd5d0 );
#endif

}

/*
 */
/*
 *  @(#) ti.bios.log.ndk.examples; 1, 0, 2, 02,34; 5-23-2009 17:29:16; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

