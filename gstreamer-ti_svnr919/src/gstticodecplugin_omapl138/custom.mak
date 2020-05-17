## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: linker.cmd

linker.cmd: \
  package/cfg/gstticodecplugin_omapl138_xv5T.ov5T \
  package/cfg/gstticodecplugin_omapl138_xv5T.xdl
	$(SED) 's"^\"\(package/cfg/gstticodecplugin_omapl138_xv5Tcfg.cmd\)\"$""\"/opt/ti-dvsdk_omapl138-evm_04_03_00_06/gstreamer-ti_svnr919/src/gstticodecplugin_omapl138/\1\""' package/cfg/gstticodecplugin_omapl138_xv5T.xdl > $@
