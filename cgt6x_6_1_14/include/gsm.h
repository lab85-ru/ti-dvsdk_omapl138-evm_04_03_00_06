/*****************************************************************************/
/*  GSM.H v6.1.14                                                             */
/*  Copyright (c) 1998-2010 Texas Instruments Incorporated                   */
/*****************************************************************************/
#ifndef _GSMHDR
#define _GSMHDR

#include <stdlib.h>
#include <linkage.h> 

#define MAX_32 (int)0x7fffffffL
#define MIN_32 (int)0x80000000L

#define MAX_16 (short)0x7fff
#define MIN_16 (short)0x8000

extern _DATA_ACCESS int Overflow;
extern _DATA_ACCESS int Carry;

/******************************************************************************/
/* Macros for GSM ETSI math operations                                        */
/*   Note: The shift ETSI operations defined here do not support shift        */
/*         amounts larger than 31.  For instance, if you code calls --        */
/*         "L_shl(x, 500);" these macros are not designed to handle a SSHL    */
/*         by 500 bits.  These macros can be modified accordingly for these   */
/*         situations.                                                        */
/******************************************************************************/
#define L_add(a,b)     (_sadd((a),(b)))             /* int sat addition       */
#define L_sub(a,b)     (_ssub((a),(b)))             /* int sat subtract       */
#define L_sub_c(a,b)   L_add_c((a),~(b))            /* integer subtraction    */
#define L_negate(a)    (_ssub(0,(a)))               /* integer negation       */
#define L_deposit_h(a) ((a)<<16)                    /* put short in upper 16  */
#define L_deposit_l(a) ((int)(a))                   /* put short in lower 16  */
#define L_abs(a)       (_abs(a))                    /* int absolute value     */
#define L_mult(a,b)    (_smpy((a),(b)))             /* short sat mpy => 32    */
#define L_mac(a,b,c)   (_sadd((a),L_mult(b, c)))    /* saturated mpy & accum  */
#define L_macNs(a,b,c) L_add_c((a),L_mult(b,c))     /* mpy & accum w/o saturat*/
#define L_msu(a,b,c)   (_ssub((a),L_mult(b,c)))     /* saturated mpy & sub    */
#define L_msuNs(a,b,c) L_sub_c(a,L_mult(b,c))       /* mpy & sub w/o saturate */
#define L_shl(a,b)     ((b) < 0 ? (a) >> -(b) : _sshl((a),(b))) 
#define L_shr(a,b)     ((b) < 0 ? _sshl((a),-(b)) : (a) >> (b))
#define L_shr_r(a,b)   (L_shr((a),(b)) + ((b)>0 && (((a) & (1<<((b)-1))) != 0)))
#define abs_s(a)       (_abs((a)<<16)>>16)          /* short absolute value   */
#define add(a,b)       (_sadd((a)<<16,(b)<<16)>>16) /* short sat add          */
#define sub(a,b)       (_ssub((a)<<16,(b)<<16)>>16) /* short sat subtract     */
#define extract_h(a)   ((unsigned)(a)>>16)          /* extract upper 16 bits  */
#define extract_l(a)   ((a)&0xffff)                 /* extract lower 16 bits  */
#define round(a)       extract_h(_sadd((a),0x8000)) /* round                  */
#define mac_r(a,b,c)   (round(L_mac(a,b,c))) 	    /* mac w/ rounding        */
#define msu_r(a,b,c)   (round(L_msu(a,b,c)))        /* msu w/ rounding        */
#define mult_r(a,b)    (round(L_mult(a,b)))         /* sat mpy w/ round       */
#define mult(a,b)      (L_mult(a,b)>>16)            /* short sat mpy upper 16 */
#define norm_l(a)      (_norm(a))                   /* return NORM of int     */
#define norm_s(a)      (_norm(a)-16)                /* return NORM of short   */
#define negate(a)      (_ssub(0, ((a)<<16)) >> 16)  /* short sat negate       */
#define shl(a,b)       ((b) < 0 ? (a) >> -(b) : (_sshl((a),((b)+16))>>16)) 
#define shr(a,b)       ((b) < 0 ? (_sshl((a),(-(b)+16))>>16) : ((a) >> (b))) 
#define shr_r(a,b)     ((b) < 0 ? (_sshl((a),(-(b)+16))>>16) : (b)==0 ? (a) : \
                                    ((a)+(1<<((b)-1))) >> (b)) 

/******************************************************************************/
/* For C++ code, place these function names in the C name space.              */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

_IDECL int   L_add_c (int, int);
_IDECL int   L_sat   (int);
_IDECL short div_s   (short, short);


#ifdef _INLINE
/******************************************************************************/
/* Integer (32-bit) add with carry and overflow testing.                      */
/******************************************************************************/
static inline int L_add_c (int L_var1, int L_var2)
{   
    unsigned int uv1    = L_var1;       
    unsigned int uv2    = L_var2;
    int          cin    = Carry;
    unsigned int result = uv1 + uv2 + cin;
    
    Carry     = ((~result & (uv1 | uv2)) | (uv1 & uv2)) >> 31;
    Overflow  = ((~(uv1 ^ uv2)) & (uv1 ^ result)) >> 31;

    if (cin && result == 0x80000000) Overflow = 1;

    return (int) result;
}       

/******************************************************************************/
/* Saturate any result after L_add_c or L_sub_c if overflow is set.           */
/******************************************************************************/
static inline int L_sat (int L_var1)
{
    int cin = Carry;

    return !Overflow ? L_var1 : (Carry = Overflow = 0, 0x7fffffff+cin);
}

/******************************************************************************/
/* Short (16-bit) divide.                                                     */
/******************************************************************************/
static inline short div_s (short var1, short var2)
{   
    int          iteration;         
    unsigned int var1int;
    int          var2int;

    if (var1 == 0)    return 0;
    if (var1 == var2) return 0x7fff;
    var1int = var1 << 16;                
    var2int = var2 << 16;
    
    for (iteration = 0; iteration < 16; iteration++)
        var1int = _subc(var1int,var2int);

    return var1int & 0xffff;
}

#endif /* _INLINE */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !_GSMHDR */
