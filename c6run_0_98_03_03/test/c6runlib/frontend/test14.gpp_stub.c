// Standard library required includes
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Copied includes from original source file
#include <stdio.h>

#include "gpp_to_dsp_stub.h"
extern intptr_t FXN_ADDR_test14_v_stub;
void test14_v
(
  void
)
{
  RPC_INIT_VOIDRETURN(test14_v, void);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_SYNC_DISPATCH();
  RPC_RETURN_VOIDRETURN();
}

C6RUN_RPC_AsyncHandle test14_v_asyncBegin
(
  void
)
{
  RPC_INIT_ASYNC_BEGIN(test14_v);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_ASYNC_DISPATCH_BEGIN();
}

bool test14_v_asyncIsDone
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  return C6RUN_RPC_asyncIsComplete( __C6RUN_asyncHandle__ );
}

void test14_v_asyncEnd
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  RPC_INIT_ASYNC_END_VOIDRETURN(void);
  RPC_ASYNC_DISPATCH_END();
  RPC_RETURN_VOIDRETURN();
}

extern intptr_t FXN_ADDR_test14_i_stub;
int test14_i
(
  void
)
{
  RPC_INIT(test14_i, int);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_SYNC_DISPATCH();
  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);
  RPC_RETURN();
}

C6RUN_RPC_AsyncHandle test14_i_asyncBegin
(
  void
)
{
  RPC_INIT_ASYNC_BEGIN(test14_i);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_ASYNC_DISPATCH_BEGIN();
}

bool test14_i_asyncIsDone
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  return C6RUN_RPC_asyncIsComplete( __C6RUN_asyncHandle__ );
}

int test14_i_asyncEnd
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  RPC_INIT_ASYNC_END(int);
  RPC_ASYNC_DISPATCH_END();
  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);
  RPC_RETURN();
}

extern intptr_t FXN_ADDR_test14_f_stub;
float test14_f
(
  void
)
{
  RPC_INIT(test14_f, float);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_SYNC_DISPATCH();
  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);
  RPC_RETURN();
}

C6RUN_RPC_AsyncHandle test14_f_asyncBegin
(
  void
)
{
  RPC_INIT_ASYNC_BEGIN(test14_f);
  RPC_TERMINATE_CACHE_OPS();
  RPC_TERMINATE_CACHE_OPS();
  RPC_ASYNC_DISPATCH_BEGIN();
}

bool test14_f_asyncIsDone
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  return C6RUN_RPC_asyncIsComplete( __C6RUN_asyncHandle__ );
}

float test14_f_asyncEnd
(
  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__
)
{
  RPC_INIT_ASYNC_END(float);
  RPC_ASYNC_DISPATCH_END();
  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);
  RPC_RETURN();
}


