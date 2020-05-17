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

#include <ti/bios/include/std.h>
#include <netmain.h>
#include <stkmain.h>

#include <ti/bios/log/ndk/LogServerCgi.h>

/* Title String */
char *VerStr = "\nLog Server Example\n\n";

/* Our NETCTRL callback functions */
static void   NetworkOpen();
static void   NetworkClose();
static void   NetworkIPAddr( IPN IPAddr, uint IfIdx, uint fAdd );

/* Fun reporting function */
static void ServiceReport(uint Item, uint Status, uint Report, HANDLE hCfgEntry);

/*
 *  DHCP reset code 
 *  
 *  The code for DHCP reset looks to be unnecessary for the log server example's
 *  stack setup.  It is currently being "commented" out by using #ifdef
 *  pre-processing conditions.  The code has been left in this file for insurance
 *  purposes, just incase removing does indeed cause some unforeseen issue.
 *
 *  If the user wishes to use this reset code, then USE_DHCP_RESET should be
 *  defined in this file:
 *
 *  #define USE_DHCP_RESET 1
 */
#ifdef USE_DHCP_RESET
void DHCP_reset( uint IfIdx, uint fOwnTask );
UINT8 DHCP_OPTIONS[] = { DHCPOPT_SERVER_IDENTIFIER, DHCPOPT_ROUTER };
#endif

/* Configuration */
char *HostName = "tidsp";
char *LocalIPAddr = "0.0.0.0";                  // Set to "0.0.0.0" for DHCP
char *LocalIPMask = "255.255.254.0";    // Not used when using DHCP
char *GatewayIP = "0.0.0.0";                  // Not used when using DHCP
char *DomainName = "demo.net";         // Not used when using DHCP
char *DNSServer = "0.0.0.0";          // Used when set to anything but zero

/*
 * Main Thread
 */
int Stack_run()
{
    int               rc;
    HANDLE            hCfg;

    /*
     *     *** Log Server Setup Code ***
     *
     *  The following code should be added by the user in order to initialize
     *  the log server CGI module.
     *
     */
    LogServerCgi_init();

    //
    // THIS MUST BE THE ABSOLUTE FIRST THING DONE IN AN APPLICATION!!
    //
    rc = NC_SystemOpen( NC_PRIORITY_LOW, NC_OPMODE_INTERRUPT );
    if( rc )
    {
        printf("NC_SystemOpen Failed (%d)\n",rc);
        for(;;);
    }

    // Print out our banner
    printf(VerStr);

    //
    // Create and build the system configuration from scratch.
    //

    // Create a new configuration
    hCfg = CfgNew();
    if( !hCfg )
    {
        printf("Unable to create configuration\n");
        goto main_exit;
    }

    // We better validate the length of the supplied names
    if( strlen( DomainName ) >= CFG_DOMAIN_MAX ||
        strlen( HostName ) >= CFG_HOSTNAME_MAX )
    {
        printf("Names too long\n");
        goto main_exit;
    }

    // Add our global hostname to hCfg (to be claimed in all connected domains)
    CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0,
                 strlen(HostName), (UINT8 *)HostName, 0 );

    // If the IP address is specified, manually configure IP and Gateway
    if( inet_addr(LocalIPAddr) )
    {
        CI_IPNET NA;
        CI_ROUTE RT;
        IPN      IPTmp;

        // Setup manual IP address
        bzero( &NA, sizeof(NA) );
        NA.IPAddr  = inet_addr(LocalIPAddr);
        NA.IPMask  = inet_addr(LocalIPMask);
        strcpy( NA.Domain, DomainName );
        NA.NetType = 0;

        // Add the address to interface 1
        CfgAddEntry( hCfg, CFGTAG_IPNET, 1, 0,
                           sizeof(CI_IPNET), (UINT8 *)&NA, 0 );

        // Add the default gateway. Since it is the default, the
        // destination address and mask are both zero (we go ahead
        // and show the assignment for clarity).
        bzero( &RT, sizeof(RT) );
        RT.IPDestAddr = 0;
        RT.IPDestMask = 0;
        RT.IPGateAddr = inet_addr(GatewayIP);

        // Add the route
        CfgAddEntry( hCfg, CFGTAG_ROUTE, 0, 0,
                           sizeof(CI_ROUTE), (UINT8 *)&RT, 0 );

        // Manually add the DNS server when specified
        IPTmp = inet_addr(DNSServer);
        if( IPTmp )
            CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER,
                         0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0 );
    }
    // Else we specify DHCP
    else
    {
        CI_SERVICE_DHCPC dhcpc;

        // Specify DHCP Service on IF-1
        bzero( &dhcpc, sizeof(dhcpc) );
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = 1;
        dhcpc.cisargs.pCbSrv = &ServiceReport;

#ifdef USE_DHCP_RESET
        dhcpc.param.pOptions = DHCP_OPTIONS;
        dhcpc.param.len = 2;
#endif

        CfgAddEntry( hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0,
                     sizeof(dhcpc), (UINT8 *)&dhcpc, 0 );
    }

    /*
     *     *** Log Server Setup Code ***
     *  
     *  The following code should be added by the user in order to enable the
     *  log server CGI capability, as well as the HTTP server (this is only
     *  required if the user does not already have the neccessary code to run
     *  the HTTP server in their program).
     */
    
    /*
     *  Create RAM based WEB files.  This function call is required in order to
     *  use the log server CGI command capabilities.  The user must copy and
     *  paste this code into their stack setup function.
     */
    LogServerCgi_add();

    /*
     *  Specify HTTP service.  This is only required if the user's program does
     *  NOT already contain the code to setup an HTTP server.  The user should
     *  copy and paste this code into their stack setup function if they do not
     *  have the HTTP server code.
     */
    {
        CI_SERVICE_HTTP   http;
        bzero( &http, sizeof(http) );
        http.cisargs.IPAddr = INADDR_ANY;
        http.cisargs.pCbSrv = &ServiceReport;
        CfgAddEntry( hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_HTTP, 0,
                     sizeof(http), (UINT8 *)&http, 0 );
    }
    /*  *** End Of Log Server Setup Code ***  */


    //
    // Configure IPStack/OS Options
    //

    // We don't want to see debug messages less than WARNINGS
    rc = DBG_WARN;
    CfgAddEntry( hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL,
                 CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0 );

    //
    // This code sets up the TCP buffer sizes
    // (Note 8192 is actually the default. This code is here to
    // illustrate how the buffer and limit sizes are configured.)
    //

    /*
     *  Set up the TCP buffer sizes.  If the user wishes to change the transmit
     *  and receive buffer sizes, this should be done here:
     */
    // TCP Transmit buffer size
    rc = 8192;
    CfgAddEntry( hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPTXBUF,
                 CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0 );

    // TCP Receive buffer size (copy mode)
    rc = 8192;
    CfgAddEntry( hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXBUF,
                 CFG_ADDMODE_UNIQUE, sizeof(uint), (UINT8 *)&rc, 0 );

    //
    // Boot the system using this configuration
    //
    // We keep booting until the function returns 0. This allows
    // us to have a "reboot" command.
    //
    do
    {
        rc = NC_NetStart( hCfg, NetworkOpen, NetworkClose, NetworkIPAddr );
    } while( rc > 0 );

    /*
     *     *** Log Server Setup Code ***
     *  
     *  The following code should be added by the user in order to free the
     *  log server CGI files that have been created.
     *
     *  Delete RAM based WEB files.  This function call is required in order to
     *  remove the log server CGI web files created by calling
     *  LogServerCgi_add().  The user must copy and paste this code into their
     *  stack setup function.
     */
    LogServerCgi_remove(); // Free the WEB files
    
    /*  *** End Of Log Server Setup Code ***  */

    // Delete Configuration
    CfgFree( hCfg );

    // Close the OS
main_exit:
    NC_SystemClose();
    return(0);
}


//
// NetworkOpen
//
// This function is called after the configuration has booted
//
static void NetworkOpen()
{
    // Create our local servers
}

//
// NetworkClose
//
// This function is called when the network is shutting down,
// or when it no longer has any IP addresses assigned to it.
//
static void NetworkClose()
{

}

//
// NetworkIPAddr
//
// This function is called whenever an IP address binding is
// added or removed from the system.
//
static void NetworkIPAddr( IPN IPAddr, uint IfIdx, uint fAdd )
{
    IPN IPTmp;

    if( fAdd )
        printf("Network Added: ");
    else
        printf("Network Removed: ");

    // Print a message
    IPTmp = ntohl( IPAddr );
    printf("If-%d:%d.%d.%d.%d\n", IfIdx,
            (UINT8)(IPTmp>>24)&0xFF, (UINT8)(IPTmp>>16)&0xFF,
            (UINT8)(IPTmp>>8)&0xFF, (UINT8)IPTmp&0xFF );

}

void CheckDHCPOptions();

//
// Service Status Reports
//
// Here's a quick example of using service status updates
//
static char *TaskName[]  = { "Telnet","HTTP","NAT","DHCPS","DHCPC","DNS" };
static char *ReportStr[] = { "","Running","Updated","Complete","Fault" };
static char *StatusStr[] = { "Disabled","Waiting","IPTerm","Failed","Enabled" };
static void ServiceReport( uint Item, uint Status, uint Report, HANDLE h )
{
    printf( "Service Status: %-9s: %-9s: %-9s: %03d\n",
            TaskName[Item-1], StatusStr[Status],
            ReportStr[Report/256], Report&0xFF );

    //
    // Example of adding to the DHCP configuration space
    //
    // When using the DHCP client, the client has full control over access
    // to the first 256 entries in the CFGTAG_SYSINFO space.
    //
    // Note that the DHCP client will erase all CFGTAG_SYSINFO tags except
    // CFGITEM_DHCP_HOSTNAME. If the application needs to keep manual
    // entries in the DHCP tag range, then the code to maintain them should
    // be placed here.
    //
    // Here, we want to manually add a DNS server to the configuration, but
    // we can only do it once DHCP has finished its programming.
    //
    if( Item == CFGITEM_SERVICE_DHCPCLIENT &&
        Status == CIS_SRV_STATUS_ENABLED &&
        (Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPADD) ||
         Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPRENEW)) )
    {
        IPN IPTmp;

        // Manually add the DNS server when specified
        IPTmp = inet_addr(DNSServer);
        if( IPTmp )
            CfgAddEntry( 0, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER,
                         0, sizeof(IPTmp), (UINT8 *)&IPTmp, 0 );
    }

#ifdef USE_DHCP_RESET
    // Reset DHCP client service on failure
    if( Item==CFGITEM_SERVICE_DHCPCLIENT && (Report&~0xFF)==NETTOOLS_STAT_FAULT )
    {
        CI_SERVICE_DHCPC dhcpc;
        int tmp;

        // Get DHCP entry data (for index to pass to DHCP_reset).
        tmp = sizeof(dhcpc);
        CfgEntryGetData( h, &tmp, (UINT8 *)&dhcpc );

        // Create the task to reset DHCP on its designated IF
        // We must use TaskCreate instead of just calling the function as
        // we are in a callback function.
        TaskCreate( DHCP_reset, "DHCPreset", OS_TASKPRINORM, 0x1000,
                    dhcpc.cisargs.IfIdx, 1, 0 );
    }
#endif
}

#ifdef USE_DHCP_RESET
void CheckDHCPOptions()
{
    char IPString[16];
    IPN  IPAddr;
    int  i, rc;

    // Now scan for DHCPOPT_SERVER_IDENTIFIER via configuration
    printf("\nDHCP Server ID:\n");
    for(i=1;;i++)
    {
        // Try and get a DNS server
        rc = CfgGetImmediate( 0, CFGTAG_SYSINFO, DHCPOPT_SERVER_IDENTIFIER,
                              i, 4, (UINT8 *)&IPAddr );
        if( rc != 4 )
            break;

        // We got something

        // Convert IP to a string:
        NtIPN2Str( IPAddr, IPString );
        printf("DHCP Server %d = '%s'\n", i, IPString);
    }
    if( i==1 )
        printf("None\n\n");
    else
        printf("\n");

    // Now scan for DHCPOPT_ROUTER via the configuration
    printf("Router Information:\n");
    for(i=1;;i++)
    {
        // Try and get a DNS server
        rc = CfgGetImmediate( 0, CFGTAG_SYSINFO, DHCPOPT_ROUTER,
                              i, 4, (UINT8 *)&IPAddr );
        if( rc != 4 )
            break;

        // We got something

        // Convert IP to a string:
        NtIPN2Str( IPAddr, IPString );
        printf("Router %d = '%s'\n", i, IPString);
    }
    if( i==1 )
        printf("None\n\n");
    else
        printf("\n");
}

void DHCP_reset( uint IfIdx, uint fOwnTask )
{
    CI_SERVICE_DHCPC dhcpc;
    HANDLE h;
    int    rc,tmp;
    uint   idx;

    // If we were called from a newly created task thread, allow
    // the entity that created us to complete
    if( fOwnTask )
        TaskSleep(500);

    // Find DHCP on the supplied interface
    for(idx=1; ; idx++)
    {
        // Find a DHCP entry
        rc = CfgGetEntry( 0, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT,
                          idx, &h );
        if( rc != 1 )
            goto RESET_EXIT;

        // Get DHCP entry data
        tmp = sizeof(dhcpc);
        rc = CfgEntryGetData( h, &tmp, (UINT8 *)&dhcpc );

        // If not the right entry, continue
        if( (rc<=0) || dhcpc.cisargs.IfIdx != IfIdx )
        {
            CfgEntryDeRef(h);
            h = 0;
            continue;
        }

        // This is the entry we want!

        // Remove the current DHCP service
        CfgRemoveEntry( 0, h );

        // Specify DHCP Service on specified IF
        bzero( &dhcpc, sizeof(dhcpc) );
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = IfIdx;
        dhcpc.cisargs.pCbSrv = &ServiceReport;
        CfgAddEntry( 0, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0,
                     sizeof(dhcpc), (UINT8 *)&dhcpc, 0 );
        break;
    }

RESET_EXIT:
    // If we are a function, return, otherwise, call TaskExit()
    if( fOwnTask )
        TaskExit();
}
#endif

/*
 *  ======== logserverstacksetup.c ========
 *
 */
/*
 *  @(#) ti.bios.log.ndk.examples; 1, 0, 2, 02,34; 5-23-2009 17:29:16; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

