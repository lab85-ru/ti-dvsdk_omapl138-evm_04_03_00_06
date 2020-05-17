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
*  Defines entry-point for DSP-side code. Derived from DSP/Bios source  *
*  file boot.c                                                          *
************************************************************************/


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern void exit(), _auto_init();
extern void IDL_loop();
extern void BIOS_start();
extern far void BCACHE_bootInit();


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


/*---------------------------------------------------------------------------*/
/* ALLOCATE THE MEMORY FOR THE SYSTEM STACK.  THIS SECTION WILL BE SIZED     */
/* BY THE LINKER.                                                            */
/*---------------------------------------------------------------------------*/
asm(" .global __STACK_SIZE");
asm(" .global __stack");

/*
 * Define special symbol to guarantee that this boot file gets linked
 * instead of boot file from rts.lib (SDSsq24217). The GBL_config macro
 * will reference this symbol forcing a linker error if non-bios boot.obj
 * has been linked with application.
 */
asm(" .def GBL_boot");
asm("GBL_boot .set 1");

asm("__stack: .usect   .stack, 0, 8");


/************************************************************
* Global Function Definitions                               *
************************************************************/

/*
 * Put c_int00 in the .sysinit section.
 */
asm(" .sect \".sysinit\"");

#pragma CODE_SECTION(c_int00,".sysinit")
extern void interrupt c_int00()
{
    /*
    * Clear IER register.  This is necessary to prohibit left-over
    * interrupts from being serviced.  This is not necessary if users
    * reset the part before reload or restart but ...
    */
    asm("        zero b0");
    asm("        mvc b0,ier");

    /*------------------------------------------------------------------------*/
    /* SET UP THE STACK POINTER IN B15.                                       */
    /* THE STACK POINTER POINTS 1 WORD PAST THE TOP OF THE STACK, SO SUBTRACT */
    /* 1 WORD FROM THE SIZE.                                                  */
    /*------------------------------------------------------------------------*/
    asm("      mvkl __stack,SP");
    asm("      mvkh __stack,SP");
    asm("      mvkl __STACK_SIZE - 4,B0");
    asm("      mvkh __STACK_SIZE - 4,B0");
    asm("      add  B0,SP,SP");

    /*------------------------------------------------------------------------*/
    /* THE SP MUST BE ALIGNED ON AN 8-BYTE BOUNDARY.                          */
    /*------------------------------------------------------------------------*/
    asm("      and  ~7,SP,SP");               

    /*------------------------------------------------------------------------*/
    /* SET UP THE GLOBAL PAGE POINTER IN B14.                                 */
    /*------------------------------------------------------------------------*/
    asm("      .global $bss");
    asm("      mvkl $bss,DP");
    asm("      mvkh $bss,DP");

    /*------------------------------------------------------------------------*/
    /* SET UP FLOATING POINT REGISTERS FOR C67 ONLY                           */
    /*------------------------------------------------------------------------*/
  #ifdef _TMS320C6700
    asm("      mvk   0,B3");     /* round to nearest */
    asm("      mvc   B3,FADCR");
    asm("      mvc   B3,FMCR");
  #endif

    /*------------------------------------------------------------------------*/
    /* INITIALIZE CONTROL REGISTERS (FOR BIOS ONLY)                           */
    /*------------------------------------------------------------------------*/
    asm("      mvk     0,B3");
    asm("      mvc     B3,AMR");          /* addressing mode register */
    asm("      mvc     B3,CSR");          /* control status register */

    /*------------------------------------------------------------------------*/
    /* CALL BCACHE_bootInit function.  This function is determined by config  */
    /* and bound in the generated linker command file.                        */
    /*------------------------------------------------------------------------*/
    BCACHE_bootInit();

    /*------------------------------------------------------------------------*/
    /* GET THE POINTER TO THE AUTOINITIALIZATION TABLES INTO THE FIRST        */
    /* ARGUMENT REGISTER (A4)                                                 */
    /*------------------------------------------------------------------------*/
    asm("      .global cinit");
    asm("      mvkl cinit,A4");
    asm("      mvkh cinit,A4");

    /*------------------------------------------------------------------------*/
    /* PASS THE CURRENT DP TO THE AUTOINITIALIZATION ROUTINE.                 */
    /*------------------------------------------------------------------------*/
    asm("      mv   DP,B4");

    /*------------------------------------------------------------------------*/
    /* CALL THE AUTOINITIALIZATION ROUTINE.                                   */
    /*------------------------------------------------------------------------*/
    asm("      .global __auto_init");
    asm("      mvkl __auto_init, B0");
    asm("      mvkh __auto_init, B0");
    asm("      mvkl boot_ret, B3");
    asm("      mvkh boot_ret, B3");
    asm("      b    B0");
    asm("      nop  5");
    asm("boot_ret:");


    /*------------------------------------------------------------------------*/
    /* CALL THE USER'S PROGRAM.                                               */
    /*------------------------------------------------------------------------*/
    //   _C6Run_main();                 /* _C6Run_main(void); */
    asm("      .global __C6Run_main");
    asm("      mvkl __C6Run_main, B0");
    asm("      mvkh __C6Run_main, B0");
    asm("      b    B0");   
    asm("      mvkl main_ret, B3");
    asm("      mvkh main_ret, B3");
    asm("      nop  3");
    asm("main_ret:");

    /*------------------------------------------------------------------------*/
    /* START RUNTIME FOR BIOS.                                                */
    /* The call to BIOS_start does not return if the Task Manager is enabled  */
    /*------------------------------------------------------------------------*/
    BIOS_start();

    /*------------------------------------------------------------------------*/
    /* FALL INTO THE BIOS IDLE LOOP, NEVER RETURN.                            */
    /*------------------------------------------------------------------------*/
    IDL_loop();
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
