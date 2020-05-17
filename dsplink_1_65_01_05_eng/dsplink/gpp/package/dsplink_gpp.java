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

public class dsplink_gpp
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
        pkgP = (Proto.Obj)om.bind("dsplink.gpp.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("dsplink.gpp", new Value.Obj("dsplink.gpp", pkgP));
    }

    void Global$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("dsplink.gpp.Global.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("dsplink.gpp.Global", new Value.Obj("dsplink.gpp.Global", po));
        pkgV.bind("Global", vo);
        // decls 
        om.bind("dsplink.gpp.Global.RuntimeOS", new Proto.Enm("dsplink.gpp.Global.RuntimeOS"));
        om.bind("dsplink.gpp.Global.DeviceName", new Proto.Enm("dsplink.gpp.Global.DeviceName"));
    }

    void Global$$CONSTS()
    {
        // module Global
        om.bind("dsplink.gpp.Global.LINUX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.RuntimeOS", "dsplink.gpp"), "dsplink.gpp.Global.LINUX", 0));
        om.bind("dsplink.gpp.Global.PROS", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.RuntimeOS", "dsplink.gpp"), "dsplink.gpp.Global.PROS", 1));
        om.bind("dsplink.gpp.Global.WINCE", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.RuntimeOS", "dsplink.gpp"), "dsplink.gpp.Global.WINCE", 2));
        om.bind("dsplink.gpp.Global.DAVINCI", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.DAVINCI", 0));
        om.bind("dsplink.gpp.Global.DAVINCIHD", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.DAVINCIHD", 1));
        om.bind("dsplink.gpp.Global.OMAP3530", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.OMAP3530", 2));
        om.bind("dsplink.gpp.Global.OMAP2530", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.OMAP2530", 3));
        om.bind("dsplink.gpp.Global.LINUXPC", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.LINUXPC", 4));
        om.bind("dsplink.gpp.Global.DA8XX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.DA8XX", 5));
        om.bind("dsplink.gpp.Global.OMAPL1XX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.OMAPL1XX", 6));
        om.bind("dsplink.gpp.Global.DM357", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.DM357", 7));
        om.bind("dsplink.gpp.Global.LEO", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.LEO", 8));
        om.bind("dsplink.gpp.Global.OMAPL138", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.OMAPL138", 9));
        om.bind("dsplink.gpp.Global.DA850", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), "dsplink.gpp.Global.DA850", 10));
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
        fxn = (Proto.Fxn)om.bind("dsplink.gpp.Global$$getIncludes", new Proto.Fxn(om.findStrict("dsplink.gpp.Global.Module", "dsplink.gpp"), $$T_Str, 0, 0, false));
        // fxn Global.getDefines
        fxn = (Proto.Fxn)om.bind("dsplink.gpp.Global$$getDefines", new Proto.Fxn(om.findStrict("dsplink.gpp.Global.Module", "dsplink.gpp"), $$T_Str, 0, 0, false));
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

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "dsplink/gpp/Global.xs");
        om.bind("dsplink.gpp.Global$$capsule", cap);
        po = (Proto.Obj)om.findStrict("dsplink.gpp.Global.Module", "dsplink.gpp");
        po.init("dsplink.gpp.Global.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("runtimeOS", (Proto)om.findStrict("dsplink.gpp.Global.RuntimeOS", "dsplink.gpp"), om.find("dsplink.gpp.Global.LINUX"), "wh");
                po.addFld("deviceName", (Proto)om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"), $$UNDEF, "wh");
                po.addFld("useProc", $$T_Bool, 1L, "wh");
                po.addFld("usePool", $$T_Bool, 1L, "wh");
                po.addFld("useNotify", $$T_Bool, 1L, "wh");
                po.addFld("useMpcs", $$T_Bool, 1L, "wh");
                po.addFld("useRingio", $$T_Bool, 1L, "wh");
                po.addFld("useMplist", $$T_Bool, 1L, "wh");
                po.addFld("useMsgq", $$T_Bool, 1L, "wh");
                po.addFld("useChnl", $$T_Bool, 1L, "wh");
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("dsplink.gpp.Global$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("dsplink.gpp.Global$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("dsplink.gpp.Global$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("getIncludes", (Proto.Fxn)om.findStrict("dsplink.gpp.Global$$getIncludes", "dsplink.gpp"), Global.get(cap, "getIncludes"));
                po.addFxn("getDefines", (Proto.Fxn)om.findStrict("dsplink.gpp.Global$$getDefines", "dsplink.gpp"), Global.get(cap, "getDefines"));
    }

    void Global$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("dsplink.gpp.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "dsplink.gpp"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "dsplink/gpp/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "dsplink.gpp"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "dsplink.gpp"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "dsplink.gpp"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "dsplink.gpp"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "dsplink.gpp"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "dsplink.gpp"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "dsplink.gpp", Value.DEFAULT, false);
        pkgV.bind("$name", "dsplink.gpp");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "dsplink.gpp.");
        pkgV.bind("$vers", Global.newArray("5, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['dsplink.gpp'];\n");
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

        vo = (Value.Obj)om.findStrict("dsplink.gpp.Global", "dsplink.gpp");
        po = (Proto.Obj)om.findStrict("dsplink.gpp.Global.Module", "dsplink.gpp");
        vo.init2(po, "dsplink.gpp.Global", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("dsplink.gpp.Global$$capsule", "dsplink.gpp"));
        vo.bind("$package", om.findStrict("dsplink.gpp", "dsplink.gpp"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        vo.bind("RuntimeOS", om.findStrict("dsplink.gpp.Global.RuntimeOS", "dsplink.gpp"));
        vo.bind("DeviceName", om.findStrict("dsplink.gpp.Global.DeviceName", "dsplink.gpp"));
        vo.bind("LINUX", om.findStrict("dsplink.gpp.Global.LINUX", "dsplink.gpp"));
        vo.bind("PROS", om.findStrict("dsplink.gpp.Global.PROS", "dsplink.gpp"));
        vo.bind("WINCE", om.findStrict("dsplink.gpp.Global.WINCE", "dsplink.gpp"));
        vo.bind("DAVINCI", om.findStrict("dsplink.gpp.Global.DAVINCI", "dsplink.gpp"));
        vo.bind("DAVINCIHD", om.findStrict("dsplink.gpp.Global.DAVINCIHD", "dsplink.gpp"));
        vo.bind("OMAP3530", om.findStrict("dsplink.gpp.Global.OMAP3530", "dsplink.gpp"));
        vo.bind("OMAP2530", om.findStrict("dsplink.gpp.Global.OMAP2530", "dsplink.gpp"));
        vo.bind("LINUXPC", om.findStrict("dsplink.gpp.Global.LINUXPC", "dsplink.gpp"));
        vo.bind("DA8XX", om.findStrict("dsplink.gpp.Global.DA8XX", "dsplink.gpp"));
        vo.bind("OMAPL1XX", om.findStrict("dsplink.gpp.Global.OMAPL1XX", "dsplink.gpp"));
        vo.bind("DM357", om.findStrict("dsplink.gpp.Global.DM357", "dsplink.gpp"));
        vo.bind("LEO", om.findStrict("dsplink.gpp.Global.LEO", "dsplink.gpp"));
        vo.bind("OMAPL138", om.findStrict("dsplink.gpp.Global.OMAPL138", "dsplink.gpp"));
        vo.bind("DA850", om.findStrict("dsplink.gpp.Global.DA850", "dsplink.gpp"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "dsplink.gpp")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("dsplink.gpp.Global$$instance$static$init", null) ? 1 : 0);
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
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("dsplink.gpp.Global", "dsplink.gpp"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("dsplink.gpp.Global")).bless();
        ((Value.Arr)om.findStrict("$packages", "dsplink.gpp")).add(pkgV);
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
