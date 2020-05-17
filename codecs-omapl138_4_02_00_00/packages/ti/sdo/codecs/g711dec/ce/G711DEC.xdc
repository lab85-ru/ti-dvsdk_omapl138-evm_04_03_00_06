/*!
 *  ======== G711DEC========
 *  G711DEC codec specification
 *
 *  This file specifies information necessary to integrate with the Codec
 *  Engine.
 *
 *  By inheriting ti.sdo.ce.speech1.ISPHDEC1, G711DEC declares that it "is
 *  a" speech1 algorithm.  This allows the codec engine to automatically
 *  supply simple stubs and skeletons for transparent execution of DSP
 *  codecs by the GPP.
 *
 *  In addition to declaring the type of the G711DEC algorithm, we
 *  declare the external symbol required by xDAIS that identifies the
 *  algorithms implementation functions.
 */
metaonly module G711DEC inherits ti.sdo.ce.speech1.ISPHDEC1
{
    readonly config ti.sdo.codecs.g711dec.G711DEC.Module alg =
        ti.sdo.codecs.g711dec.G711DEC;
    
    override readonly config String ialgFxns = "G711DEC_TII_IG711DEC";

}
