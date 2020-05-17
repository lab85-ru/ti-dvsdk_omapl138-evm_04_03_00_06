#ifndef __TIDEVICE_H__
#define __TIDEVICE_H__

#include <assert.h>
#include "std.h"
/**
 *  @file         tiDevice.h
 *  @author       Nandakumar Raghavan [nand@ti.com]
 *  @brief        device specific stuffs.
 *                has definitions for DM642 specific stuffs like cache, timer
 *                etc. 
 *  @version      version 1.00
 *  @date         15th December 2004
 *
 *  @remarks
 *  @paragraph
 *    Changes:
 *      When    :   1st March 2005
 *      Who     :   Nandakumar Raghavan [nand@ti.com]
 *      What    :   Changes to reflect the changes made to timer module in C6400+ hardware
 *  
 *  */
 

/* this macro will enable tracing within the decoder code base. We need a
 * common place to turn this on. XXX */
#ifndef _TMS320C6400_KERNEL
/* for host builds. */
//#define __DEBUG_TRACE__
#endif /* __DEBUG_TRACE__ */


#ifdef _TMS320C6400_KERNEL
//typedef double Double64x;
#else /* _TMS320C6400 */
#ifdef __GNUC__
//typedef long long int Double64x;
#else /* __GNUC__ */
#if 0
/* For VC, use int64 for double.  */
//typedef _int64 Double64x;
#endif // 0
#endif /* __GNUC__ */
#endif //_TMS320C6400_KERNEL



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "intrinsic.h"

#define INPUT
#define OUTPUT
#define RESERVED

/* hardware options: */
#ifdef _TMS320C6400
#define REGISTER(x)   (*((volatile unsigned int *)(x)))

/* Following are 6400 cache related */
/* MAR registers: */
#define MAR_128         (REGISTER(0x01848200))
#define MAR_129         (REGISTER(0x01848204))
#define MAR_130         (REGISTER(0x01848208))
#define MAR_131         (REGISTER(0x0184820C))
#define MAR_132         (REGISTER(0x01848210))
#define MAR_133         (REGISTER(0x01848214))
#define MAR_134         (REGISTER(0x01848218))
#define MAR_135         (REGISTER(0x0184821C))
#define MAR_136         (REGISTER(0x01848220))
#define MAR_137         (REGISTER(0x01848224))
#define MAR_138         (REGISTER(0x01848228))
#define MAR_139         (REGISTER(0x0184822C))
#define MAR_140         (REGISTER(0x01848230))
#define MAR_141         (REGISTER(0x01848234))
#define MAR_142         (REGISTER(0x01848238))
#define MAR_143         (REGISTER(0x0184823C))

extern Double64x addDouble( Double64x left, Double64x right);
extern Double64x subDouble( Double64x left, Double64x right);

/* The following macro enables benchmarking, and measures frame level duration
 * by default. */
//#define _BENCHMARKS

#ifdef _QUICKTURN_TESTS
#define _BENCHMARKS
#endif /* _QUICKTURN_TESTS */

/* Enables benchmarking for frame level components. like I/P/B frames, pre and
 * post processing. */
//#define _BENCHMARK_LEVEL_0

/* Enables benchmarks at frame levels etc, like loop filter  */
//#define _BENCHMARK_LEVEL_1

/* Enables even deeper benchmarks at MB levels in (predicted) frames */
//#define _BENCHMARK_LEVEL_2

#ifndef _BENCHMARKS
#undef _BENCHMARK_LEVEL_0
#undef _BENCHMARK_LEVEL_1
#undef _BENCHMARK_LEVEL_2
#endif /* ! _BENCHMARKS */

#ifndef _API_VERSION
#define _BMFILE
#endif /* ! _API_VERSION */

#endif /* _TMS320C6400 */

extern void TSC_enable( void);
extern Double64x TSC_read();

extern int timerSetup( unsigned int numTimers, unsigned int numIter);
extern int timerRestart( void);
extern unsigned int timerReset( unsigned int index);

#ifndef _BENCHMARKS
static inline void timerSet( unsigned int index) { return ; }
static inline unsigned int timerGet( unsigned int index) { return 0; }
extern int printBenchmarks( char *columnHeaders);
#endif /* ! _BENCHMARKS */

#ifdef _BENCHMARKS
#define SUCCESS      0
#define FAILURE     -1

#ifdef _BMFILE
#define BMFILENAME  "timer.txt"

typedef struct _tBmStruct {
  char    INPUT     *ipFileName;
  char    INPUT     *opFileName;
  char    INPUT     resolution[20];   // resolution of the sequence can be written as a string here. 
  float   INPUT     bitRate;
  FILE    RESERVED  *fpBmFile;
  char    RESERVED  bmFile[255];
} bmStruct;
extern bmStruct   bmInfo;
#endif /* _BMFILE */

typedef struct _tClocks {
  Double64x     temp;         /* temperory storage for timer */
  unsigned int  bucket;       /* actual accumulated time, which won't need 64 bit. */
  unsigned int  dummyToPackStructArray;
} clocks_t ;

extern unsigned int timerOverhead;
extern clocks_t *TMR_clocks;
extern unsigned int  *TMR_timer;
extern int  *TMR_timerIndices;
extern int  TMR_numTimers;
extern int  TMR_numIterations;

extern int sleepClocks( int clocks);
/** 
 * @note
*
*   a few words on the timer support:
*   data structure to the timer routines are clocks_t, clocks.  hash defines are
*   NUM_TIMERS, and NUM_IETRATIONS. for example, NUM_IETRATIONS would be equal to number of
*   frames in our case. NUM_TIMERS would be the different taps at which we need to take
*   timings.
*   there are three calls to the timer routines.
*   timerSetup  : this will reset all the objects to their initialization values.
*   timerSet    : turns on (opens) a timer bucket. till the next call the timerGet, with the
*                   same clocks address.
*   timerGet    : turns off (closes the lid) the timer tap and records the accumulated time. 
*   timerReset  : resets/flushes the timer bucket.
* */

static inline void timerSet( unsigned int index)
{
    if ( index >= TMR_numTimers)
        return ;
    assert( index < TMR_numTimers);
    TMR_clocks[ index].temp = TSC_read();
    return;
}

static inline unsigned int timerGet( unsigned int index)
{
  /** The new timer hardware's precision is 64 bit. Starting from 0, this can go for nearly
   * 585 years without overlapping. XXX We must make sure we start from 0 though, or at
   * least when the system starts this defaults to zero. */
  Double64x stop;
  unsigned int  result;
  stop = TSC_read();
  if ( index >= TMR_numTimers)
      return 0;
  assert( index < TMR_numTimers);
  stop = subDouble( stop, TMR_clocks[ index].temp);
  result = _lo( stop);
  if( result > timerOverhead)
    result -= timerOverhead;
  TMR_clocks[ index].bucket += result;      // accumulated time
  return( result);
}
#endif /* _BENCHMARKS */

/******************************************************************************
 * Fucntion Name      :   loop1000Cycles 
 * Description        :   loops (wastes time) 1000 cycles. no dependency on
 *                          cache, external memory etc. 
 * Parameters
 *                None
 * Return Values
 *                0
 * Errors
 *                None
 * Dependencies
 *                None
 * Assumptions        :   None
 * *****************************************************************************/ 
extern unsigned int  loop1000Cycles( void);


/******************************************************************************
 * Fucntion Name      :   sleepCycles1000 
 * Description        :   loops till the number of cycles are expired. So this
 *                          is not really 'sleep', but cycles wasted doing
 *                          looping. 
 * Parameters 
 *                numCycles2Sleep - number of 1000 cycles to be expired
 * Return Values
 *                always 0
 * Errors
 *                None
 * Dependencies
 *                This code is expected to compiled with optimization -o3 and
 *                  no debug. Any change to it could result in more cycles
 *                  wasted.
 *
 * Assumptions        :   See Dependencies above.
 * *****************************************************************************/ 
extern unsigned int sleepCycles1000( unsigned int numCycles2Sleep);

extern void setup_davinci_qdma_idma(void);
extern void enableEmifACaching( void);

#endif /* __TIDEVICE_H__ */
