#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = gnu.targets.arm.GCArmv5T{1,0,4.3,3
#
ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale/scale.ov5T.dep
endif

package/lib/lib/scale/scale.ov5T: | .interfaces
package/lib/lib/scale/scale.ov5T: scale.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale.ov5T:LD_LIBRARY_PATH=

package/lib/lib/scale/scale.sv5T: | .interfaces
package/lib/lib/scale/scale.sv5T: scale.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale.sv5T:LD_LIBRARY_PATH=

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T.dep
endif

package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T: | .interfaces
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T:LD_LIBRARY_PATH=

package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.sv5T: | .interfaces
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.sv5T: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.sv5T:LD_LIBRARY_PATH=

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale/scale_skel.ov5T.dep
endif

package/lib/lib/scale/scale_skel.ov5T: | .interfaces
package/lib/lib/scale/scale_skel.ov5T: scale_skel.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_skel.ov5T:LD_LIBRARY_PATH=

package/lib/lib/scale/scale_skel.sv5T: | .interfaces
package/lib/lib/scale/scale_skel.sv5T: scale_skel.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_skel.sv5T:LD_LIBRARY_PATH=

ifneq (clean,$(MAKECMDGOALS))
-include package/lib/lib/scale/scale_stubs.ov5T.dep
endif

package/lib/lib/scale/scale_stubs.ov5T: | .interfaces
package/lib/lib/scale/scale_stubs.ov5T: scale_stubs.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_stubs.ov5T:LD_LIBRARY_PATH=

package/lib/lib/scale/scale_stubs.sv5T: | .interfaces
package/lib/lib/scale/scale_stubs.sv5T: scale_stubs.c lib/scale.av5T.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clv5T -S $< ...
	$(gnu.targets.arm.GCArmv5T.rootDir)/bin/arm-arago-linux-gnueabi-gcc -c -MD -MF $@.dep -x c -S -fPIC -Wunused -Wall -fno-strict-aliasing  -march=armv5t -Dfar=  -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_4_3_3 -O2  $(XDCINCS)  -o $@ $<
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_stubs.sv5T:LD_LIBRARY_PATH=

clean,v5T ::
	-$(RM) package/lib/lib/scale/scale.ov5T
	-$(RM) package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T
	-$(RM) package/lib/lib/scale/scale_skel.ov5T
	-$(RM) package/lib/lib/scale/scale_stubs.ov5T
	-$(RM) package/lib/lib/scale/scale.sv5T
	-$(RM) package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.sv5T
	-$(RM) package/lib/lib/scale/scale_skel.sv5T
	-$(RM) package/lib/lib/scale/scale_stubs.sv5T

lib/scale.av5T: package/lib/lib/scale/scale.ov5T package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.ov5T package/lib/lib/scale/scale_skel.ov5T package/lib/lib/scale/scale_stubs.ov5T lib/scale.av5T.mak

clean::
	-$(RM) lib/scale.av5T.mak