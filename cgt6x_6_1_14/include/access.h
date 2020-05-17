/****************************************************************************/
/*  access.h         v6.1.14                                                 */
/*  Copyright (c) 1997-2010  Texas Instruments Incorporated                 */
/****************************************************************************/

#ifndef __EXTERN

   #undef __EXTERN
   #undef __INLINE
   #undef __STATIC

      #include <linkage.h>
      #ifdef __cplusplus
         #define __EXTERN extern "C" _CODE_ACCESS
      #else
         #define __EXTERN extern _CODE_ACCESS
      #endif
      #define __INLINE _IDECL
      #define __STATIC static _DATA_ACCESS
#endif


