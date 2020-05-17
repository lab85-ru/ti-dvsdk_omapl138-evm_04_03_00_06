/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 * ======== autoinit.c ========
 *
 */

/*****************************************************************************/
/* _AUTO_INIT()   v3.00 - Perform initialization of C variables.            */
/*  Copyright (c) 1993-1998  Texas Instruments Incorporated                  */
/*****************************************************************************/

#pragma CODE_SECTION(_auto_init, ".bios")

#include <string.h>

#define ALIGN_MASK 0x7
#define ALIGN_PTR(ptr) \
   ((unsigned *)(((unsigned)ptr + ALIGN_MASK) & ~ALIGN_MASK))

typedef void (*PTRFUNC)();
extern far const PTRFUNC __pinit__[];
extern far void BIOS_init();

void _auto_init(const void *cinit, const unsigned int dp)
{
   const unsigned int *recptr = cinit;
   int length;

   if ((int)recptr != -1)
      while ((length = *recptr++) != 0)
      {
         if (length < 0)
         {
            int i;

            for (i = -length; i != 0; i-=4) 
            {
                unsigned int *to  = (unsigned int *) *recptr++;
                *to += dp; 
            }
            recptr = ALIGN_PTR(recptr);
         }
         else
         {
            char *to   = (void*) *recptr++; 
            char *from = (void*) recptr; 

            memcpy(to, from, length); 

            from   += length;
            recptr  = ALIGN_PTR(from);
         }
      }

   /*------------------------------------------------------------------------*/
   /* INITIALIZE THE RUNTIME ENVIRONMENT FOR BIOS                            */
   /*------------------------------------------------------------------------*/
   BIOS_init();

   /*------------------------------------------------------------------------*/
   /* Process Pinit table.                                                   */
   /* consists of pointers to init functions.                                */
   /* section is NULL terminated                                             */
   /* pointer is = -1 if section does not exist.                             */
   /*------------------------------------------------------------------------*/
   if ((int)__pinit__ != -1)
   {
      int i = 0;
      while (__pinit__[i] != NULL )
         __pinit__[i++]();
   }
}
