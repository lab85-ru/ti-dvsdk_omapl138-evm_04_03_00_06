/** ============================================================================
 *  @file   main.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mpcsxfer/Linux/
 *
 *  @desc   Linux specific implementation of the mpcsxfer sample application's
 *          main function.
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
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <mpcsxfer_os.h>
#include <mpcsxfer.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
 extern Uint32 MPCSXFER_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
extern Uint32 MPCSXFER_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
extern Uint32 MPCSXFER_argsAddr ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;
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
    Char8 * dspExecutable    = NULL ;
    Char8 * strBufferSize    = NULL ;
    Char8 * strNumIterations = NULL ;
    Char8 * strProcessorId   = NULL ;
    Uint8 processorId        = 0    ;
#if defined (DA8XXGEM)
    Char8 * strDspAddr       = NULL ;
    Char8 * strShmAddr       = NULL ;
    Char8 * strArgsAddr      = NULL ;
#endif

    if ((argc != 5) && (argc != 4)) {
        printf ("Usage : %s <absolute path of DSP executable> "
                "<Buffer Size> <number of transfers> <DSP ProcessorId>\n"
                "For infinite transfers,"
                "use value of 0 for <number of transfers>\n"
                "For DSP Processor Id,"
                "\n\t use value of 0  if sample needs to be run on DSP 0 "
                "\n\t use value of 1  if sample needs to be run on DSP 1 \n",
                argv [0]) ;
    }
    else {
        dspExecutable    = argv [1] ;
        strBufferSize    = argv [2] ;
        strNumIterations = argv [3] ;
         if (argc == 4) {
             strProcessorId = "0" ;
             processorId    =  0  ;
        }
        else  if (argc == 5) {
            strProcessorId   = argv [4] ;
            processorId      =  atoi (argv [4]) ;
        }
        if (processorId < MAX_PROCESSORS) {
#if defined(DA8XXGEM)
            if (LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl
                  ==  DSP_BootMode_NoBoot) {
                /* strDspAddr(c_int00 address)  and .args address are not required
                 * for noboot mode. ShmBaseAddr is not provided since
                 * DSPLINK_shmBaseAddress is provided in linker commnad file.
                 */
                 strShmAddr  = "0x0" ;
                 strDspAddr  = "0x0" ;
                 strArgsAddr = "0x0" ;
                 MPCSXFER_shmAddr       = MPCSXFER_Atoll (strShmAddr) ;
                 MPCSXFER_dspAddr       = MPCSXFER_Atoll (strDspAddr) ;
                 MPCSXFER_argsAddr      = MPCSXFER_Atoll (strArgsAddr) ;
                 /* For No bootmode Hard coding the values
                 * since DSP side app is using the same values
                 */
                strBufferSize    = "1024" ;
                strNumIterations = "10000" ;

            }
#endif
            MPCSXFER_Main (dspExecutable,
                           strBufferSize,
                           strNumIterations,
                           strProcessorId) ;
        }

    }

    return 0 ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
