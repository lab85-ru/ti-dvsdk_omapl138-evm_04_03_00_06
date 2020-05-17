/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn.c ========
 *  Streaming Generator Driver
 *
 *  A DGN device is a "pseudo-device" that generates one of several
 *  possible data streams.  For example, a DGN can generate sin/cos
 *  series or white noise.  This driver can be very useful for testing
 *  applications that require an input stream of data.
 *
 */

#include <std.h>

#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sys.h>

#include <dgn.h>

#ifdef _FLOAT_

/*
 *  Default values for this generator.
 */
DGN_Params DGN_PARAMS = {
    {1.0},                      /* constant: value */
    {1L, 0.0, 1.0},             /* rand: seed, lower, upper */
    {1.0, 1000.0, 0.0, 44000L}, /* sine: gain, freq, phase, sample rate */
    {NULL, NULL}                /* user: fxn, arg */
};

#else  /* _FLOAT_ */

#define MAXRANGE        ((1U << (16 - 1)) - 1)

DGN_IParams DGN_PARAMS = {
    { 1 },                      /* constant: value */
    { 1, -MAXRANGE, MAXRANGE }, /* rand: seed, lower, upper */
    { MAXRANGE, 1, 0, 256 },    /* sine: gain, freq, phase, sample rate */
    { NULL, NULL }              /* user: fxn, arg */
};

#endif /* _FLOAT_ */

Int DGN_close(DEV_Handle device);
Int DGN_idle(DEV_Handle device, Bool flush);
Int DGN_open(DEV_Handle device, String name);
Int DGN_ioFunc(DEV_Handle device);

/*
 *  Driver function table.
 */
DEV_Fxns DGN_FXNS = {
    DGN_close,          /* close */
    DEV_CTRL,           /* ctrl */
    DGN_idle,           /* idle */
    DGN_ioFunc,         /* issue */
    DGN_open,           /* open */
    DEV_READY,          /* ready */     /* generator is always ready */
    (DEV_Treclaim)SYS_zero,     /* reclaim */
};

/*
 *  ======== DGN_close ========
 *  All that needs to be done to close a generator is to
 *  free memory associated with the driver.  And return
 *  a status when done.
 */
Int DGN_close(DEV_Handle device)
{
    MEM_freeJ(0, device->object, sizeof(DGN_GenObj));
    
    return (SYS_OK);
}

/*
 *  ======== DGN_idle ========
 *  For DGN idling, we don't need to do any queue manipulation since
 *  there's no way for frames to build up on a queue (they always get
 *  processed).
 *  All we need to do is reset state variables.
 */
Int DGN_idle(DEV_Handle device, Bool flush)
{
    DGN_GenObj  *gen = (DGN_GenObj *)device->object;

    gen->seed = gen->dparams->rand.seed;

#ifdef _FLOAT_
    gen->count = 0;
#else  /* _FLOAT_ */
    gen->index = 0;
#endif /* _FLOAT_ */

    return (SYS_OK);
}

/*
 *  ======== DGN_ioFunc ========
 *  This routine removes a frame from the 'todevice' list, fills it
 *  with data, and moves it to the 'fromdevice' list.
 */
Int DGN_ioFunc(DEV_Handle device)
{
    DGN_GenObj  *gen;
    DEV_Frame   *frame;
    
    frame = QUE_get(device->todevice);
    gen = (DGN_GenObj *)device->object;

    (*gen->fxn)(gen, frame->addr, frame->size);

    QUE_putJ(device->fromdevice, frame);

    return (SYS_OK);
}

/*
 *  ======== DGN_open ========
 *  This routine creates a generator object that will be used
 *  when manipulating this device (ie. getting data, getting
 *  status, etc.).
 */
Int DGN_open(DEV_Handle device, String name)
{
    DGN_GenObj  *gen;
#ifndef _FLOAT_
    Int         gain;
    Int         cnst;

    cnst = 0;
#endif
    
    /* allocate generator object */
    if ((gen = MEM_allocJ(0, sizeof(DGN_GenObj), 0)) == MEM_ILLEGAL) {
        SYS_error("DGN", SYS_EALLOC);
        return (SYS_EALLOC);
    }

#ifdef _FLOAT_
    gen->dparams = (device->params) ?
      (DGN_Params *)device->params : &DGN_PARAMS;

    gen->count = 0;
#else  /* _FLOAT_ */
    gen->dparams = (device->params) ?
      (DGN_IParams *)device->params : &DGN_PARAMS;

    /*
     * bug fix MR 4493
     * gain = gain - 1 to fix boundary case --
     * gain of 128 was yielding values between -256 and 256
     * gain of 16 gave -32 to 32
     * etc.
     */
    gain = gen->dparams->sine.gain - 1;
    while (gain > 0) {
        cnst++;
        gain >>= 1;
    }

    gen->shift = 16 - 1 - cnst;
    gen->step = (256 * gen->dparams->sine.freq) / gen->dparams->sine.rate;
    gen->index = 0;
#endif /* _FLOAT_ */

    gen->seed = gen->dparams->rand.seed;

    gen->fxn = (Fxn)device->devid;

    device->object = (Ptr)gen;

    return (SYS_OK);
}
