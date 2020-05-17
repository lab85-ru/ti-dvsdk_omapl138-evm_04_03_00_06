/*!
 *  ======== AACHEDEC========
 *  AACHEDEC codec specification
 *
 *  This file specifies information necessary to integrate with the Codec
 *  Engine.
 *
 *  By inheriting ti.sdo.ce.audio1.IAUDDEC1, AACHEDEC declares that it "is
 *  a" audio1 algorithm.  This allows the codec engine to automatically
 *  supply simple stubs and skeletons for transparent execution of DSP
 *  codecs by the GPP.
 *
 *  In addition to declaring the type of the AACHEDEC algorithm, we
 *  declare the external symbol required by xDAIS that identifies the
 *  algorithms implementation functions.
 */
metaonly module AACHEDEC inherits ti.sdo.ce.audio1.IAUDDEC1
{
    readonly config ti.sdo.codecs.aachedec.AACHEDEC.Module alg =
        ti.sdo.codecs.aachedec.AACHEDEC;
    
    override readonly config String ialgFxns = "AACDEC_TII_IAACDEC";

}
