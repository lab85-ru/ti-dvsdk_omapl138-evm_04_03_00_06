#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.C674{1,0,6.1,14
#
ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale_debug/scale.o674.dep
endif

package/lib/lib/scale_debug/scale.o674: | .interfaces
package/lib/lib/scale_debug/scale.o674: scale.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C   -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale.o674:C_DIR=
package/lib/lib/scale_debug/scale.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_debug/scale.s674: | .interfaces
package/lib/lib/scale_debug/scale.s674: scale.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale.s674:C_DIR=
package/lib/lib/scale_debug/scale.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674.dep
endif

package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674: | .interfaces
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug/package -fr=./package/lib/lib/scale_debug/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug/package -s o674 $< -C   -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug/package -fr=./package/lib/lib/scale_debug/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674:C_DIR=
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.s674: | .interfaces
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.s674: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug/package -fr=./package/lib/lib/scale_debug/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug/package -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug/package -fr=./package/lib/lib/scale_debug/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.s674:C_DIR=
package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale_debug/scale_skel.o674.dep
endif

package/lib/lib/scale_debug/scale_skel.o674: | .interfaces
package/lib/lib/scale_debug/scale_skel.o674: scale_skel.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C   -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale_skel.o674:C_DIR=
package/lib/lib/scale_debug/scale_skel.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_debug/scale_skel.s674: | .interfaces
package/lib/lib/scale_debug/scale_skel.s674: scale_skel.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale_skel.s674:C_DIR=
package/lib/lib/scale_debug/scale_skel.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale_debug/scale_stubs.o674.dep
endif

package/lib/lib/scale_debug/scale_stubs.o674: | .interfaces
package/lib/lib/scale_debug/scale_stubs.o674: scale_stubs.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C   -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale_stubs.o674:C_DIR=
package/lib/lib/scale_debug/scale_stubs.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_debug/scale_stubs.s674: | .interfaces
package/lib/lib/scale_debug/scale_stubs.s674: scale_stubs.c lib/scale_debug.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_debug -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -pden -pds=195  -mv6740 -eo.o674 -ea.s674  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_6_1_14 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale_debug -fr=./package/lib/lib/scale_debug
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_debug/scale_stubs.s674:C_DIR=
package/lib/lib/scale_debug/scale_stubs.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

clean,674 ::
	-$(RM) package/lib/lib/scale_debug/scale.o674
	-$(RM) package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674
	-$(RM) package/lib/lib/scale_debug/scale_skel.o674
	-$(RM) package/lib/lib/scale_debug/scale_stubs.o674
	-$(RM) package/lib/lib/scale_debug/scale.s674
	-$(RM) package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.s674
	-$(RM) package/lib/lib/scale_debug/scale_skel.s674
	-$(RM) package/lib/lib/scale_debug/scale_stubs.s674

lib/scale_debug.a674: package/lib/lib/scale_debug/scale.o674 package/lib/lib/scale_debug/package/package_ti.sdo.ce.examples.extensions.scale.o674 package/lib/lib/scale_debug/scale_skel.o674 package/lib/lib/scale_debug/scale_stubs.o674 lib/scale_debug.a674.mak

clean::
	-$(RM) lib/scale_debug.a674.mak