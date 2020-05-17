/*****************************************************************************/
/*  FILE.H v6.1.14                                                            */
/*  Copyright (c) 1995-2010 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Macros and declarations used in lowlevel I/O functions.                   */
/*****************************************************************************/
#ifndef _FILE
#define _FILE

#include <linkage.h>

/*---------------------------------------------------------------------------*/
/* constants for file manipulations                                          */
/*---------------------------------------------------------------------------*/
#define  O_RDONLY    (0x0000) /* open for reading      */
#define  O_WRONLY    (0x0001) /* open for writing      */
#define  O_RDWR      (0x0002) /* open for read & write */
#define  O_APPEND    (0x0008) /* append on each write  */
#define  O_CREAT     (0x0200) /* open with file create */
#define  O_TRUNC     (0x0400) /* open with truncation  */
#define  O_BINARY    (0x8000) /* open in binary mode   */

/*---------------------------------------------------------------------------*/
/* lowlevel I/O declarations                                                 */
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
#define _DECL extern "C"
#else /* ! __cplusplus */
#define _DECL extern
#endif

#ifndef _OFF_T
#define _OFF_T
typedef int off_t;
#endif /* _OFF_T */

#ifndef SEEK_SET
#define SEEK_SET  (0x0000)
#endif /* SEEK_SET */
#ifndef SEEK_CUR
#define SEEK_CUR  (0x0001)
#endif /*SEEK_CUR */
#ifndef SEEK_END
#define SEEK_END  (0x0002)
#endif /* SEEK_END */

_DECL _CODE_ACCESS int   open(const char *path, unsigned flags, int mode);
_DECL _CODE_ACCESS int   read(int fildes, char *bufptr, unsigned cnt);
_DECL _CODE_ACCESS int   write(int fildes, const char *bufptr, unsigned cnt);
_DECL _CODE_ACCESS off_t lseek(int fildes, off_t offset, int origin);
_DECL _CODE_ACCESS int   close(int fildes);
_DECL _CODE_ACCESS int   unlink(const char *path);
_DECL _CODE_ACCESS int   rename(const char *old_name, const char *new_name);

_DECL _CODE_ACCESS int add_device(
    char     *name,			           
    unsigned  flags,
    int      (*dopen)(const char *path, unsigned flags, int llv_fd),
    int      (*dclose)(int dev_fd),
    int      (*dread)(int dev_fd, char *buf, unsigned count),
    int      (*dwrite)(int dev_fd, const char *buf, unsigned count),
    off_t    (*dlseek)(int dev_fd, off_t offset, int origin),
    int      (*dunlink)(const char *path),
    int      (*drename)(const char *old_name, const char *new_name));

#define _NSTREAM        10                   /* Size of stream table        */
#define _NDEVICE         3                   /* Size of device table        */

#define _SSA      (0x0000)             /* Single Stream allowed       */
#define _BUSY     (0x0001)             /* Device busy                 */
#define _MSA      (0x0002)             /* Multiple Streams Allowed    */

#undef _DECL

#endif /* _FILE */

