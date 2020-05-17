/*****************************************************************************/
/* locale.h v6.1.14                                                           */
/* Copyright (c) 1993-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/
#ifndef _LOCALE
#define _LOCALE


#ifdef __cplusplus
extern "C" namespace std {
#endif

#ifndef NULL
 #define NULL	0
#endif

		/* LOCALE CATEGORY INDEXES */

#define LC_ALL                  0
#define LC_COLLATE		1
#define LC_CTYPE		2
#define LC_MONETARY	        3
#define LC_NUMERIC		4
#define LC_TIME		        5
#define LC_MESSAGES	        6


		/* TYPE DEFINITIONS */
struct lconv
{
    /* LC_NUMERIC */
    char *decimal_point;
    char *grouping;
    char *thousands_sep;
    /* LC_MONETARY */
    char *mon_decimal_point;
    char *mon_grouping;
    char *mon_thousands_sep;

    char *negative_sign;
    char *positive_sign;

    char *currency_symbol;

    char frac_digits;
    char n_cs_precedes;
    char n_sep_by_space;
    char n_sign_posn;
    char p_cs_precedes;
    char p_sep_by_space;
    char p_sign_posn;

    char *int_curr_symbol;
    char int_frac_digits;
};

		/* DECLARATIONS */
struct lconv *localeconv(void);

char *setlocale(int, const char *);

#ifdef __cplusplus
} /* extern "C" namespace std */
#endif /* __cplusplus */

#endif /* _LOCALE */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)

using std::lconv; using std::localeconv; using std::setlocale;


#endif /* _CPP_STYLE_HEADER */

