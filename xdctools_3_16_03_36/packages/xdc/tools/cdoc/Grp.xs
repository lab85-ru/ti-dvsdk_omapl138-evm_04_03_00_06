/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * --/COPYRIGHT--*/
/*
 *  ======== Grp.xs ========
 */

var Doc = xdc.loadCapsule('Doc.xs');
var Gbl = xdc.loadCapsule('Gbl.xs');
var Pag = xdc.loadCapsule('Pag.xs');
var Syn = xdc.loadCapsule('Syn.xs');

function PL( s ) { Gbl.outstr().println(s); }

/*
 *  ======== genElem ========
 */
function genElem( gNode, uNode )
{
    if (String(gNode.@name).indexOf('$') == 0) return;

    var iflg = (uNode.@kind == 'interface') && Gbl.hasSpecSyn(gNode);

    if (Gbl.hasMetaSyn(gNode) || Gbl.hasTargSyn(gNode) || iflg) {
        Gbl.genRowB();
        Pag.genHdr(gNode, null, uNode, null);
        if (iflg) {
            _genElemSynop(gNode, Gbl.SPEC_SYN);
        }
        else {
            if (Gbl.hasMetaSyn(gNode)) {
                _genElemSynop(gNode, Gbl.META_SYN);
            }
            if (Gbl.hasTargSyn(gNode)) {
                _genElemSynop(gNode, Gbl.TARG_SYN);
            }
        }
    }

    Doc.genAll(gNode);
}

/*
 *  ======== genUnitSynop ========
 */
function genUnitSynop( gNode, sName )
{
    if (String(gNode.@name).indexOf('$') == 0) {
        Gbl.genRowB();
        Gbl.genRowB();
        Syn.genLines(gNode[sName], 0, gNode);
        return;
    }

    PL(Gbl.sprintf("<div class='xdocSynHdr'>$1</div>\n", String(gNode.@name)));
    if (gNode[sName].length()) {
        Syn.genLines(gNode[sName], 1, gNode);
        return;
    }

    var oflg = false;
    var sflg = false;

    for each (var dNode in gNode.decl) {
        if (dNode.@spacer == '1') {
            sflg = true;
            continue;
        }

        if (dNode.@external == '0') continue;
        if (!Gbl.opts().nodFlg && dNode.@nodoc == '1') continue;
        if (dNode[sName].length() == 0) continue;

        if (oflg && sflg) {
            Gbl.genRowB();
        }

        Syn.genLines(dNode[sName], 1, dNode);

        oflg = true;
        sflg = false;
    }
}

/*
 *  ======== hasUnitSynop ========
 */
function hasUnitSynop( gNode, sName )
{
    if (gNode[sName].length()) return true;

    for each (var dNode in gNode.decl) {
        if (dNode.@external == '0') continue;
        if (!Gbl.opts().nodFlg && dNode.@nodoc == '1') continue;
        if (dNode[sName].length()) {
            return true;
            break;
        }
    }

    return false;
}

/*
    if (gNode.children().length() == 0) return false;
    if (gNode.decl.length() == 0 && gNode[sName].length() == 0) return false;

    if (gNode.decl.length() > 0) {
        var found = false;
        for each (var dNode in gNode.decl) {
            if (dNode.@external == '0') continue;
            if (!Gbl.opts().nodFlg && dNode.@nodoc == '1') continue;
            if (dNode[sName].length()) {
                found = true;
                break;
            }
        }
        return found;
    }

    return true;
*/    

/*
 *  ======== _genElemSynop ========
 */
function _genElemSynop( gNode, sName )
{
    Syn.genPrefix(gNode, sName);
    Syn.genLines(gNode[sName], 0, gNode, true);
    Syn.genSuffix();
}

