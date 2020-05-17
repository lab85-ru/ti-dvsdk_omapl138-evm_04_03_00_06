/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gconf.h ========
 *
 *
 */

/*
 *  ======== gconf Field Types ========
 *  The following definitions are all of the field types currently
 *  supported by gconf; i.e., fields with the "Type" property.
 *
 *  An integral style can be specified by including a "Style" property
 *  for the field in question.  If a "Style" property is provided,
 *  it should return an integer with bits carrying meanings as follows:
 *
 *  #define GCONF_SHORTSTYLE    0x01
 *  #define GCONF_UNSIGNEDSTYLE 0x02
 *
 *  The absence of a "Style" property implies that the field is a
 *  signed 32-bit integer.
 *
 *  These definitions should be in a separate header defined by gconf.
 */

#define GCONF_INT32     "{21455EA2-B96A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_INT       "{21455EA2-B96A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_BOOL      "{21455EA0-B96A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_STRING    "{21455EA3-B96A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_EINT16    "{A11FE980-C07A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_ESTRING   "{21455EA1-B96A-11cf-9BFE-0000C0AC14C7}"
#define GCONF_LABEL     "{7C434D00-1629-11d0-9BFE-0000C0AC14C7}"
#define GCONF_ARG       "{7C434D01-1629-11d0-9BFE-0000C0AC14C7}"
#define GCONF_STRARG    "{E91790E0-0456-11d2-B8D0-006008BDB66F}"
#define GCONF_REF       "{7BA2DA00-5A53-11d0-9BFE-0000C0AC14C7}"
#define GCONF_NUMERIC   "{3EE4CB80-E273-11d0-BB8F-0000C008F2E9}"
#define GCONF_TEXTBOX   "{0C355120-70B4-11d1-B3DD-0020AFEAE00F}"
#define GCONF_FORMATSTR "{A8EEDC40-35D8-11d2-B8D0-006008BDB66F}"

#define GCONF_SHORTSTYLE    0x01        
#define GCONF_UNSIGNEDSTYLE 0x02
#define GCONF_FLOATSTYLE    0x04

/* 
 *  These three styles here are used by the generation 
 *  to determine how many bits to generate
 */
#define GCONF_CODEPTRSTYLE  0x08
#define GCONF_DATAPTRSTYLE  0x10
#define GCONF_LONGSTYLE     0x20        /* Generate 32 bits on all targets */
