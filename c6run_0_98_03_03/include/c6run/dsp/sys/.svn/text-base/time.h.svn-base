/*
 * sys/time.h
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

/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <stddef.h>
#include <sys/cdefs.h>
#include <sys/types.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

#define CLOCK_SGI_CYCLE 10
#define MAX_CLOCKS 16
#define CLOCKS_MASK (CLOCK_REALTIME | CLOCK_MONOTONIC)
#define CLOCKS_MONO CLOCK_MONOTONIC

#define TIMER_ABSTIME 0x01



#define timerclear(a)   \
        ((a)->tv_sec = (a)->tv_usec = 0)

#define timerisset(a)    \
        ((a)->tv_sec != 0 || (a)->tv_usec != 0)

#define timercmp(a, b, op)               \
        ((a)->tv_sec == (b)->tv_sec      \
        ? (a)->tv_usec op (b)->tv_usec   \
        : (a)->tv_sec op (b)->tv_sec)

#define timeradd(a, b, res)                           \
    do {                                              \
        (res)->tv_sec  = (a)->tv_sec  + (b)->tv_sec;  \
        (res)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
        if ((res)->tv_usec >= 1000000) {              \
            (res)->tv_usec -= 1000000;                \
            (res)->tv_sec  += 1;                      \
        }                                             \
    } while (0)

#define timersub(a, b, res)                           \
    do {                                              \
        (res)->tv_sec  = (a)->tv_sec  - (b)->tv_sec;  \
        (res)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
        if ((res)->tv_usec < 0) {                     \
            (res)->tv_usec += 1000000;                \
            (res)->tv_sec  -= 1;                      \
        }                                             \
    } while (0)

#define NFDBITS __NFDBITS

#define FD_SETSIZE __FD_SETSIZE
#define FD_SET(fd,fdsetp) __FD_SET(fd,fdsetp)
#define FD_CLR(fd,fdsetp) __FD_CLR(fd,fdsetp)
#define FD_ISSET(fd,fdsetp) __FD_ISSET(fd,fdsetp)
#define FD_ZERO(fdsetp) __FD_ZERO(fdsetp)

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/

struct timeval
{
  time_t tv_sec;        // seconds
  suseconds_t tv_usec;  // microseconds
};

struct timezone {
 int tz_minuteswest;
 int tz_dsttime;
};

struct itimerval {
 struct timeval it_interval;
 struct timeval it_value;
};


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern int gettimeofday(struct timeval *restrict tp, void *restrict tzp);

extern int getitimer(int, struct itimerval *);
extern int setitimer(int, const struct itimerval *restrict, struct itimerval *restrict);
#if (0)
extern int select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict, struct timeval *restrict);
#endif
extern int utimes(const char *, const struct timeval *);



/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TIME_H_ */
