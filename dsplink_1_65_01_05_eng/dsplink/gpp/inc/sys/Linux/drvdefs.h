/** ============================================================================
 *  @file   drvdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/Linux/
 *
 *  @desc   Linux module driver interface file.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (DRV_PMGR_H)
#define DRV_PMGR_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


#include <linux/ioctl.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>
#include <_idm_usr.h>
#if defined (POOL_COMPONENT)
#include <pooldefs.h>
#endif /* if defined (POOL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (CHNL_COMPONENT)
#include <chnldefs.h>
#endif /* if defined (CHNL_COMPONENT) */
#if defined (RINGIO_COMPONENT)
#include <_ringiodefs.h>
#include <ringiodefs.h>
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (MPCS_COMPONENT)
#include <mpcsdefs.h>
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
#include <mplistdefs.h>
#endif /* if defined (MPLIST_COMPONENT) */

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */

/*  ============================================================================
 *  @const  GROUP_BASE_CMD
 *
 *  @desc   Base command id to be used for trap/ioctl.
 *  ============================================================================
 */
#define GROUP_BASE_CMD                     0xe0

/*  ============================================================================
 *  @const  BASE_CMD
 *
 *  @desc   Base command id to be used for trap/ioctl.
 *  ============================================================================
 */
#define BASE_CMD                           0x10

/*  ============================================================================
 *  @macro  CMD_PROC_XXXX
 *
 *  @desc   Command ids for PROC functions.
 *  ============================================================================
 */
#define CMD_PROC_SETUP                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 1u,\
                                            CMD_Args)

#define CMD_PROC_DESTROY                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 2u,\
                                            CMD_Args)


#define CMD_PROC_START                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 3u,\
                                            CMD_Args)

#define CMD_PROC_STOP                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 4u,\
                                            CMD_Args)

#define CMD_PROC_LOAD                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 5u,\
                                            CMD_Args)

#define CMD_PROC_LOADSECTION               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 6u,\
                                            CMD_Args)

#define CMD_PROC_DETACH                    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 7u,\
                                            CMD_Args)

#define CMD_PROC_ATTACH                    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 8u,\
                                            CMD_Args)

#define CMD_PROC_GETSTATE                  _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 9u,\
                                            CMD_Args)

#define CMD_PROC_CONTROL                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 10u,\
                                            CMD_Args)

#define CMD_PROC_READ                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 11u,\
                                            CMD_Args)

#define CMD_PROC_WRITE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 12u,\
                                            CMD_Args)

#define CMD_PROC_GETSYMBOLADDRESS          _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 13u,\
                                            CMD_Args)

#define CMD_PROC_ISLASTDESTROY             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 14u,\
                                            CMD_Args)

#define CMD_PROC_ISLASTDETACH              _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 15u,\
                                            CMD_Args)


#if defined (DDSP_PROFILE)
#define CMD_PROC_INSTRUMENT                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 16u,\
                                            CMD_Args)

#endif

#if defined (DDSP_DEBUG)
#define CMD_PROC_DEBUG                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 17u,\
                                            CMD_Args)

#endif

#define CMD_PROC_CLEANUP                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 18u,\
                                            CMD_Args)

#define CMD_PROC_ADDRTRANSLATE             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 19u,\
                                            CMD_Args)



#if defined (CHNL_COMPONENT)
/*  ============================================================================
 *  @macro  CMD_CHNL_XXXX
 *
 *  @desc   Command ids for CHNL functions.
 *  ============================================================================
 */
#define CMD_CHNL_CREATE                    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 20u,\
                                            CMD_Args)

#define CMD_CHNL_DELETE                    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 21u,\
                                            CMD_Args)

#define CMD_CHNL_ISSUE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 22u,\
                                            CMD_Args)

#define CMD_CHNL_RECLAIM                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 23u,\
                                            CMD_Args)

#define CMD_CHNL_ALLOCATEBUFFER            _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 24u,\
                                            CMD_Args)

#define CMD_CHNL_FREEBUFFER                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 25u,\
                                            CMD_Args)

#define CMD_CHNL_IDLE                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 26u,\
                                            CMD_Args)

#define CMD_CHNL_FLUSH                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 27u,\
                                            CMD_Args)

#define CMD_CHNL_CONTROL                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 28u,\
                                            CMD_Args)



#if defined (DDSP_PROFILE)
#define CMD_CHNL_INSTRUMENT                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 29u,\
                                            CMD_Args)

#endif /* if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
#define CMD_CHNL_DEBUG                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 30u,\
                                            CMD_Args)

#endif /* if defined (DDSP_DEBUG) */
#endif /* if defined (CHNL_COMPONENT) */


/*  ============================================================================
 *  @macro  CMD_DRV_XXXX
 *
 *  @desc   Command ids for DRV specific calls.
 *  ============================================================================
 */
#if defined (CHNL_COMPONENT)
#define CMD_DRV_GETCHNLMAPTABLE_ADDRESS    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 31u,\
                                            CMD_Args)

#endif /* if defined (CHNL_COMPONENT) */


#if defined (MSGQ_COMPONENT)
/*  ============================================================================
 *  @macro  CMD_MSGQ_XXXX
 *
 *  @desc   Command ids for CHNL functions.
 *  ============================================================================
 */
#define CMD_MSGQ_TRANSPORTOPEN             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 32u,\
                                            CMD_Args)

#define CMD_MSGQ_TRANSPORTCLOSE            _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 33u,\
                                            CMD_Args)

#define CMD_MSGQ_OPEN                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 34u,\
                                            CMD_Args)

#define CMD_MSGQ_CLOSE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 35u,\
                                            CMD_Args)

#define CMD_MSGQ_LOCATE                    _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 36u,\
                                            CMD_Args)

#define CMD_MSGQ_LOCATEASYNC               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 37u,\
                                            CMD_Args)

#define CMD_MSGQ_RELEASE                   _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 38u,\
                                            CMD_Args)

#define CMD_MSGQ_ALLOC                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 39u,\
                                            CMD_Args)

#define CMD_MSGQ_FREE                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 40u,\
                                            CMD_Args)

#define CMD_MSGQ_PUT                       _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 41u,\
                                            CMD_Args)

#define CMD_MSGQ_GET                       _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 42u,\
                                            CMD_Args)

#define CMD_MSGQ_SETERRORHANDLER           _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 43u,\
                                            CMD_Args)

#define CMD_MSGQ_COUNT                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 44u,\
                                            CMD_Args)



#if defined (DDSP_PROFILE)
#define CMD_MSGQ_INSTRUMENT                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 45u,\
                                            CMD_Args)

#endif /* if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
#define CMD_MSGQ_DEBUG                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 46u,\
                                            CMD_Args)

#endif /* if defined (DDSP_DEBUG) */


#endif /* if defined (MSGQ_COMPONENT) */

/*  ============================================================================
 *  @macro  CMD_POOL_XXXX
 *
 *  @desc   Command ids for POOL.
 *  ============================================================================
 */
#define CMD_POOL_OPEN                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 47u,\
                                            CMD_Args)

#define CMD_POOL_CLOSE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 48u,\
                                            CMD_Args)

#define CMD_POOL_ALLOC                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 49u,\
                                            CMD_Args)

#define CMD_POOL_FREE                      _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 50u,\
                                            CMD_Args)

#define CMD_POOL_TRANSLATEADDR             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 51u,\
                                            CMD_Args)

#define CMD_POOL_RECONFIGURE               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 52u,\
                                            CMD_Args)

#define CMD_POOL_WRITEBACK                 _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 53u,\
                                            CMD_Args)

#define CMD_POOL_INVALIDATE                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 54u,\
                                            CMD_Args)


#if defined (MPCS_COMPONENT)
/*  ============================================================================
 *  @macro  CMD_MPCS_XXXX
 *
 *  @desc   Command ids for MPCS.
 *  ============================================================================
 */
#define CMD_MPCS_MAPREGION                 _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 55u,\
                                            CMD_Args)

#define CMD_MPCS_UNMAPREGION               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 56u,\
                                            CMD_Args)

#endif /* if defined (MPCS_COMPONENT) */

/*  ============================================================================
 *  @macro  CMD_NOTIFY_XXXX
 *
 *  @desc   Command ids for NOTIFY.
 *  ============================================================================
 */
#define CMD_NOTIFY_INITIALIZE              _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 57u,\
                                            CMD_Args)

#define CMD_NOTIFY_FINALIZE                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 58u,\
                                            CMD_Args)

#define CMD_NOTIFY_REGISTER                _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 59u,\
                                            CMD_Args)

#define CMD_NOTIFY_UNREGISTER              _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 60u,\
                                            CMD_Args)

#define CMD_NOTIFY_NOTIFY                  _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 61u,\
                                            CMD_Args)


#if defined (DDSP_PROFILE)
#define CMD_NOTIFY_INSTRUMENT              _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 62u,\
                                            CMD_Args)

#endif

/*  ============================================================================
 *  @macro  CMD_RINGIO_XXXX
 *
 *  @desc   Command ids for RINGIO.
 *  ============================================================================
 */
#define CMD_RINGIO_MAPREGION               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 63u,\
                                            CMD_Args)

#define CMD_RINGIO_UNMAPREGION             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 64u,\
                                            CMD_Args)


/*  ============================================================================
 *  @macro  CMD_MPLIST_XXXX
 *
 *  @desc   Command ids for MPLIST.
 *  ============================================================================
 */
#define CMD_MPLIST_MAPREGION               _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 65u,\
                                            CMD_Args)

#define CMD_MPLIST_UNMAPREGION             _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 66u,\
                                            CMD_Args)


/*  ============================================================================
 *  @macro  CMD_IDM_XXXX
 *
 *  @desc   Command ids for IDM component.
 *  ============================================================================
 */
#define CMD_IDM_INIT                       _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 67u,\
                                            CMD_Args)

#define CMD_IDM_EXIT                       _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 68u,\
                                            CMD_Args)

#define CMD_IDM_CREATE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 69u,\
                                            CMD_Args)

#define CMD_IDM_DELETE                     _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 70u,\
                                            CMD_Args)

#define CMD_IDM_ACQUIREID                  _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 71u,\
                                            CMD_Args)

#define CMD_IDM_RELEASEID                  _IOWR(GROUP_BASE_CMD,\
                                            BASE_CMD + 72u,\
                                            CMD_Args)


/** ============================================================================
 *  @macro  DRV_INITIALIZE
 *
 *  @desc   OS dependent definition of initialization function.
 *  ============================================================================
 */
#define DRV_INITIALIZE(handle)         DRV_Initialize (handle, NULL)

/** ============================================================================
 *  @macro  DRV_FINALIZE
 *
 *  @desc   OS dependent definition of finalization function.
 *  ============================================================================
 */
#define DRV_FINALIZE(handle)           DRV_Finalize (handle, NULL)

/** ============================================================================
 *  @macro  DRV_INVOKE
 *
 *  @desc   OS dependent definition to call functions through ioctl's.
 *  ============================================================================
 */
#define DRV_INVOKE(handle,cmd,arg1)    DRV_Invoke (handle, cmd, arg1, NULL)


/** ============================================================================
 *  @name   CMD_Args
 *
 *  @desc   Union defining arguments to be passed to ioctl calls. For the
 *          explanation of individual field please see the corresponding APIs.

 *  @field  apiStatus
 *              Status returned by this API.
 *          apiArgs
 *              Union representing arguments for different APIs.
 *  ============================================================================
 */
typedef struct CMD_Args_tag {
    DSP_STATUS apiStatus ;
    union {
        struct {
            LINKCFG_Object * linkCfg ;
        } procSetupArgs ;

        struct {
            Char8 dummy ;
        } procDestroyArgs ;

        struct {
            ProcessorId     procId ;
        } procStartArgs ;

        struct {
            ProcessorId     procId ;
        } procStopArgs ;

        struct {
            ProcessorId     procId     ;
            Char8 *         symbolName ;
            Uint32 *        dspAddr    ;
        } procGetSymbolAddressArgs ;

        struct {
            ProcessorId     procId ;
            PROC_Attrs *    attr   ;
        } procAttachArgs ;

        struct {
            ProcessorId    procId ;
        } procDetachArgs ;

        struct {
            ProcessorId    procId    ;
            Char8 *        imagePath ;
            Uint32         argc      ;
            Char8 **       argv      ;
        } procLoadArgs ;

        struct {
            ProcessorId    procId   ;
            Uint32         dspAddr  ;
            Uint32         numBytes ;
            Pvoid          buffer   ;
        } procReadArgs ;

        struct {
            ProcessorId    procId   ;
            Uint32         dspAddr  ;
            Uint32         numBytes ;
            Pvoid          buffer   ;
        } procWriteArgs ;

        struct {
            ProcessorId     procId    ;
            PROC_State *    procState ;
        } procGetStateArgs ;

        struct {
            ProcessorId     procId ;
            Int32           cmd    ;
            Pvoid           arg    ;
        } procControlArgs ;

        struct {
            ProcessorId     procId    ;
            Char8 *         imagePath ;
            Uint32          sectID    ;
        } procLoadSectionArgs ;

        struct {
            Bool            lastDestroy ;
        } procIsLastDestroyArgs ;

        struct {
            ProcessorId     procId     ;
            Bool            lastDetach ;
        } procIsLastDetachArgs ;

#if defined (DDSP_PROFILE)
        struct {
            ProcessorId        procId    ;
            PROC_Instrument *  procStats ;
        } procInstrumentArgs ;
#endif

#if defined (DDSP_DEBUG)
        struct {
            ProcessorId       procId ;
        } procDebugArgs ;
#endif

        struct {
            ProcessorId     procId  ;
            Uint32          bufAddr ;
            Uint32 *        physAddr ;
        } procAddrTranslateArgs ;

#if defined (CHNL_COMPONENT)
        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
            ChannelAttrs *  attrs  ;
        } chnlCreateArgs ;

        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
        } chnlDeleteArgs ;

        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
            ChannelIOInfo * ioReq  ;
        } chnlIssueArgs ;

        struct {
            ProcessorId     procId  ;
            ChannelId       chnlId  ;
            Uint32          timeout ;
            ChannelIOInfo * ioReq   ;
        } chnlReclaimArgs ;

        struct {
            ProcessorId     procId   ;
            ChannelId       chnlId   ;
            Char8 **        bufArray ;
            Uint32          size     ;
            Uint32          numBufs  ;
        } chnlAllocateBufferArgs ;

        struct {
            ProcessorId     procId   ;
            ChannelId       chnlId   ;
            Char8 **        bufArray ;
            Uint32          numBufs  ;
        } chnlFreeBufferArgs ;

        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
        } chnlFlushArgs ;

        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
        } chnlIdleArgs ;

        struct {
            ProcessorId     procId ;
            ChannelId       chnlId ;
            Int32           cmd    ;
            Pvoid           arg    ;
        } chnlControlArgs ;

#if defined (DDSP_PROFILE)
        struct {
            ProcessorId       procId    ;
            ChannelId         chnlId    ;
            CHNL_Instrument * chnlStats ;
        } chnlInstrumentArgs ;
#endif

#if defined (DDSP_DEBUG)
        struct {
            ProcessorId       procId ;
            ChannelId         chnlId ;
        } chnlDebugArgs ;
#endif
#endif /* if defined (CHNL_COMPONENT) */

        struct {
            Void * phyAddr ;
        } drvPhyAddrArgs ;

#if defined (MSGQ_COMPONENT)
        struct {
            ProcessorId procId ;
            Pvoid       attrs ;
        } msgqTransportOpenArgs ;

        struct {
            ProcessorId procId ;
        } msgqTransportCloseArgs ;

        struct {
            Pstr         queueName ;
            MSGQ_Queue * msgqQueue ;
            MSGQ_Attrs * attrs ;
        } msgqOpenArgs ;

        struct {
            MSGQ_Queue   msgqQueue ;
        } msgqCloseArgs ;

        struct {
            Pstr                 queueName ;
            MSGQ_Queue *         msgqQueue ;
            MSGQ_LocateAttrs *   attrs ;
        } msgqLocateArgs ;

        struct {
            Pstr                     queueName ;
            MSGQ_Queue               replyQueue ;
            MSGQ_LocateAsyncAttrs *  attrs ;
        } msgqLocateAsyncArgs ;

        struct {
            MSGQ_Queue   msgqQueue ;
        } msgqReleaseArgs ;

        struct {
            PoolId      poolId ;
            Uint16      size ;
            Uint32      msgAddr ;
        } msgqAllocArgs ;

        struct {
            MSGQ_Msg     msg ;
        } msgqFreeArgs ;

        struct {
            MSGQ_Queue   msgqQueue ;
            MSGQ_Msg     msg ;
        } msgqPutArgs ;

        struct {
            Uint32      poolId ;
            MSGQ_Queue  msgqQueue ;
            Uint32      timeout ;
            Uint32      msgAddr ;
        } msgqGetArgs ;

        struct {
            MSGQ_Queue  errorQueue ;
            PoolId      poolId  ;
        } msgqSetErrorHandlerArgs ;

        struct {
            MSGQ_Queue  msgqQueue ;
            Uint16 *    count  ;
        } msgqCountArgs ;

#if defined (DDSP_PROFILE)
        struct {
            MSGQ_Queue          msgqQueue ;
            MSGQ_Instrument *   retVal ;
        } msgqInstrumentArgs ;
#endif /* if defined (DDSP_PROFILE) */

#if defined (DDSP_DEBUG)
        struct {
            MSGQ_Queue msgqQueue ;
        } msgqDebugArgs ;
#endif /* if defined (DDSP_DEBUG) */
#endif /* if defined (MSGQ_COMPONENT) */

        struct {
            PoolId poolId ;
            Pvoid  params ;
        } poolOpenArgs ;

        struct {
            PoolId poolId ;
        } poolCloseArgs ;

#if defined (POOL_COMPONENT)
        struct {
            PoolId  poolId ;
            Uint32  size ;
            Pvoid * bufPtr ;
        } poolAllocArgs ;

        struct {
            PoolId  poolId ;
            Uint32  size ;
            Pvoid   bufPtr ;
        } poolFreeArgs ;

        struct {
            PoolId           poolId ;
            Pvoid *          dstAddr ;
            AddrType         dstAddrType ;
            Pvoid            srcAddr ;
            AddrType         srcAddrType ;
        } poolTranslateAddrArgs ;

        struct {
            PoolId  poolId ;
            Uint32  size ;
            Pvoid   bufPtr ;
        } poolWBArgs ;

        struct {
            PoolId  poolId ;
            Uint32  size ;
            Pvoid   bufPtr ;
        } poolInvArgs ;
#endif /* #if defined (POOL_COMPONENT) */

#if defined (RINGIO_COMPONENT)
        struct {
            RingIO_MemInfo ringioRegionArgs ;
        } ringIoArgs ;
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MPLIST_COMPONENT)
        struct {
            MPLIST_MemInfo mplistRegionArgs ;
        } mplistArgs ;
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MPCS_COMPONENT)
        struct {
            MPCS_MemInfo  mpcsRegionArgs ;
        } mpcsMapArgs ;
#endif /* if defined (MPCS_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
        struct {
            ProcessorId dspId ;
        } notifyInitializeArgs ;

        struct {
            ProcessorId dspId ;
        } notifyFinalizeArgs ;

        struct {
            ProcessorId dspId ;
            Uint32      ipsId ;
            Uint32      eventNo ;
            Pvoid       fnNotifyCbck ;
            Pvoid       cbckArg ;
        } notifyRegisterArgs ;

        struct {
            ProcessorId dspId ;
            Uint32      ipsId ;
            Uint32      eventNo ;
            Pvoid       fnNotifyCbck ;
            Pvoid       cbckArg ;
        } notifyUnregisterArgs ;

        struct {
            ProcessorId dspId ;
            Uint32      ipsId ;
            Uint32      eventNo ;
            Uint32      payload ;
        } notifyNotifyArgs ;
#endif /* #if defined (NOTIFY_COMPONENT) */

#if defined (DDSP_PROFILE)
        struct {
            ProcessorId       dspId    ;
            Uint32            ipsId    ;
            IPS_Instrument *  ipsStats ;
        } ipsInstrumentArgs ;
#endif

        struct {
            Uint32      key ;
            IDM_Attrs * attrs ;
        } idmCreateArgs ;

        struct {
            Uint32      key ;
        } idmDeleteArgs ;

        struct {
            Uint32      key ;
            Pstr        idKey ;
            Uint32 *    id ;
        } idmAcquireIdArgs ;

        struct {
            Uint32      key ;
            Uint32      id ;
        } idmReleaseIdArgs ;
    } apiArgs ;
} CMD_Args ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (DRV_PMGR_H) */
