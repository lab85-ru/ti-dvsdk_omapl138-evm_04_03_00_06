/*
 * C6Run_boot.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
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
 
/************************************************************************
* Re-implments the RTS-libs _args_main() function with a custom version *
* that will call the required _C6Run_main instead of the standard main. *
* Based on the args_main.c file of the C6000 RTS library.               *
************************************************************************/


/******************************************************************************/
/* The ARGS data structure is defined according to a convention with linker.  */
/*                                                                            */
/* If the user want to pass arguments to loader, "--args=###" option has to   */
/* be used in linking to generate executable. With this option, the linker    */
/* will allocate a section starting with __c_args__, and with this "###" many */
/* bytes. The loader when parses the arguments, will dump the number of       */
/* arguments, argc as the 1st arguments at address __c_args__, all the actual */
/* arguments will be dumped after that. The total space for these arguments   */
/* will not exceed "###" bytes.                                               */
/*                                                                            */
/* if "--args="###" is not used as a linker option, linker will put -1 at     */
/* __c_args__ location.                                                       */
/* Use _symval to load the 32-bit value corresponding to symbol __c_args__    */
/*                                                                            */
/* Based on the above convention, the following code implements the access to */
/* these arguments when main is called.                                       */
/*                                                                            */
/* This function is called from boot.asm or boot.c.                           */
/******************************************************************************/


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern void __tdeh_init();
extern int _C6Run_main( void );


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/


/************************************************************
* Global Variable Definitions                               *
************************************************************/


/************************************************************
* Global Function Definitions                               *
************************************************************/

int _args_main()
{
  /*------------------------------------------------------------------------*/
  /* For those targets that support table driven exception handling,        */
  /* initialize the global data structure here before main() to prepare     */
  /* for the first exception, especially from new().                        */
  /*------------------------------------------------------------------------*/
#if __TI_TABLE_DRIVEN_EXCEPTIONS
  __tdeh_init();
#endif
  
  return _C6Run_main();
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
