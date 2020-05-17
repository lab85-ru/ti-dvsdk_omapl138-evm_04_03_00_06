/*  =========================================================
 *  Gpp Side Global.xdc (RTSC specific file) 
 *
 *  CAUTION! This is a generated file.
 *           All changes will be lost.
 *
 *  This file was generated on MAR 30, 2017  20:12:16
 *  =========================================================
 */


metaonly module Global
{
    enum RuntimeOS {
        LINUX,
        PROS,
        WINCE
    };

    config RuntimeOS runtimeOS = LINUX ;

    enum DeviceName{
	DAVINCI ,
	DAVINCIHD ,
	OMAP3530 ,
	OMAP2530 ,
	LINUXPC ,
	DA8XX ,
	OMAPL1XX ,
	DM357 ,
	LEO ,
	OMAPL138 ,
	DA850 
    };

    config DeviceName deviceName  = undefined  ;

    config Bool useProc    = 1 ;
    config Bool usePool    = 1 ;
    config Bool useNotify  = 1 ;
    config Bool useMpcs    = 1 ;
    config Bool useRingio  = 1 ;
    config Bool useMplist  = 1 ;
    config Bool useMsgq    = 1 ;
    config Bool useChnl    = 1 ;
    string getIncludes() ;
    string getDefines() ;


}
