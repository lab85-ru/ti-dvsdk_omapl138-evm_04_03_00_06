// Standard library required includes
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Copied includes from original source file
#include <stdio.h>

#include "dsp_from_gpp_stub.h"
#if defined(__TI_EABI__)
  #define test14_v_stub _test14_v_stub
#endif
void test14_v_stub( void *msgBuffer )
{
  RPC_INIT_VOIDRETURN( void );
  RPC_PROCESS_CACHE_IN_OPS();
  {
    test14_v( );
  }
  RPC_PROCESS_CACHE_OUT_OPS();
}

#if defined(__TI_EABI__)
  #define test14_i_stub _test14_i_stub
#endif
void test14_i_stub( void *msgBuffer )
{
  RPC_INIT( int );
  RPC_PROCESS_CACHE_IN_OPS();
  {
    __C6RUN_fxnResult__ = test14_i( );
  }
  RPC_PROCESS_CACHE_OUT_OPS();
  RPC_RESULT_PACK(__C6RUN_fxnResult__);
}

#if defined(__TI_EABI__)
  #define test14_f_stub _test14_f_stub
#endif
void test14_f_stub( void *msgBuffer )
{
  RPC_INIT( float );
  RPC_PROCESS_CACHE_IN_OPS();
  {
    __C6RUN_fxnResult__ = test14_f( );
  }
  RPC_PROCESS_CACHE_OUT_OPS();
  RPC_RESULT_PACK(__C6RUN_fxnResult__);
}


