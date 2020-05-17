/*
 * time.h
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
 
#ifndef _TIME_H_
#define _TIME_H_

#include <sys/cdefs.h>
#include <sys/time.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/* defining TM_ZONE indicates that we have a "timezone abbreviation" field in
 * struct tm, the value should be the field name
 */
#define   TM_ZONE   tm_zone

// FIXME - define in terms of DSP/BIOS in copmpiling for TMS320C64x+ or TMS320C674x
#define CLOCKS_PER_SEC     1000000

/* BIONIC: extra linux clock goodies */
#define CLOCK_REALTIME             0
#define CLOCK_MONOTONIC            1
#define CLOCK_PROCESS_CPUTIME_ID   2
#define CLOCK_THREAD_CPUTIME_ID    3
#define CLOCK_REALTIME_HR          4
#define CLOCK_MONOTONIC_HR         5


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/

struct tm 
{
  int     tm_sec;         /* seconds */
  int     tm_min;         /* minutes */
  int     tm_hour;        /* hours */
  int     tm_mday;        /* day of the month */
  int     tm_mon;         /* month */
  int     tm_year;        /* year */
  int     tm_wday;        /* day of the week */
  int     tm_yday;        /* day in the year */
  int     tm_isdst;       /* daylight saving time */

  long int tm_gmtoff;     /* Seconds east of UTC.  */
  const char *tm_zone;    /* Timezone abbreviation.  */
};

/* ANDROID-BEGIN */
struct strftime_locale 
{
  const char *  mon[12];
  const char *  month[12];
  const char *  standalone_month[12];
  const char *  wday[7];
  const char *  weekday[7];
  const char *  X_fmt;
  const char *  x_fmt;
  const char *  c_fmt;
  const char *  am;
  const char *  pm;
  const char *  date_fmt;
};
/* ANDROID-END */


#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
struct timespec
{
  time_t tv_sec;        // seconds
  long tv_nsec;         // nanoseconds
};
#endif

struct itimerspec
{
  struct timespec it_interval;
  struct timespec it_value;
};


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern char*     tzname[];
extern int       daylight;
extern long int  timezone;


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern time_t   time(time_t *);
extern int      nanosleep(const struct timespec *, struct timespec *);

extern char *strtotimeval(const char *str, struct timeval *tv);

extern char* asctime(const struct tm* a);
extern char* asctime_r(const struct tm* a, char* buf);

/* Return the difference between TIME1 and TIME0.  */
extern double difftime (time_t __time1, time_t __time0);
extern time_t mktime (struct tm *a);

extern struct tm*  localtime(const time_t *t);
extern struct tm*  localtime_r(const time_t *timep, struct tm *result);

extern struct tm*  gmtime(const time_t *timep);
extern struct tm*  gmtime_r(const time_t *timep, struct tm *result);

extern char*       strptime(const char *buf, const char *fmt, struct tm *tm);
extern size_t      strftime(char *s, size_t max, const char *format, const struct tm *tm);

/* ANDROID-BEGIN */
extern size_t      strftime_tz(char *s, size_t max, const char *format, const struct tm *tm, const struct strftime_locale*  lc);
/* ANDROID-END */

extern char *ctime(const time_t *timep);
extern char *ctime_r(const time_t *timep, char *buf);

extern void  tzset(void);

extern clock_t   clock(void);

extern int clock_getcpuclockid(pid_t, clockid_t *);
extern int clock_getres(clockid_t, struct timespec *);
extern int clock_gettime(clockid_t, struct timespec *);
extern int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *);
extern int clock_settime(clockid_t, const struct timespec *);
#if (0)
extern int  timer_create(clockid_t, struct sigevent *restrict, timer_t *restrict);
extern int  timer_delete(timer_t);
extern int  timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
extern int  timer_gettime(timer_t timerid, struct itimerspec *value);
extern int  timer_getoverrun(timer_t  timerid);
#endif
/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */
