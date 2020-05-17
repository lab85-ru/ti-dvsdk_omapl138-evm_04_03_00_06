/*
 * aio.h
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


#ifndef _AIO_H_
#define _AIO_H_

#include <fcntl.h>
#include <signals.h>
#include <time.h> 
#include <sys/types.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/

/* AIO control block structure. */
struct aiocb
{
  int             aio_fildes,     /* File descriptor. */
  off_t           aio_offset,     /* File offset. */
  volatile void  *aio_buf,        /* Location of buffer. */
  size_t          aio_nbytes,     /* Length of transfer. */
  int             aio_reqprio,    /* Request priority offset. */
  struct sigevent aio_sigevent,   /* Signal number and value. */
  int             aio_lio_opcode  /* Operation to be performed. */
};

/* Return values of cancelation function. */
enum
{
  AIO_CANCELED,
  AIO_NOTCANCELED,
  AIO_ALLDONE
};

/* Synchronization options for lio_listio() API. */
enum
{
  LIO_WAIT,
  LIO_NOWAIT,
};

/* Operation options for lio_listio() API. */
enum
{
  LIO_READ,
  LIO_WRITE,
  LIO_NOP
}

/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern int      aio_cancel(int, struct aiocb *);
extern int      aio_error(const struct aiocb *);
extern int      aio_fsync(int, struct aiocb *);
extern int      aio_read(struct aiocb *);
extern ssize_t  aio_return(struct aiocb *);
extern int      aio_suspend(const struct aiocb *const [], int, const struct timespec *);
extern int      aio_write(struct aiocb *);
extern int      lio_listio(int, struct aiocb *restrict const [restrict], int, struct sigevent *restrict);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif
             
#endif /* _AIO_H_ */


