/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestApp.h
  @brief    This is the top level client header file that drives the G711
            Speech Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.1 - Jan 30,2006    initial version
********************************************************************************
*/

#ifndef _TESTAPP_
#define _TESTAPP_


#include <xdas.h>
#include <xdm.h>

#define STRING_SIZE            256
#define NUM_ALGS                 1      /* Number of Algorithms              */

/*!
@struct sTokenMapping 
@brief  Token Mapping structure for parsing codec specific configuration file
@param  tokenName : String name in the configuration file
@param  place     : Place Holder for the data
*/

typedef struct _sTokenMapping {
  XDAS_Int8 *tokenName;
  XDAS_Void *place;
} sTokenMapping;

/* Function Declarations */
/*!
********************************************************************************
  @fn                 XDAS_Void  TestApp_EnableCache(void)
  @brief              Enable cache settings for system
  @param   void       None
  @return             None
********************************************************************************
*/
XDAS_Void   TestApp_EnableCache(void);

/*!
********************************************************************************
  @fn                       XDAS_Int32 readparamfile(FILE * fname) ;
  @brief                    Parses codec specific parameter file and populates
                            configurable parameters
  @param  fname[IN]         parameter FILE handle
  @return                   NONE
********************************************************************************
*/
XDAS_Int32 readparamfile(FILE * fname) ;

/*!
********************************************************************************
  @fn                       XDAS_Void byteSwapBitStream(XDAS_Int16 *p, 
                                XDAS_Int16 size) ;
  @brief                    Swaps the bytes of the input bitstream supplied
                            for the supplied size.
  @param  p[IN]             input bit-stream
  @param  size[IN]          size of the input bit-stream
  @return                   NONE
********************************************************************************
*/
XDAS_Void byteSwapBitStream(XDAS_Int16 *p, XDAS_Int16 size) ;

#endif //_TESTAPP_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
