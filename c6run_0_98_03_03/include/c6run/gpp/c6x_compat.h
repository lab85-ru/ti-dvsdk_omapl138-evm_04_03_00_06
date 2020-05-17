/*
 * c6x_compat.h
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
 
 
#ifndef _C6X_COMPAT_H_
#define _C6X_COMPAT_H_
 
 
// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

// Dummy macro for _nassert
#define _nassert(_ignore) ((void)0)

// Dummy macros for aligned and unaligned mem intrinsics
#define _amem2(P)         *((unsigned short *)P)
#define _amem2_const(P)   *((const unsigned short *)P)
#define _amem4(P)         *((unsigned *)P)
#define _amem4_const(P)   *((const unsigned *)P)
#define _amem8(P)         *((long long *)P)
#define _amem8_const(P)   *((const long long *)P)
#define _amemd8(P)        *((double *)P)
#define _amemd8_const(P)  *((const double *)P)

#define _mem2(P)          *((unsigned short *)P)
#define _mem2_const(P)    *((const unsigned short *)P)
#define _mem4(P)          *((unsigned *)P)
#define _mem4_const(P)    *((const unsigned *)P)
#define _mem8(P)          *((long long *)P)
#define _mem8_const(P)    *((const long long *)P)
#define _memd8(P)         *((double *)P)
#define _memd8_const(P)   *((const double *)P)


/*****************************************************************************/
/* DATA_IS_ALIGNED_2, DATA_IS_ALIGNED_4, DATA_IS_ALIGNED_8 -                 */
/*     Tell the compiler that data is already aligned to a 2-byte, 4-byte    */
/*     or 8-byte boundary.  Note: this macro does not change the             */
/*     alignment of data.  Use DATA_ALIGN to change alignment.               */
/*****************************************************************************/
#define DATA_IS_ALIGNED_2(x)	(_nassert(((unsigned int)(x) & 0x1) == 0))
#define DATA_IS_ALIGNED_4(x)	(_nassert(((unsigned int)(x) & 0x3) == 0))
#define DATA_IS_ALIGNED_8(x)	(_nassert(((unsigned int)(x) & 0x7) == 0))


/*****************************************************************************/
/* SAVE_AMR -                                                                */
/*     Define a local 'volatile unsigned int' variable in your interrupt     */
/*     routine.                                                              */
/*     When invoking this macro, pass that local variable to save the AMR.   */
/*                                                                           */
/*     If you interrupted an assembly coded routine that may be using        */
/*     circular addressing, and you interrupt into a C coded interrupt       */
/*     service routine, you need to set the AMR to 0 for the C code and save */
/*     off the AMR register, so that it will have the correct value upon     */
/*     leaving the C interrupt service routine and returning to the assembly */
/*     code.                                                                 */
/*                                                                           */
/*     Add this routine immediately after your local variable definitions    */
/*     and before the start of your C interrupt code.                        */
/*****************************************************************************/
#define SAVE_AMR(temp_AMR)                                                    \
        do {                                                                  \
        } while (0)

/*****************************************************************************/
/* RESTORE_AMR -                                                             */
/*    When invoking this macro, pass the same local variable that was passed */
/*    to the SAVE_AMR macro.  This macro will restore the AMR to the value   */
/*    it had when interrupted out of the hand assembly routine.              */
/*                                                                           */
/*    Add this macro immediately before exiting the C interrupt service      */
/*    routine.                                                               */ 
/*****************************************************************************/
#define RESTORE_AMR(temp_AMR)                                                 \
        do {                                                                  \
        } while (0)

/*****************************************************************************/
/* SAVE_SAT -                                                                */
/*     Define a local 'volatile unsigned int' variable in your interrupt     */
/*     routine.                                                              */
/*     When invoking this macro, pass that local variable to save the SAT    */
/*     bit.                                                                  */
/*                                                                           */
/*     If you interrupted a routine that was performing saturated arithmetic */
/*     and the interrupt service routine is also performing saturated        */
/*     arithmetic, then you must save and restore the SAT bit in your        */
/*     interrupt service routine.                                            */
/*                                                                           */
/*     Add this routine immediately after your local variable definitions    */
/*     and before the start of your C interrupt code.                        */
/*****************************************************************************/
#define SAVE_SAT(temp_SAT)                                                    \
        do {                                                                  \
        } while (0)

/*****************************************************************************/
/* RESTORE_SAT -                                                             */
/*    When invoking this macro, pass the same local variable that was passed */
/*    to the SAVE_SAT macro.  This macro will restore the SAT bit to the     */
/*    value it had when your application was interrupted.                    */
/*                                                                           */
/*    Add this macro immediately before exiting the C interrupt service      */
/*    routine.                                                               */ 
/*****************************************************************************/
#define RESTORE_SAT(temp_SAT)                                                 \
        do {                                                                  \
        } while (0)


/***********************************************************
* Global Typedef Declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern volatile unsigned int AMR;
extern volatile unsigned int CSR;
extern volatile unsigned int IFR;
extern volatile unsigned int ISR;
extern volatile unsigned int ICR;
extern volatile unsigned int IER;
extern volatile unsigned int ISTP;
extern volatile unsigned int IRP;
extern volatile unsigned int NRP;

#if defined(_TMS320C6400) || defined(_TMS320C6740)
extern volatile unsigned int GFPGFR;
extern volatile unsigned int DIER;
#endif

#ifdef _TMS320C6700
extern volatile unsigned int FADCR;
extern volatile unsigned int FAUCR;
extern volatile unsigned int FMCR;
#endif

#ifdef _TMS320C6700_PLUS
extern volatile unsigned int DESR;
extern volatile unsigned int DETR;
#endif


#if defined(_TMS320C6400_PLUS) || defined(_TMS320C6740)
extern volatile unsigned int REP;
extern volatile unsigned int TSCL;
extern volatile unsigned int TSCH;
extern volatile unsigned int ARP;
extern volatile unsigned int ILC;
extern volatile unsigned int RILC;
extern volatile unsigned int PCE1;
extern volatile unsigned int DNUM;
extern volatile unsigned int SSR;
extern volatile unsigned int GPLYA;
extern volatile unsigned int GPLYB;
extern volatile unsigned int TSR;
extern volatile unsigned int ITSR;
extern volatile unsigned int NTSR;
extern volatile unsigned int ECR;
extern volatile unsigned int EFR;
extern volatile unsigned int IERR;

extern volatile unsigned int DMSG;
extern volatile unsigned int CMSG;
extern volatile unsigned int DT_DMA_ADDR;
extern volatile unsigned int DT_DMA_DATA;
extern volatile unsigned int DT_DMA_CNTL;
extern volatile unsigned int TCU_CNTL;
extern volatile unsigned int RTDX_REC_CNTL;
extern volatile unsigned int RTDX_XMT_CNTL;
extern volatile unsigned int RTDX_CFG;
extern volatile unsigned int RTDX_RDATA;
extern volatile unsigned int RTDX_WDATA;
extern volatile unsigned int RTDX_RADDR;
extern volatile unsigned int RTDX_WADDR;
extern volatile unsigned int MFREG0;
extern volatile unsigned int DBG_STAT;
extern volatile unsigned int BRK_EN;
extern volatile unsigned int HWBP0_CNT;
extern volatile unsigned int HWBP0;
extern volatile unsigned int HWBP1;
extern volatile unsigned int HWBP2;
extern volatile unsigned int HWBP3;
extern volatile unsigned int OVERLAY;
extern volatile unsigned int PC_PROF;
extern volatile unsigned int ATSR;
extern volatile unsigned int TRR;
extern volatile unsigned int TCRR;
#endif



/***********************************************************
* Global Function Declarations                             *
***********************************************************/

// Generic C6000 intrinsics
extern unsigned  _extu	   (unsigned, unsigned, unsigned);
extern int       _ext	   (int,      unsigned, unsigned);
extern unsigned  _set	   (unsigned, unsigned, unsigned);
extern unsigned  _clr	   (unsigned, unsigned, unsigned);
extern unsigned  _extur   (unsigned, int);
extern int       _extr	   (int,      int);
extern unsigned  _setr	   (unsigned, int);
extern unsigned  _clrr	   (unsigned, int);
extern int       _sadd	   (int,      int);
extern int	  _ssub	   (int,      int);
extern int       _sshl	   (int,      unsigned);
extern int	  _add2	   (int,      int);
extern int	  _sub2	   (int,      int);
extern unsigned  _subc	   (unsigned, unsigned);
extern unsigned  _lmbd	   (unsigned, unsigned);
extern int       _abs	   (int);
extern long      _labs	   (long);
extern unsigned  _norm	   (int);
extern int	  _smpy	   (int,      int);
extern int	  _smpyhl  (int,      int);
extern int	  _smpylh  (int,      int);
extern int	  _smpyh   (int,      int);
extern int	  _mpy	   (int,      int);
extern int	  _mpyus   (unsigned, int);
extern int	  _mpysu   (int,      unsigned);
extern unsigned  _mpyu	   (unsigned, unsigned);
extern int	  _mpyhl   (int,      int);
extern int	  _mpyhuls (unsigned, int);
extern int	  _mpyhslu (int,      unsigned);
extern unsigned  _mpyhlu  (unsigned, unsigned);
extern int	  _mpylh   (int,      int);
extern int	  _mpyluhs (unsigned, int);
extern int	  _mpylshu (int,      unsigned);
extern unsigned  _mpylhu  (unsigned, unsigned);
extern int	  _mpyh	   (int,      int);
extern int	  _mpyhus  (unsigned, int);
extern int	  _mpyhsu  (int,      unsigned);
extern unsigned  _mpyhu   (unsigned, unsigned);
#ifdef __TI_40BIT_LONG__
extern long      _lsadd   (int,      long);
extern long	  _lssub   (int,      long);
extern int       _sat	   (long);
extern unsigned  _lnorm   (long);
#else
extern long long _lsadd   (int,      long long);
extern long long _lssub   (int,      long long);
extern int       _sat	   (long long);
extern unsigned  _lnorm   (long long);
extern long long          _add40_s32 (int,      int);
extern unsigned long long _add40_u32 (unsigned, unsigned);
extern long long          _add40_s40 (int,      long long);
extern unsigned long long _add40_u40 (unsigned, unsigned long long);
extern int                _cmpeq40   (int, long long);
extern int                _cmpgt40   (int, long long);
extern int                _cmplt40   (int, long long);
extern int                _cmpltu40  (int, long long);
extern int                _cmpgtu40  (int, long long);
extern long long          _mov40     (long long);
extern long long          _neg40     (long long);
extern long long          _labs40    (long long);
extern long long          _shl40     (long long, int);
extern long long          _shr40     (long long, int);
extern unsigned long long _shru40    (unsigned long long, int);
extern unsigned long long _shl40_s32 (int      , int);
extern long long          _sub40_s32 (int      , int);
extern unsigned long long _sub40_u32 (unsigned , unsigned);
extern long long          _sub40_s40 (int,       long long);
extern long long          _zero40    ();
#endif
extern unsigned  _hi(double);      /* return the hi 32 bits of a double as an int    */
extern float     _hif(double);     /* return the hi 32 bits of a double as a float   */
extern unsigned  _hill(long long); /* return the hi 32 bits of a long long as an int */
extern unsigned  _lo(double);      /* return the lo 32 bits of a double as an int    */
extern float     _lof(double);     /* return the hi 32 bits of a double as a float   */
extern unsigned  _loll(long long); /* return the lo 32 bits of a long long as an int */
extern double 	  _itod(unsigned, unsigned);  /* Create a double from 2 ints    */
extern double 	  _ftod(float,    float);     /* Create a double from 2 floats  */
extern long long _itoll(unsigned, unsigned); /* Create a long long from 2 ints */
extern float  	  _itof(unsigned);            /* Create a float from an int     */
extern unsigned  _ftoi(float);               /* Create an int from a float     */



// C67x and C6740 intrinsics (floating point)
extern double    _fabs    (double);
extern float     _fabsf   (float);
extern double    _mpyid   (int,      int);
extern long long _mpyidll (int,      int);
extern int    	  _spint   (float);
extern int    	  _dpint   (double);
extern float  	  _rcpsp   (float);
extern double 	  _rcpdp   (double);
extern float  	  _rsqrsp  (float);
extern double 	  _rsqrdp  (double);



// C64x and C6740 intrinsics
extern int       _add4      (int,      int);
extern int       _avg2      (int,      int);
extern unsigned  _avgu4     (unsigned, unsigned);
extern int       _cmpeq2    (int,      int);
extern int       _cmpeq4    (int,      int);
extern int       _cmpgt2    (int,      int);
extern unsigned  _cmpgtu4   (unsigned, unsigned);
extern int       _dotp2     (int,      int);
extern int       _dotpn2    (int,      int);
extern int       _dotpnrsu2 (int,      unsigned);
extern int       _dotprsu2  (int,      unsigned);
extern int       _dotpsu4   (int,      unsigned);
extern unsigned  _dotpu4    (unsigned, unsigned);
extern int       _gmpy4     (int,      int);
#ifdef __TI_40BIT_LONG__
extern long      _ldotp2    (int,      int);
#else
extern long long _ldotp2    (int,      int);
#endif
extern int       _max2      (int,      int);
extern unsigned  _maxu4     (unsigned, unsigned);
extern int       _min2      (int,      int);
extern unsigned  _minu4     (unsigned, unsigned);
extern double 	  _mpy2      (int,      int);
extern long long _mpy2ll    (int,      int);
extern double 	  _mpyhi     (int,      int);
extern long long _mpyhill   (int,      int);
extern int       _mpyhir    (int,      int);
extern double    _mpyli     (int,      int);
extern long long _mpylill   (int,      int);
extern int       _mpylir    (int,      int);
extern double    _mpysu4    (int,      unsigned);
extern long long _mpysu4ll  (int,      unsigned);
extern double    _mpyu4     (unsigned, unsigned);
extern long long _mpyu4ll   (unsigned, unsigned);
extern unsigned  _pack2     (unsigned, unsigned);
extern unsigned  _packh2    (unsigned, unsigned);
extern unsigned  _packh4    (unsigned, unsigned);
extern unsigned  _packhl2   (unsigned, unsigned);
extern unsigned  _packl4    (unsigned, unsigned);
extern unsigned  _packlh2   (unsigned, unsigned);
extern unsigned  _rotl      (unsigned, unsigned);
extern int       _sadd2     (int,      int);
extern unsigned  _saddu4    (unsigned, unsigned);
extern int       _saddus2   (unsigned, int);
extern unsigned  _shlmb     (unsigned, unsigned);
extern int       _shr2      (int,      unsigned);
extern unsigned  _shrmb     (unsigned, unsigned);
extern unsigned  _shru2     (unsigned, unsigned);
extern double    _smpy2     (int,      int);
extern long long _smpy2ll   (int,      int);
extern int       _spack2    (int,      int);
extern unsigned  _spacku4   (int,      int);
extern int       _sshvl     (int,      int);
extern int       _sshvr     (int,      int);
extern int       _sub4      (int,      int);
extern int       _subabs4   (int,      int);
     
extern int       _abs2      (int);
extern unsigned  _bitc4     (unsigned);
extern unsigned  _bitr      (unsigned);
extern unsigned  _deal      (unsigned);
extern int       _mvd       (int);
extern unsigned  _shfl      (unsigned);
extern unsigned  _swap4     (unsigned);
extern unsigned  _unpkhu4   (unsigned);
extern unsigned  _unpklu4   (unsigned);
extern unsigned  _xpnd2     (unsigned);
extern unsigned  _xpnd4     (unsigned);



// C64x+ and C6740 intrinsics
extern long long _addsub    (int,       int);
extern long long _addsub2   (unsigned,  unsigned);
extern long long _cmpy      (unsigned,  unsigned);
extern unsigned  _cmpyr     (unsigned,  unsigned);
extern unsigned  _cmpyr1    (unsigned,  unsigned);
extern long long _ddotph2   (long long, unsigned);
extern unsigned  _ddotph2r  (long long, unsigned);
extern long long _ddotpl2   (long long, unsigned);
extern unsigned  _ddotpl2r  (long long, unsigned);
extern long long _ddotp4    (unsigned,  unsigned);
extern long long _dpack2    (unsigned,  unsigned);
extern long long _dpackx2   (unsigned,  unsigned);
extern long long _dmv       (unsigned,  unsigned);
extern double    _fdmv      (float,     float);
extern unsigned  _gmpy      (unsigned,  unsigned);
extern long long _mpy32ll   (int,       int);
extern int       _mpy32     (int,       int);
extern long long _mpy32su   (int,       unsigned);
extern long long _mpy32us   (unsigned,  int);
extern long long _mpy32u    (unsigned,  unsigned);
extern long long _mpy2ir    (unsigned,  int);
extern unsigned  _rpack2    (unsigned,  unsigned);
extern long long _saddsub   (int,       int);
extern long long _saddsub2  (unsigned,  unsigned);
extern long long _shfl3     (unsigned,  unsigned);
extern int       _smpy32    (int,       int);
extern int       _ssub2     (int,       int);
extern unsigned  _xormpy    (unsigned,  unsigned);


// Interrupt/Atomic intrinsics
extern unsigned int _disable_interrupts ( );
extern unsigned int _enable_interrupts ( );
extern void _restore_interupts (unsigned int);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_C6X_COMPAT_H_
