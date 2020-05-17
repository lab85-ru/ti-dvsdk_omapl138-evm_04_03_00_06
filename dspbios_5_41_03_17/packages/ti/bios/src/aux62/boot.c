/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== boot.c ========
 *  BIOS Boot routine C6200.
 *
 *  Derived from boot.c version 4.00 Alpha (01/11/00) rts.src
 *
 *   Entry points:
 *       _c_int00:   called at reset.
 *
 */

extern void exit(), _auto_init();               /* OTHER CALLS ARE NEAR      */

/*---------------------------------------------------------------------------*/
/* EXTERNAL BIOS SETUP FUNCTIONS                                             */
/*---------------------------------------------------------------------------*/
extern void IDL_loop();
/*
 *  BIOS_init and BIOS_start are located in .sysinit rather than .text, hence
 *  they may require far branch.
 */
extern void BIOS_start();

#ifdef _TMS320C6400_PLUS
extern far void BCACHE_bootInit();
#endif

/*---------------------------------------------------------------------------*/
/* BIOS C ENVIROMENT SETUP SYMBOLS                                           */
/*---------------------------------------------------------------------------*/
asm("args: .sect  \".args\"");        /* address of arguments space */

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

/*****************************************************************************/
/* C_INT00() - C ENVIRONMENT ENTRY POINT                                     */
/*****************************************************************************/
asm(" .sect \".sysinit\"");

/*
 * Put c_int00 in the .sysinit section.
 */
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
#if 0
/* already done above as first thing in c_int00 */
   asm("      mvc     B3,IER");          /* interrupt enable register */
#endif
   asm("      mvc     B3,CSR");          /* control status register */

   /*------------------------------------------------------------------------*/
   /* CALL BCACHE_bootInit function.  This function is determined by config  */
   /* and bound in the generated linker command file.                        */
   /*------------------------------------------------------------------------*/
#ifdef _TMS320C6400_PLUS
   BCACHE_bootInit();
#endif
  
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

   asm("      mvkl args,A0");
   asm("      mvkh args,A0");
   asm("      ldw *+A0[2],A6");          /* envp */
   asm("      ldw *+A0[1],B4");          /* argv */
   asm("      ldw *+A0[0],A4");          /* argc */

   /*------------------------------------------------------------------------*/
   /* CALL THE USER'S PROGRAM.                                               */
   /*------------------------------------------------------------------------*/
   //   main();                 /* main(A4, B4, A6); */
   asm("      .global _main");
   asm("      mvkl _main, B0");
   asm("      mvkh _main, B0");
   asm("      mvkl main_ret, B3");
   asm("      mvkh main_ret, B3");
   asm("      b    B0");
   asm("      nop  5");
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
