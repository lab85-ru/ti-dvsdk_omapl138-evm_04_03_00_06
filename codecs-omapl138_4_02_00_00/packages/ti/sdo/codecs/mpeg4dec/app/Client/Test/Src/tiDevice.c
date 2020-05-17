
/**
 *  @file         tiDevice.c
 *  @author       Nandakumar Raghavan [nand@ti.com]
 *  @brief        device specific functions for cache and timer 
 *                Has a benchmarking library built in here, controlled by
 *                macroes and a few runtime parameters. Easy to use but
 *                difficult to customize. 
 *  @version      version 1.00
 *  @date         15th December 2004
 *  @note @b On @b benchmarking @b library
 *
 *    The benchmarking library works with an initialization call to timerSetup
 *    with numTimers and numIter as parameters. For example, if we need to
 *    measure timing across two operations and for the next 1024 frames
 *    (iterations), we call it as \c timerSetup( 2, 1024); \c
 *    Then timerSet(0), will kick off measuring timing in the first register
 *    till the call to timerGet(0). This can be repeated many times at multiple
 *    places. When the data has been collected for one iteration call
 *    timerReset(0), which will start collecting data in the next iteration
 *    register for timerSet calls. 
 *    In the end when all the iterations' data has been collected a call to
 *    printBenchmarks will print the data into into a csv format. The Browsing
 *    the individual functions will provide more information. 
 *
 *    imagine timers as a set of registers formaed in a two dimensional array
 *    form >-> timers[ numTimers][numIter]; Every call to timerReset will
 *    point to next register in the single dimension of the array, and
 *    parameter to timer[SG]et or timerReset will decide the second dimension,
 *    as shown below.
 *
 *  @verbatim
      timerSetup( 2, 1024)      # during initialization *
      ....
      ...
      timerSet(0);
      doSomeThing1();
      timerGet(0);
      ...
      ...
      timerSet(0);
      doSomeThing2();
      timerGet(0);
      ...
      ...
      timerReset(0);   # here time taken for doSomeThing1 and doSomeThing2
                       # will be added and kept in timers[0][0]
      @endverbatim
 *    
 *  */
 
#include <stdio.h>
#include <time.h>
#include "intrinsic.h"
#include "tiDevice.h"


#ifndef _TMS320C6400_KERNEL
#undef _BENCHMARKS
#endif /* _TMS320C6400 */

#ifdef _BENCHMARKS

/* This macro enable printing out of the profiling info */
#define _BMFILE

unsigned int timerOverhead;
clocks_t *TMR_clocks;
unsigned int  *TMR_timer;
int  *TMR_timerIndices;
int  TMR_numTimers = 0;
int  TMR_numIterations = 0;

/**
 *  @fn         int timerRestart( void)
 *  @brief      restarts the benchmarking library.
 *  @param      none
 *  @exception  Fatal errors can happen if the globals are wrong.
 *  @warning    assumed that all the relevant globals are properly initialized
 *              and allocated, else illegal memory access can happen.
 *  @return     0 (success) is returned all the time.
 *  */
 
int timerRestart( void)
{
  memset( TMR_timerIndices, 0, TMR_numTimers * sizeof(int));
  memset( TMR_clocks, 0, TMR_numTimers * sizeof( clocks_t));
  memset( TMR_timer, 0, TMR_numTimers * TMR_numIterations * sizeof( unsigned int));
  return 0;
} /* timerRestart */

/**
 *  @fn         int timerSetup( unsigned int numTimers, unsigned int numIter)
 *  @brief      initialize the benchmarking library.
 *  @param[in]  numTimers - number of benchmarking instances
 *  @param[in]  numIter - number of iterations all these (numTimers) timers are
 *                          collecting data
 *  @exception  errors can happen timers are used by multiple routines. This
 *              won't be known or flagged, because there is no way of knowing
 *              it other than build time. (BIOS uses timer 0. timer 2 is safe,
 *              usually)
 *  @warning    assumed that the timer used by the benchmarking module is not
 *              used by anyone else.
 *  @return     SUCCESS, or FAILURE in case of failed malloc's.
 *  */
 
int timerSetup( unsigned int numTimers, unsigned int numIter)
{
  TMR_numTimers = numTimers;
  TMR_numIterations = numIter;

  TSC_enable();
  
  TMR_timerIndices = (int *)malloc( TMR_numTimers * sizeof(int));
  TMR_clocks = (clocks_t*) malloc( TMR_numTimers * sizeof( clocks_t));
  TMR_timer = ( unsigned int *) malloc( TMR_numTimers * TMR_numIterations * sizeof( unsigned int));
  if( !TMR_timerIndices || !TMR_clocks || !TMR_timer) {
    /* allocation failure */
    fprintf( stderr, " benchmarks: memory allocation failures\n");
    return FAILURE;
  }

  memset( TMR_timerIndices, 0, TMR_numTimers * sizeof(int));
  memset( TMR_clocks, 0, TMR_numTimers * sizeof( clocks_t));
  memset( TMR_timer, 0, TMR_numTimers * TMR_numIterations * sizeof( unsigned int));

  timerSet(0);
  timerOverhead = timerGet(0);
  memset( TMR_clocks, 0, TMR_numTimers * sizeof( clocks_t)); //reset TMR_clocks

  return SUCCESS;
}

/**
 *  @fn         unsigned int timerReset( unsigned int index)
 *  @brief      resets the respective register and go for next. See the note in
 *              the beginning of the file
 *  @param[in]  index - which timer are we resetting.
 *  @exception  can flash an error if called more than numIter times
 *  @warning    assumed that the above error won't happen ;)
 *  @return     returns the time accumulated in this "register"
 *  */
unsigned int timerReset( unsigned int index)
{
  if( (index >= TMR_numTimers) || (TMR_timerIndices[ index] >= TMR_numIterations)) {
#ifdef _TIMER_DEBUG_    
    if(index >= TMR_numTimers) {
      fprintf( stderr, "only %d timers initialized, but tried to use %dth one.\n", 
                      TMR_numTimers, index+1);
    }
    if(TMR_timerIndices[ index] >= TMR_numIterations) {
      fprintf( stderr, "only %d timer buckets initialized, but tried to use %dth one on timer%d .\n", 
                      TMR_numIterations, TMR_timerIndices[ index] + 1, index);
    }
#endif /* _TIMER_DEBUG_ */    
    return (unsigned int) -1;
  }
  TMR_timer[ index * TMR_numIterations + TMR_timerIndices[ index]] = TMR_clocks[ index].bucket;
  TMR_clocks[ index].bucket = 0;
  TMR_timerIndices[ index]++;

  return TMR_timer[ index * TMR_numIterations + TMR_timerIndices[ index] - 1];
}

#ifdef _BMFILE
bmStruct  bmInfo;

/**
 *  @fn         int printBenchmarks( char columnHeaders)
 *  @brief      prints all the benchmark info collected so far. 
 *              need to be called at the end of the benchmarking session.
 *              prints average, minimum, maximum and a bunch of other info
 *              about the data collected so far.
 *  @param[in]  columnHeaders -> a comma separated column headers for the
 *              different timing durations.
 *  @exception  errors can happen if so and so.
 *  @warning    assumed that so and so.
 *  @return     this is returned by functionName
 *  */
int printBenchmarks( char columnHeaders)
{
  unsigned int index1, index2, numValues, numIter = 0;
  float average, min, max;
  FILE *fp;
	
	strcpy( bmInfo.bmFile, BMFILENAME);
	fp = fopen("timer.txt","a");
	if(fp==NULL)
		printf("ERROR IN BENCHMARK FILE OPEN");

#define _DETAILED_BENCHMARKS
#ifdef _DETAILED_BENCHMARKS
  fprintf( fp, "\nFrame Num,%s\n", columnHeaders);

  {
    numIter = TMR_numIterations;
    /* Sometimes, we declare so many iterations during compile time, but won't use it
     * during runtime. To take care of this: */
    numIter = TMR_timerIndices[ 0];
    for( index1 = 1; index1 < TMR_numTimers; index1++) {
      if( numIter < TMR_timerIndices[ index1]) {
        numIter = TMR_timerIndices[ index1];
      }
    }
  }
  
  for( index1 = 0; index1 < numIter; index1++) {
    fprintf( fp, "\n%d,", index1);
    for( index2 = 0; index2 < TMR_numTimers; index2++) {
      fprintf( fp, "%10.3f,", ((float)TMR_timer[ (index2 * TMR_numIterations) + index1]) / 1000.0 );
    }
  }
#endif /* _DETAILED_BENCHMARKS */  
  
  for( index2 = 0; index2 < TMR_numTimers; index2++) {
    min = 9.0e23; // some large number.
    numValues = 0;
    average = 0.0;
    max = 0.0;
    for( index1 = 0; index1 < TMR_timerIndices[index2]; index1++) {
      unsigned int  ticks;
      float instance;
      ticks = TMR_timer[ (index2 * TMR_numIterations) + index1];
      instance = (float)ticks;
      min = ( min > instance) ? instance : min;
      max = ( max > instance) ? max : instance;
      average += instance;
      /* When calculating average, we have to ignore iterations where this
       * timer was not invoked. For example, in P frame average, we should
       * ignore all I frame iterations. This is done assuming that every zero
       * in the array of ticks corresponds to a case like that. */
      if( ticks) {
        numValues++;
      }
    }
    if( numValues) {
      average /= (float) numValues;
    }
    min /= 1000.0;
    max /= 1000.0;
    average /= 1000.0;
    fprintf( fp, "%10.3f, %10.3f, %10.3f,", min, max, average);
    fprintf( stdout, "%10.3f\t %10.3f\t %10.3f", min, max, average);
  }

  fflush( stdout);
  fflush( fp);
#ifndef _QUICKTURN_TESTS
  fclose( fp);
#endif /* ! _QUICKTURN_TESTS */
  printf( "\n instrumentation data written to %s\n", bmInfo.bmFile);
  return 0;
}

 
int printBenchmarks_old( char columnHeaders)
{
  unsigned int index1, index2, numValues, numIter = 0;
  float average, min, max;
#ifdef _QUICKTURN_TESTS
  bmInfo.fpBmFile = stdout;
#else /* _QUICKTURN_TESTS */
  strcpy( bmInfo.bmFile, BMFILENAME);
  bmInfo.fpBmFile = fopen( bmInfo.bmFile, "a");
  if( !bmInfo.fpBmFile) {
    fprintf( stderr, " benchmarking enabled, but failed to open file : %s\n", bmInfo.bmFile);
    return -1;
  }
#endif /* _QUICKTURN_TESTS */
  /* Build version is not required all the time *
   * fprintf( bmInfo.fpBmFile, "\n\nBuild Version : %s", buildVersion); */

  fprintf( bmInfo.fpBmFile, "\n input file name   : %s", bmInfo.ipFileName);
  fprintf( bmInfo.fpBmFile, "\n output file name  : %s", bmInfo.opFileName);
  fprintf( bmInfo.fpBmFile, "%10.3f,", bmInfo.bitRate);
#define _DETAILED_BENCHMARKS
#ifdef _DETAILED_BENCHMARKS
  fprintf( bmInfo.fpBmFile, "\nFrame Num,%s\n", columnHeaders);

  {
    numIter = TMR_numIterations;
    /* Sometimes, we declare so many iterations during compile time, but won't use it
     * during runtime. To take care of this: */
    numIter = TMR_timerIndices[ 0];
    for( index1 = 1; index1 < TMR_numTimers; index1++) {
      if( numIter < TMR_timerIndices[ index1]) {
        numIter = TMR_timerIndices[ index1];
      }
    }
  }
  
  for( index1 = 0; index1 < numIter; index1++) {
    fprintf( bmInfo.fpBmFile, "\n%d,", index1);
    for( index2 = 0; index2 < TMR_numTimers; index2++) {
      fprintf( bmInfo.fpBmFile, "%10.3f,", ((float)TMR_timer[ (index2 * TMR_numIterations) + index1]) / 1000.0 );
    }
  }
#endif /* _DETAILED_BENCHMARKS */  
  
  for( index2 = 0; index2 < TMR_numTimers; index2++) {
    min = 9.0e23; // some large number.
    numValues = 0;
    average = 0.0;
    max = 0.0;
    for( index1 = 0; index1 < TMR_timerIndices[index2]; index1++) {
      unsigned int  ticks;
      float instance;
      ticks = TMR_timer[ (index2 * TMR_numIterations) + index1];
      instance = (float)ticks;
      min = ( min > instance) ? instance : min;
      max = ( max > instance) ? max : instance;
      average += instance;
      /* When calculating average, we have to ignore iterations where this
       * timer was not invoked. For example, in P frame average, we should
       * ignore all I frame iterations. This is done assuming that every zero
       * in the array of ticks corresponds to a case like that. */
      if( ticks) {
        numValues++;
      }
    }
    if( numValues) {
      average /= (float) numValues;
    }
    min /= 1000.0;
    max /= 1000.0;
    average /= 1000.0;
    fprintf( bmInfo.fpBmFile, "%10.3f, %10.3f, %10.3f,", min, max, average);
    fprintf( stdout, "%10.3f\t %10.3f\t %10.3f", min, max, average);
  }

  fflush( stdout);
  fflush( bmInfo.fpBmFile);
#ifndef _QUICKTURN_TESTS
  fclose( bmInfo.fpBmFile);
#endif /* ! _QUICKTURN_TESTS */
  printf( "\n instrumentation data written to %s\n", bmInfo.bmFile);
  return 0;
}

#endif /* _BMFILE */


#else /* _BENCHMARKS */
int timerSetup( unsigned int x, unsigned int y) { return x+y;}
unsigned int timerReset( unsigned int x) { return x;}
int printBenchmarks( char *columnHeaders) { return (int) columnHeaders;}
#endif /* _BENCHMARKS */

#ifdef _TMS320C6400

#ifdef _BENCHMARKS
/* These are not for benchmarking, but timer registers are defined only if
 * _BENCHMARKS is defined. So: */
/* temp code */
int loop4Timer( int counter) {
  int result = 0;
  while( counter--) {
    result += TSC_read();
  }
  return result;
} /* loop4Timer */

#endif /* _BENCHMARKS */

#endif /* _TMS320C6400 */

#ifdef _TMS320C6400
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
unsigned int  loop1000Cycles( void)
{
  asm( "            MVK     166,   B0    ");
  asm( "_LOOP:                           ");      
  asm( "    [ B0]   BDEC    _LOOP,  B0   ");
  asm( "            NOP     5            ");

  return 0;
} /* loop1000Cycles */

/******************************************************************************
 * Fucntion Name      :   sleepCycles1000 
 * Description        :   loops till the number of cycles are expired. So this
 *                          is not really 'sleep', but cycles wasted doing
 *                          looping. 
 * Parameters 
 *                numCycles2Sleep - number of 1000 cycles to be expired
 * Return Values
 *                not useful
 * Errors
 *                None
 * Dependencies
 *                This code is expected to compiled with optimization -o3 and
 *                  no debug. Any change to it could result in more cycles
 *                  wasted.
 *
 * Assumptions        :   See Dependencies above.
 * *****************************************************************************/ 
unsigned int sleepCycles1000( unsigned int numCycles2Sleep)
{
  /* There is no sleep function in the C6000 RTS. Usually in other
   * distributions it's declared in unistd.h. So we are forced to use this 
   * imperfect way of doing sleep. The loop below takes 6 cycles to execute,
   * excluding prolog and epilog. */
  unsigned int index;
  for( index = 0; index < numCycles2Sleep; index++) {
    loop1000Cycles();
  }
  return 0;
} /* sleepCycles */
#endif /* _TMS320C6400 */


/**
 *  @fn         Double64x addDouble( Double64x left, Double64x right)
 *  @author     Nandakumar Raghavan [nand@ti.com]
 *  @brief      adds two 64 bit integral types to get another of the same.
 *  @param      1. left   - addition argument
 *  @param      2. right  - addition argument
 *  @exception  errors can happen if the result overflows.
 *  @warning    None
 *  @return     the sum of left and right
 *  */
 
Double64x addDouble( Double64x left, Double64x right)
{
  Double64x result;
  unsigned int  t1, t2, t3, t4, t5, t6;

  t1 = _lo( left);
  t2 = _lo( right);
  t3 = _hi( left);
  t4 = _hi( right);

  t1 >>= 1;
  t2 >>= 1;

  t5 = t1 + t2;
  t6 = t3 + t4;
  if( t5 >> 31) {
    /* addition would have resulted in overflow. */
    t6++;
  }
  t5 <<= 1;
  result = _itod( t6, t5);
  
  return result;
  
} /* addDouble */

/**
 *  @fn         Double64x subDouble( Double64x left, Double64x right)
 *  @author     Nandakumar Raghavan [nand@ti.com]
 *  @brief      subtracts two 64 integral types to get another.
 *  @param      1. left   - left side of the subtraction
 *  @param      2. right  -  right side of the subtraction
 *  @exception  errors can happen if the result overflows.
 *  @warning    None
 *  @return     the result of the subtraction is returned.
 *  */
 
Double64x subDouble( Double64x left, Double64x right)
{
  Double64x result;
  unsigned int  t1, t2, t3, t4, t5, t6, factor;

  /* this module is used only for timer and is guaranteed that left is higher than right. */
//  assert( left > right);
  
  t1 = _lo( left);
  t2 = _lo( right);
  t3 = _hi( left);
  t4 = _hi( right);

  factor = 0;
  if( t1 < t2) {
    t5 = 0x80000000 - (t2 - t1);
    factor = 1;
  } else {
    t5 = t1 - t2;
  }
  t6 = t3 - t4 - factor;

  result = _itod( t6, t5);
  
  return result;
  
} /* subDouble */

#ifdef _DAVINCI
#include "csl_cache.h"
//#include "qdma.h"

/**
 *  @fn         void setup_davinci_qdma_idma(void)
 *  @author     Nandakumar Raghavan [nand@ti.com]
 *  @brief      Function to allocate 4 parameter sets for 4 qdma channels and respective
 *                tcc values.
 *  @param      None
 *  @exception  None
 *  @warning    None
 *  @return     None
 *  */
 /*
void setup_davinci_qdma_idma(void)
{
    /*--------------------------------------------------------------*/
    /*  Since there are four QDMA channels on Davinci, create a     */
    /*  user defined array "param_array" to accept which parameter  */
    /*  RAM entries to use for the individual channels.             */
    /*  This needs to be allocated through a central resource       */
    /*  in CSL and then passed to the QDMA library at setup.        */
    /*--------------------------------------------------------------* /
    
    volatile unsigned int *params_array[4];
    
    /*--------------------------------------------------------------*/
    /*  This array accepts a set of four transfer completion codes  */
    /*  to post completion on Interrupt Pending Register. These     */
    /*  need to be allocated through a central resource in CSL      */
    /*  and then passed to the QDMA library at setup.               */
    /*--------------------------------------------------------------* /
    
    unsigned int tcc_array[4] = {0, 1, 2, 3};
    unsigned int physical_channel[4] = {4,5,6,7};
    /*----------------------------------------------------------*/
    /*  A recommended allocation for supporting upto 16 linked  */
    /*  transfers in one shot is as follows. In this case if    */
    /*  Channel 0 is used then PARAM64-PARAM79 will be used     */
    /*  by the application. Thus each of the four channels      */
    /*  are allocated PARAM entries in the last 1/4 of the      */
    /*  upper entries of the PARAM space of 64-128.             */
    /*----------------------------------------------------------* /
    
    params_array[0] = PARAM102;
    params_array[1] = PARAM110;
    params_array[2] = PARAM118;
    params_array[3] = PARAM126;

    /*-----------------------------------------------------------*/
    /*  Open 4 QDMA channels using the "user" passed PARAM       */
    /*  and "TCC" arrays.                                        */
    /*-----------------------------------------------------------* /
    
    M4H3DEC_TI_QDMA_open(params_array, tcc_array, physical_channel,4,NULL);
}    */

/**
 *  @fn         void enableEmifACaching( void)
 *  @author     Nandakumar Raghavan [nand@ti.com]
 *  @brief      enables various EMIFA CE spaces for caching.
 *  @param      None
 *  @exception  None
 *  @warning    None
 *  @return     None
 *  */
 
void enableEmifACaching( void)
{
  CACHE_enableCaching( CACHE_EMIFA_CE00  );
  CACHE_enableCaching( CACHE_EMIFA_CE01  );
  CACHE_enableCaching( CACHE_EMIFA_CE02  );
  CACHE_enableCaching( CACHE_EMIFA_CE03  );
  CACHE_enableCaching( CACHE_EMIFA_CE04  );
  CACHE_enableCaching( CACHE_EMIFA_CE05  );
  CACHE_enableCaching( CACHE_EMIFA_CE06  );
  CACHE_enableCaching( CACHE_EMIFA_CE07  );
  CACHE_enableCaching( CACHE_EMIFA_CE08  );
  CACHE_enableCaching( CACHE_EMIFA_CE09  );
  CACHE_enableCaching( CACHE_EMIFA_CE010 );
  CACHE_enableCaching( CACHE_EMIFA_CE011 );
  CACHE_enableCaching( CACHE_EMIFA_CE012 );
  CACHE_enableCaching( CACHE_EMIFA_CE013 );
  CACHE_enableCaching( CACHE_EMIFA_CE014 );
  CACHE_enableCaching( CACHE_EMIFA_CE015 );

  CACHE_enableCaching( CACHE_EMIFA_CE10  );
  CACHE_enableCaching( CACHE_EMIFA_CE11  );
  CACHE_enableCaching( CACHE_EMIFA_CE12  );
  CACHE_enableCaching( CACHE_EMIFA_CE13  );
  CACHE_enableCaching( CACHE_EMIFA_CE14  );
  CACHE_enableCaching( CACHE_EMIFA_CE15  );
  CACHE_enableCaching( CACHE_EMIFA_CE16  );
  CACHE_enableCaching( CACHE_EMIFA_CE17  );
  CACHE_enableCaching( CACHE_EMIFA_CE18  );
  CACHE_enableCaching( CACHE_EMIFA_CE19  );
  CACHE_enableCaching( CACHE_EMIFA_CE110 );
  CACHE_enableCaching( CACHE_EMIFA_CE111 );
  CACHE_enableCaching( CACHE_EMIFA_CE112 );
  CACHE_enableCaching( CACHE_EMIFA_CE113 );
  CACHE_enableCaching( CACHE_EMIFA_CE114 );
  CACHE_enableCaching( CACHE_EMIFA_CE115 );

  CACHE_enableCaching( CACHE_EMIFA_CE20  );
  CACHE_enableCaching( CACHE_EMIFA_CE21  );
  CACHE_enableCaching( CACHE_EMIFA_CE22  );
  CACHE_enableCaching( CACHE_EMIFA_CE23  );
  CACHE_enableCaching( CACHE_EMIFA_CE24  );
  CACHE_enableCaching( CACHE_EMIFA_CE25  );
  CACHE_enableCaching( CACHE_EMIFA_CE26  );
  CACHE_enableCaching( CACHE_EMIFA_CE27  );
  CACHE_enableCaching( CACHE_EMIFA_CE28  );
  CACHE_enableCaching( CACHE_EMIFA_CE29  );
  CACHE_enableCaching( CACHE_EMIFA_CE210 );
  CACHE_enableCaching( CACHE_EMIFA_CE211 );
  CACHE_enableCaching( CACHE_EMIFA_CE212 );
  CACHE_enableCaching( CACHE_EMIFA_CE213 );
  CACHE_enableCaching( CACHE_EMIFA_CE214 );
  CACHE_enableCaching( CACHE_EMIFA_CE215 );

  CACHE_enableCaching( CACHE_EMIFA_CE30  );
  CACHE_enableCaching( CACHE_EMIFA_CE31  );
  CACHE_enableCaching( CACHE_EMIFA_CE32  );
  CACHE_enableCaching( CACHE_EMIFA_CE33  );
  CACHE_enableCaching( CACHE_EMIFA_CE34  );
  CACHE_enableCaching( CACHE_EMIFA_CE35  );
  CACHE_enableCaching( CACHE_EMIFA_CE36  );
  CACHE_enableCaching( CACHE_EMIFA_CE37  );
  CACHE_enableCaching( CACHE_EMIFA_CE38  );
  CACHE_enableCaching( CACHE_EMIFA_CE39  );
  CACHE_enableCaching( CACHE_EMIFA_CE310 );
  CACHE_enableCaching( CACHE_EMIFA_CE311 );
  CACHE_enableCaching( CACHE_EMIFA_CE312 );
  CACHE_enableCaching( CACHE_EMIFA_CE313 );
  CACHE_enableCaching( CACHE_EMIFA_CE314 );
  CACHE_enableCaching( CACHE_EMIFA_CE315 );

  return;
}
#endif /* _DAVINCI */





