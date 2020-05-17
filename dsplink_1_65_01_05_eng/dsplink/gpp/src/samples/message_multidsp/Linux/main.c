/** ============================================================================
 *  @file   main.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multidsp/Linux/
 *
 *  @desc   Linux specific implementation of multi DSP message sample
 *          application.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <gpptypes.h>
#include <dsplink.h>
#include <errbase.h>

/*  ----------------------------------- Application Header            */
#include <message.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#if defined (PCI_INTERFACE)
/** ============================================================================
 *  @name   DM6437_PCI_Config_0
 *
 *  @desc   DSP/BIOS LINK configuration structure for LC Processsor id 0 .
 *  ============================================================================
 */
extern LINKCFG_DspConfig  DM6437_PCI_Config_0 ;

/** ============================================================================
 *  @name   DM6437_PCI_Config_1
 *
 *  @desc   DSP/BIOS LINK configuration structure for LC Processsor id 1 .
 *  ============================================================================
 */
extern LINKCFG_DspConfig  DM6437_PCI_Config_1 ;
#endif
/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
int main (int argc, char ** argv)
{
    Char8 * dspExecutable_0    = NULL ;
    Char8 * dspExecutable_1    = NULL ;
    Char8 * strNumIterations   = NULL ;


    if (argc != 8) {
        printf ("Usage : %s "
             "<absolute path of DSP executableto be run on first DSP Processor> "
             "<absolute path of DSP executableto be run on Second DSP Processor> "
             "< Bus no of first DSP Processor PCI card >"
             "< Slot no of first DSP Processor PCI card >"
             "< Bus no of second DSP Processor PCI card >"
             "< Slot no of second DSP Processor PCI card >"
             "<number of transfers> \n"
             "For infinite transfers, use value of 0 for <number of transfers>\n",
             argv [0]) ;
    }
    else {
        dspExecutable_0    = argv [1] ;
        dspExecutable_1    = argv [2] ;
        /* Update the bus and slot numbers */
        DM6437_PCI_Config_0.dspObject->arg1 = atoi (argv [3]);
        DM6437_PCI_Config_0.dspObject->arg2 = atoi (argv [4]);
        DM6437_PCI_Config_1.dspObject->arg1 = atoi (argv [5]) ;
        DM6437_PCI_Config_1.dspObject->arg2 = atoi (argv [6]) ;

        strNumIterations   = argv [7] ;

        MESSAGEMULTIDSP_Main (dspExecutable_0,
                              dspExecutable_1,
                              strNumIterations) ;
    }

    return 0 ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
