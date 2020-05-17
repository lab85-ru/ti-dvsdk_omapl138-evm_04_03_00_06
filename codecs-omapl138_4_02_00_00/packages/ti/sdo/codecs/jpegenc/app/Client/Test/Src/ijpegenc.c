/* Standard C header files                                                    */
        typedef int                 XDAS_Int32;
	    typedef short               XDAS_Int16;
	    typedef char                XDAS_Int8;
	    typedef unsigned int        XDAS_UInt32;
	    typedef unsigned short      XDAS_UInt16;
        typedef unsigned char       XDAS_UInt8;

#include <idmjpge.h>
#include <dmjpge_tigem.h>

  IDMJPGE_TIGEM_Params DMJPGE_PARAMS = {
  sizeof (IDMJPGE_TIGEM_Params),       /* Size of params structure */
  65520,
  65520,
  0,
  XDM_LE_32,
  XDM_YUV_422P,
  XDM_DEFAULT,
  XDM_DEFAULT,
  XDM_DEFAULT,
  XDM_DEFAULT,
  XDM_DEFAULT,
  XDM_DEFAULT
};
