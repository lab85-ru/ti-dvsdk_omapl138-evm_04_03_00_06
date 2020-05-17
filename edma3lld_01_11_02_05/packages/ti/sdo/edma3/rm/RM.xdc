/*
 * ======== RM.xdc ========
 *
 */

/*!
 *  ======== RM ========
 *  EDMA3 Resource Manager.
 */
@Template("./RM.xdt")

metaonly module RM {
    config UInt edma3_max_rm_instances = 8u;
}

