/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-u11
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.*;

public class ti_bios_rov
{
    static final String VERS = "@(#) xdc-u11\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.bios.rov.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.bios.rov", new Value.Obj("ti.bios.rov", pkgP));
    }

    void KNL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.KNL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.KNL", new Value.Obj("ti.bios.rov.KNL", po));
        pkgV.bind("KNL", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.KNL$$KNL_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.KNL.KNL_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.KNL$$KNL_Job", new Proto.Obj());
        om.bind("ti.bios.rov.KNL.KNL_Job", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.KNL$$SystemView", new Proto.Obj());
        om.bind("ti.bios.rov.KNL.SystemView", new Proto.Str(spo, false));
    }

    void MEM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.MEM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.MEM", new Value.Obj("ti.bios.rov.MEM", po));
        pkgV.bind("MEM", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$MEM_Segment6x", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.MEM_Segment6x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$MEM_Segment28x55x", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.MEM_Segment28x55x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$MEM_Header6x", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.MEM_Header6x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$MEM_Header28x55x", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.MEM_Header28x55x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$InstanceData", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.InstanceData", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MEM$$FreeListElement", new Proto.Obj());
        om.bind("ti.bios.rov.MEM.FreeListElement", new Proto.Str(spo, false));
    }

    void SWI$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.SWI.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.SWI", new Value.Obj("ti.bios.rov.SWI", po));
        pkgV.bind("SWI", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.SWI$$SWI_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.SWI.SWI_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.SWI$$InstanceData", new Proto.Obj());
        om.bind("ti.bios.rov.SWI.InstanceData", new Proto.Str(spo, false));
    }

    void QUE$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.QUE.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.QUE", new Value.Obj("ti.bios.rov.QUE", po));
        pkgV.bind("QUE", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.QUE$$QUE_Elem", new Proto.Obj());
        om.bind("ti.bios.rov.QUE.QUE_Elem", new Proto.Str(spo, false));
    }

    void ROV$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.ROV.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.ROV", new Value.Obj("ti.bios.rov.ROV", po));
        pkgV.bind("ROV", vo);
        // decls 
    }

    void OBJ$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.OBJ.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.OBJ", new Value.Obj("ti.bios.rov.OBJ", po));
        pkgV.bind("OBJ", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.OBJ$$OBJ_TSK", new Proto.Obj());
        om.bind("ti.bios.rov.OBJ.OBJ_TSK", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.OBJ$$OBJ_TSK_55x", new Proto.Obj());
        om.bind("ti.bios.rov.OBJ.OBJ_TSK_55x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.OBJ$$OBJ_SEM", new Proto.Obj());
        om.bind("ti.bios.rov.OBJ.OBJ_SEM", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.OBJ$$OBJ_MBX", new Proto.Obj());
        om.bind("ti.bios.rov.OBJ.OBJ_MBX", new Proto.Str(spo, false));
        om.bind("ti.bios.rov.OBJ.objType", new Proto.Enm("ti.bios.rov.OBJ.objType"));
    }

    void TSK$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.TSK.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.TSK", new Value.Obj("ti.bios.rov.TSK", po));
        pkgV.bind("TSK", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.TSK$$TSK_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.TSK.TSK_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.TSK$$TSK_Obj_55x", new Proto.Obj());
        om.bind("ti.bios.rov.TSK.TSK_Obj_55x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.TSK$$InstanceData", new Proto.Obj());
        om.bind("ti.bios.rov.TSK.InstanceData", new Proto.Str(spo, false));
    }

    void SEM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.SEM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.SEM", new Value.Obj("ti.bios.rov.SEM", po));
        pkgV.bind("SEM", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.SEM$$SEM_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.SEM.SEM_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.SEM$$InstanceData", new Proto.Obj());
        om.bind("ti.bios.rov.SEM.InstanceData", new Proto.Str(spo, false));
    }

    void LOG$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.LOG.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.LOG", new Value.Obj("ti.bios.rov.LOG", po));
        pkgV.bind("LOG", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.LOG$$LOG_Event_6x", new Proto.Obj());
        om.bind("ti.bios.rov.LOG.LOG_Event_6x", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.LOG$$LOG_Event_28_55", new Proto.Obj());
        om.bind("ti.bios.rov.LOG.LOG_Event_28_55", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.LOG$$LOG_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.LOG.LOG_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.LOG$$LogRecordView", new Proto.Obj());
        om.bind("ti.bios.rov.LOG.LogRecordView", new Proto.Str(spo, false));
    }

    void MBX$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.MBX.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.MBX", new Value.Obj("ti.bios.rov.MBX", po));
        pkgV.bind("MBX", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.rov.MBX$$MBX_Obj", new Proto.Obj());
        om.bind("ti.bios.rov.MBX.MBX_Obj", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.bios.rov.MBX$$InstanceData", new Proto.Obj());
        om.bind("ti.bios.rov.MBX.InstanceData", new Proto.Str(spo, false));
    }

    void KNL$$CONSTS()
    {
        // module KNL
    }

    void MEM$$CONSTS()
    {
        // module MEM
    }

    void SWI$$CONSTS()
    {
        // module SWI
    }

    void QUE$$CONSTS()
    {
        // module QUE
    }

    void ROV$$CONSTS()
    {
        // module ROV
    }

    void OBJ$$CONSTS()
    {
        // module OBJ
        om.bind("ti.bios.rov.OBJ.SEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.SEM", 0));
        om.bind("ti.bios.rov.OBJ.SWI", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.SWI", 1));
        om.bind("ti.bios.rov.OBJ.TSK", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.TSK", 2));
        om.bind("ti.bios.rov.OBJ.MBX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.MBX", 3));
        om.bind("ti.bios.rov.OBJ.SIO", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.SIO", 4));
        om.bind("ti.bios.rov.OBJ.DEV", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.DEV", 5));
        om.bind("ti.bios.rov.OBJ.BUF", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"), "ti.bios.rov.OBJ.BUF", 6));
    }

    void TSK$$CONSTS()
    {
        // module TSK
    }

    void SEM$$CONSTS()
    {
        // module SEM
    }

    void LOG$$CONSTS()
    {
        // module LOG
    }

    void MBX$$CONSTS()
    {
        // module MBX
    }

    void KNL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void MEM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SWI$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void QUE$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ROV$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void OBJ$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TSK$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SEM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LOG$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void MBX$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void KNL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn KNL.getCurTaskAddr
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.KNL$$getCurTaskAddr", new Proto.Fxn(om.findStrict("ti.bios.rov.KNL.Module", "ti.bios.rov"), new Proto.Adr("xdc_Ptr", "Pv"), 0, 0, false));
        // fxn KNL.getQueues
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.KNL$$getQueues", new Proto.Fxn(om.findStrict("ti.bios.rov.KNL.Module", "ti.bios.rov"), $$T_Obj, 0, 0, false));
    }

    void MEM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SWI$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SWI.getNickName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.SWI$$getNickName", new Proto.Fxn(om.findStrict("ti.bios.rov.SWI.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "addr", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
        // fxn SWI.fxnToHandle
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.SWI$$fxnToHandle", new Proto.Fxn(om.findStrict("ti.bios.rov.SWI.Module", "ti.bios.rov"), new Proto.Adr("xdc_Ptr", "Pv"), 1, 1, false));
                fxn.addArg(0, "fxn", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
    }

    void QUE$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn QUE.scanQue
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.QUE$$scanQue", new Proto.Fxn(om.findStrict("ti.bios.rov.QUE.Module", "ti.bios.rov"), $$T_Obj, 1, 1, false));
                fxn.addArg(0, "obj", $$T_Obj, $$UNDEF);
    }

    void ROV$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ROV.fetchGlobal
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.ROV$$fetchGlobal", new Proto.Fxn(om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov"), $$T_Obj, 2, 2, false));
                fxn.addArg(0, "fetchDesc", $$T_Obj, $$UNDEF);
                fxn.addArg(1, "symbolName", $$T_Str, $$UNDEF);
        // fxn ROV.getISA
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.ROV$$getISA", new Proto.Fxn(om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov"), $$T_Str, 0, 0, false));
        // fxn ROV.getNickName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.ROV$$getNickName", new Proto.Fxn(om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov"), $$T_Str, 2, 2, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "addr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
        // fxn ROV.getName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.ROV$$getName", new Proto.Fxn(om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov"), $$T_Str, 4, 4, false));
                fxn.addArg(0, "modName", $$T_Str, $$UNDEF);
                fxn.addArg(1, "addr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
                fxn.addArg(2, "hasDynamic", $$T_Bool, $$UNDEF);
                fxn.addArg(3, "fetchDesc", $$T_Obj, $$UNDEF);
        // fxn ROV.getStaticName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.ROV$$getStaticName", new Proto.Fxn(om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "addr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
    }

    void OBJ$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn OBJ.getTasks
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.OBJ$$getTasks", new Proto.Fxn(om.findStrict("ti.bios.rov.OBJ.Module", "ti.bios.rov"), $$T_Obj, 0, 0, false));
        // fxn OBJ.getSems
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.OBJ$$getSems", new Proto.Fxn(om.findStrict("ti.bios.rov.OBJ.Module", "ti.bios.rov"), $$T_Obj, 0, 0, false));
        // fxn OBJ.getMailboxes
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.OBJ$$getMailboxes", new Proto.Fxn(om.findStrict("ti.bios.rov.OBJ.Module", "ti.bios.rov"), $$T_Obj, 0, 0, false));
    }

    void TSK$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn TSK.getNickName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.TSK$$getNickName", new Proto.Fxn(om.findStrict("ti.bios.rov.TSK.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "tskObjAddr", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
        // fxn TSK.getBlockedTask
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.TSK$$getBlockedTask", new Proto.Fxn(om.findStrict("ti.bios.rov.TSK.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "tskObjAddr", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
    }

    void SEM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SEM.getNickName
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.SEM$$getNickName", new Proto.Fxn(om.findStrict("ti.bios.rov.SEM.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "addr", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF);
        // fxn SEM.getTskBlockedOn
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.SEM$$getTskBlockedOn", new Proto.Fxn(om.findStrict("ti.bios.rov.SEM.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "tskAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
    }

    void LOG$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void MBX$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn MBX.getTskBlockedOn
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.MBX$$getTskBlockedOn", new Proto.Fxn(om.findStrict("ti.bios.rov.MBX.Module", "ti.bios.rov"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "tskAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF);
    }

    void KNL$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.KNL.KNL_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("ready", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("alarm", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("setpri", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("queue", "UPtr"));
        sizes.add(Global.newArray("priority", "TInt"));
        sizes.add(Global.newArray("mask", "UInt"));
        sizes.add(Global.newArray("sp", "UPtr"));
        sizes.add(Global.newArray("timeout", "UInt"));
        sizes.add(Global.newArray("mode", "TInt"));
        sizes.add(Global.newArray("sts", "UPtr"));
        sizes.add(Global.newArray("signalled", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.KNL.KNL_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.KNL.KNL_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.KNL.KNL_Obj'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.KNL.KNL_Job", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("wListElem", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("wCount", "TInt"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.KNL.KNL_Job']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.KNL.KNL_Job']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.KNL.KNL_Job'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.KNL.SystemView", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("systemTick", "TInt"));
        sizes.add(Global.newArray("stackBase", "UPtr"));
        sizes.add(Global.newArray("stackSize", "TInt"));
        sizes.add(Global.newArray("stackPeak", "TInt"));
        sizes.add(Global.newArray("sysStackBase", "UPtr"));
        sizes.add(Global.newArray("sysStackSize", "TInt"));
        sizes.add(Global.newArray("sysStackPeak", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.KNL.SystemView']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.KNL.SystemView']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.KNL.SystemView'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void MEM$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.MEM.MEM_Segment6x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("base", "UPtr"));
        sizes.add(Global.newArray("len", "UInt"));
        sizes.add(Global.newArray("space", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.MEM.MEM_Segment6x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.MEM.MEM_Segment6x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.MEM.MEM_Segment6x'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.MEM.MEM_Segment28x55x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("base", "UPtr"));
        sizes.add(Global.newArray("len", "ULong"));
        sizes.add(Global.newArray("space", "TInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.MEM.MEM_Segment28x55x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.MEM.MEM_Segment28x55x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.MEM.MEM_Segment28x55x'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.MEM.MEM_Header6x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("size", "UInt"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.MEM.MEM_Header6x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.MEM.MEM_Header6x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.MEM.MEM_Header6x'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.MEM.MEM_Header28x55x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("size", "ULong"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.MEM.MEM_Header28x55x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.MEM.MEM_Header28x55x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.MEM.MEM_Header28x55x'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SWI$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.SWI.SWI_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("lock", "TInt"));
        sizes.add(Global.newArray("ready", "UPtr"));
        sizes.add(Global.newArray("mask", "UInt"));
        sizes.add(Global.newArray("link", "UPtr"));
        sizes.add(Global.newArray("initkey", "UInt"));
        sizes.add(Global.newArray("mailbox", "UInt"));
        sizes.add(Global.newArray("fxn", "UFxn"));
        sizes.add(Global.newArray("arg1", "TIArg"));
        sizes.add(Global.newArray("arg0", "TIArg"));
        sizes.add(Global.newArray("stslock", "TInt"));
        sizes.add(Global.newArray("sts", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.SWI.SWI_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.SWI.SWI_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.SWI.SWI_Obj'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void QUE$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("next", "UPtr"));
        sizes.add(Global.newArray("prev", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.QUE.QUE_Elem']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.QUE.QUE_Elem']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.QUE.QUE_Elem'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void ROV$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void OBJ$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.OBJ.OBJ_TSK", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("link", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("type", "UInt"));
        sizes.add(Global.newArray("obj", "Sti.bios.rov.TSK;TSK_Obj"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.OBJ.OBJ_TSK_55x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("link", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("type", "UInt"));
        sizes.add(Global.newArray("obj", "Sti.bios.rov.TSK;TSK_Obj_55x"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK_55x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK_55x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.OBJ.OBJ_TSK_55x'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.OBJ.OBJ_SEM", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("link", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("type", "UInt"));
        sizes.add(Global.newArray("obj", "Sti.bios.rov.SEM;SEM_Obj"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.OBJ.OBJ_SEM']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.OBJ.OBJ_SEM']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.OBJ.OBJ_SEM'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.OBJ.OBJ_MBX", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("link", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("type", "UInt"));
        sizes.add(Global.newArray("obj", "Sti.bios.rov.MBX;MBX_Obj"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.OBJ.OBJ_MBX']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.OBJ.OBJ_MBX']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.OBJ.OBJ_MBX'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TSK$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.TSK.TSK_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("kobj", "Sti.bios.rov.KNL;KNL_Obj"));
        sizes.add(Global.newArray("stack", "UPtr"));
        sizes.add(Global.newArray("stacksize", "USize"));
        sizes.add(Global.newArray("stackseg", "TInt"));
        sizes.add(Global.newArray("name", "UPtr"));
        sizes.add(Global.newArray("environ", "UPtr"));
        sizes.add(Global.newArray("errno", "TInt"));
        sizes.add(Global.newArray("exitflag", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.TSK.TSK_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.TSK.TSK_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.TSK.TSK_Obj'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.TSK.TSK_Obj_55x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("kobj", "Sti.bios.rov.KNL;KNL_Obj"));
        sizes.add(Global.newArray("stack", "UPtr"));
        sizes.add(Global.newArray("sysstack", "UPtr"));
        sizes.add(Global.newArray("stacksize", "USize"));
        sizes.add(Global.newArray("sysstacksize", "USize"));
        sizes.add(Global.newArray("stackseg", "TInt"));
        sizes.add(Global.newArray("name", "UPtr"));
        sizes.add(Global.newArray("environ", "UPtr"));
        sizes.add(Global.newArray("errno", "TInt"));
        sizes.add(Global.newArray("exitflag", "UShort"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.TSK.TSK_Obj_55x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.TSK.TSK_Obj_55x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.TSK.TSK_Obj_55x'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void SEM$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("job", "Sti.bios.rov.KNL;KNL_Job"));
        sizes.add(Global.newArray("count", "TInt"));
        sizes.add(Global.newArray("pendQ", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("name", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.SEM.SEM_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.SEM.SEM_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.SEM.SEM_Obj'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void LOG$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.LOG.LOG_Event_6x", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("seqnum", "UInt"));
        sizes.add(Global.newArray("val1", "TIArg"));
        sizes.add(Global.newArray("val2", "TIArg"));
        sizes.add(Global.newArray("val3", "TIArg"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.LOG.LOG_Event_6x']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.LOG.LOG_Event_6x']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.LOG.LOG_Event_6x'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.LOG.LOG_Event_28_55", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("pad", "UInt"));
        sizes.add(Global.newArray("seqnum", "UInt"));
        sizes.add(Global.newArray("val1", "TIArg"));
        sizes.add(Global.newArray("val2", "TIArg"));
        sizes.add(Global.newArray("val3", "TIArg"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.LOG.LOG_Event_28_55']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.LOG.LOG_Event_28_55']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.LOG.LOG_Event_28_55'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.bios.rov.LOG.LOG_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("bufend", "UPtr"));
        sizes.add(Global.newArray("flag", "UInt"));
        sizes.add(Global.newArray("seqnum", "UInt"));
        sizes.add(Global.newArray("curptr", "UPtr"));
        sizes.add(Global.newArray("lenmask", "UInt"));
        sizes.add(Global.newArray("bufbeg", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.LOG.LOG_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.LOG.LOG_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.LOG.LOG_Obj'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void MBX$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.bios.rov.MBX.MBX_Obj", "ti.bios.rov");
        sizes.clear();
        sizes.add(Global.newArray("dataQue", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("freeQue", "Sti.bios.rov.QUE;QUE_Elem"));
        sizes.add(Global.newArray("dataSem", "Sti.bios.rov.SEM;SEM_Obj"));
        sizes.add(Global.newArray("freeSem", "Sti.bios.rov.SEM;SEM_Obj"));
        sizes.add(Global.newArray("segid", "TInt"));
        sizes.add(Global.newArray("size", "USize"));
        sizes.add(Global.newArray("len", "UInt"));
        sizes.add(Global.newArray("name", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.bios.rov.MBX.MBX_Obj']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.bios.rov.MBX.MBX_Obj']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.bios.rov.MBX.MBX_Obj'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void KNL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/KNL.xs");
        om.bind("ti.bios.rov.KNL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL.Module", "ti.bios.rov");
        po.init("ti.bios.rov.KNL.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.KNL$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.KNL$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.KNL$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.KNL$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getCurTaskAddr", (Proto.Fxn)om.findStrict("ti.bios.rov.KNL$$getCurTaskAddr", "ti.bios.rov"), Global.get(cap, "getCurTaskAddr"));
                po.addFxn("getQueues", (Proto.Fxn)om.findStrict("ti.bios.rov.KNL$$getQueues", "ti.bios.rov"), Global.get(cap, "getQueues"));
        // struct KNL.KNL_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$KNL_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.KNL.KNL_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("ready", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("alarm", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("setpri", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("queue", new Proto.Adr("ti_bios_rov_QUE_QUE_Elem*", "PS"), $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("sp", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("timeout", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mode", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sts", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("signalled", $$T_Bool, $$UNDEF, "w");
        // struct KNL.KNL_Job
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$KNL_Job", "ti.bios.rov");
        po.init("ti.bios.rov.KNL.KNL_Job", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("wListElem", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("wCount", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Fxn", "Pf"), $$UNDEF, "w");
        // struct KNL.SystemView
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$SystemView", "ti.bios.rov");
        po.init("ti.bios.rov.KNL.SystemView", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("systemTick", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("stackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("stackPeak", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sysStackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("sysStackSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sysStackPeak", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void MEM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/MEM.xs");
        om.bind("ti.bios.rov.MEM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM.Module", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.MEM$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.MEM$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.MEM$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.MEM$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct MEM.MEM_Segment6x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Segment6x", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.MEM_Segment6x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("base", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("len", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("space", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct MEM.MEM_Segment28x55x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Segment28x55x", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.MEM_Segment28x55x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("base", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("len", Proto.Elm.newCNum("(xdc_ULong)"), $$UNDEF, "w");
                po.addFld("space", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
        // struct MEM.MEM_Header6x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Header6x", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.MEM_Header6x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("next", new Proto.Adr("ti_bios_rov_MEM_MEM_Header6x*", "PS"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
        // struct MEM.MEM_Header28x55x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Header28x55x", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.MEM_Header28x55x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("next", new Proto.Adr("ti_bios_rov_MEM_MEM_Header6x*", "PS"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_ULong)"), $$UNDEF, "w");
        // struct MEM.InstanceData
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$InstanceData", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.InstanceData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("largestFree", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("freeMem", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("usedMem", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("totalSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("startAddress", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("endAddress", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct MEM.FreeListElement
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$FreeListElement", "ti.bios.rov");
        po.init("ti.bios.rov.MEM.FreeListElement", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("address", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void SWI$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/SWI.xs");
        om.bind("ti.bios.rov.SWI$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SWI.Module", "ti.bios.rov");
        po.init("ti.bios.rov.SWI.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.SWI$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.SWI$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.SWI$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.SWI$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getNickName", (Proto.Fxn)om.findStrict("ti.bios.rov.SWI$$getNickName", "ti.bios.rov"), Global.get(cap, "getNickName"));
                po.addFxn("fxnToHandle", (Proto.Fxn)om.findStrict("ti.bios.rov.SWI$$fxnToHandle", "ti.bios.rov"), Global.get(cap, "fxnToHandle"));
        // struct SWI.SWI_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SWI$$SWI_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.SWI.SWI_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("lock", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("ready", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("mask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("link", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("initkey", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("mailbox", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("fxn", new Proto.Adr("xdc_Fxn", "Pf"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("stslock", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sts", new Proto.Adr("xdc_Ptr*", "PPv"), $$UNDEF, "w");
        // struct SWI.InstanceData
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SWI$$InstanceData", "ti.bios.rov");
        po.init("ti.bios.rov.SWI.InstanceData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("handle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("mailboxVal", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("fxn", $$T_Str, $$UNDEF, "w");
                po.addFld("arg0", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("arg1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("fxnAddr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
    }

    void QUE$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/QUE.xs");
        om.bind("ti.bios.rov.QUE$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.QUE.Module", "ti.bios.rov");
        po.init("ti.bios.rov.QUE.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.QUE$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.QUE$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.QUE$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.QUE$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("scanQue", (Proto.Fxn)om.findStrict("ti.bios.rov.QUE$$scanQue", "ti.bios.rov"), Global.get(cap, "scanQue"));
        // struct QUE.QUE_Elem
        po = (Proto.Obj)om.findStrict("ti.bios.rov.QUE$$QUE_Elem", "ti.bios.rov");
        po.init("ti.bios.rov.QUE.QUE_Elem", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("next", new Proto.Adr("ti_bios_rov_QUE_QUE_Elem*", "PS"), $$UNDEF, "w");
                po.addFld("prev", new Proto.Adr("ti_bios_rov_QUE_QUE_Elem*", "PS"), $$UNDEF, "w");
    }

    void ROV$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/ROV.xs");
        om.bind("ti.bios.rov.ROV$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov");
        po.init("ti.bios.rov.ROV.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.ROV$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.ROV$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.ROV$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.ROV$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("fetchGlobal", (Proto.Fxn)om.findStrict("ti.bios.rov.ROV$$fetchGlobal", "ti.bios.rov"), Global.get(cap, "fetchGlobal"));
                po.addFxn("getISA", (Proto.Fxn)om.findStrict("ti.bios.rov.ROV$$getISA", "ti.bios.rov"), Global.get(cap, "getISA"));
                po.addFxn("getNickName", (Proto.Fxn)om.findStrict("ti.bios.rov.ROV$$getNickName", "ti.bios.rov"), Global.get(cap, "getNickName"));
                po.addFxn("getName", (Proto.Fxn)om.findStrict("ti.bios.rov.ROV$$getName", "ti.bios.rov"), Global.get(cap, "getName"));
                po.addFxn("getStaticName", (Proto.Fxn)om.findStrict("ti.bios.rov.ROV$$getStaticName", "ti.bios.rov"), Global.get(cap, "getStaticName"));
    }

    void OBJ$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/OBJ.xs");
        om.bind("ti.bios.rov.OBJ$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ.Module", "ti.bios.rov");
        po.init("ti.bios.rov.OBJ.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.OBJ$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.OBJ$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.OBJ$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.OBJ$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getTasks", (Proto.Fxn)om.findStrict("ti.bios.rov.OBJ$$getTasks", "ti.bios.rov"), Global.get(cap, "getTasks"));
                po.addFxn("getSems", (Proto.Fxn)om.findStrict("ti.bios.rov.OBJ$$getSems", "ti.bios.rov"), Global.get(cap, "getSems"));
                po.addFxn("getMailboxes", (Proto.Fxn)om.findStrict("ti.bios.rov.OBJ$$getMailboxes", "ti.bios.rov"), Global.get(cap, "getMailboxes"));
        // struct OBJ.OBJ_TSK
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_TSK", "ti.bios.rov");
        po.init("ti.bios.rov.OBJ.OBJ_TSK", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("link", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("type", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("obj", (Proto)om.findStrict("ti.bios.rov.TSK.TSK_Obj", "ti.bios.rov"), $$DEFAULT, "w");
        // struct OBJ.OBJ_TSK_55x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_TSK_55x", "ti.bios.rov");
        po.init("ti.bios.rov.OBJ.OBJ_TSK_55x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("link", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("type", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("obj", (Proto)om.findStrict("ti.bios.rov.TSK.TSK_Obj_55x", "ti.bios.rov"), $$DEFAULT, "w");
        // struct OBJ.OBJ_SEM
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_SEM", "ti.bios.rov");
        po.init("ti.bios.rov.OBJ.OBJ_SEM", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("link", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("type", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("obj", (Proto)om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov"), $$DEFAULT, "w");
        // struct OBJ.OBJ_MBX
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_MBX", "ti.bios.rov");
        po.init("ti.bios.rov.OBJ.OBJ_MBX", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("link", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("type", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("obj", (Proto)om.findStrict("ti.bios.rov.MBX.MBX_Obj", "ti.bios.rov"), $$DEFAULT, "w");
    }

    void TSK$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/TSK.xs");
        om.bind("ti.bios.rov.TSK$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK.Module", "ti.bios.rov");
        po.init("ti.bios.rov.TSK.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.TSK$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.TSK$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.TSK$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.TSK$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getNickName", (Proto.Fxn)om.findStrict("ti.bios.rov.TSK$$getNickName", "ti.bios.rov"), Global.get(cap, "getNickName"));
                po.addFxn("getBlockedTask", (Proto.Fxn)om.findStrict("ti.bios.rov.TSK$$getBlockedTask", "ti.bios.rov"), Global.get(cap, "getBlockedTask"));
        // struct TSK.TSK_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK$$TSK_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.TSK.TSK_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("kobj", (Proto)om.findStrict("ti.bios.rov.KNL.KNL_Obj", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("stack", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("stacksize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stackseg", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("environ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("errno", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("exitflag", $$T_Bool, $$UNDEF, "w");
        // struct TSK.TSK_Obj_55x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK$$TSK_Obj_55x", "ti.bios.rov");
        po.init("ti.bios.rov.TSK.TSK_Obj_55x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("kobj", (Proto)om.findStrict("ti.bios.rov.KNL.KNL_Obj", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("stack", new Proto.Arr(Proto.Elm.newCNum("(xdc_Char)"), false), $$DEFAULT, "w");
                po.addFld("sysstack", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("stacksize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("sysstacksize", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("stackseg", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("environ", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("errno", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("exitflag", $$T_Bool, $$UNDEF, "w");
        // struct TSK.InstanceData
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK$$InstanceData", "ti.bios.rov");
        po.init("ti.bios.rov.TSK.InstanceData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("handle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("state", $$T_Str, $$UNDEF, "w");
                po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timeout", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("timeRemaining", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("blockedOn", $$T_Str, $$UNDEF, "w");
                po.addFld("stackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("stackSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("stackPeak", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sysStackBase", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("sysStackSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("sysStackPeak", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void SEM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/SEM.xs");
        om.bind("ti.bios.rov.SEM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SEM.Module", "ti.bios.rov");
        po.init("ti.bios.rov.SEM.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.SEM$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.SEM$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.SEM$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.SEM$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getNickName", (Proto.Fxn)om.findStrict("ti.bios.rov.SEM$$getNickName", "ti.bios.rov"), Global.get(cap, "getNickName"));
                po.addFxn("getTskBlockedOn", (Proto.Fxn)om.findStrict("ti.bios.rov.SEM$$getTskBlockedOn", "ti.bios.rov"), Global.get(cap, "getTskBlockedOn"));
        // struct SEM.SEM_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SEM$$SEM_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.SEM.SEM_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("job", (Proto)om.findStrict("ti.bios.rov.KNL.KNL_Job", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("pendQ", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
        // struct SEM.InstanceData
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SEM$$InstanceData", "ti.bios.rov");
        po.init("ti.bios.rov.SEM.InstanceData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("handle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("count", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("numTasksPending", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("pendQ", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
    }

    void LOG$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/LOG.xs");
        om.bind("ti.bios.rov.LOG$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG.Module", "ti.bios.rov");
        po.init("ti.bios.rov.LOG.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.LOG$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.LOG$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.LOG$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.LOG$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct LOG.LOG_Event_6x
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Event_6x", "ti.bios.rov");
        po.init("ti.bios.rov.LOG.LOG_Event_6x", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("seqnum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("val1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("val2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("val3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LOG.LOG_Event_28_55
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Event_28_55", "ti.bios.rov");
        po.init("ti.bios.rov.LOG.LOG_Event_28_55", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("pad", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("seqnum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("val1", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("val2", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
                po.addFld("val3", new Proto.Adr("xdc_IArg", "Pv"), $$UNDEF, "w");
        // struct LOG.LOG_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.LOG.LOG_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("bufend", new Proto.Adr("xdc_IArg*", "PPv"), $$UNDEF, "w");
                po.addFld("flag", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("seqnum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("curptr", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("lenmask", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("bufbeg", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
        // struct LOG.LogRecordView
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LogRecordView", "ti.bios.rov");
        po.init("ti.bios.rov.LOG.LogRecordView", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("seqnum", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("timestamp", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("message", $$T_Str, $$UNDEF, "w");
    }

    void MBX$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/MBX.xs");
        om.bind("ti.bios.rov.MBX$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MBX.Module", "ti.bios.rov");
        po.init("ti.bios.rov.MBX.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.bios.rov"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
                        po.addFld("rovViewInfo", (Proto)om.findStrict("xdc.rov.ViewInfo.Instance", "ti.bios.rov"), $$UNDEF, "wh");
        }//isCFG
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.MBX$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.MBX$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$static$init");
                if (fxn != null) om.bind("ti.bios.rov.MBX$$module$static$init", true);
                if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.MBX$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getTskBlockedOn", (Proto.Fxn)om.findStrict("ti.bios.rov.MBX$$getTskBlockedOn", "ti.bios.rov"), Global.get(cap, "getTskBlockedOn"));
        // struct MBX.MBX_Obj
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MBX$$MBX_Obj", "ti.bios.rov");
        po.init("ti.bios.rov.MBX.MBX_Obj", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("dataQue", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("freeQue", (Proto)om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("dataSem", (Proto)om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("freeSem", (Proto)om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov"), $$DEFAULT, "w");
                po.addFld("segid", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("size", Proto.Elm.newCNum("(xdc_SizeT)"), $$UNDEF, "w");
                po.addFld("len", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "w");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
        // struct MBX.InstanceData
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MBX$$InstanceData", "ti.bios.rov");
        po.init("ti.bios.rov.MBX.InstanceData", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("name", $$T_Str, $$UNDEF, "w");
                po.addFld("handle", new Proto.Adr("xdc_Ptr", "Pv"), $$UNDEF, "w");
                po.addFld("numTasksPending", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("tasksPending", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("numTasksBlockedPosting", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("tasksPosting", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("numMsgs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("maxMsgs", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("msgSize", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
                po.addFld("memSegment", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void KNL$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.KNL", "ti.bios.rov");
        vo.bind("KNL_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.KNL.KNL_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$KNL_Obj", "ti.bios.rov");
        po.bind("queue$fetchDesc", Global.newObject("type", "ti.bios.rov.QUE.QUE_Elem", "isScalar", false));
        vo.bind("KNL_Job$fetchDesc", Global.newObject("type", "ti.bios.rov.KNL.KNL_Job", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$KNL_Job", "ti.bios.rov");
        vo.bind("SystemView$fetchDesc", Global.newObject("type", "ti.bios.rov.KNL.SystemView", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL$$SystemView", "ti.bios.rov");
    }

    void MEM$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.MEM", "ti.bios.rov");
        vo.bind("MEM_Segment6x$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Segment6x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Segment6x", "ti.bios.rov");
        vo.bind("MEM_Segment28x55x$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Segment28x55x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Segment28x55x", "ti.bios.rov");
        vo.bind("MEM_Header6x$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Header6x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Header6x", "ti.bios.rov");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Header6x", "isScalar", false));
        vo.bind("MEM_Header28x55x$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Header28x55x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM$$MEM_Header28x55x", "ti.bios.rov");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.bios.rov.MEM.MEM_Header6x", "isScalar", false));
    }

    void SWI$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.SWI", "ti.bios.rov");
        vo.bind("SWI_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.SWI.SWI_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SWI$$SWI_Obj", "ti.bios.rov");
    }

    void QUE$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.QUE", "ti.bios.rov");
        vo.bind("QUE_Elem$fetchDesc", Global.newObject("type", "ti.bios.rov.QUE.QUE_Elem", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.QUE$$QUE_Elem", "ti.bios.rov");
        po.bind("next$fetchDesc", Global.newObject("type", "ti.bios.rov.QUE.QUE_Elem", "isScalar", false));
        po.bind("prev$fetchDesc", Global.newObject("type", "ti.bios.rov.QUE.QUE_Elem", "isScalar", false));
    }

    void ROV$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.ROV", "ti.bios.rov");
    }

    void OBJ$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.OBJ", "ti.bios.rov");
        vo.bind("OBJ_TSK$fetchDesc", Global.newObject("type", "ti.bios.rov.OBJ.OBJ_TSK", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_TSK", "ti.bios.rov");
        vo.bind("OBJ_TSK_55x$fetchDesc", Global.newObject("type", "ti.bios.rov.OBJ.OBJ_TSK_55x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_TSK_55x", "ti.bios.rov");
        vo.bind("OBJ_SEM$fetchDesc", Global.newObject("type", "ti.bios.rov.OBJ.OBJ_SEM", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_SEM", "ti.bios.rov");
        vo.bind("OBJ_MBX$fetchDesc", Global.newObject("type", "ti.bios.rov.OBJ.OBJ_MBX", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ$$OBJ_MBX", "ti.bios.rov");
    }

    void TSK$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.TSK", "ti.bios.rov");
        vo.bind("TSK_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.TSK.TSK_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK$$TSK_Obj", "ti.bios.rov");
        po.bind("stack$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
        vo.bind("TSK_Obj_55x$fetchDesc", Global.newObject("type", "ti.bios.rov.TSK.TSK_Obj_55x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK$$TSK_Obj_55x", "ti.bios.rov");
        po.bind("stack$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_Char", "isScalar", true));
    }

    void SEM$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.SEM", "ti.bios.rov");
        vo.bind("SEM_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.SEM.SEM_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SEM$$SEM_Obj", "ti.bios.rov");
    }

    void LOG$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.LOG", "ti.bios.rov");
        vo.bind("LOG_Event_6x$fetchDesc", Global.newObject("type", "ti.bios.rov.LOG.LOG_Event_6x", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Event_6x", "ti.bios.rov");
        vo.bind("LOG_Event_28_55$fetchDesc", Global.newObject("type", "ti.bios.rov.LOG.LOG_Event_28_55", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Event_28_55", "ti.bios.rov");
        vo.bind("LOG_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.LOG.LOG_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG$$LOG_Obj", "ti.bios.rov");
    }

    void MBX$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.MBX", "ti.bios.rov");
        vo.bind("MBX_Obj$fetchDesc", Global.newObject("type", "ti.bios.rov.MBX.MBX_Obj", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MBX$$MBX_Obj", "ti.bios.rov");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.bios.rov.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.bios.rov"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.bios.rov", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.bios.rov");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.bios.rov.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.bios.rov'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        sb.append("if('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void KNL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.KNL", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.KNL.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.KNL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.KNL$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("KNL_Obj", om.findStrict("ti.bios.rov.KNL.KNL_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.KNL.KNL_Obj", "ti.bios.rov"));
        vo.bind("KNL_Job", om.findStrict("ti.bios.rov.KNL.KNL_Job", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.KNL.KNL_Job", "ti.bios.rov"));
        vo.bind("SystemView", om.findStrict("ti.bios.rov.KNL.SystemView", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.KNL.SystemView", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_KNL_Handle__label__E", "ti_bios_rov_KNL_Module__startupDone__E", "ti_bios_rov_KNL_Object__create__E", "ti_bios_rov_KNL_Object__delete__E", "ti_bios_rov_KNL_Object__destruct__E", "ti_bios_rov_KNL_Object__get__E", "ti_bios_rov_KNL_Object__first__E", "ti_bios_rov_KNL_Object__next__E", "ti_bios_rov_KNL_Params__init__E", "ti_bios_rov_KNL_Proxy__abstract__E", "ti_bios_rov_KNL_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("KNL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("KNL");
    }

    void MEM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.MEM", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MEM.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.MEM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.MEM$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("MEM_Segment6x", om.findStrict("ti.bios.rov.MEM.MEM_Segment6x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.MEM_Segment6x", "ti.bios.rov"));
        vo.bind("MEM_Segment28x55x", om.findStrict("ti.bios.rov.MEM.MEM_Segment28x55x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.MEM_Segment28x55x", "ti.bios.rov"));
        vo.bind("MEM_Header6x", om.findStrict("ti.bios.rov.MEM.MEM_Header6x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.MEM_Header6x", "ti.bios.rov"));
        vo.bind("MEM_Header28x55x", om.findStrict("ti.bios.rov.MEM.MEM_Header28x55x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.MEM_Header28x55x", "ti.bios.rov"));
        vo.bind("InstanceData", om.findStrict("ti.bios.rov.MEM.InstanceData", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.InstanceData", "ti.bios.rov"));
        vo.bind("FreeListElement", om.findStrict("ti.bios.rov.MEM.FreeListElement", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MEM.FreeListElement", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_MEM_Handle__label__E", "ti_bios_rov_MEM_Module__startupDone__E", "ti_bios_rov_MEM_Object__create__E", "ti_bios_rov_MEM_Object__delete__E", "ti_bios_rov_MEM_Object__destruct__E", "ti_bios_rov_MEM_Object__get__E", "ti_bios_rov_MEM_Object__first__E", "ti_bios_rov_MEM_Object__next__E", "ti_bios_rov_MEM_Params__init__E", "ti_bios_rov_MEM_Proxy__abstract__E", "ti_bios_rov_MEM_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("MEM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MEM");
    }

    void SWI$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.SWI", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SWI.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.SWI", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.SWI$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("SWI_Obj", om.findStrict("ti.bios.rov.SWI.SWI_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.SWI.SWI_Obj", "ti.bios.rov"));
        vo.bind("InstanceData", om.findStrict("ti.bios.rov.SWI.InstanceData", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.SWI.InstanceData", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_SWI_Handle__label__E", "ti_bios_rov_SWI_Module__startupDone__E", "ti_bios_rov_SWI_Object__create__E", "ti_bios_rov_SWI_Object__delete__E", "ti_bios_rov_SWI_Object__destruct__E", "ti_bios_rov_SWI_Object__get__E", "ti_bios_rov_SWI_Object__first__E", "ti_bios_rov_SWI_Object__next__E", "ti_bios_rov_SWI_Params__init__E", "ti_bios_rov_SWI_Proxy__abstract__E", "ti_bios_rov_SWI_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SWI", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SWI");
    }

    void QUE$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.QUE", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.QUE.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.QUE", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.QUE$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("QUE_Elem", om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.QUE.QUE_Elem", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_QUE_Handle__label__E", "ti_bios_rov_QUE_Module__startupDone__E", "ti_bios_rov_QUE_Object__create__E", "ti_bios_rov_QUE_Object__delete__E", "ti_bios_rov_QUE_Object__destruct__E", "ti_bios_rov_QUE_Object__get__E", "ti_bios_rov_QUE_Object__first__E", "ti_bios_rov_QUE_Object__next__E", "ti_bios_rov_QUE_Params__init__E", "ti_bios_rov_QUE_Proxy__abstract__E", "ti_bios_rov_QUE_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("QUE", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("QUE");
    }

    void ROV$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.ROV", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.ROV.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.ROV", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.ROV$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_ROV_Handle__label__E", "ti_bios_rov_ROV_Module__startupDone__E", "ti_bios_rov_ROV_Object__create__E", "ti_bios_rov_ROV_Object__delete__E", "ti_bios_rov_ROV_Object__destruct__E", "ti_bios_rov_ROV_Object__get__E", "ti_bios_rov_ROV_Object__first__E", "ti_bios_rov_ROV_Object__next__E", "ti_bios_rov_ROV_Params__init__E", "ti_bios_rov_ROV_Proxy__abstract__E", "ti_bios_rov_ROV_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("ROV", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ROV");
    }

    void OBJ$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.OBJ", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.OBJ.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.OBJ", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.OBJ$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("OBJ_TSK", om.findStrict("ti.bios.rov.OBJ.OBJ_TSK", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.OBJ.OBJ_TSK", "ti.bios.rov"));
        vo.bind("OBJ_TSK_55x", om.findStrict("ti.bios.rov.OBJ.OBJ_TSK_55x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.OBJ.OBJ_TSK_55x", "ti.bios.rov"));
        vo.bind("OBJ_SEM", om.findStrict("ti.bios.rov.OBJ.OBJ_SEM", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.OBJ.OBJ_SEM", "ti.bios.rov"));
        vo.bind("OBJ_MBX", om.findStrict("ti.bios.rov.OBJ.OBJ_MBX", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.OBJ.OBJ_MBX", "ti.bios.rov"));
        vo.bind("objType", om.findStrict("ti.bios.rov.OBJ.objType", "ti.bios.rov"));
        vo.bind("SEM", om.findStrict("ti.bios.rov.OBJ.SEM", "ti.bios.rov"));
        vo.bind("SWI", om.findStrict("ti.bios.rov.OBJ.SWI", "ti.bios.rov"));
        vo.bind("TSK", om.findStrict("ti.bios.rov.OBJ.TSK", "ti.bios.rov"));
        vo.bind("MBX", om.findStrict("ti.bios.rov.OBJ.MBX", "ti.bios.rov"));
        vo.bind("SIO", om.findStrict("ti.bios.rov.OBJ.SIO", "ti.bios.rov"));
        vo.bind("DEV", om.findStrict("ti.bios.rov.OBJ.DEV", "ti.bios.rov"));
        vo.bind("BUF", om.findStrict("ti.bios.rov.OBJ.BUF", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_OBJ_Handle__label__E", "ti_bios_rov_OBJ_Module__startupDone__E", "ti_bios_rov_OBJ_Object__create__E", "ti_bios_rov_OBJ_Object__delete__E", "ti_bios_rov_OBJ_Object__destruct__E", "ti_bios_rov_OBJ_Object__get__E", "ti_bios_rov_OBJ_Object__first__E", "ti_bios_rov_OBJ_Object__next__E", "ti_bios_rov_OBJ_Params__init__E", "ti_bios_rov_OBJ_Proxy__abstract__E", "ti_bios_rov_OBJ_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("OBJ", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("OBJ");
    }

    void TSK$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.TSK", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.TSK.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.TSK", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.TSK$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("TSK_Obj", om.findStrict("ti.bios.rov.TSK.TSK_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.TSK.TSK_Obj", "ti.bios.rov"));
        vo.bind("TSK_Obj_55x", om.findStrict("ti.bios.rov.TSK.TSK_Obj_55x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.TSK.TSK_Obj_55x", "ti.bios.rov"));
        vo.bind("InstanceData", om.findStrict("ti.bios.rov.TSK.InstanceData", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.TSK.InstanceData", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_TSK_Handle__label__E", "ti_bios_rov_TSK_Module__startupDone__E", "ti_bios_rov_TSK_Object__create__E", "ti_bios_rov_TSK_Object__delete__E", "ti_bios_rov_TSK_Object__destruct__E", "ti_bios_rov_TSK_Object__get__E", "ti_bios_rov_TSK_Object__first__E", "ti_bios_rov_TSK_Object__next__E", "ti_bios_rov_TSK_Params__init__E", "ti_bios_rov_TSK_Proxy__abstract__E", "ti_bios_rov_TSK_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TSK", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TSK");
    }

    void SEM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.SEM", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.SEM.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.SEM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.SEM$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("SEM_Obj", om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.SEM.SEM_Obj", "ti.bios.rov"));
        vo.bind("InstanceData", om.findStrict("ti.bios.rov.SEM.InstanceData", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.SEM.InstanceData", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_SEM_Handle__label__E", "ti_bios_rov_SEM_Module__startupDone__E", "ti_bios_rov_SEM_Object__create__E", "ti_bios_rov_SEM_Object__delete__E", "ti_bios_rov_SEM_Object__destruct__E", "ti_bios_rov_SEM_Object__get__E", "ti_bios_rov_SEM_Object__first__E", "ti_bios_rov_SEM_Object__next__E", "ti_bios_rov_SEM_Params__init__E", "ti_bios_rov_SEM_Proxy__abstract__E", "ti_bios_rov_SEM_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SEM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SEM");
    }

    void LOG$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.LOG", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.LOG.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.LOG", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.LOG$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("LOG_Event_6x", om.findStrict("ti.bios.rov.LOG.LOG_Event_6x", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.LOG.LOG_Event_6x", "ti.bios.rov"));
        vo.bind("LOG_Event_28_55", om.findStrict("ti.bios.rov.LOG.LOG_Event_28_55", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.LOG.LOG_Event_28_55", "ti.bios.rov"));
        vo.bind("LOG_Obj", om.findStrict("ti.bios.rov.LOG.LOG_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.LOG.LOG_Obj", "ti.bios.rov"));
        vo.bind("LogRecordView", om.findStrict("ti.bios.rov.LOG.LogRecordView", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.LOG.LogRecordView", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_LOG_Handle__label__E", "ti_bios_rov_LOG_Module__startupDone__E", "ti_bios_rov_LOG_Object__create__E", "ti_bios_rov_LOG_Object__delete__E", "ti_bios_rov_LOG_Object__destruct__E", "ti_bios_rov_LOG_Object__get__E", "ti_bios_rov_LOG_Object__first__E", "ti_bios_rov_LOG_Object__next__E", "ti_bios_rov_LOG_Params__init__E", "ti_bios_rov_LOG_Proxy__abstract__E", "ti_bios_rov_LOG_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("LOG", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LOG");
    }

    void MBX$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.MBX", "ti.bios.rov");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.MBX.Module", "ti.bios.rov");
        vo.init2(po, "ti.bios.rov.MBX", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.MBX$$capsule", "ti.bios.rov"));
        vo.bind("$package", om.findStrict("ti.bios.rov", "ti.bios.rov"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        mcfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        mcfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        vo.bind("MBX_Obj", om.findStrict("ti.bios.rov.MBX.MBX_Obj", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MBX.MBX_Obj", "ti.bios.rov"));
        vo.bind("InstanceData", om.findStrict("ti.bios.rov.MBX.InstanceData", "ti.bios.rov"));
        tdefs.add(om.findStrict("ti.bios.rov.MBX.InstanceData", "ti.bios.rov"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_bios_rov_MBX_Handle__label__E", "ti_bios_rov_MBX_Module__startupDone__E", "ti_bios_rov_MBX_Object__create__E", "ti_bios_rov_MBX_Object__delete__E", "ti_bios_rov_MBX_Object__destruct__E", "ti_bios_rov_MBX_Object__get__E", "ti_bios_rov_MBX_Object__first__E", "ti_bios_rov_MBX_Object__next__E", "ti_bios_rov_MBX_Params__init__E", "ti_bios_rov_MBX_Proxy__abstract__E", "ti_bios_rov_MBX_Proxy__delegate__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("MBX", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MBX");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.KNL", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.MEM", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.SWI", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.QUE", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.ROV", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.OBJ", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.TSK", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.SEM", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.LOG", "ti.bios.rov"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.MBX", "ti.bios.rov"));
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.KNL", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"System", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitSystem", "structName", "SystemView")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.MEM", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitInstances", "structName", "InstanceData")}), Global.newArray(new Object[]{"FreeLists", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitFreeLists", "structName", "FreeListElement")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.SWI", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitInstances", "structName", "InstanceData")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.TSK", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitInstances", "structName", "InstanceData")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.SEM", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitInstances", "structName", "InstanceData")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.LOG", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Logs", Global.newObject("type", om.find("xdc.rov.ViewInfo.INSTANCE_DATA"), "viewInitFxn", "viewInitLogs", "structName", "LogRecordView")})}), "showRawTab", false)));
        }//isCFG
        if (isCFG) {
            vo = (Value.Obj)om.findStrict("ti.bios.rov.MBX", "ti.bios.rov");
            Global.put(vo, "rovViewInfo", Global.callFxn("create", (Scriptable)om.find("xdc.rov.ViewInfo"), Global.newObject("viewMap", Global.newArray(new Object[]{Global.newArray(new Object[]{"Instances", Global.newObject("type", om.find("xdc.rov.ViewInfo.MODULE_DATA"), "viewInitFxn", "viewInitInstances", "structName", "InstanceData")})}), "showRawTab", false)));
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.bios.rov.KNL")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.MEM")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.SWI")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.QUE")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.ROV")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.OBJ")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.TSK")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.SEM")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.LOG")).bless();
        ((Value.Obj)om.getv("ti.bios.rov.MBX")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.bios.rov")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        KNL$$OBJECTS();
        MEM$$OBJECTS();
        SWI$$OBJECTS();
        QUE$$OBJECTS();
        ROV$$OBJECTS();
        OBJ$$OBJECTS();
        TSK$$OBJECTS();
        SEM$$OBJECTS();
        LOG$$OBJECTS();
        MBX$$OBJECTS();
        KNL$$CONSTS();
        MEM$$CONSTS();
        SWI$$CONSTS();
        QUE$$CONSTS();
        ROV$$CONSTS();
        OBJ$$CONSTS();
        TSK$$CONSTS();
        SEM$$CONSTS();
        LOG$$CONSTS();
        MBX$$CONSTS();
        KNL$$CREATES();
        MEM$$CREATES();
        SWI$$CREATES();
        QUE$$CREATES();
        ROV$$CREATES();
        OBJ$$CREATES();
        TSK$$CREATES();
        SEM$$CREATES();
        LOG$$CREATES();
        MBX$$CREATES();
        KNL$$FUNCTIONS();
        MEM$$FUNCTIONS();
        SWI$$FUNCTIONS();
        QUE$$FUNCTIONS();
        ROV$$FUNCTIONS();
        OBJ$$FUNCTIONS();
        TSK$$FUNCTIONS();
        SEM$$FUNCTIONS();
        LOG$$FUNCTIONS();
        MBX$$FUNCTIONS();
        KNL$$SIZES();
        MEM$$SIZES();
        SWI$$SIZES();
        QUE$$SIZES();
        ROV$$SIZES();
        OBJ$$SIZES();
        TSK$$SIZES();
        SEM$$SIZES();
        LOG$$SIZES();
        MBX$$SIZES();
        KNL$$TYPES();
        MEM$$TYPES();
        SWI$$TYPES();
        QUE$$TYPES();
        ROV$$TYPES();
        OBJ$$TYPES();
        TSK$$TYPES();
        SEM$$TYPES();
        LOG$$TYPES();
        MBX$$TYPES();
        if (isROV) {
            KNL$$ROV();
            MEM$$ROV();
            SWI$$ROV();
            QUE$$ROV();
            ROV$$ROV();
            OBJ$$ROV();
            TSK$$ROV();
            SEM$$ROV();
            LOG$$ROV();
            MBX$$ROV();
        }//isROV
        $$SINGLETONS();
        KNL$$SINGLETONS();
        MEM$$SINGLETONS();
        SWI$$SINGLETONS();
        QUE$$SINGLETONS();
        ROV$$SINGLETONS();
        OBJ$$SINGLETONS();
        TSK$$SINGLETONS();
        SEM$$SINGLETONS();
        LOG$$SINGLETONS();
        MBX$$SINGLETONS();
        $$INITIALIZATION();
    }
}
