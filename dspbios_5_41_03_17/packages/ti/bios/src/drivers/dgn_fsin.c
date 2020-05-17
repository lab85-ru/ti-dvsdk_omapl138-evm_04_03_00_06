/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_fsin.c ========
 *  Spectron-supplied "user" function for DGN.
 *
 */

#include <math.h>

#include <std.h>
#include <stddef.h>

#include <dev.h>

#include <dgn.h>

#define PI      3.14159265358979323846

/*
 *  ======== DGN_fsine ========
 *  Fill the buffer whose address is "addr" with sine values generated
 *  "size" times.
 *
 *  NOTE:  If the sampling rate is not evenly divisible by the
 *         frequency, the generated output is not a sine wave
 *         but a sequence of a sine-like wave.
 */
Void DGN_fsine(register DGN_GenObj *gen, Float *addr, size_t size)
{
    register size_t        i;
    register Int        sperc;
    Float               freq, gain, phase;

    /*
     *  "sperc" is the whole number of samples in one cycle of a
     *  sine wave of the desired frequency at the desired sampling
     *  rate.  This value is used to limit the range of values
     *  passed to the sin() function.
     */
    sperc = gen->dparams->sine.rate / gen->dparams->sine.freq;

    /*
     *  freq is frequency normalized to the sampling rate.
     */
    freq = (2.0 * PI * gen->dparams->sine.freq) / gen->dparams->sine.rate;
    gain = gen->dparams->sine.gain;
    phase = gen->dparams->sine.phase;

    /*
     *  This loop generates the sine wave.  The sine wave is generated
     *  by calling the sine function supplied by a runtime support library.
     *  After a complete cycle of the sine wave is generated, the counter
     *  is reset and another cycle is generated.  If the sampling rate
     *  is not evenly divisible by frequency, the resultant waveform 
     *  will have a phase "glitch" at these boundries.  
     */
    for (i = (size / sizeof(Float)); i > 0; i--) {
        *addr++ = gain * sin(freq * gen->count + phase);
        if (++gen->count == sperc) {
            gen->count = 0;
        }
    }
}

