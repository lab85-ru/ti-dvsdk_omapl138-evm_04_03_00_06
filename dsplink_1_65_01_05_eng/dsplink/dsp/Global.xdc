/*  =========================================================
 *  Dsp Side Global.xdc (RTSC specific file) 
 *
 *  CAUTION! This is a generated file.
 *           All changes will be lost.
 *
 *  This file was generated on MAR 30, 2017  20:12:16
 *  =========================================================
 */


metaonly module Global
{

    string getIncludes() ;
    string getDefines() ;


    enum DeviceName{
	DM6446GEM ,
	DM6467GEM ,
	OMAP3530 ,
	OMAP2530 ,
	DM648 ,
	DM357GEM ,
	DA8XXGEM ,
	OMAPL1XXGEM ,
	DRA44XGEM ,
	DM6437 ,
	DRX416GEM ,
	TNETV107XGEM ,
	DRX45XGEM ,
	OMAPL138GEM ,
	DA850GEM 
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
}
