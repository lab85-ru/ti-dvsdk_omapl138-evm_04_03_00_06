/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== Engine.h ========
 */

/**
 *  @file       ti/sdo/ce/Engine.h
 *
 *  @brief      The Codec Engine Runtime.  Provides the user an inteface to
 *              open and manipulate an Engine which can instantiate
 *              and communicate with XDAIS algorithms that run either
 *              on the local CPU or on a "remote" DSP Server.
 */
/**
 *  @addtogroup   CODECENGINE     Codec Engine Runtime
 */

#ifndef Engine_
#define Engine_

#ifdef __cplusplus
extern "C" {
#endif


#include <ti/sdo/ce/Server.h>
#include <ti/sdo/ce/ipc/Comm.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/ce/node/node.h>

#include <stddef.h> /* def of size_t */
#include <stdio.h>  /* def of FILE * */

/** @ingroup    CODECENGINE */
/*@{*/

/**
 *  @brief      Trace name for the Engine module
 */
#define Engine_GTNAME "CE"

/**
 *  @brief      Opaque handle to an engine.
 */
typedef struct Engine_Obj *Engine_Handle;

/**
 *  @brief      Engine error code
 */
typedef Int Engine_Error;

#define Engine_EOK          0   /**< Success. */
#define Engine_EEXIST       1   /**< Name does not exist. */
#define Engine_ENOMEM       2   /**< Unable to allocate memory. */
#define Engine_EDSPLOAD     3   /**< Unable to load the DSP. */
#define Engine_ENOCOMM      4   /**< Unable to create a comm connection to
                                 *   the DSP.
                                 */
#define Engine_ENOSERVER    5   /**< Unable to locate the server on the DSP. */
#define Engine_ECOMALLOC    6   /**< Unable to allocate communication buffer. */
#define Engine_ERUNTIME     7   /**< Internal engine runtime failure. */
#define Engine_ECODECCREATE 8   /**< Creation of the Codec failed. */
#define Engine_ECODECSTART  9   /**< Start of the Codec failed.  For codecs
                                 *   which are implemented as a thread, this
                                 *   implies that the codec thread of execution
                                 *   failed to start.
                                 */
#define Engine_EINVAL       10  /**< Bad paramater passed to method. */
#define Engine_EBADSERVER   11  /**< Incompatible server specified. */
#define Engine_ENOTAVAIL    12  /**< Service not available. */
#define Engine_EWRONGSTATE  13  /**< Call can not be made at this time. */
#define Engine_EINUSE       14  /**< Call can't be made at this time because
                                 *   a required name/resource is in use.
                                 */
#define Engine_ENOTFOUND    15  /**< Entity was not found. */
#define Engine_ETIMEOUT     16  /**< Timeout-based operation timed out. */

/** @cond INTERNAL */

/**
 *  @brief      Opaque handle to a node.
 */
typedef struct Engine_NodeObj *Engine_Node;

/**
 *  @brief      Special value for timeout parameter of Engine_callWait()
 */
#define Engine_FOREVER Comm_FOREVER

/** @endcond */

/**
 *  @brief      Attributes of an Engine
 *
 *  @sa         Engine_open() and Engine_ATTRS.
 */
typedef struct Engine_Attrs {
    String procId;  /**< id of the processor that runs the server; only
                     *   needed in the case that there's more than one
                     *   processor that can provide the same server.
                     */
} Engine_Attrs;

/**
 *  @brief      Properties of an Engine algorithm
 *
 *  @sa         Engine_getAlgInfo()
 */
typedef struct Engine_AlgInfo {
    Int         algInfoSize;    /**< Size of this structure. */
    String      name;           /**< Name of algorithm. */
    String      *typeTab;       /**< Inheritance hierarchy. */
    Bool        isLocal;        /**< If TRUE, run locally. */
} Engine_AlgInfo;

/**
 *  @brief      Properties of an Engine algorithm. This structure is identical
 *              to Engine_AlgInfo except that the @c typeTab array of strings
 *              is replaced by a singie string called @c types. The string,
 *              @c types, represents a ';' separated list of inheritance
 *              hierarchies of the algorithm, for example,
 *              "ti.sdo.ce.video.IVIDDEC;ti.sdo.ce.test.xvideo.IVIDE".
 *
 *  @sa         Engine_getAlgInfo2()
 */
typedef struct Engine_AlgInfo2 {
    Int         algInfoSize;    /**< Size of this structure. */
    String      name;           /**< Name of algorithm. */
    String      types;          /**< Inheritance hierarchy. */
    Bool        isLocal;        /**< If TRUE, run locally. */
} Engine_AlgInfo2;

/**
 *  @brief      Default engine attributes.
 */
extern Engine_Attrs Engine_ATTRS;    /**< Default attrs. */

/** @cond INTERNAL */

typedef Int Engine_Ctrl;

#define Engine_CEXIT    0
#define Engine_MAXSEGNAMELENGTH 32

/**
 *  @brief      Engine Cacheable Memory types.
 *
 *  @enumWarning
 */
typedef enum Engine_CachedMemType {
    Engine_USECACHEDMEM_DEFAULT = -1,  /**< Use default cache setting */
    Engine_USECACHEDMEM_NONCACHED = 0, /**< Use non-cached memory */
    Engine_USECACHEDMEM_CACHED = 1     /**< Use cached memory */
} Engine_CachedMemType;


/*
 *  ======== Engine_AlgDesc ========
 */
typedef struct Engine_AlgDesc {
    String      name;           /**< Name of algorithm. */
    NODE_Uuid   uuid;           /**< Fixed length perfect hash of name. */
    IALG_Fxns   *fxns;          /**< If null, served by RMS. */
    Ptr         idmaFxns;       /**< DMA resource negotiation when not null. */
    String      *typeTab;       /**< inheritance hierarchy. */
    Bool        isLocal;        /**< if TRUE, run locally. */
    Int         groupId;        /**< Resource sharing group Id. */
    Int         rpcProtocolVersion; /**< Protocol version. */
    Ptr         iresFxns;       /**< IRES_Fxns when not null. */
    Void        *codecClassConfig; /**< codec class config data, if any */
    Engine_CachedMemType memType;  /**< Memory type for alg's mem reqs. */

    /**< ';' separated string of types.
     *   This is only needed for Engine_getAlgInfo2(). If the Engine_AlgDesc
     *   structure was auto-generated with XDC tools, or if it was filled
     *   in at run-time from data obtained from the server, this field will
     *   not be NULL. If this was filled in by an app for a local alg, using
     *   run-time config macros, this field will be NULL, but in that case,
     *   typeTab[0] will contain the correct inheritance hierarchy string.
     */
    String      types;
} Engine_AlgDesc;

/*
 *  ======== Engine_AlgCreateAttrs ========
 */
typedef struct Engine_AlgCreateAttrs {
    Bool            useExtHeap; /**< Use a single external heap for alg's
                                 *   memory requests if TRUE, otherwise attempt
                                 *   to honor the alg's algAlloc() function for
                                 *   memory heap assignments.
                                 */
    Int             priority;   /**< Alg instance priority (-1: use value from
                                 *   configuration). */
} Engine_AlgCreateAttrs;

/*
 *  ======== Engine_Desc ========
 */
typedef struct Engine_Desc {
    String          name;       /**< Name of this server. */
    Engine_AlgDesc  *algTab;    /**< Table of algs served by this server. */
    String          remoteName; /**< Name of remote DSP RMS server. */
    String          linkCfg;    /**< Link config for DSP (when using LAD). */
    Int             numAlgs;    /**< Number of algorithms served by this
                                 *   server. */
} Engine_Desc;

/*
 *  ======== Engine_Config ========
 */
typedef struct Engine_Config {
    Engine_Desc *engineTab;
    String      localEngine;
} Engine_Config;

/*
 *  ======== Engine_MemStat ========
 *  This structure must match Server_MemStat.
 */
typedef struct Engine_MemStat {
    Char   name[Engine_MAXSEGNAMELENGTH + 1]; /* Name of memory segment */
    Uint32 base;           /* Base address of memory segment */
    Uint32 size;           /* Original size of the memory segment. */
    Uint32 used;           /* Number of bytes used. */
    Uint32 maxBlockLen;    /* Size of the largest contiguous free block. */
} Engine_MemStat;

/* Default alg create attributes */
extern Engine_AlgCreateAttrs Engine_ALGCREATEATTRS;

/*
 *  ======== Engine_config ========
 */
extern Engine_Config Engine_config;

/*
 *  ======== Engine_addStubFxns ========
 */
/**
 *  @brief      Register stub functions through which a remote algorithm
 *              can be called.
 *
 *  @param[in]  fxnsName    The name of the stub function table, eg
 *                          "VIDDEC_STUBS".
 *  @param[in]  fxns        Address of stub function table, eg &VIDDEC_STUBS
 *
 *  @retval     Engine_EOK     Success.
 *  @retval     Engine_ENOMEM  Memory allocation failed.
 */
extern Engine_Error Engine_addStubFxns(String fxnsName, IALG_Fxns *fxns);

/*
 *  ======== Engine_call ========
 */
extern Int Engine_call(Engine_Node node, Comm_Msg *msg);

/*
 *  ======== Engine_callAsync ========
 */
extern Int Engine_callAsync(Engine_Node node, Comm_Msg *msg);

/*
 *  ======== Engine_callWait ========
 */
extern Int Engine_callWait(Engine_Node node, Comm_Msg *msg, UInt timeout);

/*
 *  ======== Engine_initFromServer ========
 */
extern Engine_Error Engine_initFromServer(Engine_Handle engine);

/*
 *  ======== Engine_ctrlNode ========
 */
extern Int Engine_ctrlNode(Engine_Node node, Comm_Msg *msg, Engine_Ctrl code);

/** @endcond */

/*
 *  ======== Engine_close ========
 */
/**
 *  @brief      Close an Engine.
 *
 *  @param[in]  engine          The handle to an engine, previously acquired
 *                              by a call to Engine_open().
 *
 *  @pre        @c engine must not be referenced by any codec instance
 *              object; i.e., you must first delete all codec instances
 *              associated with @c engine before closing it.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle which is
 *              in the open state.
 *
 */
extern Void Engine_close(Engine_Handle engine);

/** @cond INTERNAL */
/*
 *  ======== Engine_createNode ========
 */
/**
 *  @brief      Create a remote algorithm.
 *
 *  @param[in]  engine          The handle to an engine, previously acquired
 *                              by a call to Engine_open().
 *  @param[in]  name            Name of the algorithm to create.
 *  @param[in]  msgSize         Size of the internal message required to
 *                              communicate with the remote algorithm.
 *  @param[in]  nodeAttrs       Creation parameters for the remote algorithm.
 *  @param[in]  attrs           Attributes used by the framework for creating
 *                              the remote algorithm.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle which is
 *              in the open state.
 *
 *  @remarks    Engine_createNode2() was added after Engine_createNode() to
 *              support more use cases.  Engine_createNode() is a wrapper
 *              around Engine_createNode2(), and is maintained for compatibility.
 *
 *  @retval     NULL            Failure
 *  @retval     non-NULL        A handle to the created remote algorithm.
 *
 *  @sa         Engine_createNode2()
 *  @sa         Engine_deleteNode()
 */
extern Engine_Node Engine_createNode(Engine_Handle engine, String name,
    size_t msgSize, IALG_Params *nodeAttrs, Engine_AlgCreateAttrs *attrs);


/*
 *  ======== Engine_createNode2 ========
 */
/**
 *  @brief      Create a remote algorithm.
 *
 *  @param[in]  engine          The handle to an engine, previously acquired
 *                              by a call to Engine_open().
 *  @param[in]  name            Name of the algorithm to create.
 *  @param[in]  msgSize         Size of the internal message required to
 *                              communicate with the remote algorithm.
 *  @param[in]  nodeAttrs       Creation parameters for the remote algorithm.
 *  @param[in]  nodeAttrsSize   Size of @c nodeAttrs.
 *  @param[in]  attrs           Attributes used by the framework for creating
 *                              the remote algorithm.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle which is
 *              in the open state.
 *
 *  @remarks    Engine_createNode() is the preferred method to create remote
 *              algorithms.  However, some algorithm interfaces incorrectly
 *              fail to provide a size field of type "Int" as the first field
 *              in their creation parameters, which the XDAIS spec defines.
 *              This service allows the creation of remote algorithms where the
 *              size of the creation params is specified "some other way" than
 *              the XDAIS spec defines.
 *
 *  @retval     NULL            Failure
 *  @retval     non-NULL        A handle to the created remote algorithm.
 *
 *  @sa         Engine_createNode()
 *  @sa         Engine_deleteNode()
 */
extern Engine_Node Engine_createNode2(Engine_Handle engine, String name,
    size_t msgSize, IALG_Params *nodeAttrs, Int nodeAttrsSize,
    Engine_AlgCreateAttrs *attrs);


/*
 *  ======== Engine_deleteNode ========
 */
extern Void Engine_deleteNode(Engine_Node node);

/*
 *  ======== Engine_getAlgMemRecs ========
 */
/**
 *  @brief      Get the IALG_MemRecs used by an algorithm.
 *
 *  @param[in]  node    Handle to an algorithm instance.
 *  @param[out] memTab  Location to store the IALG_MemRecs.
 *  @param[in]  size    Maximum number of IALG_MemRecs to put in memTab array.
 *  @param[out] numRecs Actual number of IALG_MemRecs copied into memTab array.
 *
 *  @retval     Engine_EOK       Success.
 *  @retval     Engine_ERUNTIME  Failure.
 *
 *  @sa         Engine_getAlgNumRecs()
 */
extern Engine_Error Engine_getAlgMemRecs(Engine_Node node, IALG_MemRec *memTab, Int size,
        Int *numRecs);

/*
 *  ======== Engine_getAlgNumRecs ========
 */
/**
 *  @brief      Get the number of IALG_MemRecs used by a remote algorithm.
 *
 *  @param[in]  node    Handle to an algorithm instance.
 *  @param[out] numRecs Location to store the number of IALG_MemRecs used.
 *
 *  @retval     Engine_EOK       Success.
 *  @retval     Engine_ERUNTIME  Failure.
 *
 *  @sa         Engine_getAlgMemRecs()
 */
extern Engine_Error Engine_getAlgNumRecs(Engine_Node node, Int *numRecs);

/*
 *  ======== Engine_getConstName ========
 */
extern String Engine_getConstName(Engine_Handle engine, String name,
    String type);

/*
 *  ======== Engine_getFxns ========
 */
extern IALG_Fxns *Engine_getFxns(Engine_Handle svr, String name, String type,
        Bool *isLocal, Ptr *idmaFxns, Ptr *iresFxns, Int *groupId,
        Engine_CachedMemType *memType);

/*
 *  ======== Engine_getMemId ========
 */
extern Int Engine_getMemId(Engine_Handle engine);

/*
 *  ======== Engine_getLocalEngine ========
 */
extern Engine_Handle Engine_getLocalEngine(Void);

/*
 *  ======== Engine_getEngine ========
 */
extern Engine_Handle Engine_getEngine(Engine_Node node);

/*
 *  ======== Engine_getMemStat ========
 */
extern Engine_Error Engine_getMemStat(Server_Handle server, Int segNum,
    Engine_MemStat *stat);

/*
 *  ======== Engine_getNumServerAlgs ========
 */
extern Engine_Error Engine_getNumServerAlgs(Server_Handle server, Int *nAlgs);

/*
 *  ======== Engine_getServerAlgInfo ========
 */
extern Engine_Error Engine_getServerAlgInfo(Server_Handle server, Int algNum,
        Engine_AlgInfo *algInfo);

/*
 *  ======== Engine_getNumMemSegs ========
 */
extern Engine_Error Engine_getNumMemSegs(Server_Handle server, Int *numSegs);

/*
 *  ======== Engine_hasServer ========
 */
extern Bool Engine_hasServer(Engine_Handle engine);

/*
 *  ======== Engine_init ========
 */
extern Void Engine_init(Void);

/*
 *  ======== Engine_getRemoteVisa ========
 */
extern UInt32 Engine_getRemoteVisa(Engine_Node node);

/*
 *  ======== Engine_getCodecClassConfig ========
 */
extern Ptr Engine_getCodecClassConfig(Engine_Handle engine, String name,
    String type);

/*
 *  ======== Engine_getNodeQueues ========
 */
extern Void Engine_getNodeQueues(Engine_Node node, Comm_Queue *stdIn,
        Comm_Queue *stdOut);

/*
 *  ======== Engine_redefineHeap ========
 */
extern Engine_Error Engine_redefineHeap(Server_Handle server, String name,
        Uint32 base, Uint32 size);

/*
 *  ======== Engine_releaseTraceToken ========
 */
extern Bool Engine_releaseTraceToken(Server_Handle server);

/*
 *  ======== Engine_requestTraceToken ========
 */
extern Engine_Error Engine_requestTraceToken(Server_Handle server);

/*
 *  ======== Engine_restoreHeap ========
 */
extern Engine_Error Engine_restoreHeap(Server_Handle server, String name);

/** @endcond */

/*
 *  ======== Engine_open ========
 */
/**
 *  @brief Open an Engine.
 *
 *  The handle returned may be used to create one or more instances of a codec
 *  "contained" in the specified engine.
 *
 *  An engine may be opened more than once; each open returns a unique
 *  handle that can be used to create codec instances or get status of any
 *  underlying server.
 *
 *  Engine handles must not be concurrently accessed by multiple threads; each
 *  thread must either obtain its own handle (via Engine_open()) or explicitly
 *  serialize access to a shared handle.
 *
 *  @param[in]  name            The name of the engine to open.  @c name is
 *                              specified in the engine configuration.
 *  @param[in]  attrs           Attributes for the open engine.
 *  @param[out] ec              Optional output error code
 *
 *  @retval     NULL            An error has occurred.
 *  @retval     non-NULL        The handle to the opened engine.
 *
 *  @pre        @c name is a non-NULL string.
 *
 *  @pre        @c name is a valid, pre-configured name of an engine.
 *
 *  @pre        As with all Codec Engine API's, CERuntime_init() must have
 *              previously been called.
 *
 *  @post       If the return handle is NULL and @c ec is non-NULL, @c *ec
 *              is set to a non-zero value indicating the cause of the failure.
 *
 *  @post       If @c ec is non-NULL, the Engine_Error value is set to one of
 *              the following values:
 *                - #Engine_EOK         success
 *                - #Engine_EEXIST      name does not exist
 *                - #Engine_ENOMEM      can't allocate memory
 *                - #Engine_EDSPLOAD    can't load the DSP
 *                - #Engine_ENOCOMM     can't create a comm connection to DSP
 *                - #Engine_ENOSERVER   can't locate the server on the DSP
 *                - #Engine_ECOMALLOC   can't allocate communication buffer
 */
extern Engine_Handle Engine_open(String name, Engine_Attrs *attrs,
    Engine_Error *ec);

/*
 *  ======== Engine_fwriteTrace ========
 */
/**
 *  @brief              Write Server's trace buffer to specifed file stream
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @param[in]  prefix  A string to prepend to each line output; this
 *                      allows one to easily identify trace from the
 *                      server from the application's trace, for
 *                      example.
 *  @param[in]  out     A open FILE stream used to output the
 *                      Server's trace characters.
 *
 *  @retval             Integer number of characters copied to the specified
 *                      FILE stream.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 *
 *  @post       In the event a negative value is returned,
 *              Engine_getLastError() will return one of the following values:
 *                - #Engine_ERUNTIME    Either an internal runtime error
 *                                      occured or the underlying server
 *                                      error occured.
 *                - #Engine_EINUSE      Server trace resource is already in use.
 */
extern Int Engine_fwriteTrace(Engine_Handle engine, String prefix, FILE *out);

/*
 *  ======== Engine_getAlgInfo ========
 */
/**
 *  @brief      Get details of an algorithm configured into an engine.
 *
 *  @param[in]  name        The name of the engine.  @c name is
 *                          specified in the engine configuration.
 *  @param[out] algInfo     Structure to store algorithm details. The
 *                          @c algInfoSize field of this structure must
 *                          be set to @c sizeof(Engine_AlgInfo) by the
 *                          application.
 *  @param[out] index       The index of the algorithm to get the information.
 *
 *  @retval     Engine_EOK        Success.
 *  @retval     Engine_EEXIST     There is no engine with the given name.
 *  @retval     Engine_ENOTFOUND  @c index is greater than or equal to the
 *                                total number of algorithms configured for
 *                                the engine, or @c index < 0.
 *  @retval     Engine_EINVAL     The value of @c algInfoSize passed to this
 *                                function does not match the CE library's
 *                                @c sizeof(Engine_AlgInfo).
 *
 *  @pre        @c name is a non-NULL string.
 *
 *  @pre        @c algInfo is non-NULL.
 *
 *  @pre        As with all Codec Engine API's, CERuntime_init() must have
 *              previously been called.
 *
 *  @post       If @c name is a valid engine name and 0 <= @c index < the
 *              total number of algorithms configured for the engine, then
 *              @c algInfo will contain the information for the engine's
 *              ith (i = @c index) algorithm.
 *
 *  @sa         Engine_getNumAlgs().
 */
extern Engine_Error Engine_getAlgInfo(String name, Engine_AlgInfo *algInfo,
        Int index);

/*
 *  ======== Engine_getAlgInfo2 ========
 */
/**
 *  @brief      Get details of an algorithm that is configured into an engine,
 *              or has been added to an opened engine. If the engine has not
 *              been opened yet, the name of the engine is used to get the
 *              statically configured alg. If the engine has been opened, the
 *              engine handle can be used to get either information for a
 *              statically configured alg, or a remote alg that was added when
 *              the server was queried during Engine_open().
 *
 *  @param[in]  name        The name of the engine.  @c name is
 *                          specified in the engine configuration. This
 *                          may be NULL if @c engine contains a valid
 *                          engine handle.
 *  @param[in]  engine      The handle of an engine returned by Engine_open().
 *                          If this is NULL, only information for a static
 *                          alg can be obtained.
 *  @param[out] algInfo2    Structure to store algorithm details. The
 *                          @c algInfoSize field of this structure must
 *                          be set to @c sizeof(Engine_AlgInfo2) by the
 *                          application.
 *  @param[out] index       The index of the algorithm to get the information.
 *
 *  @retval     Engine_EOK        Success.
 *  @retval     Engine_EEXIST     There is no engine with the given name.
 *  @retval     Engine_ENOTFOUND  @c index is greater than or equal to the
 *                                total number of algorithms configured for
 *                                the engine, or @c index < 0.
 *  @retval     Engine_EINVAL     The value of @c algInfoSize passed to this
 *                                function does not match the CE library's
 *                                @c sizeof(Engine_AlgInfo2).
 *
 *  @pre        @c name is a non-NULL string or @c engine is non-NULL.
 *
 *  @pre        @c algInfo2 is non-NULL.
 *
 *  @pre        As with all Codec Engine API's, CERuntime_init() must have
 *              previously been called.
 *
 *  @post       If @c name is a valid engine name and 0 <= @c index < the
 *              total number of algorithms configured for the engine, then
 *              @c algInfo2 will contain the information for the engine's
 *              ith (i = @c index) algorithm.
 *
 *  @sa         Engine_getNumAlgs2().
 */
extern Engine_Error Engine_getAlgInfo2(String name, Engine_Handle engine,
        Engine_AlgInfo2 *algInfo2, Int index);

/*
 *  ======== Engine_getCpuLoad ========
 */
/**
 *  @brief      Get Server's cpu usage in percent
 *
 *  @deprecated This service has been replaced by Server_getCpuLoad()
 *              to better indicate that this API is not intended for
 *              obtaining the current processor's CPU load, rather it
 *              obtains the CPU load of a remote Server.
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @retval             integer between 0-100 indicating percentage
 *                      of time the Server is processing measured
 *                      over a period of approximately 1 second.  If
 *                      the load is unavailable, a negative value is
 *                      returned.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 *
 *  @post       In the event a negative value is returned,
 *              Engine_getLastError() will return one of the following values:
 *                - #Engine_ERUNTIME    Either an internal runtime error
 *                                      occured or the underlying server
 *                                      error occured.
 *                - #Engine_ENOTAVAIL   The CPU load can not be computed.
 *
 *  @sa         Server_getCpuLoad()
 */
extern Int Engine_getCpuLoad(Engine_Handle engine);

/*
 *  ======== Engine_getLastError ========
 */
/**
 *  @brief              Get error code of the last failed operation
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @retval             error code (Engine_Error) of the last failed
 *                      engine operation.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 */
extern Engine_Error Engine_getLastError(Engine_Handle engine);

/*
 *  ======== Engine_getName ========
 */
/**
 *  @brief Get the name of an opened engine.
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @retval     NULL            An error has occurred.
 *  @retval     non-NULL        The name of the opened engine.
 */
extern String Engine_getName(Engine_Handle engine);


/*
 *  ======== Engine_getNumAlgs ========
 */
/**
 *  @brief Get the number of algorithms configured into an engine.
 *
 *  @param[in]  name            The name of the engine.  @c name is
 *                              specified in the engine configuration.
 *  @param[out] numAlgs         The number of algorithms that are configured
 *                              in the given engine.
 *
 *  @retval     Engine_EOK      Success.
 *  @retval     Engine_EEXIST   There is no engine with the given name.
 *
 *  @pre        @c name is a non-NULL string.
 *
 *  @pre        @c numAlgs is non-NULL.
 *
 *  @pre        As with all Codec Engine API's, CERuntime_init() must have
 *              previously been called.
 *
 *  @post       If @c name is a valid engine name, then numAlgs will contain
 *              the number of algorithms configured for the given engine.
 *
 *  @sa         Engine_getAlgs().
 */
extern Engine_Error Engine_getNumAlgs(String name, Int *numAlgs);

/*
 *  ======== Engine_getNumAlgs2 ========
 */
/**
 *  @brief Get the number of algorithms statically configured into an engine
 *         or the total number of algorithms both statically configured and
 *         dynamically added through server information when the engine was
 *         opened.
 *
 *  @param[in]  name            The name of the engine.  @c name is
 *                              specified in the engine configuration. @c name
 *                              can be NULL, if @c engine is a valid
 *                              Engine_Handle.
 *  @param[in]  engine          The handle of an engine returned by
 *                              Engine_open(). If @c engine is NULL, @c name
 *                              must be non-NULL, and only the number of
 *                              statically configured algorithms will be
 *                              returned in @c numAlgs.
 *                              specified in the engine configuration.
 *  @param[out] numAlgs         The number of algorithms that are configured
 *                              in the given engine.
 *
 *  @retval     Engine_EOK      Success.
 *  @retval     Engine_EEXIST   There is no engine with the given name.
 *
 *  @pre        @c name is a non-NULL string or @c engine is non-NULL.
 *
 *  @pre        @c numAlgs is non-NULL.
 *
 *  @pre        As with all Codec Engine API's, CERuntime_init() must have
 *              previously been called.
 *
 *  @post       If @c name is a valid engine name, then numAlgs will contain
 *              the number of algorithms configured for the given engine.
 *
 *  @remarks    If the engine has a server, but was not configured with
 *              Engine.createFromServer() number of remote algorithms (if any)
 *              that were statically configured into the engine, will be
 *              counted twice: once for the static alg table, and once for
 *              the information queried from the server.
 *
 *  @sa         Engine_getAlgs().
 */
extern Engine_Error Engine_getNumAlgs2(String name, Engine_Handle engine,
        Int *numAlgs);

/*
 *  ======== Engine_getServer ========
 */
/**
 *  @brief              Get handle to an Engine's server.
 *
 *  This function returns the handle to an Engines server, that can be used
 *  with Server APIs to obtain information from and control the remote DSP
 *  server.
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @retval             Handle to engine's server.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 */
extern Server_Handle Engine_getServer(Engine_Handle engine);


/*
 *  ======== Engine_getUsedMem ========
 */
/**
 *  @brief              Get Server's total memory usage
 *
 *  @deprecated This service has been replaced by Server_getMemStat()
 *              to better indicate that this API is not intended for
 *              obtaining the current processor's memory statistics,
 *              rather it obtains the memory statistics of a remote
 *              Server.  Also, Server_getMemStat() provides more granularity
 *              than Engine_getUsedMem().
 *
 *  @param[in]  engine  The handle to the opened engine.
 *
 *  @retval     Total amount of used memory (in MAUs).  If the amount is not
 *              available, 0 is returned and the reason can be retrieved via
 *              Engine_getLastError().
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 *
 *  @post       in the event that 0 is returned, Engine_getLastError() will
 *              return one of the following values:
 *                - #Engine_ERUNTIME    Either an internal runtime error
 *                                      occured or the underlying server
 *                                      error occured.
 *                - #Engine_ENOTAVAIL   The memory usage can not be computed.
 */
extern UInt32 Engine_getUsedMem(Engine_Handle engine);


/*
 *  ======== Engine_setTrace ========
 */
/**
 *  @brief      Set Server's trace mask
 *
 *  @param[in]  engine  The handle to the opened engine.
 *  @param[in]  mask    Trace mask, e.g. "*=01234567"
 *
 *  @retval     Engine_ENOSERVER   No server for this engine.
 *  @retval     Engine_EINUSE      Trace resource is already in use.
 *  @retval     Engine_ERUNTIME    Internal runtime error has occurred.
 *
 *  @pre        @c engine is a valid (non-NULL) engine handle and the engine
 *              is in the open state.
 *
 *  @remarks    This only sets the trace for a remote server.  To change
 *              the trace mask for the application-side of the framework,
 *              use GT_set().
 *
 *  @sa         GT_set()
 */
extern Int Engine_setTrace(Engine_Handle engine, String mask);

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.sdo.ce; 1, 0, 6,432; 12-2-2010 21:19:07; /db/atree/library/trees/ce/ce-r11x/src/ xlibrary

 */

