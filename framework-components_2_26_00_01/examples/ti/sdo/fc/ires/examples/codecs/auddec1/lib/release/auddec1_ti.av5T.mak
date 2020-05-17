#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = gnu.targets.arm.GCArmv5T{1,0,4.2,0
#
ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T.dep
endif

package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T: auddec1_ti.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T:LD_LIBRARY_PATH=

package/lib/lib/release/auddec1_ti/auddec1_ti.sv5T: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti.sv5T: auddec1_ti.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti.sv5T:LD_LIBRARY_PATH=

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T.dep
endif

package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T: auddec1_ti_ires.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T:LD_LIBRARY_PATH=

package/lib/lib/release/auddec1_ti/auddec1_ti_ires.sv5T: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.sv5T: auddec1_ti_ires.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.sv5T:LD_LIBRARY_PATH=

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T.dep
endif

package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T: | .interfaces
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T:LD_LIBRARY_PATH=

package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.sv5T: | .interfaces
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.sv5T: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/release/auddec1_ti.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm_v5t_le-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=   -DDBC_ASSERTS=0 -DGT_ASSERT=0 -DGT_TRACE=0 -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_2_0 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.sv5T:LD_LIBRARY_PATH=

clean,v5T ::
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T
	-$(RM) package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti.sv5T
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti_ires.sv5T
	-$(RM) package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.sv5T

lib/release/auddec1_ti.av5T: package/lib/lib/release/auddec1_ti/auddec1_ti.ov5T package/lib/lib/release/auddec1_ti/auddec1_ti_ires.ov5T package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.ov5T lib/release/auddec1_ti.av5T.mak

clean::
	-$(RM) lib/release/auddec1_ti.av5T.mak