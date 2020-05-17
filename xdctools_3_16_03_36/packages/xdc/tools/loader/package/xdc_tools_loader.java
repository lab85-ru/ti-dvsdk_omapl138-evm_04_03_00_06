/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-t57
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.*;

public class xdc_tools_loader
{
    static final String VERS = "@(#) xdc-t57\n";

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
        Global.callFxn("loadPackage", xdcO, "xdc.tools");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("xdc.tools.loader.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("xdc.tools.loader", new Value.Obj("xdc.tools.loader", pkgP));
    }

    void Main$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("xdc.tools.loader.Main.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("xdc.tools.loader.Main", new Value.Obj("xdc.tools.loader.Main", po));
        pkgV.bind("Main", vo);
        // decls 
        // insts 
        Object insP = om.bind("xdc.tools.loader.Main.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("xdc.tools.loader.Main$$Object", new Proto.Obj());
        Object objP = om.bind("xdc.tools.loader.Main.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("xdc.tools.loader.Main$$Params", new Proto.Obj());
        om.bind("xdc.tools.loader.Main.Params", new Proto.Str(po, true));
    }

    void Main$$CONSTS()
    {
        // module Main
    }

    void Main$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("xdc.tools.loader.Main$$create", new Proto.Fxn(om.findStrict("xdc.tools.loader.Main.Module", "xdc.tools.loader"), om.findStrict("xdc.tools.loader.Main.Instance", "xdc.tools.loader"), 1, 0, false));
                fxn.addArg(0, "__params", (Proto)om.findStrict("xdc.tools.loader.Main.Params", "xdc.tools.loader"), Global.newObject());
        sb = new StringBuilder();
        sb.append("xdc$tools$loader$Main$$create = function( __params ) {\n");
            sb.append("var __mod = xdc.om['xdc.tools.loader.Main'];\n");
            sb.append("var __inst = xdc.om['xdc.tools.loader.Main.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['xdc.tools.loader']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (__p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, []);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("xdc.tools.loader.Main$$construct", new Proto.Fxn(om.findStrict("xdc.tools.loader.Main.Module", "xdc.tools.loader"), null, 2, 0, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("xdc.tools.loader.Main$$Object", "xdc.tools.loader"), null);
                fxn.addArg(1, "__params", (Proto)om.findStrict("xdc.tools.loader.Main.Params", "xdc.tools.loader"), Global.newObject());
        sb = new StringBuilder();
        sb.append("xdc$tools$loader$Main$$construct = function( __obj, __params ) {\n");
            sb.append("var __mod = xdc.om['xdc.tools.loader.Main'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (__p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void Main$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Main$$SIZES()
    {
    }

    void Main$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "xdc/tools/loader/Main.xs");
        om.bind("xdc.tools.loader.Main$$capsule", cap);
        po = (Proto.Obj)om.findStrict("xdc.tools.loader.Main.Module", "xdc.tools.loader");
        po.init("xdc.tools.loader.Main.Module", om.findStrict("xdc.tools.ICmd.Module", "xdc.tools.loader"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("usage", new Proto.Arr($$T_Str, false), Global.newArray(new Object[]{"prog [args]"}), "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("xdc.tools.loader.Main$$create", "xdc.tools.loader"), Global.get("xdc$tools$loader$Main$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("xdc.tools.loader.Main$$construct", "xdc.tools.loader"), Global.get("xdc$tools$loader$Main$$construct"));
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("xdc.tools.loader.Main$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("xdc.tools.loader.Main$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "instance$meta$init");
                if (fxn != null) om.bind("xdc.tools.loader.Main$$instance$meta$init", true);
                if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("xdc.tools.loader.Main$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "main");
                if (fxn != null) po.addFxn("main", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$main", "xdc.tools.loader"), fxn);
                fxn = Global.get(cap, "exec");
                if (fxn != null) po.addFxn("exec", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$exec", "xdc.tools.loader"), fxn);
        po = (Proto.Obj)om.findStrict("xdc.tools.loader.Main.Instance", "xdc.tools.loader");
        po.init("xdc.tools.loader.Main.Instance", om.findStrict("xdc.tools.ICmd.Instance", "xdc.tools.loader"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "xdc.tools.loader"), fxn);
        po = (Proto.Obj)om.findStrict("xdc.tools.loader.Main$$Params", "xdc.tools.loader");
        po.init("xdc.tools.loader.Main.Params", om.findStrict("xdc.tools.ICmd$$Params", "xdc.tools.loader"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po = (Proto.Obj)om.findStrict("xdc.tools.loader.Main$$Object", "xdc.tools.loader");
        po.init("xdc.tools.loader.Main.Object", om.findStrict("xdc.tools.loader.Main.Instance", "xdc.tools.loader"));
                fxn = Global.get(cap, "run");
                if (fxn != null) po.addFxn("run", (Proto.Fxn)om.findStrict("xdc.tools.ICmd$$run", "xdc.tools.loader"), fxn);
    }

    void Main$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("xdc.tools.loader.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "xdc.tools.loader"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "xdc.tools.loader", Value.DEFAULT, false);
        pkgV.bind("$name", "xdc.tools.loader");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "xdc.tools.loader.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['xdc.tools.loader'];\n");
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

    void Main$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("xdc.tools.loader.Main", "xdc.tools.loader");
        po = (Proto.Obj)om.findStrict("xdc.tools.loader.Main.Module", "xdc.tools.loader");
        vo.init2(po, "xdc.tools.loader.Main", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("xdc.tools.loader.Main$$capsule", "xdc.tools.loader"));
        vo.bind("Instance", om.findStrict("xdc.tools.loader.Main.Instance", "xdc.tools.loader"));
        vo.bind("Params", om.findStrict("xdc.tools.loader.Main.Params", "xdc.tools.loader"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("xdc.tools.loader.Main.Params", "xdc.tools.loader")).newInstance());
        vo.bind("$package", om.findStrict("xdc.tools.loader", "xdc.tools.loader"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "xdc.tools.loader")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("xdc.tools.loader.Main$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("xdc.tools.loader.Main.Object", "xdc.tools.loader"));
        pkgV.bind("Main", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Main");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("xdc.tools.loader.Main", "xdc.tools.loader"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("xdc.tools.loader.Main")).bless();
        ((Value.Arr)om.findStrict("$packages", "xdc.tools.loader")).add(pkgV);
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
        Main$$OBJECTS();
        Main$$CONSTS();
        Main$$CREATES();
        Main$$FUNCTIONS();
        Main$$SIZES();
        Main$$TYPES();
        if (isROV) {
            Main$$ROV();
        }//isROV
        $$SINGLETONS();
        Main$$SINGLETONS();
        $$INITIALIZATION();
    }
}
