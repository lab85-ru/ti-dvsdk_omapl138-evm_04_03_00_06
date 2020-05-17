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

public class ti_bios_rov_recaps
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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.bios.rov.recaps.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.bios.rov.recaps", new Value.Obj("ti.bios.rov.recaps", pkgP));
    }

    void Recap$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.rov.recaps.Recap.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.rov.recaps.Recap", new Value.Obj("ti.bios.rov.recaps.Recap", po));
        pkgV.bind("Recap", vo);
        // decls 
    }

    void Recap$$CONSTS()
    {
        // module Recap
    }

    void Recap$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Recap$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Recap.locateRecap
        fxn = (Proto.Fxn)om.bind("ti.bios.rov.recaps.Recap$$locateRecap", new Proto.Fxn(om.findStrict("ti.bios.rov.recaps.Recap.Module", "ti.bios.rov.recaps"), $$T_Str, 1, 1, false));
                fxn.addArg(0, "symTab", (Proto)om.findStrict("xdc.rov.ISymbolTable.Instance", "ti.bios.rov.recaps"), $$UNDEF);
    }

    void Recap$$SIZES()
    {
    }

    void Recap$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/rov/recaps/Recap.xs");
        om.bind("ti.bios.rov.recaps.Recap$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.rov.recaps.Recap.Module", "ti.bios.rov.recaps");
        po.init("ti.bios.rov.recaps.Recap.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                fxn = Global.get(cap, "module$use");
                if (fxn != null) om.bind("ti.bios.rov.recaps.Recap$$module$use", true);
                if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
                fxn = Global.get(cap, "module$meta$init");
                if (fxn != null) om.bind("ti.bios.rov.recaps.Recap$$module$meta$init", true);
                if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
                fxn = Global.get(cap, "module$validate");
                if (fxn != null) om.bind("ti.bios.rov.recaps.Recap$$module$validate", true);
                if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("locateRecap", (Proto.Fxn)om.findStrict("ti.bios.rov.recaps.Recap$$locateRecap", "ti.bios.rov.recaps"), Global.get(cap, "locateRecap"));
    }

    void Recap$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.bios.rov.recaps.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.bios.rov.recaps"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.bios.rov.recaps", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.bios.rov.recaps");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.bios.rov.recaps.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.bios.rov.recaps'];\n");
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

    void Recap$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.rov.recaps.Recap", "ti.bios.rov.recaps");
        po = (Proto.Obj)om.findStrict("ti.bios.rov.recaps.Recap.Module", "ti.bios.rov.recaps");
        vo.init2(po, "ti.bios.rov.recaps.Recap", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.rov.recaps.Recap$$capsule", "ti.bios.rov.recaps"));
        vo.bind("$package", om.findStrict("ti.bios.rov.recaps", "ti.bios.rov.recaps"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios.rov.recaps")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.rov.recaps.Recap$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Recap", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Recap");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.rov.recaps.Recap", "ti.bios.rov.recaps"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.bios.rov.recaps.Recap")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.bios.rov.recaps")).add(pkgV);
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
        Recap$$OBJECTS();
        Recap$$CONSTS();
        Recap$$CREATES();
        Recap$$FUNCTIONS();
        Recap$$SIZES();
        Recap$$TYPES();
        if (isROV) {
            Recap$$ROV();
        }//isROV
        $$SINGLETONS();
        Recap$$SINGLETONS();
        $$INITIALIZATION();
    }
}
