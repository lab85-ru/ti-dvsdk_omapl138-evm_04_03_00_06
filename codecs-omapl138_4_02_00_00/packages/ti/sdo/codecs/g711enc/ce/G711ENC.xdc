/*!
 *  ======== G711ENC========
 *  G711ENC codec specification
 *
 *  This file specifies information necessary to integrate with the Codec
 *  Engine.
 *
 *  By inheriting ti.sdo.ce.speech1.ISPHENC1, G711ENC declares that it "is
 *  a" speech1 algorithm.  This allows the codec engine to automatically
 *  supply simple stubs and skeletons for transparent execution of DSP
 *  codecs by the GPP.
 *
 *  In addition to declaring the type of the G711ENC algorithm, we
 *  declare the external symbol required by xDAIS that identifies the
 *  algorithms implementation functions.
 */
metaonly module G711ENC inherits ti.sdo.ce.speech1.ISPHENC1
{
    readonly config ti.sdo.codecs.g711enc.G711ENC.Module alg =
        ti.sdo.codecs.g711enc.G711ENC;
    
    override readonly config String ialgFxns = "G711ENC_TII_IG711ENC";

}
