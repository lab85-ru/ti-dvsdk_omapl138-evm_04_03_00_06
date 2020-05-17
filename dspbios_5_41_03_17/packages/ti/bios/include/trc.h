/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== trc.h ========
 *
 */

#ifndef TRC_
#define TRC_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== Trace Event Classes ========
 */
#define TRC_LOGSWI      0x0001
#define TRC_LOGPRD      0x0002
#define TRC_LOGCLK      0x0004

#define TRC_LOGTSK      0x0008          /* Reserved for SPOX */
#define TRC_PWRMINST    0x0010

#define TRC_STSSWI      0x0020
#define TRC_STSPRD      0x0040
#define TRC_STSPIP      0x0080
#define TRC_STSHWI      0x0100

#define TRC_STSTSK      0x0200          /* Reserved for SPOX */
#define TRC_STSSWO      0x0400          /* Reserved for SPOX */
#define TRC_USER0       0x0800
#define TRC_USER1       0x1000
#define TRC_USER2       0x2000
#define TRC_GBLTARG     0x4000          /* set/cleared by target */
#define TRC_GBLHOST     0x8000          /* set/cleared by host */

#ifdef _6x_
extern far Uns TRC_R_mask;
#else
extern Uns TRC_R_mask;
#endif

/*
 *  ======== TRC_disable ========
 *  Clear the trace bits specified by mask.
 */
#define TRC_disable(mask) (TRC_R_mask = TRC_R_mask & ~(mask))

/*
 *  ======== TRC_enable ========
 *  Set the trace bits specified by mask.
 */
#define TRC_enable(mask) (TRC_R_mask = TRC_R_mask | (mask))

/*
 *  ======== TRC_query ========
 *  Returns 0 if all the trace bits specified by mask are set and both
 *  TRC_GBLTRG and TRC_GBLHOST are set; otherwise TRC_query returns a
 *  non-zero value.
 *
 *  For example,
 *      if (TRC_query(TRC_USER0 | TRC_USER1) == 0) {
 *          LOG_printf("both user0 and user1 trace is enabled");
 *      }
 *
 */
#define TRC_query(mask) \
        ( (((mask) | (TRC_GBLHOST | TRC_GBLTARG)) & TRC_R_mask)^ \
          ((mask) | (TRC_GBLHOST | TRC_GBLTARG)) )      

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* TRC_ */
