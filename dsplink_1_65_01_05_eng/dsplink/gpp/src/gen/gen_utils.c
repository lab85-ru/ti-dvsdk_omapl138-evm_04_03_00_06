/** ============================================================================
 *  @file   gen_utils.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implementation of platform independent common library
 *          functions interface.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and subcomponent identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_GEN_UTILS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_GEN_UTILS, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   GEN_Initialize
 *
 *  @desc   Initializes the GEN module's private state.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Initialize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("GEN_Initialize") ;

    TRC_1LEAVE ("GEN_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Finalize
 *
 *  @desc   Discontinues usage of the GEN module.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Finalize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("GEN_Finalize") ;

    TRC_1LEAVE ("GEN_Finalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_NumToAscii
 *
 *  @desc   Converts a 1 or 2 digit number to a 2 digit string.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_NumToAscii (IN Uint32 number, OUT Char8 * strNumber)
{
    DSP_STATUS  status = DSP_SOK ;
    Char8       tens   = '0'     ;

    TRC_2ENTER ("GEN_NumToAscii", number, strNumber) ;

    DBC_Require (number <= 99) ;
    DBC_Require (strNumber != NULL) ;

    if ((number > 99) || (strNumber == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;

    }
    else {
        tens = (Char8) (number / 10) ;
        number = number % 10 ;
        if (tens != 0) {
            strNumber [0] = (Char8) (tens + '0') ;
            strNumber [1] = (Char8) (number + '0') ;
            strNumber [2] = '\0' ;
        }
        else {
            strNumber [0] = (Char8) (number + '0') ;
            strNumber [1] = '\0' ;
        }
    }

    DBC_Ensure (strNumber != NULL) ;

    TRC_1LEAVE ("GEN_NumToAscii", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Strcmp
 *
 *  @desc   Compares 2 ASCII strings.  Works the same way as stdio's strcmp.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcmp (IN  CONST Char8 * string1,
            IN  CONST Char8 * string2,
            OUT       Int32 * cmpResult)
{
    DSP_STATUS     status         = DSP_SOK ;
    Int32          charCompare    = 0       ;
    Uint32          string1Length = 0       ;
    Uint32          string2Length = 0       ;

    TRC_3ENTER ("GEN_Strcmp", string1, string2, cmpResult) ;

    DBC_Require (string1 != NULL) ;
    DBC_Require (string2 != NULL) ;
    DBC_Require (cmpResult != NULL) ;

    if ((string1 == NULL) || (string2 == NULL) || (cmpResult == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {

        status = GEN_Strlen (string1, &string1Length) ;
        DBC_Assert (DSP_SUCCEEDED (status)) ;

        status = GEN_Strlen (string2, &string2Length) ;
        DBC_Assert (DSP_SUCCEEDED (status)) ;

        if (string1Length == string2Length) {

            /* iterate for comparing the strings - character by character
             * Note: The comparison takes care of termination of both strings
             *  - by explicitly checking for string termination of string2
             *  - the check for string1 termination is implicit, since when
             *    string1 terminates with a NULL ('\0') the charCompare value
             *    will be negative (if string2 also doesn't terminate then).
             */
            charCompare =  ((*(const Uint8 *) ((Pvoid) string1))
                    - (*(const Uint8 *) ((Pvoid) string2))) ;
            while ((charCompare == 0) && (*string2 != '\0')) {
                charCompare =  ((*(const Uint8 *) ((Pvoid) string1))
                        - (*(const Uint8 *)  ((Pvoid) string2))) ;
                ++string1 ;
                ++string2 ;
            }

            if (charCompare < 0) {
                charCompare = -1 ;
            }
            else if (charCompare > 0) {
                charCompare = 1 ;
            }
        }
        else {
            if (string1Length < string2Length) {
                charCompare = -1 ;
            }
            else {
                charCompare = 1 ;
            }
        }

        *cmpResult = charCompare ;
    }

    TRC_1LEAVE ("GEN_Strcmp", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Strcpyn
 *
 *  @desc   Safe strcpy function.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcpyn (OUT Char8 * destination,
             IN  Char8 * source,
             IN  Int32   maxNum)
{
    DSP_STATUS     status = DSP_SOK     ;
    Char8 *        dest   = NULL        ;
    CONST Char8 *  src    = NULL        ;

    TRC_3ENTER ("GEN_Strcpyn", destination, source, maxNum) ;

    DBC_Require (destination != NULL) ;
    DBC_Require (source != NULL) ;
    DBC_Require (maxNum >= 0) ;

    if ((destination == NULL) || (source == NULL) || (maxNum < 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dest   = destination ;
        src    = source      ;

        DBC_Assert (src != NULL) ;
        DBC_Assert (dest != NULL) ;

        while ((maxNum != 0) && (*src != '\0')) {
            *dest++ = *src++ ;
            maxNum-- ;
        }

        if (maxNum != 0) {
            /* pad out with zeroes */
            while (--maxNum) {
                *dest++ = '\0' ;
            }
        }
        else {
            *dest = '\0' ;
        }
    }

    TRC_1LEAVE ("GEN_Strcpyn", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Strlen
 *
 *  @desc   Determines the length of a null terminated ASCI string.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strlen (IN CONST Char8 * strSrc, OUT Uint32 * length)
{
    DSP_STATUS     status = DSP_SOK ;
    CONST Char8 *  str    = NULL    ;

    TRC_2ENTER ("GEN_Strlen", strSrc, length) ;

    DBC_Require (strSrc != NULL) ;
    DBC_Require (length != NULL) ;

    if ((strSrc == NULL) || (length == NULL)) {
        status = DSP_EINVALIDARG ;
    }
    else {
        str = strSrc  ;
        DBC_Assert (str != NULL) ;

        while (*str++)
        {
            /* Added for MISRAC compliance */
        }

        *length = (Uint32) (str - strSrc - 1) ;
    }

    TRC_1LEAVE ("GEN_Strlen", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_WcharToAnsi
 *
 *  @desc   Converts a wide char string to an ansi string.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_WcharToAnsi (OUT Char8 *   destination,
                 IN  Char16 *  source,
                 IN  Int32     numChars)
{
    DSP_STATUS  status = DSP_SOK     ;
    Char8 *     temp   = NULL        ;

    TRC_3ENTER ("GEN_WcharToAnsi", destination, source, numChars) ;

    DBC_Require (destination != NULL) ;
    DBC_Require (source != NULL) ;
    DBC_Require (numChars >= 0) ;

    if ((source == NULL) || (destination == NULL) || (numChars == 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else  {
        temp = destination ;
        DBC_Assert (temp != NULL) ;
        while ((*source != '\0') && (numChars > 0)) {
            *temp++= (Char8) *source++ ;
            numChars-- ;
        }
        *temp = '\0' ;
    }

    TRC_1LEAVE ("GEN_WcharToAnsi", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_AnsiToWchar
 *
 *  @desc   Converts an ANSI string to a wide char string.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_AnsiToWchar (OUT Char16 *  destination,
                 IN  Char8 *   source,
                 IN  Int32     numChars)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("GEN_AnsiToWchar", destination, source, numChars) ;

    DBC_Require (destination != NULL) ;
    DBC_Require (source != NULL) ;
    DBC_Require (numChars >= 0) ;

    if ((source == NULL) || (destination == NULL) || (numChars == 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        while ((*source != '\0') && (numChars > 0)) {
            *destination++= (Char16) *source++ ;
            numChars-- ;
        }
        *destination = (Char16) '\0' ;
    }

    TRC_1LEAVE ("GEN_AnsiToWchar", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Wstrlen
 *
 *  @desc   Determines the length of a null terminated wide character string.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Wstrlen (IN Char16 * strSrc, OUT Uint32 * length)
{
    DSP_STATUS      status = DSP_SOK ;
    CONST Char16 *  str    = NULL ;

    TRC_2ENTER ("GEN_Wstrlen", strSrc, length) ;

    DBC_Require (strSrc != NULL) ;
    DBC_Require (length != NULL );

    if ((strSrc == NULL) || (length == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        str = strSrc  ;
        DBC_Assert (str != NULL) ;
        while (*str++)
        {
        }
        *length = str - strSrc - 1 ;
    }

    TRC_1LEAVE ("GEN_Wstrlen", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Strcatn
 *
 *  @desc   Safe strcat function.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcatn (OUT Char8 * destination,
             IN  Char8 * source,
             IN  Int32   maxNum)
{
    DSP_STATUS     status = DSP_SOK     ;
    Char8 *        dest   = NULL        ;
    CONST Char8 *  src    = NULL        ;

    TRC_3ENTER ("GEN_Strcatn", destination, source, maxNum) ;

    DBC_Require (destination != NULL) ;
    DBC_Require (source != NULL) ;
    DBC_Require (maxNum >= 0) ;

    if ((destination == NULL) || (source == NULL) || (maxNum < 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dest   = destination ;
        src    = source      ;
        DBC_Assert (dest != NULL) ;
        DBC_Assert (src != NULL) ;

        while ((maxNum != 0) && (*dest != '\0')) {
            dest++ ;
            maxNum-- ;
        }
        while ((maxNum != 0) && (*src != '\0')) {
            *dest++ = *src++ ;
            maxNum-- ;
        }

        if (maxNum != 0) {
            /* pad out with zeroes */
            while (--maxNum) {
                *dest++ = '\0' ;
            }
        }
        else {
            *dest = '\0' ;
        }
    }

    TRC_1LEAVE ("GEN_Strcatn", status) ;

    return status ;
}


/** ============================================================================
 *  @func   GEN_Strcmpn
 *
 *  @desc   Function to compare two string only first n characters.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
GEN_Strcmpn (IN  Char8 * str1,
             IN  Char8 * str2,
             IN  Int32   numChars,
             OUT Int32 * cmpResult)
{
    DSP_STATUS     status = DSP_SOK     ;

    TRC_4ENTER ("GEN_Strcmpn", str1, str2, numChars, cmpResult) ;

    DBC_Require (str1 != NULL) ;
    DBC_Require (str2 != NULL) ;
    DBC_Require (cmpResult != NULL) ;
    DBC_Require (numChars >= 0) ;

    if ((str1 == NULL) || (str2 == NULL) || (numChars < 0) || ((cmpResult == NULL))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        while (numChars) {
            if (((*cmpResult = *str1 - *str2++) != 0) || !*str1++) {
                break ;
            }
            numChars-- ;
        }
    }

    TRC_1LEAVE ("GEN_Strcmpn", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
