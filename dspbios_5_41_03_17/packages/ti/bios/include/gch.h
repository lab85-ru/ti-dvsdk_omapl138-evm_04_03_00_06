/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*!
 *!  ======== gch.h ========
 *!  Graphical Character Set definitions
 *!
 */

#ifndef GCH_
#define GCH_

/*
 *  ======== GCH_Char ========
 *  The graphical character type.  Each graphical character has
 *  two properties, color and shape.
 */
typedef unsigned int GCH_Char;

#define GCH_COLORSHIFT      0
#define GCH_COLORMASK       0xff        /* 8 bit color index */

#define GCH_SHAPESHIFT      8
#define GCH_SHAPEMASK       0xff        /* 8 bit shape value */

/*
 *  ======== Color definitions ========
 */
/*
   If modified, also check:
        _VSM_stateInfoTab in vsm_tab.c (vbd)
        graph state values in _vsm.h (vbd)
        LOG_EXECUTION_STATE_COUNT in rta.h (rta)
        gcrColor  in stateplot.cpp (gui)
        NUM_COLORS in statechartapp.h (gui)
*/
#define GCH_CBLANK          0   /* completely invisible */
#define GCH_CTRANSPARENT    1   /* invisible except outline of solid shapes */
#define GCH_COLOR1          2   /* user definable color */
#define GCH_COLOR2          3   /* user definable color */
#define GCH_COLOR3          4   /* user definable color */
#define GCH_COLOR4          5   /* user definable color */
#define GCH_COLOR5          6   /* user definable color */
#define GCH_COLOR6          7   /* dsg-8/18/00-user definable color */

/*
 *  ======== Shape definitions ========
 */
#define GCH_SBOX        0       /* solid box */
#define GCH_SUPTICK     1       /* straight line with centered up tick */
#define GCH_SDOWNTICK   2       /* straight line with centered down tick */
#define GCH_SLINE       3       /* straight line */

/*
 *  ======== GCH_getColor ========
 *  Extract color value from a graphical character.
 */
#define GCH_getColor(c) ((c) & GCH_COLORMASK)

/*
 *  ======== GCH_getShape ========
 *  Extract shape value from a graphical character.
 */
#define GCH_getShape(c) (((c) >> GCH_SHAPESHIFT) & GCH_SHAPEMASK)

/*
 *  ======== GCH_makeChar ========
 *  Construct a graphical character from the shape and color
 *  parameters
 */
#define GCH_makeChar(c,s) ((c) | ((s) << GCH_SHAPESHIFT))

#endif  /* GCH_ */
