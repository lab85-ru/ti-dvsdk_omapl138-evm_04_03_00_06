/*
 * unitstd.h - Header file for POSIX standard symbolic constants
 *             IEEE Std 1003.1-2008 / The Open Group Base Specifications Issue 7.
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

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include <sys/types.h>
#include <inttypes.h>


// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

#define _POSIX_VERSION  (200809L)
#define _POSIX2_VERSION (200809L)

#define _XOPEN_VERSION  (700)


#define _POSIX_ASYNC_IO (-1)  // No support in our implementation
#define _POSIX_PRIO_IO  (-1)  // No support in our implementation
#define _POSIX_SYNC_IO  (-1)  // No support in our implementation

/* Should be defined from stddef.h */
#ifndef NULL
  #define NULL ((void *) 0)
#endif

/* Should be defined from stdio.h */
#if !defined(SEEK_CUR)
  #define SEEK_CUR  (0x0001)
#endif
#if !defined(SEEK_END)
  #define SEEK_END  (0x0002)
#endif  
#if !defined(SEEK_SET)
  #define SEEK_SET  (0x0000)
#endif

/* Standard file descriptor numbers. */
#define STDERR_FILENO (2) // File number of stderr; 2.
#define STDIN_FILENO  (0) // File number of stdin; 0.
#define STDOUT_FILENO (1) // File number of stdout; 1.

/* Macros for access() */
#define R_OK  (4)  /* Read */
#define W_OK  (2)  /* Write */
#define X_OK  (1)  /* Execute */
#define F_OK  (0)  /* Existence */


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern char **environ;

extern char  *optarg;
extern int    opterr, optind, optopt;


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern int access(const char *, int);
extern unsigned int alarm(unsigned int);
extern int chdir(const char *);
extern int chown(const char *, uid_t, gid_t);
extern int close(int);
extern size_t confstr(int, char *, size_t);

#if (0)
//[XSI][Option Start]
char        *crypt(const char *, const char *);
//[Option End]
#endif

#if (0)
//[CX][Option Start]
char        *ctermid(char *);
//[Option End]
#endif

extern int dup(int);
extern int dup2(int, int);

extern void _exit(int);

extern int execl(const char *, const char *, ...);
extern int execlp(const char *, const char *, ...);
extern int execle(const char *, const char *, ...);

extern int execv(const char *, char * const *);
extern int execve(const char *, char * const *, char * const *);
extern int execvp(const char *, char * const *);
#if (0)
  int          faccessat(int, const char *, int, int);
#endif  
extern int fchdir(int);
extern int fchown(int, uid_t, gid_t);
#if (0)
  int          fchownat(int, const char *, uid_t, gid_t, int);
#endif

extern int fdatasync(int);

#if (0)
  int          fexecve(int, char *const [], char *const []);
#endif
extern pid_t  fork(void);
#if (0)
  long         fpathconf(int, int);
#endif
extern int fsync(int);

extern int ftruncate(int, off_t);
extern char *getcwd(char *, size_t);
extern gid_t getegid(void);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern int getgroups(int, gid_t *);

#if (0)
//[XSI][Option Start]
long         gethostid(void);
//[Option End]
#endif

extern int gethostname(char *, size_t);

extern char* getlogin(void);
extern int   getlogin_r(char* name, size_t namesize);

extern int getopt(int, char * const *, const char *);
extern pid_t getpgid(pid_t);
extern pid_t getpgrp(void);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern pid_t getsid(pid_t);
extern uid_t getuid(void);

extern int isatty(int);
extern int lchown(const char *, uid_t, gid_t);
extern int link(const char *, const char *);
#if (0)
int          linkat(int, const char *, int, const char *, int);
#endif

#if (0)
//[XSI][Option Start]
int          lockf(int, int, off_t);
//[Option End]
#endif

extern off_t lseek(int, off_t, int);

extern int nice(int);

#if (0)
  long         pathconf(const char *, int);
#endif

extern int pause(void);
extern int pipe(int *);
extern ssize_t pread(int, void *, size_t, off_t);
extern ssize_t pwrite(int, const void *, size_t, off_t);
extern ssize_t read(int, void *, size_t);

extern ssize_t  readlink(const char *restrict, char *restrict, size_t);
#if (0)
  ssize_t      readlinkat(int, const char *restrict, char *restrict, size_t);
#endif
extern int rmdir(const char *);
extern int setegid(gid_t);
extern int seteuid(uid_t);

extern int setgid(gid_t);

extern int    setpgid(pid_t, pid_t);
extern int    setpgrp(void);

extern int setregid(gid_t, gid_t);
extern int setreuid(uid_t, uid_t);

extern pid_t  setsid(void);
extern int    setuid(uid_t);
extern unsigned int sleep(unsigned int);

#if (0)
//[XSI][Option Start]
void         swab(const void *restrict, void *restrict, ssize_t);
//[Option End]
#endif


extern int symlink(const char *, const char *);
#if (0)
  int          symlinkat(const char *, int, const char *);
#endif

extern void  sync(void);
extern long  sysconf(int  name);
extern pid_t tcgetpgrp(int fd);
extern int   tcsetpgrp(int fd, pid_t _pid);
#if (0)
  int          truncate(const char *, off_t);
  char        *ttyname(int);
  int          ttyname_r(int, char *, size_t);
#endif

extern int unlink(const char *);
#if (0)
  int          unlinkat(int, const char *, int);
#endif
extern ssize_t write(int, const void *, size_t);


#if (0)
/* Functions from BIONIC unistd.h not taken into our implementation
   since they don't match up with the POSIX.1-2008 standard */
extern __noreturn void _exit(int);
extern pid_t  vfork(void);
extern pid_t  gettid(void);
extern int execvpe(const char *, char * const *, char * const *);
extern int execlpe(const char *, const char *, ...);
extern int capget(cap_user_header_t hdrp, cap_user_data_t datap);
extern int capset(cap_user_header_t hdrp, const cap_user_data_t datap);
extern int prctl(int  option,  unsigned long arg2, unsigned long arg3,
                 unsigned long arg4, unsigned long arg5);
extern int setgroups(size_t, const gid_t *);
extern int setresuid(uid_t, uid_t, uid_t);
extern int setresgid(gid_t, gid_t, gid_t);
extern int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
extern int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
extern int getfsuid(uid_t);
extern int setfsuid(uid_t);
extern int issetugid(void);
extern int chroot(const char *);
extern int fcntl(int, int, ...);
extern int ioctl(int, int, ...);
extern int flock(int, int);
extern void usleep(unsigned long);
extern int sethostname(const char *, size_t);
extern int getdomainname(char *, size_t);
extern int setdomainname(const char *, size_t);
extern int getdtablesize(void);
extern int  acct(const char*  filepath);
static inline int getpagesize(void) {
  extern unsigned int __page_size;
  return __page_size;
}
static inline int __getpageshift(void) {
  extern unsigned int __page_shift;
  return __page_shift;
}
extern int daemon(int, int);
#endif


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif
             
#endif /* _UNISTD_H_ */


