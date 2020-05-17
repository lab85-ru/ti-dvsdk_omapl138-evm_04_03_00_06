/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== rtaerror.h ========
 *
 */

#ifndef _RTAERROR_H_
#define _RTAERROR_H_

/*
 *  Bit 29 is reserved for 'application error codes'.  No Windows
 *  error codes will have bit 29 set.
 */
#define RTA_APP_DEFINED_ERROR_BIT       0x20000000

/*
 *  HRESULT errors have this bit set.
 */
#define RTA_ERROR_BIT                   0x80000000

/*
 *  RTA error codes must be in the 0xa0000001 - 0xa00000ff range.  To avoid
 *  collisions, other modules must use error codes *outside* of
 *  this range.
 */

/*
 *  The operation completed successfully.
 */
#define RTA_OK          0

/*
 *  The operation failed.
 */
#define RTA_E_FAIL      0xa0000001

/*
 *  Code Composer threw an exception which was caught.
 */
#define RTA_E_CCEXCEPTION 0xa0000002

/*
 *  The function that returns this code is required to run in the main
 *  thread, and was not called from the main thread.
 */
#define RTA_E_THREAD    0xa0000003

/*
 *  Failed to initialize COM
 */
#define RTA_E_COMINIT   0xa0000004

/*
 *  File open failed.
 */
#define RTA_E_FOPEN     0xa0000005

/*
 *  Failed to load a CDB file
 */
#define RTA_E_CDBLOAD   0xa0000006

/*
 *  An attempt to read target memory failed.
 */
#define RTA_E_READ      0xa0000007

/*
 *  An attempt to write target memory failed.
 */
#define RTA_E_WRITE     0xa0000008

/*
 *  This error code indicates that an error occurred in the driver used
 *  to upload RTA data from the target.  This error is only relevant
 *  when RTDX is not used as the host link type.
 */
#define RTA_E_DRVERROR  0xa0000009

/*
 *  The configuration file (.cdb file) is inconsistent with the target
 *  executable. This is probably because the .cdb file was modified and
 *  the target program was not rebuilt.
 */
#define RTA_E_CDBVERSION        0xa000000a

/*
 *  The configuration file for the target program is incomplete, missing
 *  an essential module, or missing a symbol.
 */
#define RTA_E_BADCDB    0xa000000b

/*
 *  A target state change occurred (eg, running to halted) during an
 *  attempt to upload RTA data. The data should be ignored.
 *  This error will also be returned if RTA request to get data is made
 *  while the target is restarting (inside of an OnRTARestarted()
 *  notification.)
 */
#define RTA_E_STATE     0xa000000c

/*
 *  A file was loaded while attempting to upload RTA data. The data
 *  should be ignored.
 */
#define RTA_E_FILELOAD  0xa000000d

/*
 *  Failed to open an HST channel or RTA channel.
 */
#define RTA_E_CHNOPEN   0xa000000e

/*
 *  Failed to close an HST channel or RTA channel.
 */
#define RTA_E_CHNCLOSE  0xa000000f

/*
 *  The target program is not a BIOS program.
 */
#define RTA_E_NON_BIOS  0xa0000010

/*
 *  The configuration file (.cdb file) for the target program could not be
 *  found.  The .cdb file must be in the same directory as the target
 *  program.
 */
#define RTA_E_NOCDBFILE 0xa0000011

/*
 *  A symbol needed for RTA was not found in the target program.
 */
#define RTA_E_SYMBOL    0xa0000012

/*
 *  RTA is not enabled due to channel failure (eg target not capable
 *  of RTDX).
 */
#define RTA_E_RTADISABLED       0xa0000013

/*
 *  Can't find a named section in the target program.
 *  RTA SDK clients will not see this. (Will be remapped to RTA_E_NON_BIOS).
 */
#define RTA_E_NOSECTION         0xa0000014

/*
 *  Target does not support RTDX.
 */
#define RTA_E_RTDXCAPABLE       0xa0000015

/*
 *  Unable to configure RTDX.
 */
#define RTA_E_CONFIGRTDX                0xa0000016

/*
 *  Unable to enable RTDX.
 */
#define RTA_E_RTDXENABLE                0xa0000017

/*
 *  Channel Id out of range.
 */
#define RTA_E_CHNID             0xa0000018

/*
 *  Channel name is too long.
 */
#define RTA_E_CHNNAME           0xa0000019

/*
 *  Channel is already in use.
 */
#define RTA_E_CHNINUSE          0xa000001a

/*
 *  Unable to disable a channel.
 */
#define RTA_E_CHNDISABLE                0xa000001b

/*
 *  Unable to enable a channel
 */
#define RTA_E_CHNENABLE         0xa000001c

/*
 *  Unable to flush channel.
 */
#define RTA_E_CHNFLUSH          0xa000001d

/*
 *  All channels are in use.
 */
#define RTA_E_ALLCHNINUSE       0xa000001e

/*
 *  Can't get RTDX configuration.
 */
#define RTA_E_RTDXSTATUS                0xa000001f

/*
 *  The current RTDX configuration is not usable.
 */
#define RTA_E_BADRTDXCONFIG     0xa0000020

/*
 *  Exiting RTA before an upload of RTA data completed.  Data should
 *  be ignored.
 */
#define RTA_E_DONE              0xa0000021

/*
 *  Failed to load a dynamic link library.
 */
#define RTA_E_LOADLIB           0xa0000022

/*
 *  GetProcAddress failed for a dynamic link library.
 */
#define RTA_E_PROCADDRESS       0xa0000023

/*
 *  A request was made to upload log data, but no logs are enabled.
 */
#define RTA_E_NO_LOG_ENABLED    0xa0000024

/*
 *  An RTA wrapper object has not been properly initialized.  See the
 *  header file for the definition of that particular wrapper class.
 */
#define RTA_E_UNINITIALIZED_OBJECT      0xa0000025

/*
 *  The token passed to get data is not valid. Make sure the call to get
 *  data is made inside the data notification callback, and that the same
 *  token that is passed to the callback is used in the call to get data.
 */
#define RTA_E_INVALID_TOKEN     0xa0000026

/*
 *  An index passed to a function is out of range.
 */
#define RTA_E_INVALID_INDEX     0xa0000027

/*
 *  Bad data passed to function.
 */
#define RTA_E_INVALID_DATA      0xa0000028

/*
 *  No data is available.
 */
#define RTA_E_NO_DATA   0xa0000029

/*
 *  There is no program loaded on the target, or a program is loaded that
 *  does not support Real Time Analysis.
 */
#define RTA_E_NO_PROGRAM_LOADED 0xa000002a

/*
 *  An attempt was made to deactivate a log that was already deactivated.
 */
#define RTA_E_INVALID_ACTIVATION        0xa000002b

/*
 *  An invalid state change on an HST channel was attempted (eg, trying
 *  to stop an HST channel that is already stopped.)
 */
#define RTA_E_INVALID_STATE     0xa000002c

/*
 *  An error code passed to an RTA function is not an RTA error code.
 */
#define RTA_E_INVALID_ERROR_CODE        0xa000002d

/*
 *  This value is returned by an RTA function when it cannot perform the
 *  operation, either because no program is currently loaded on the target,
 *  the loaded program is not a BIOS program, or some error has occurred
 *  during RTA initialization when the program was loaded (eg, a cdb file
 *  was inconsistent with the target program).
 */
#define RTA_E_NO_RTA                    0xa000002e

/*
 *  An attempt was made to get execution data for a log that is not a
 *  system log, and there is no execution data.
 */
#define RTA_E_NO_EXECUTION_DATA         0xa000002f

/*
 *  Invalid array passed as function argument.
 */
#define RTA_E_INVALID_ARRAY             0xa0000030

/*
 *  An internal RTA error occurred.
 */
#define RTA_E_INTERNAL_ERROR            0xa0000031

/*
 *  Cannot access Real Time Analysis server.
 */
#define RTA_E_SERVER                    0xa0000032

/*
 *  An attempt was made to get a conversion factor for user defined STS units.
 */
#define RTA_E_NO_CONVERSION             0xa0000033

/*
 *  Number of error codes (+ 0xa0000000).
 */
#define RTA_ENUM                0xa0000034


#endif  /* _RTAERROR_H_ */
