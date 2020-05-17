/** ============================================================================
 *  @file   trc.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines the interfaces and data structures for the
 *          sub-component TRC.
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


#if !defined (TRC_H)
#define TRC_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MAXIMUM_COMPONENTS
 *
 *  @desc   maximum number of components supported.
 *  ============================================================================
 */
#define MAXIMUM_COMPONENTS         16u
	

/** ============================================================================
 *  @name   ErrorInfo
 *
 *  @desc   Structure for storing error reason.
 *
 *  @field  IsSet
 *              Flag to indicate error is set.
 *  @field  ErrCode
 *              Error Code.
 *  @field  OsMajor
 *              OS  Version Major version number.
 *  @field  OsMinor
 *              OS  Version Minor version number.
 *  @field  OsBuild
 *              OS  Version Build number.
 *  @field  PddMajor
 *              PDD Version Major version number.
 *  @field  PddMinor
 *              PDD Version Minor version number.
 *  @field  PddBuild
 *              PDD Version Build number.
 *  @field  FileId
 *              ID of the file where failure occured.
 *  @field  LineNum
 *              Line number where failure occured.
 *  ============================================================================
 */
typedef struct ErrorInfo_tag {
    Bool       IsSet    ;

    DSP_STATUS ErrCode  ;

    Int32      OsMajor  ;
    Int32      OsMinor  ;
    Int32      OsBuild  ;

    Int32      PddMajor ;
    Int32      PddMinor ;
    Int32      PddBuild ;

    Int32      FileId   ;
    Int32      LineNum  ;
} ErrorInfo ;


/** ============================================================================
 *  @func   TRC_SetReason
 *
 *  @desc   This function logs failure if no previous failure has been logged.
 *
 *  @arg    status
 *              Error status to be logged.
 *  @arg    FileId
 *              File identifier.
 *  @arg    Line
 *              Line number where error occurred.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Void
TRC_SetReason (DSP_STATUS status, Int32 FileId, Int32 Line) ;

#if defined (TRACE_ENABLE)

#if defined (TRACE_KERNEL)

/** ============================================================================
 *  @macro  TRC_ENABLE
 *
 *  @desc   Wrapper for function TRC_Enable ().
 *  ============================================================================
 */
#define TRC_ENABLE(map)             TRC_Enable (map)

/** ============================================================================
 *  @macro  TRC_DISABLE
 *
 *  @desc   Wrapper for function TRC_Disable ().
 *  ============================================================================
 */
#define TRC_DISABLE(map)            TRC_Disable (map)

/** ============================================================================
 *  @macro  TRC_SET_SEVERITY
 *
 *  @desc   Wrapper for function TRC_SetSeverity ().
 *  ============================================================================
 */
#define TRC_SET_SEVERITY(level)     TRC_SetSeverity (level)

/** ============================================================================
 *  @macro  TRC_nPRINT
 *
 *  @desc   Uses corresponding TRC_nPrint function to print debug strings and
 *          optional arguments.
 *  ============================================================================
 */

#define TRC_0PRINT(a,b)                                           \
    TRC_0Print (COMPONENT_ID, (a), (Char8 *)(b))

#define TRC_1PRINT(a,b,c)                                         \
    TRC_1Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c))

#define TRC_2PRINT(a,b,c,d)                                       \
    TRC_2Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c),   \
                                              (Uint32) (d))

#define TRC_3PRINT(a,b,c,d,e)                                     \
    TRC_3Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c),   \
                                              (Uint32) (d),       \
                                              (Uint32) (e))

#define TRC_4PRINT(a,b,c,d,e,f)                                   \
    TRC_4Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c),   \
                                              (Uint32) (d),       \
                                              (Uint32) (e),       \
                                              (Uint32) (f))

#define TRC_5PRINT(a,b,c,d,e,f,g)                                 \
    TRC_5Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c),   \
                                              (Uint32) (d),       \
                                              (Uint32) (e),       \
                                              (Uint32) (f),       \
                                              (Uint32) (g))

#define TRC_6PRINT(a,b,c,d,e,f,g,h)                               \
    TRC_6Print (COMPONENT_ID, (a), (Char8 *) (b), (Uint32) (c),   \
                                              (Uint32) (d),       \
                                              (Uint32) (e),       \
                                              (Uint32) (f),       \
                                              (Uint32) (g),       \
                                              (Uint32) (h))

/** ============================================================================
 *  @name   TrcObject
 *
 *  @desc   TRC Object that stores the severity and component and
 *          subcomponent maps on a global level.
 *
 *  @field  components
 *              component map
 *  @field  level
 *              severity level
 *  @field  subcomponents
 *              subcomponent map
 *  ============================================================================
 */
typedef struct TrcObject_tag {
    Uint16 components ;
    Uint16 level      ;
    Uint16 subcomponents [MAXIMUM_COMPONENTS] ;
} TrcObject ;

/** ============================================================================
 *  @func   TRC_Enable
 *
 *  @desc   Enables debug prints on a component and subcomponent level.
 *
 *  @arg    componentMap
 *             The component & subcomponent map
 *
 *  @ret    DSP_SOK
 *              Operation successful
 *          DSP_EINVALIDARG
 *              Invalid argument to function call
 *          DSP_EFAIL
 *              Operation not successful
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    TRC_Disable, TRC_SetSeverity
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
TRC_Enable (IN Uint32 componentMap);


/** ============================================================================
 *  @func   TRC_Disable
 *
 *  @desc   Disables debug prints on a component and subcomponent level.
 *
 *  @arg    componentMap
 *             The component & subcomponent map
 *
 *  @ret    DSP_SOK
 *              Operation successful
 *          DSP_EINVALIDARG
 *              Invalid argument to function call
 *          DSP_EFAIL
 *              Operation not successful
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    TRC_Enable, TRC_SetSeverity
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
TRC_Disable (IN Uint32 componentMap);


/** ============================================================================
 *  @func   TRC_SetSeverity
 *
 *  @desc   set the severity of the required debug prints.
 *
 *  @arg    level
 *             The severity level of the debug prints required
 *
 *  @ret    DSP_SOK
 *              Operation successful
 *          DSP_EINVALIDARG
 *              Invalid argument to function call
 *          DSP_EFAIL
 *              Operation not successful
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    TRC_Enable, TRC_Disable
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
TRC_SetSeverity (IN Uint16   level) ;

/** ============================================================================
 *  @func   TRC_0Print
 *
 *  @desc   Prints a null terminated character string based on its severity,
 *          the subcomponent and component it is associated with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_1Print, TRC_2Print, TRC_3Print, TRC_4Print, TRC_5Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_0Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString) ;


/** ============================================================================
 *  @func   TRC_1Print
 *
 *  @desc   Prints a null terminated character string and an integer argument
 *          based on its severity, the subcomponent and component it is
 *          associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_2Print, TRC_3Print, TRC_4Print, TRC_5Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_1Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1) ;


/** ============================================================================
 *  @func   TRC_2Print
 *
 *  @desc   Prints a null terminated character string and two integer arguments
 *          based on its severity, the subcomponent and component it is
 *          associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The first integer argument to be printed
 *  @arg    argument2
 *             The second integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_1Print, TRC_3Print, TRC_4Print, TRC_5Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_2Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1,
            IN  Uint32   argument2) ;


/** ============================================================================
 *  @func   TRC_3Print
 *
 *  @desc   Prints a null terminated character string and three integer
 *          arguments based on its severity, the subcomponent and component it
 *          is associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The first integer argument to be printed
 *  @arg    argument2
 *             The second integer argument to be printed
 *  @arg    argument3
 *             The third integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_1Print, TRC_2Print, TRC_4Print, TRC_5Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_3Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1,
            IN  Uint32   argument2,
            IN  Uint32   argument3) ;


/** ============================================================================
 *  @func   TRC_4Print
 *
 *  @desc   Prints a null terminated character string and four integer
 *          arguments based on its severity, the subcomponent and component it
 *          is associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The first integer argument to be printed
 *  @arg    argument2
 *             The second integer argument to be printed
 *  @arg    argument3
 *             The third integer argument to be printed
 *  @arg    argument4
 *             The fourth integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_1Print, TRC_2Print, TRC_3Print, TRC_5Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_4Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1,
            IN  Uint32   argument2,
            IN  Uint32   argument3,
            IN  Uint32   argument4) ;


/** ============================================================================
 *  @func   TRC_5Print
 *
 *  @desc   Prints a null terminated character string and five integer
 *          arguments based on its severity, the subcomponent and component it
 *          is associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The first integer argument to be printed
 *  @arg    argument2
 *             The second integer argument to be printed
 *  @arg    argument3
 *             The third integer argument to be printed
 *  @arg    argument4
 *             The fourth integer argument to be printed
 *  @arg    argument5
 *             The fifth integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_1Print, TRC_2Print, TRC_3Print, TRC_4Print,
 *          TRC_6Print
 *  ============================================================================
 */
EXPORT_API
Void
TRC_5Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1,
            IN  Uint32   argument2,
            IN  Uint32   argument3,
            IN  Uint32   argument4,
            IN  Uint32   argument5) ;


/** ============================================================================
 *  @func   TRC_6Print
 *
 *  @desc   Prints a null terminated character string and six integer
 *          arguments based on its severity, the subcomponent and component it
 *          is associated  with.
 *
 *  @arg    componentMap
 *             The component & subcomponent to which this print belongs
 *  @arg    severity
 *             The severity associated with the print
 *  @arg    debugString
 *             The null terminated character string to be printed
 *  @arg    argument1
 *             The first integer argument to be printed
 *  @arg    argument2
 *             The second integer argument to be printed
 *  @arg    argument3
 *             The third integer argument to be printed
 *  @arg    argument4
 *             The fourth integer argument to be printed
 *  @arg    argument5
 *             The fifth integer argument to be printed
 *  @arg    argument6
 *             The sixth integer argument to be printed
 *
 *  @ret    None
 *
 *  @enter  The character string is valid
 *
 *  @leave  None
 *
 *  @see    TRC_0Print, TRC_1Print, TRC_2Print, TRC_3Print, TRC_4Print,
 *          TRC_5Print.
 *  ============================================================================
 */
EXPORT_API
Void
TRC_6Print (IN  Uint32   componentMap,
            IN  Uint16   severity,
            IN  Char8 *  debugString,
            IN  Uint32   argument1,
            IN  Uint32   argument2,
            IN  Uint32   argument3,
            IN  Uint32   argument4,
            IN  Uint32   argument5,
            IN  Uint32   argument6) ;

#else /* defined (TRACE_KERNEL) */

#define TRC_ENABLE(map)
#define TRC_DISABLE(map)
#define TRC_SET_SEVERITY(level)

#define TRC_0PRINT(a,b)                 \
    PRINT_Printf (b)

#define TRC_1PRINT(a,b,c)               \
    PRINT_Printf ((b), (int)(c))

#define TRC_2PRINT(a,b,c,d)             \
    PRINT_Printf ((b), (int)(c),        \
                       (int)(d))

#define TRC_3PRINT(a,b,c,d,e)           \
    PRINT_Printf ((b),(int)(c),         \
                      (int)(d),         \
                       (int)(e))

#define TRC_4PRINT(a,b,c,d,e,f)         \
    PRINT_Printf ((b), (int) (c),       \
                       (int) (d),       \
                       (int) (e),       \
                       (int) (f))

#define TRC_5PRINT(a,b,c,d,e,f,g)       \
    PRINT_Printf ((b), (int) (c),       \
                       (int) (d),       \
                       (int) (e),       \
                       (int) (f),       \
                       (int) (g))

#define TRC_6PRINT(a,b,c,d,e,f,g,h)     \
    PRINT_Printf ((b), (int) (c),       \
                       (int) (d),       \
                       (int) (e),       \
                       (int) (f),       \
                       (int) (g),       \
                       (int) (h))
#endif /* defined(TRACE_KERNEL) */

#define TRC_0ENTER(str)                     \
    TRC_0PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n")

#define TRC_1ENTER(str,a)                   \
    TRC_1PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n",         \
                a)

#define TRC_2ENTER(str,a,b)                 \
    TRC_2PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n"          \
                "\t"#b"\t[0x%x]\n",         \
                a,b)

#define TRC_3ENTER(str,a,b,c)               \
    TRC_3PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n"          \
                "\t"#b"\t[0x%x]\n"          \
                "\t"#c"\t[0x%x]\n",         \
                a,b,c)

#define TRC_4ENTER(str,a,b,c,d)             \
    TRC_4PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n"          \
                "\t"#b"\t[0x%x]\n"          \
                "\t"#c"\t[0x%x]\n"          \
                "\t"#d"\t[0x%x]\n",         \
                a,b,c,d)

#define TRC_5ENTER(str,a,b,c,d,e)           \
    TRC_5PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n"          \
                "\t"#b"\t[0x%x]\n"          \
                "\t"#c"\t[0x%x]\n"          \
                "\t"#d"\t[0x%x]\n"          \
                "\t"#e"\t[0x%x]\n",         \
                a,b,c,d,e)

#define TRC_6ENTER(str,a,b,c,d,e,f)         \
    TRC_6PRINT (TRC_ENTER,                  \
                "Entered " str " ()\n"    \
                "\t"#a"\t[0x%x]\n"          \
                "\t"#b"\t[0x%x]\n"          \
                "\t"#c"\t[0x%x]\n"          \
                "\t"#d"\t[0x%x]\n"          \
                "\t"#e"\t[0x%x]\n"          \
                "\t"#f"\t[0x%x]\n",         \
                a,b,c,d,e,f)

#define TRC_0LEAVE(str)                     \
    TRC_0PRINT (TRC_LEAVE,                  \
                "Leaving " str " ()\n")

#define TRC_1LEAVE(str,status)                                  \
    TRC_1PRINT (TRC_LEAVE,                                      \
                "Leaving " str " () \t"#status" [0x%x]\n",    \
                status)


#else  /* defined (TRACE_ENABLE) */

#define TRC_ENABLE(map)
#define TRC_DISABLE(map)
#define TRC_SET_SEVERITY(level)

#define TRC_0PRINT(a,b)
#define TRC_1PRINT(a,b,c)
#define TRC_2PRINT(a,b,c,d)
#define TRC_3PRINT(a,b,c,d,e)
#define TRC_4PRINT(a,b,c,d,e,f)
#define TRC_5PRINT(a,b,c,d,e,f,g)
#define TRC_6PRINT(a,b,c,d,e,f,g,h)

#define TRC_0ENTER(str)
#define TRC_1ENTER(str,a)
#define TRC_2ENTER(str,a,b)
#define TRC_3ENTER(str,a,b,c)
#define TRC_4ENTER(str,a,b,c,d)
#define TRC_5ENTER(str,a,b,c,d,e)
#define TRC_6ENTER(str,a,b,c,d,e,f)

#define TRC_0LEAVE(str)
#define TRC_1LEAVE(str,status)


#endif  /* defined (TRACE_ENABLE) */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* !defined (TRC_H) */
