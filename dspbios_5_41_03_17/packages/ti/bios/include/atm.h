/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== atm.h ========
 *
 *  Atomic processor operations:
 *      ATM_and()   ATM_clear() ATM_dec()
 *      ATM_inc()   ATM_or()    ATM_set()
 *
 */

#ifndef ATM_
#define ATM_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== ATM_and ========
 *  atomically and integer or unsigned with mask and return its old value
 */
extern Int ATM_andi( volatile Int *dst, Int mask );
extern Uns ATM_andu( volatile Uns *dst, Uns mask );

/*
 *  ======== ATM_clear ========
 *  atomically clear integer or unsigned and return its old value
 */
extern Int ATM_cleari( volatile Int *dst );
extern Uns ATM_clearu( volatile Uns *dst );

/*
 *  ======== ATM_dec ========
 *  atomically decrement integer or unsigned and return new value
 */
extern Int ATM_deci( volatile Int *dst );
extern Uns ATM_decu( volatile Uns *dst );

/*
 *  ======== ATM_inc ========
 *  atomically increment integer or unsigned and return new value
 */
extern Int ATM_inci( volatile Int *dst );
extern Uns ATM_incu( volatile Uns *dst );

/*
 *  ======== ATM_or ========
 *  atomically or integer or unsigned with mask and return its old value
 */
extern Int ATM_ori( volatile Int *dst, Int mask );
extern Uns ATM_oru( volatile Uns *dst, Uns mask );

/*
 *  ======== ATM_set ========
 *  atomically set integer or unsigned and return its old value
 */
extern Int ATM_seti( volatile Int *dst, Int x );
extern Uns ATM_setu( volatile Uns *dst, Uns x );

#ifdef _BIOSROM_

extern Int (*ATM_andiPtr)(volatile Int *dst, Int mask);
extern Uns (*ATM_anduPtr)(volatile Uns *dst, Uns mask);
extern Int (*ATM_cleariPtr)(volatile Int *dst);
extern Uns (*ATM_clearuPtr)(volatile Uns *dst);
extern Int (*ATM_deciPtr)(volatile Int *dst);
extern Uns (*ATM_decuPtr)(volatile Uns *dst);
extern Int (*ATM_inciPtr)(volatile Int *dst);
extern Uns (*ATM_incuPtr)(volatile Uns *dst);
extern Int (*ATM_oriPtr)(volatile Int *dst, Int mask);
extern Uns (*ATM_oruPtr)(volatile Uns *dst, Uns mask);
extern Int (*ATM_setiPtr)(volatile Int *dst, Int x);
extern Uns (*ATM_setuPtr)(volatile Uns *dst, Uns x);

#define ATM_andiJ(dst, mask)    (ATM_andiPtr)(dst, mask)
#define ATM_anduJ(dst, mask)    (ATM_anduPtr)(dst, mask)
#define ATM_cleariJ(dst)        (ATM_cleariPtr)(dst)
#define ATM_clearuJ(dst)        (ATM_clearuPtr)(dst)
#define ATM_deciJ(dst)          (ATM_deciPtr)(dst)
#define ATM_decuJ(dst)          (ATM_decuPtr)(dst)
#define ATM_inciJ(dst)          (ATM_inciPtr)(dst)
#define ATM_incuJ(dst)          (ATM_incuPtr)(dst)
#define ATM_oriJ(dst, mask)     (ATM_oriPtr)(dst, mask)
#define ATM_oruJ(dst, mask)     (ATM_oruPtr)(dst, mask)
#define ATM_setiJ(dst, x)       (ATM_setiPtr)(dst, x)
#define ATM_setuJ(dst, x)       (ATM_setuPtr)(dst, x)

#else

#define ATM_andiJ(dst, mask)    ATM_andi(dst, mask)
#define ATM_anduJ(dst, mask)    ATM_andu(dst, mask)
#define ATM_cleariJ(dst)        ATM_cleari(dst)
#define ATM_clearuJ(dst)        ATM_clearu(dst)
#define ATM_deciJ(dst)          ATM_deci(dst)
#define ATM_decuJ(dst)          ATM_decu(dst)
#define ATM_inciJ(dst)          ATM_inci(dst)
#define ATM_incuJ(dst)          ATM_incu(dst)
#define ATM_oriJ(dst, mask)     ATM_ori(dst, mask)
#define ATM_oruJ(dst, mask)     ATM_oru(dst, mask)
#define ATM_setiJ(dst, x)       ATM_seti(dst, x)
#define ATM_setuJ(dst, x)       ATM_setu(dst, x)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* ATM_ */
