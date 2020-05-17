/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== RtdxModule.xdc ========
 *
 *
 */

/*!
 *  ======== RtdxModule ========
 *  the module for setting RTDX protocol and RTDX buffer size, etc.
 */

@Template("./RtdxModule.xdt")

metaonly module RtdxModule {
    /*! Rtdx Transfer Protocol. */
    enum TransferProtocol {
        PROTOCOL_SIMULATION,
        PROTOCOL_JTAG,
        PROTOCOL_HIGH_SPEED
    };

    config TransferProtocol protocol = PROTOCOL_JTAG;

    /*! Write buffer size in words */
    config UInt bufferSizeInWords = 256;

    /*! No Auto Init (do not call RTDX_Init_RT_Monitor at startup) */
    config bool noAutoInit = false;

    config bool overrideLegacyLib = false;

}

/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

