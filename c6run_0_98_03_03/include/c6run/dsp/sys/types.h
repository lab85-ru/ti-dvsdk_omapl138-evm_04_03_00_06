/*
 * sys/types.h
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
#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

#define __need_size_t
#define __need_ptrdiff_t


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/

typedef unsigned short umode_t;

typedef signed char __s8;
typedef unsigned char __u8;

typedef signed short __s16;
typedef unsigned short __u16;

typedef signed int __s32;
typedef unsigned int __u32;

typedef signed long __s40;
typedef unsigned long __u40;

typedef signed long long __s64;
typedef unsigned long long __u64;


typedef __u32    __kernel_dev_t;

/* be careful with __kernel_gid_t and __kernel_uid_t
 * these are defined as 16-bit for legacy reason, but
 * the kernel uses 32-bits instead.
 *
 * 32-bit valuea are required for Android, so use
 * __kernel_uid32_t and __kernel_gid32_t
 */


typedef unsigned long   __kernel_blkcnt_t;
typedef unsigned long   __kernel_blksize_t;

/* these aren't really defined by the kernel headers though... */
typedef unsigned long   __kernel_fsblkcnt_t;
typedef unsigned long   __kernel_fsfilcnt_t;
typedef unsigned int    __kernel_id_t;

typedef void (*__kernel_sighandler_t)(int);

typedef int __kernel_key_t;
typedef int __kernel_mqd_t;
 
typedef unsigned long           __kernel_ino_t;
typedef unsigned short          __kernel_mode_t;
typedef unsigned short          __kernel_nlink_t;
typedef long                    __kernel_off_t;
typedef int                     __kernel_pid_t;
typedef unsigned short          __kernel_ipc_pid_t;
typedef unsigned short          __kernel_uid_t;
typedef unsigned short          __kernel_gid_t;
typedef unsigned int            __kernel_size_t;
typedef int                     __kernel_ssize_t;
typedef int                     __kernel_ptrdiff_t;
typedef long                    __kernel_time_t;
typedef long                    __kernel_suseconds_t;
typedef long                    __kernel_clock_t;
typedef int                     __kernel_timer_t;
typedef int                     __kernel_clockid_t;
typedef int                     __kernel_daddr_t;
typedef char *                  __kernel_caddr_t;
typedef unsigned short          __kernel_uid16_t;
typedef unsigned short          __kernel_gid16_t;
typedef unsigned int            __kernel_uid32_t;
typedef unsigned int            __kernel_gid32_t;
typedef unsigned short          __kernel_old_uid_t;
typedef unsigned short          __kernel_old_gid_t;
typedef unsigned short          __kernel_old_dev_t;
typedef long long               __kernel_loff_t;

 
typedef __kernel_blkcnt_t    blkcnt_t;
typedef __kernel_blksize_t   blksize_t;
typedef __kernel_clock_t     clock_t;
typedef __kernel_clockid_t   clockid_t;
typedef __kernel_dev_t       dev_t;
typedef __kernel_fsblkcnt_t  fsblkcnt_t;
typedef __kernel_fsfilcnt_t  fsfilcnt_t;
typedef __kernel_gid32_t     gid_t;
typedef __kernel_id_t        id_t;
typedef __kernel_ino_t       ino_t;
typedef __kernel_key_t       key_t;
typedef __kernel_mode_t      mode_t;
typedef __kernel_nlink_t	 nlink_t;
#define _OFF_T_DEFINED_
typedef __kernel_off_t       off_t;
typedef __kernel_loff_t      loff_t;
typedef loff_t               off64_t;  /* GLibc-specific */

typedef __kernel_pid_t		 pid_t;

/* while POSIX wants these in <sys/types.h>, we
 * declare then in <pthread.h> instead */
#if 0
  typedef  .... pthread_attr_t;
  typedef  .... pthread_cond_t;
  typedef  .... pthread_condattr_t;
  typedef  .... pthread_key_t;
  typedef  .... pthread_mutex_t;
  typedef  .... pthread_once_t;
  typedef  .... pthread_rwlock_t;
  typedef  .... pthread_rwlock_attr_t;
  typedef  .... pthread_t;
#endif

/* size_t is defined by the C6000-specific RTS <stddef.h> */
#if (!defined(_SIZE_T_DEFINED_) && !defined(_SIZE_T))
#define _SIZE_T_DEFINED_
typedef unsigned int  size_t;
#endif

#ifndef _SSIZE_T_DEFINED_
#define _SSIZE_T_DEFINED_
typedef long int  ssize_t;
#endif

typedef __kernel_suseconds_t  suseconds_t;
typedef __kernel_time_t       time_t;
typedef __kernel_uid32_t        uid_t;
typedef signed long           useconds_t;

typedef __kernel_daddr_t	daddr_t;
typedef __kernel_timer_t	timer_t;
typedef __kernel_mqd_t		mqd_t;

typedef __kernel_caddr_t    caddr_t;
typedef unsigned int        uint_t;
typedef unsigned int        uint;

// FIXME - this is not a standard macro. Do we need it?
#if (0)
/* for some applications */
  #include <sys/sysmacros.h>
#endif

#ifdef __BSD_VISIBLE
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

typedef uint32_t       u_int32_t;
typedef uint16_t       u_int16_t;
typedef uint8_t        u_int8_t;
typedef uint64_t       u_int64_t;
#endif


/***********************************************************
* Global Function Declarations                             *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TYPES_H_ */
