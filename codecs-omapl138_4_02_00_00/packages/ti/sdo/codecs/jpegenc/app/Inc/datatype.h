#ifndef __DATATYPE_H__
#define __DATATYPE_H__

#define Const const

                     /**** GENERAL TYPES DEFINITIONS *******/
                     /**************************************/
#ifdef __TMS470__

/* Added by T.Furuse (2001/11/08) */
typedef enum Bool {
  False = 0,
  True = 1
} Bool;

/* Typedefs for the ARM7 */
typedef char                 Char       ;
typedef unsigned char        Byte       ;

typedef short                Int16       ;
typedef unsigned short       Uint16     ;
typedef long                 Int32       ;
typedef unsigned long        Uint32      ;    

typedef short                SInt       ;
typedef unsigned short       UInt       ;
typedef unsigned short       USInt      ;

typedef float                Float16      ;
typedef float                Float32     ;

#else

#ifdef _TMS320C5XX

/* Added by T.Furuse (2001/11/08) */
typedef enum Bool {
  False = 0,
  True = 1
} Bool;

/* Typedefs for the C54x DSP */
typedef char                 Char       ;
typedef unsigned char        Byte       ;
 
typedef short    int         Int16      ;
typedef unsigned short       Uint16     ;
typedef long                 Int32     ;
typedef unsigned long        Uint32;    

/* SCARR - These two don't really belong in DSP code, */
/*         but I'll leave them for now.               */
typedef float                Float16    ;
typedef double               Float32    ;

typedef int                  Int        ;
typedef unsigned int         UInt     ;
typedef short    int         SInt       ;
typedef unsigned short       USInt      ;
typedef long                 Long     ;
typedef unsigned long        ULong    ;    
typedef short                Short     ;
typedef unsigned short       UShort     ;
typedef Const    int         C_Int      ;
typedef Const    short       C_SInt     ;
typedef Const    UShort      C_USInt    ;
    
#endif /* _TMS320C5XX */
 
#endif /* __TMS470__ */

/* Common typedefs */
typedef void    Void     ;
typedef Uint16   VlcCodTab;
typedef Uint16   VlcDcdTab;
typedef Int16    VlcDcdCtl;


  
#endif /* __DATATYPE_H__ */
