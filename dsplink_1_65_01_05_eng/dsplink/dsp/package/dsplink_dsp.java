/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-u17
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.*;

public class dsplink_dsp
{
    static final String VERS = "@(#) xdc-u17\n";

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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("dsplink.dsp.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("dsplink.dsp", new Value.Obj("dsplink.dsp", pkgP));
    }

    void Global$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("dsplink.dsp.Global.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("dsplink.dsp.Global", new Value.Obj("dsplink.dsp.Global", po));
        pkgV.bind("Global", vo);
        // decls 
        om.bind("dsplink.dsp.Global.DeviceName", new Proto.Enm("dsplink.dsp.Global.DeviceName"));
    }

    void Global$$CONSTS()
    {
        // module Global
        om.bind("dsplink.dsp.Global.DM6446GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DM6446GEM", 0));
        om.bind("dsplink.dsp.Global.DM6467GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DM6467GEM", 1));
        om.bind("dsplink.dsp.Global.OMAP3530", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.OMAP3530", 2));
        om.bind("dsplink.dsp.Global.OMAP2530", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.OMAP2530", 3));
        om.bind("dsplink.dsp.Global.DM648", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DM648", 4));
        om.bind("dsplink.dsp.Global.DM357GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DM357GEM", 5));
        om.bind("dsplink.dsp.Global.DA8XXGEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DA8XXGEM", 6));
        om.bind("dsplink.dsp.Global.OMAPL1XXGEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.OMAPL1XXGEM", 7));
        om.bind("dsplink.dsp.Global.DRA44XGEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DRA44XGEM", 8));
        om.bind("dsplink.dsp.Global.DM6437", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DM6437", 9));
        om.bind("dsplink.dsp.Global.DRX416GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DRX416GEM", 10));
        om.bind("dsplink.dsp.Global.TNETV107XGEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.TNETV107XGEM", 11));
        om.bind("dsplink.dsp.Global.DRX45XGEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DRX45XGEM", 12));
        om.bind("dsplink.dsp.Global.OMAPL138GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.OMAPL138GEM", 13));
        om.bind("dsplink.dsp.Global.DA850GEM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), "dsplink.dsp.Global.DA850GEM", 14));
    }

    void Global$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Global$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Global.getIncludes
        fxn = (Proto.Fxn)om.bind("dsplink.dsp.Global$$getIncludes", new Proto.Fxn(om.findStrict("dsplink.dsp.Global.Module", "dsplink.dsp"), $$T_Str, 0, 0, false));
        // fxn Global.getDefines
        fxn = (Proto.Fxn)om.bind("dsplink.dsp.Global$$getDefines", new Proto.Fxn(om.findStrict("dsplink.dsp.Global.Module", "dsplink.dsp"), $$T_Str, 0, 0, false));
    }

    void Global$$SIZES()
    {
    }

    void Global$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "dsplink/dsp/Global.xs");
        om.bind("dsplink.dsp.Global$$capsule", cap);
        po = (Proto.Obj)om.findStrict("dsplink.dsp.Global.Module", "dsplink.dsp");
        po.init("dsplink.dsp.Global.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("deviceName", (Proto)om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"), $$UNDEF, "wh");
                po.addFld("useProc", $$T_Bool, 1L, "wh");
                po.addFld("usePool", $$T_Bool, 1L, "wh");
                po.addFld("useNotify", $$T_Bool, 1L, "wh");
                po.addFld("useMpcs", $$T_Bool, 1L, "wh");
                po.addFld("useRingio", $$T_Bool, 1L, "wh");
                po.addFld("useMplist", $$T_Bool, 1L, "wh");
                po.addFld("useMsgq", $$T_Bool, 1L, "wh");
                po.addFld("useChnl", $$T_Bool, 1L, "wh");
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("dsplink.dsp.Global$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("dsplink.dsp.Global$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("dsplink.dsp.Global$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getIncludes", (Proto.Fxn)om.findStrict("dsplink.dsp.Global$$getIncludes", "dsplink.dsp"), Global.get(cap, "getIncludes"));
                po.addFxn("getDefines", (Proto.Fxn)om.findStrict("dsplink.dsp.Global$$getDefines", "dsplink.dsp"), Global.get(cap, "getDefines"));
    }

    void Global$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("dsplink.dsp.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "dsplink.dsp"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "dsplink/dsp/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "dsplink.dsp"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "dsplink.dsp"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "dsplink.dsp"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "dsplink.dsp"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "dsplink.dsp"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "dsplink.dsp"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "dsplink.dsp", Value.DEFAULT, false);
        pkgV.bind("$name", "dsplink.dsp");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "dsplink.dsp.");
        pkgV.bind("$vers", Global.newArray("1, 4, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['dsplink.dsp'];\n");
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

    void Global$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("dsplink.dsp.Global", "dsplink.dsp");
        po = (Proto.Obj)om.findStrict("dsplink.dsp.Global.Module", "dsplink.dsp");
        vo.init2(po, "dsplink.dsp.Global", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("dsplink.dsp.Global$$capsule", "dsplink.dsp"));
        vo.bind("$package", om.findStrict("dsplink.dsp", "dsplink.dsp"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        vo.bind("DeviceName", om.findStrict("dsplink.dsp.Global.DeviceName", "dsplink.dsp"));
        vo.bind("DM6446GEM", om.findStrict("dsplink.dsp.Global.DM6446GEM", "dsplink.dsp"));
        vo.bind("DM6467GEM", om.findStrict("dsplink.dsp.Global.DM6467GEM", "dsplink.dsp"));
        vo.bind("OMAP3530", om.findStrict("dsplink.dsp.Global.OMAP3530", "dsplink.dsp"));
        vo.bind("OMAP2530", om.findStrict("dsplink.dsp.Global.OMAP2530", "dsplink.dsp"));
        vo.bind("DM648", om.findStrict("dsplink.dsp.Global.DM648", "dsplink.dsp"));
        vo.bind("DM357GEM", om.findStrict("dsplink.dsp.Global.DM357GEM", "dsplink.dsp"));
        vo.bind("DA8XXGEM", om.findStrict("dsplink.dsp.Global.DA8XXGEM", "dsplink.dsp"));
        vo.bind("OMAPL1XXGEM", om.findStrict("dsplink.dsp.Global.OMAPL1XXGEM", "dsplink.dsp"));
        vo.bind("DRA44XGEM", om.findStrict("dsplink.dsp.Global.DRA44XGEM", "dsplink.dsp"));
        vo.bind("DM6437", om.findStrict("dsplink.dsp.Global.DM6437", "dsplink.dsp"));
        vo.bind("DRX416GEM", om.findStrict("dsplink.dsp.Global.DRX416GEM", "dsplink.dsp"));
        vo.bind("TNETV107XGEM", om.findStrict("dsplink.dsp.Global.TNETV107XGEM", "dsplink.dsp"));
        vo.bind("DRX45XGEM", om.findStrict("dsplink.dsp.Global.DRX45XGEM", "dsplink.dsp"));
        vo.bind("OMAPL138GEM", om.findStrict("dsplink.dsp.Global.OMAPL138GEM", "dsplink.dsp"));
        vo.bind("DA850GEM", om.findStrict("dsplink.dsp.Global.DA850GEM", "dsplink.dsp"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "dsplink.dsp")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("dsplink.dsp.Global$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Global", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Global");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("dsplink.dsp.Global", "dsplink.dsp"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("dsplink.dsp.Global")).bless();
        ((Value.Arr)om.findStrict("$packages", "dsplink.dsp")).add(pkgV);
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
        Global$$OBJECTS();
        Global$$CONSTS();
        Global$$CREATES();
        Global$$FUNCTIONS();
        Global$$SIZES();
        Global$$TYPES();
        if (isROV) {
            Global$$ROV();
        }//isROV
        $$SINGLETONS();
        Global$$SINGLETONS();
        $$INITIALIZATION();
    }
}
