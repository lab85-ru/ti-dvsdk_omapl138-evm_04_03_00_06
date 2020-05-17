#   ============================================================================
#   @file   dsplinkcfg.pl
#
#   @path   $(DSPLINK)/config/bin/
#
#   @desc   Creates configuration file for DSP/BIOS LINK.
#
#   @ver    1.65.01.05_eng
#   ============================================================================
#   Copyright (C) 2002-2009, Texas Instruments Incorporated -
#   http://www.ti.com/
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   ============================================================================


# Include & Require for the perl.
require 5.0           ;
use     strict        ;

my $DSPLINK_BUILDOS ;
my $DIRSEP ;

#   ============================================================================
#   desc    Loader database
#   ============================================================================
my %CFG_LOADER_COFF =
(
# This is displayed on the menu screen
    'NAME'   => 'COFF LOADER',
    'ID'     => 'COFF_LOADER',
# This is displayed on the menu screen
    'DESC'   => 'TI Coff file format loader',
);

my %CFG_LOADER_STATIC =
(
    'NAME'   => 'STATIC LOADER',
    'ID'     => 'STATIC_LOADER',
    'DESC'   =>   "DSP binary loaded automatically with GPP side app\n  \t"
                . "This may not avaiable on certain platforms,\n  \t please check"
                . " release notes for further details",
);


my %CFG_LOADER_BIN =
(
    'NAME'   => 'BIN LOADER',
    'ID'     => 'BIN_LOADER',
    'DESC'   =>   "DSP binary loaded automatically with GPP side app\n  \t"
                . "This may not avaiable on certain platforms,\n  \t please check"
                . " release notes for further details",
);

my %CFG_LOADERS =
(
    '0'  => \%CFG_LOADER_COFF,
    '1'  => \%CFG_LOADER_STATIC,
    '2'  => \%CFG_LOADER_BIN,
) ;


#   ============================================================================
#   desc    Filesystem database
#   ============================================================================
my %CFG_FS_PSEUDO =
(
# This is displayed on the menu screen
    'NAME'   => 'Pseudo Filesystem',
    'ID'     => 'PSEUDOFS',
# This is displayed on the menu screen
    'DESC'   =>    "Read user guide to compile a Pseudo filesytem\n \t"
                 . "and how to build it with dsplink",
);

my %CFG_FS_PRFILE =
(
    'NAME'   => 'PrFile Filesystem',
    'ID'     => 'PRFILEFS',
    'DESC'   => "Read PrFile guide for further details",
);

my %CFG_FSS =
(
    '0'  => \%CFG_FS_PSEUDO,
    '1'  => \%CFG_FS_PRFILE,
) ;


#   ============================================================================
#   desc    GPP OS - database
#   ============================================================================
# Description
# NAME   => Name of the GPP os
# PREFIX => used for generating name of the makefile
# ID     => identifer
# DESC   => Description
# VER    => Version

# Montavista Pro 5.0 Linux with uCLibc Filesystem
my %CFG_GPPOS_MVL5U =
(
    'NAME'   => 'MVL5U',
    'PREFIX' => 'uclibc5.0',
    'ID'     => 'MVL5U',
    'DESC'   => 'Montavista Pro 5.0 Linux + uCLibc Filesystem',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc'
) ;

# Montavista Pro 5.0 Linux with gLibc Filesystem
my %CFG_GPPOS_MVL5G =
(
    'NAME'   => 'MVL5G',
    'PREFIX' => 'mvlpro5.0',
    'ID'     => 'MVL5G',
    'DESC'   => 'Montavista Pro 5.0 Linux + gLibc Filesystem',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => ''
) ;

# Redhat Enterprise Linux 4.0
my %CFG_GPPOS_RHEL4 =
(
    'NAME'   => 'RHEL4',
    'PREFIX' => '2.6',
    'ID'     => 'RHEL4',
    'DESC'   => 'RedHat Enterprise Linux 4.0 (Kernel - 2.6.18)',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# Redhat Linux 9.0
my %CFG_GPPOS_RHL9 =
(
    'NAME'   => 'RHL9',
    'PREFIX' => '2.4',
    'ID'     => 'RHL9',
    'DESC'   => 'RedHat Linux 9.0 (Kernel - 2.4.20)',
    'VER'    => '2.4.20',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# OMAPLSP
my %CFG_GPPOS_OMAPLSP =
(
    'NAME'   => 'OMAPLSP',
    'PREFIX' => '2.6',
    'ID'     => 'OMAPLSP',
    'DESC'   => 'OMAP LSP for OMAP2530 and OMAP3530',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# DM6467 LSP
my %CFG_GPPOS_DM6467LSP =
(
    'NAME'   => 'DM6467LSP',
    'PREFIX' => '2.6',
    'ID'     => 'DM6467LSP',
    'DESC'   => 'DM6467 LSP for DM6467',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# DM6467 Linux with uCLibc filesystem
my %CFG_GPPOS_DM6467LSPuc =
(
    'NAME'   => 'DM6467LSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'DM6467LSPuc',
    'DESC'   => 'DM6467 LSP for DM6467 + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;


# DM6446 LSP
my %CFG_GPPOS_DM6446LSP =
(
    'NAME'   => 'DM6446LSP',
    'PREFIX' => '2.6',
    'ID'     => 'DM6446LSP',
    'DESC'   => 'DM6446 LSP for DM6446',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# DM6446 Linux with uCLibc filesystem
my %CFG_GPPOS_DM6446LSPuc =
(
    'NAME'   => 'DM6446LSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'DM6446LSPuc',
    'DESC'   => 'DM6446 LSP for DM6446 + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;


# OMAPL138 Linux with uCLibc filesystem
my %CFG_GPPOS_OMAPL138LSPuc =
(
    'NAME'   => 'OMAPL138LSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'OMAPL138LSPuc',
    'DESC'   => 'OMAPL138 LSP for DM6467 + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;

# DA850 Linux with uCLibc filesystem
my %CFG_GPPOS_DA850_LSPuc =
(
    'NAME'   => 'DA850LSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'DA850LSPuc',
    'DESC'   => 'DA850 LSP for DM6467 + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;


# OMAP Linux with uCLibc filesystem
my %CFG_GPPOS_OMAPLSPuc =
(
    'NAME'   => 'OMAPLSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'OMAPLSPuc',
    'DESC'   => 'OMAP LSP for OMAP2530 and OMAP3530 + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;

# DA8XX Linux with uCLibc filesystem
my %CFG_GPPOS_DA8XXLSPuc =
(
    'NAME'   => 'DA8XXLSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'DA8XXLSPuc',
    'DESC'   => 'DA8XX LSP for DA8XX + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;

# OMAPL1XX Linux with uCLibc filesystem
my %CFG_GPPOS_OMAPL1XXLSPuc =
(
    'NAME'   => 'OMAPL1XXLSPuc',
    'PREFIX' => 'uclibc',
    'ID'     => 'OMAPL1XXLSPuc',
    'DESC'   => 'OMAPL1XX LSP for OMAPL1XX + uCLibc Filesystem ',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => 'uc',
) ;

# WINCE GPPOS
my %CFG_GPPOS_WINCE =
(
    'NAME'   => 'WINCE',
    'PREFIX' => 'wince6.0',
    'ID'     => 'WINCE',
    'DESC'   => 'WinCE OS 6.0',
    'VER'    => '6.0',
    'TYPE'   => 'WinCE',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
) ;

# PrKernel
my %CFG_GPPOS_PROS =
(
    'NAME'   => 'PROS',
    'PREFIX' => '',
    'ID'     => 'PROS',
    'DESC'   => 'PrKernel 4.0',
    'VER'    => '4.0',
    'TYPE'   => 'PrOS',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'FS'     => \%CFG_FSS,
    'COMPS'  => 'ponslrm',
    'FSS'    => [\%CFG_FS_PSEUDO, \%CFG_FS_PRFILE],
) ;

# GIT LSP for OMAPL138/DA850
my %CFG_GPPOS_ARM =
(
    'NAME'   => 'ARM',
    'PREFIX' => 'arm',
    'ID'     => 'ARM',
    'DESC'   => 'GIT LSP for OMAPL138/DA850 and ARM toolchain',
    'VER'    => '2.6.18',
    'TYPE'   => 'Linux',
    'LOADERS'=> [\%CFG_LOADER_COFF, ],
    'COMPS'  => 'ponslrmc',
    'OSPREFIX' => ''
) ;

# Global Database of GPP OSes
my %CFG_GPPOS =
(
    '0' => \%CFG_GPPOS_MVL5U,
    '1' => \%CFG_GPPOS_MVL5G,
    '2' => \%CFG_GPPOS_RHEL4,
    '3' => \%CFG_GPPOS_RHL9,
    '4' => \%CFG_GPPOS_OMAPLSP,
    '5' => \%CFG_GPPOS_OMAPLSPuc,
    '6' => \%CFG_GPPOS_PROS,
    '7' => \%CFG_GPPOS_ARM,
    '8' => \%CFG_GPPOS_WINCE,
) ;


#   ============================================================================
#   desc    DSP OSes - database
#   ============================================================================
# Description
# NAME   => Name of the DSP os
# PREFIX => used for generating name of the makefile
# ID     => identifer
# DESC   => Description
# VER    => Version

# DSP/BIOS (TM) 5.XX
my %CFG_DSPOS_5XX =
(
    'NAME'   => 'DSPBIOS5XX',
    'PREFIX' => '5.XX',
    'ID'     => 'DSPBIOS5XX',
    'DESC'   => 'DSP/BIOS (TM) Version 5.XX',
    'VER'    => '5.XX',
    'TYPE'   => 'DspBios',

) ;

my %CFG_DSPOS_6XX =
(
    'NAME'   => 'DSPBIOS6XX',
    'PREFIX' => '6.XX',
    'ID'     => 'DSPBIOS6XX',
    'DESC'   => 'DSP/BIOS (TM) Version 6.XX',
    'VER'    => '6.XX',
    'TYPE'   => 'DspBios',
) ;

# Global Database of DSP OSes
my %CFG_DSPOS =
(
    '0' => \%CFG_DSPOS_5XX,
    '1' => \%CFG_DSPOS_6XX,
) ;


#   ============================================================================
#   desc    PHYSICAL Interface - database
#   ============================================================================
# Description
# ID     => Identifier
# DESC   => Description about the Physical Interface
# DEV    => Target Device connect by this interface

my %CFG_DSPCFG_DM6446GEMSHMEM =
(
#phy defines
    'PHYID'     => 'DM6446GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_DM6446LSP, \%CFG_GPPOS_DM6446LSPuc],
    'DSPOS'     => [\%CFG_DSPOS_5XX, \%CFG_DSPOS_6XX],
# DSP defines
    'DSPNAME'   => 'DM6446GEM',
    'DSPPREFIX' => 'DM6446GEM',
    'DSPID'     => 'DM6446GEM',
    'DSPDESC'   => 'On-Chip DSP of DaVinci SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DM6467GEMSHMEM =
(
#phy defines
    'PHYID'     => 'DM6467GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_DM6467LSP, \%CFG_GPPOS_DM6467LSPuc ],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM6467GEM',
    'DSPPREFIX' => 'DM6467GEM',
    'DSPID'     => 'DM6467GEM',
    'DSPDESC'   => 'On-Chip DSP of DaVinciHD SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;


my %CFG_DSPCFG_OMAP3530SHMEM =
(
#phy defines
    'PHYID'     => 'OMAP3530SHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_OMAPLSP, \%CFG_GPPOS_OMAPLSPuc,],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'OMAP3530',
    'DSPPREFIX' => 'OMAP3530',
    'DSPID'     => 'OMAP3530',
    'DSPDESC'   => 'On-Chip DSP of OMAP 3530 SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_OMAP2530SHMEM =
(
#phy defines
    'PHYID'     => 'OMAP2530SHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_OMAPLSP, \%CFG_GPPOS_OMAPLSPuc],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'OMAP2530',
    'DSPPREFIX' => 'OMAP2530',
    'DSPID'     => 'OMAP2530',
    'DSPDESC'   => 'On-Chip DSP of OMAP 2530 SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DM642PCI =
(
#phy defines
    'PHYID'     => 'DM642PCI',
    'PHYDESC'   => 'PCI Physical Interface',
    'PHYTYPE'   => 'PCI',
    'GPPOS'     => [\%CFG_GPPOS_RHEL4],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM642',
    'DSPPREFIX' => 'DM642',
    'DSPID'     => 'DM642',
    'DSPDESC'   => 'DM642 - PCI based DSP',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DM6437PCI =
(
#phy defines
    'PHYID'     => 'DM6437PCI',
    'PHYDESC'   => 'PCI Physical Interface',
    'PHYTYPE'   => 'PCI',
    'GPPOS'     => [\%CFG_GPPOS_RHEL4],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM6437',
    'DSPPREFIX' => 'DM6437',
    'DSPID'     => 'DM6437',
    'DSPDESC'   => 'DM6437 - PCI based DSP',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;


my %CFG_DSPCFG_DM648PCI =
(
#phy defines
    'PHYID'     => 'DM648PCI',
    'PHYDESC'   => 'PCI Physical Interface',
    'PHYTYPE'   => 'PCI',
    'GPPOS'     => [\%CFG_GPPOS_RHEL4],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM648',
    'DSPPREFIX' => 'DM648',
    'DSPID'     => 'DM648',
    'DSPDESC'   => 'DM648 - PCI based DSP',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DM357GEMSHMEM =
(
#phy defines
    'PHYID'     => 'DM357GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_MVL5U, \%CFG_GPPOS_MVL5G],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM357GEM',
    'DSPPREFIX' => 'DM357GEM',
    'DSPID'     => 'DM6446GEM',
    'DSPDESC'   => 'On-Chip DSP of DM357 SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DA8XXGEMSHMEM =
(
#phy defines
    'PHYID'     => 'DA8XXGEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_ARM, \%CFG_GPPOS_DA8XXLSPuc],
    'DSPOS'     => [\%CFG_DSPOS_6XX],
# DSP defines
    'DSPNAME'   => 'DA8XXGEM',
    'DSPPREFIX' => 'DA8XXGEM',
    'DSPID'     => 'DA8XXGEM',
    'DSPDESC'   => 'On-Chip DSP of Primus SoC',
    'DSPCLASS'  => 'C674X',
    'DSPTYPE'   => 'C674X',
) ;

my %CFG_DSPCFG_OMAPL1XXGEMSHMEM =
(
#phy defines
    'PHYID'     => 'OMAPL1XXGEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_ARM, \%CFG_GPPOS_OMAPL1XXLSPuc],
    'DSPOS'     => [\%CFG_DSPOS_5XX],
# DSP defines
    'DSPNAME'   => 'OMAPL1XXGEM',
    'DSPPREFIX' => 'OMAPL1XXGEM',
    'DSPID'     => 'DA8XXGEM',
    'DSPDESC'   => 'On-Chip DSP of Primus SoC',
    'DSPCLASS'  => 'C674X',
    'DSPTYPE'   => 'C674X',
) ;

my %CFG_DSPCFG_DRA44XGEMSHMEM =
(
#phy defines
    'PHYID'     => 'DRA44XGEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_PROS,],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DRA44XGEM',
    'DSPPREFIX' => 'DRA44XGEM',
    'DSPID'     => 'DRA44XGEM',
    'DSPDESC'   => 'On-Chip DSP of Jacinto1 SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DRX416GEMSHMEM =
(
#phy defines
    'PHYID'     => 'DRX416GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_PROS,],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DRX416GEM',
    'DSPPREFIX' => 'DRX416GEM',
    'DSPID'     => 'DRX416GEM',
    'DSPDESC'   => 'On-Chip DSP of Jacinto2 SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;


my %CFG_DSPCFG_DM6437VLYNQ =
(
#phy defines
    'PHYID'     => 'DM6437VLYNQ',
    'PHYDESC'   => 'VLYNQ Physical Interface',
    'PHYTYPE'   => 'VLYNQ',
    'GPPOS'     => [\%CFG_GPPOS_PROS, ],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DM6437',
    'DSPPREFIX' => 'DM6437',
    'DSPID'     => 'DM6437',
    'DSPDESC'   => 'DM6437 connect to host via VLYNQ',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_DRX45XGEMSHMEM =
(
#phy defines
    'PHYID'     => 'DRX45XGEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_PROS,],
    'DSPOS'     => [\%CFG_DSPOS_5XX, ],
# DSP defines
    'DSPNAME'   => 'DRX45XGEM',
    'DSPPREFIX' => 'DRX45XGEM',
    'DSPID'     => 'DRA44XGEM',
    'DSPDESC'   => 'On-Chip DSP of DRX45X SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;

my %CFG_DSPCFG_OMAPL138GEMSHMEM =
(
#phy defines
    'PHYID'     => 'OMAPL138GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_ARM, \%CFG_GPPOS_OMAPL138LSPuc],
    'DSPOS'     => [\%CFG_DSPOS_5XX],
# DSP defines
    'DSPNAME'   => 'OMAPL138GEM',
    'DSPPREFIX' => 'OMAPL138GEM',
    'DSPID'     => 'DA8XXGEM',
    'DSPDESC'   => 'On-Chip DSP of Primus SoC',
    'DSPCLASS'  => 'C674X',
    'DSPTYPE'   => 'C674X',
) ;

my %CFG_DSPCFG_DA850GEMSHMEM =
(
#phy defines
    'PHYID'     => 'DA850GEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_ARM, \%CFG_GPPOS_DA850_LSPuc],
    'DSPOS'     => [\%CFG_DSPOS_6XX],
# DSP defines
    'DSPNAME'   => 'DA850GEM',
    'DSPPREFIX' => 'DA850GEM',
    'DSPID'     => 'DA8XXGEM',
    'DSPDESC'   => 'On-Chip DSP of Primus SoC',
    'DSPCLASS'  => 'C674X',
    'DSPTYPE'   => 'C674X',
) ;
my %CFG_DSPCFG_TNETV107XGEMSHMEM =
(
#phy defines
    'PHYID'     => 'TNETV107XGEMSHMEM',
    'PHYDESC'   => 'Shared Memory Physical Interface',
    'PHYTYPE'   => 'SHMEM',
    'GPPOS'     => [\%CFG_GPPOS_MVL5G, \%CFG_GPPOS_WINCE],
    'DSPOS'     => [\%CFG_DSPOS_5XX, \%CFG_DSPOS_6XX],
# DSP defines
    'DSPNAME'   => 'TNETV107XGEM',
    'DSPPREFIX' => 'TNETV107XGEM',
    'DSPID'     => 'TNETV107XGEM',
    'DSPDESC'   => 'On-Chip DSP of Leo SoC',
    'DSPCLASS'  => 'C64XX',
    'DSPTYPE'   => 'C64XXP',
) ;



my %CFG_DSPCFGS =
(
    '0'      => \%CFG_DSPCFG_DM6446GEMSHMEM,
    '1'      => \%CFG_DSPCFG_DM6467GEMSHMEM,
    '2'      => \%CFG_DSPCFG_OMAP3530SHMEM,
    '3'      => \%CFG_DSPCFG_OMAP2530SHMEM,
    '4'      => \%CFG_DSPCFG_DM6437PCI,
    '5'      => \%CFG_DSPCFG_DM648PCI,
    '6'      => \%CFG_DSPCFG_DM357GEMSHMEM,
    '7'      => \%CFG_DSPCFG_DA8XXGEMSHMEM,
    '8'      => \%CFG_DSPCFG_OMAPL1XXGEMSHMEM,
    '9'      => \%CFG_DSPCFG_DRA44XGEMSHMEM,
    '10'     => \%CFG_DSPCFG_DM6437VLYNQ,
    '11'     => \%CFG_DSPCFG_DRX416GEMSHMEM,
    '12'     => \%CFG_DSPCFG_TNETV107XGEMSHMEM,
    '13'     => \%CFG_DSPCFG_DRX45XGEMSHMEM,
    '14'     => \%CFG_DSPCFG_OMAPL138GEMSHMEM,
    '15'     => \%CFG_DSPCFG_DA850GEMSHMEM,
) ;

#   ============================================================================
#   desc    Base Platform - database
#   ============================================================================
my %CFG_PLATFORM_DAVINCI =
(
    'NAME'   => 'DAVINCI',
    'ID'     => 'DAVINCI',
    'PREFIX' => 'davinci',
    'DESC'   =>   "DaVinci SoC - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DM6446GEMSHMEM,
                   ],
) ;

my %CFG_PLATFORM_DAVINCIHD =
(
    'NAME'   => 'DAVINCIHD',
    'ID'     => 'DAVINCIHD',
    'PREFIX' => 'davincihd',
    'DESC'   =>   "DaVinciHD SoC - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DM6467GEMSHMEM,
                   ],
) ;



my %CFG_PLATFORM_OMAP3530 =
(
    'NAME'   => 'OMAP3530',
    'ID'     => 'OMAP3530',
    'PREFIX' => 'omap3530',
    'DESC'   =>   "OMAP3530 SoC - C64P DSP interfaced directly to ARM Cortex-A8\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_OMAP3530SHMEM,
                   ],
) ;
my %CFG_PLATFORM_OMAP2530 =
(
    'NAME'   => 'OMAP2530',
    'ID'     => 'OMAP2530',
    'PREFIX' => 'omap2530',
    'DESC'   =>   "OMAP2530 SoC - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'  => [
                    \%CFG_DSPCFG_OMAP2530SHMEM,
                  ],
) ;

my %CFG_PLATFORM_LINUXPC =
(
    'NAME'   => 'Linux Box (PC)',
    'ID'     => 'LINUXPC',
    'PREFIX' => 'linuxpc',
    'DESC'   =>   "Linux Box (PC) with PCI based cards\n \t"
                . "This platform support multi DSP (PCI cards) architecture",
    'NUMINT' => 1,
    'GPPDEVICE' => 'PC',
    'DSPCFGS'   => [
                    \%CFG_DSPCFG_DM6437PCI,
                    \%CFG_DSPCFG_DM648PCI
                   ],
) ;

my %CFG_PLATFORM_DM357 =
(
    'NAME'   => 'DM357',
    'ID'     => 'DM357',
    'PREFIX' => 'dm357',
    'DESC'   =>   "DM357 SoC - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DM357GEMSHMEM,
                   ],
) ;

my %CFG_PLATFORM_DA8XX =
(
    'NAME'   => 'DA8XX',
    'ID'     => 'DA8XX',
    'PREFIX' => 'da8xx',
    'DESC'   =>   "DA8XX SoC - C674x DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                  \%CFG_DSPCFG_DA8XXGEMSHMEM,
                ],
) ;

my %CFG_PLATFORM_OMAPL1XX =
(
    'NAME'   => 'OMAPL1XX',
    'ID'     => 'OMAPL1XX',
    'PREFIX' => 'omapl1xx',
    'DESC'   =>   "OMAP-L1XX SoC - C674x DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                  \%CFG_DSPCFG_OMAPL1XXGEMSHMEM,
                ],
) ;

my %CFG_PLATFORM_JACINTO1 =
(
    'NAME'   => 'JACINTO1',
    'ID'     => 'JACINTO1',
    'PREFIX' => 'jacinto1',
    'DESC'   =>   "Jacinto SoC version 1 - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform can connect to a DM6437 device via VLYNQ",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DRA44XGEMSHMEM,
                     \%CFG_DSPCFG_DM6437VLYNQ,
                   ],
) ;

my %CFG_PLATFORM_JACINTO2 =
(
    'NAME'   => 'JACINTO2',
    'ID'     => 'JACINTO2',
    'PREFIX' => 'jacinto2',
    'DESC'   =>   "Jacinto SoC Version 2 - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DRX416GEMSHMEM,
                   ],
) ;

my %CFG_PLATFORM_LEO =
(
    'NAME'   => 'LEO',
    'ID'     => 'LEO',
    'PREFIX' => 'leo',
    'DESC'   =>   "LEO SoC - C64P DSP interfaced directly to ARM11\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_TNETV107XGEMSHMEM,
                   ],
) ;
my %CFG_PLATFORM_DRX45X =
(
    'NAME'   => 'DRX45X',
    'ID'     => 'DRX45X',
    'PREFIX' => 'drx45x',
    'DESC'   =>   "DRX45X SoC version 1 - C64P DSP interfaced directly to ARM9\n  \t"
                . "This platform can connect to a DM6437 device via VLYNQ",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                     \%CFG_DSPCFG_DRX45XGEMSHMEM,
                     \%CFG_DSPCFG_DM6437VLYNQ,
                   ],
) ;

my %CFG_PLATFORM_OMAPL138 =
(
    'NAME'   => 'OMAPL138',
    'ID'     => 'OMAPL138',
    'PREFIX' => 'omapl138',
    'DESC'   =>   "OMAP-L138 SoC - C674x DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                  \%CFG_DSPCFG_OMAPL138GEMSHMEM,
                ],
) ;

my %CFG_PLATFORM_DA850 =
(
    'NAME'   => 'DA850',
    'ID'     => 'DA850',
    'PREFIX' => 'da850',
    'DESC'   =>   "DA850 SoC - C674x DSP interfaced directly to ARM9\n  \t"
                . "This platform does not support multi DSP scenario",
    'GPPDEVICE' => 'ARM',
    'DSPCFGS'   => [
                  \%CFG_DSPCFG_DA850GEMSHMEM,
                ],
) ;

# Global database of Platforms.
my %CFG_PLATFORMS =
(
    '0'   => \%CFG_PLATFORM_DAVINCI,
    '1'   => \%CFG_PLATFORM_DAVINCIHD,
    '2'   => \%CFG_PLATFORM_OMAP3530,
    '3'   => \%CFG_PLATFORM_OMAP2530,
    '4'   => \%CFG_PLATFORM_LINUXPC,
    '5'   => \%CFG_PLATFORM_DA8XX,
    '6'   => \%CFG_PLATFORM_OMAPL1XX,
    '7'   => \%CFG_PLATFORM_DM357,
    '8'   => \%CFG_PLATFORM_LEO,
    '9'   => \%CFG_PLATFORM_OMAPL138,
    '10'  => \%CFG_PLATFORM_DA850,
) ;

# Configured Information
my %CFG_CHOSEN =
(
) ;


#   ============================================================================
#   func    IO_getYesOrNo
#
#   desc    Function to read yes or no from user
#   ============================================================================
sub IO_getYesOrNo {
    my $str  = shift (@_) ;
    my $temp = "" ;
REPEAT:
    print "  ". $str . " " . $temp . ":" ;
    my $ans = <STDIN>     ;
    chomp ($ans) ;

    if ($ans eq 'y' || $ans eq 'Y') {
        return 'TRUE'  ;
    }
    elsif ($ans eq 'n' || $ans eq 'N') {
        return 'FALSE' ;
    }
    else {
        $temp = "{ Options : [yY|nN] } " ;
        goto REPEAT ;
    }
}


#   ============================================================================
#   func    ClearScreen
#
#   desc    Clear window
#   ============================================================================
sub ClearScreen {
    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        system ("cls") ;
    }
    else {
        system ("clear") ;
    }
}

#   ============================================================================
#   func    GetDateString
#
#   desc    Returns current date as string
#   ============================================================================
sub GetDateString
{
    my (@date)      = (localtime)[0..5] ;
    my (@month)     = ("JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                       "JUL", "AUG", "SEP", "OCT", "NOV", "DEC") ;
    my ($datestr)   = $month [$date [4]] . " "  .
                      $date [3]          . ", " .
                      ($date [5] + 1900) . "  " .
                      $date [2]          . ":"  .
                      $date [1]          . ":"  ;

    if ($date [0] < 10)  {
        $datestr .= "0" ;
    }

    $datestr .= $date [0] ;
    return $datestr ;
}


#   ============================================================================
#   func    GetCfgFile
#
#   desc    Returns the full path to configuration file
#   ============================================================================
sub GetCfgFile
{
    my ($str) ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        if (!(-e $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\")) {
            system ("mkdir " . $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\") ;
        }
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\CURRENTCFG.MK" ;
    }
    else
    {
        if (!(-e $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/")) {
            system ("mkdir -p " . $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/") ;
        }
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/CURRENTCFG.MK" ;
    }

    return $str ;
}


#   ============================================================================
#   func    GetMkFile
#
#   desc    Returns the full path to configuration file
#   ============================================================================
sub GetMkFile
{
    my ($str) ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "\\etc\\host\\scripts\\msdos\\multimake.bat" ;
    }
    else
    {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "/etc/host/scripts/Linux/multimake.sh" ;
    }

    return $str ;
}


#   ============================================================================
#   func    GetSystemCFile
#
#   desc    Returns the full path to system c file
#   ============================================================================
sub GetSystemCFile
{
    my ($str) ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        if (!(-e $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\")) {
            system ("mkdir " . $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\") ;
        }
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "\\config\\BUILD\\CFG_system.c" ;
    }
    else
    {
        if (!(-e $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/")) {
            system ("mkdir -p " . $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/") ;
        }
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "/config/BUILD/CFG_system.c" ;
    }

    return $str ;
}


#   ============================================================================
#   func    GetMultiMakeFile
#
#   desc    Returns the full path to Multimake script file
#   ============================================================================
sub GetMultiMakeFile
{
    my ($str) ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "\\etc\\host\\scripts\\msdos\\multimake.bat" ;
    }
    else
    {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "/etc/host/scripts/Linux/multimake.sh" ;
    }

    return $str ;
}


#   ============================================================================
#   func    GetRootDir
#
#   desc    Returns the full path to specified root directory
#   ============================================================================
sub GetRootDir
{
    my ($dir) = @_ ;
    my ($str) ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "\\" . $dir ;
    }
    else
    {
        $str = $CFG_CHOSEN{'DSPLINKENV'} . "/" . $dir ;
    }

    return $str ;
}

#   ============================================================================
#   func    GetGppMakeFile
#
#   desc    Returns the full path to GPP side make file
#   ============================================================================
sub GetGppMakeFile
{
    my %platform = %{(shift)} ;
    my %gppos    = %{(shift)} ;
    my ($str) ;

    if ($platform {'ID'} eq "LINUXPC") {
        $str = $platform {'PREFIX'} . $gppos{'PREFIX'} ;
    }
    elsif ($gppos {'ID'} eq "PROS") {
        $str = $platform {'PREFIX'}  ;
    }
    else {
        $str = $platform {'PREFIX'} . "_" . $gppos {'PREFIX'}  ;
    }

    return $str ;
}


#   ============================================================================
#   func    GetDspMakeFile
#
#   desc    Returns the full path to DSP side make file
#   ============================================================================
sub GetDspMakeFile
{
    my %dsp    = %{(shift)} ;
    my %dspos  = %{(shift)} ;
    my ($str) ;

    $str = $dsp {'DSPTYPE'} ;
    $str =~ tr/A-Z/a-z/ ;
    $str .= "_" . lc($dspos{'PREFIX'})  ;
    if (   $DSPLINK_BUILDOS eq 'LINUX'
        || $DSPLINK_BUILDOS eq 'CYGWIN') {
        $str .= "_linux" ;
    }
    if (   $DSPLINK_BUILDOS eq 'WINDOWS'
        || $DSPLINK_BUILDOS eq 'MSWIN32') {
        $str .= "_windows" ;
    }

    return $str ;
}


#   ============================================================================
#   desc    Check for HOST os
#   ============================================================================
$DSPLINK_BUILDOS = uc($^O) ;
if (   $DSPLINK_BUILDOS eq 'LINUX'
    || $DSPLINK_BUILDOS eq 'WINDOWS'
    || $DSPLINK_BUILDOS eq 'WINDOWS_NT'
    || $DSPLINK_BUILDOS eq 'CYGWIN'
    || $DSPLINK_BUILDOS eq 'MSWIN32') {
    if (($DSPLINK_BUILDOS eq 'MSWIN32') ||
        ($DSPLINK_BUILDOS eq 'WINDOWS_NT')) {
        $DSPLINK_BUILDOS = 'WINDOWS' ;
        $DIRSEP = "\\" ;
    }
    else {
        $DIRSEP = "/" ;
    }
}
else {
    print "  Unknown Build OS " . $DSPLINK_BUILDOS . ".\n" ;
    print "  And this os is not supported by DSPLINK.\n" ;
    print "  Exiting...\n" ;
    exit (3) ;
}


#   ============================================================================
#   desc    Show Welcome banner
#   ============================================================================
#
&ClearScreen () ;
print "  Welcome to DSP/BIOS(TM) Link Configuration Utility\n" ;
print "------------------------------------------------------\n" ;
print "\n  !!DSPLINK will be configured for Build OS: " ;
print $DSPLINK_BUILDOS . "!!\n\n" ;

# Check for DSPLINK enviroment variable
my $DSPLINK_ENV = $ENV {'DSPLINK'} ;
if ($DSPLINK_ENV eq "") {
    print "  DSPLINK variable is not set!!\n" ;
    print "  Please set it:\n" ;
    print "\tOn Windows:\n\t\tset DSPLINK=<path to DSPLINK base directory>\n" ;
    print "\tOn Linux:\n\t\texport DSPLINK=<path to DSPLINK base directory>\n";
    exit (1) ;
}
else {
    print "  !!DSPLINK Directory : " . $DSPLINK_ENV . "!!\n\n" ;
    $CFG_CHOSEN{'DSPLINKENV'} = $DSPLINK_ENV ;
}

#   ============================================================================
#   func    CFG_getPlatformArg
#
#   desc    Function to parse platform input
#   ============================================================================
sub CFG_getPlatformArg {
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--platform/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            goto SUCCESS ;
        }
    }
# No match print error with help msg
    goto ERROR ;

SUCCESS:
    for (my $key = 0 ; $key < (keys (%CFG_PLATFORMS)) ; $key += 1) {
        if ($CFG_PLATFORMS {$key}{'ID'} eq $value) {
            print "\nChosen platform:\n\tIdentifier:\t" . $CFG_PLATFORMS {$key}{'ID'} . "\n" ;
            print "\tDescription:\t" . $CFG_PLATFORMS {$key}{'DESC'} . "\n" ;
            return (%{$CFG_PLATFORMS {$key}}) ;
        }
    }

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid Platform!\n" ;
    print "Following platform are supported currently:\n" ;
    for (my $key = 0 ; $key < (keys (%CFG_PLATFORMS)) ; $key += 1) {
        print "ID" . "-->" . $CFG_PLATFORMS {$key}{'ID'} . "\n" ;
        print "\t" . $CFG_PLATFORMS {$key}{'DESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --platform=DAVINCI or --platform=<ID> or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getNoDspsArg
#
#   desc    Function to parse no DSPs input
#   ============================================================================
sub CFG_getNoDspsArg {
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--nodsp/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if ($value > 0) {
                print "\nNo of DSPs:\t" . ${value} . "\n" ;
                return $value ;
            }
        }
    }

# No match print error with help msg
    goto ERROR ;

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid number of DSPs!\n" ;
    print "Please provide number of DSPs in the system\n" ;
    print "Provided: " . $value . "\n" ;
    die "Example: --nodsp=2 or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getDspCfgArg
#
#   desc    Function to parse DSP Config value
#   ============================================================================
sub CFG_getDspCfgArg {
    my $key ;
    my $dspNo    = shift ;
    my %platform = %{(shift)} ;
    my @phys     = @{$platform {'DSPCFGS'}} ;
    my $str      = sprintf ("--dspcfg_%d", $dspNo) ;
    my ($cmd, $value, $temp, $value1) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/($str)/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            ($temp, $value1) = split (/_/, $cmd) ;
            if ($value1 eq $dspNo) {
                goto SUCCESS ;
            }
        }
    }

#No match print error with help msg
    goto ERROR ;

SUCCESS:
    for (my $key = 0 ; $key < (scalar (@phys)) ; $key += 1) {
        if ($phys [$key]{'PHYID'} eq $value) {
            print "\nChosen combination for DSP" . $dspNo . ":\n" ;
            print "\t  Identifier              :\t" . $phys [$key]{'DSPID'} . "\n" ;
            print "\t  DSP Description         :\t" . $phys [$key]{'DSPDESC'} . "\n" ;
            print "\t  Physical Interface (PHY):\t" . $value . "\n" ;
            print "\t  PHY Description         :\t" . $phys [$key]{'PHYDESC'} . "\n" ;
            return (%{$phys [$key]});
        }
    }

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid DSP for DSP" . $dspNo . " with a valid Physical Interface combination!\n" ;
    print "Following DSP & Physical interface (PHY) combinations are supported by " . $platform{'ID'} . ":\n" ;
    for (my $key = 0 ; $key < (scalar (@phys)) ; $key += 1) {
        print "<ID>" . "-->" . $phys [$key]{'PHYID'} . "\n" ;
        print "\tDSP Name: " . $phys [$key]{'DSPID'} . "\n" ;
        print "\tPHY Name: " . $phys [$key]{'PHYDESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --dspcfg_" . $dspNo . "=<ID> or --dspcfg_" . $dspNo . "=" . $phys [0]{'PHYID'} . " or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getDspOsArg
#
#   desc    Function to parse DSP's OS input
#   ============================================================================
sub CFG_getDspOsArg {
    my $key ;
    my $dspNo    = shift ;
    my %platform = %{(shift)} ;
    my %dspcfg    = %{(shift)} ;
    my $str      = sprintf ("--dspos_%d", $dspNo) ;
    my ($cmd, $value, $temp, $value1, %dspos) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/($str)/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            ($temp, $value1) = split (/_/, $cmd) ;
            if ($value1 eq $dspNo) {
                goto SUCCESS ;
            }
        }
    }

#No match, print error with help msg
goto ERROR ;

SUCCESS:
    my @dsposes = @{$dspcfg {'DSPOS'}} ;
    for (my $key = 0 ; $key < (scalar (@dsposes)) ; $key += 1) {
        if ($dsposes [$key]{'ID'} eq $value) {
            %dspos = %{$dsposes [$key]} ;
            print "\nChosen DSP OS for DSP" . $dspNo. ":\n\t  Identifier:\t" . $value . "\n" ;
            print "\t  Description:\t" . $dspos {'DESC'} . "\n" ;
            return (%dspos) ;
        }
    }

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid DSP OS!\n" ;
    print "Following DSP OS are supported by " . $dspcfg{'DSPID'} . " with " . $dspcfg{'PHYDESC'} . ":\n" ;
    my @dsposes = @{$dspcfg{'DSPOS'}} ;
    for (my $key = 0 ; $key < (scalar (@dsposes)) ; $key += 1) {
        print "<ID>" . "-->" . $dsposes [$key]{'ID'} . "\n" ;
        print "\t" . $dsposes [$key]{'DESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --dspos_" . $dspNo . "=<ID> or --dspos_" . $dspNo . "=" . $dsposes [0]{'ID'} . " or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getGppOsArg
#
#   desc    Function to parse GPP's OS input
#   ============================================================================
sub CFG_getGppOsArg {
    my $key ;
    my (%platform) = %{(shift)} ;
    my @dspcfgs    = @{(shift)} ;
    my (@commonGppOs, $commonGppOsCount, @gpposes) ;
    my ($cmd, $value, $done, $key, %gppos) ;

#generate a list of common GPP os.
    $commonGppOsCount = 0 ;
    @gpposes = @{$dspcfgs [0]{'GPPOS'}} ;
    for ($key = 0 ; $key < (scalar (@gpposes)) ; $key += 1) {
        my $isCommon = 1 ;
        for (my $dspNo = 1 ; $dspNo < (scalar (@dspcfgs)) ; $dspNo += 1) {
            my @dspgpposes = @{$dspcfgs [$dspNo]{'GPPOS'}} ;
            for (my $key1 = 0 ; $key1 < (scalar (@dspgpposes)) ; $key1 += 1) {
                if ($gpposes [$key]{'ID'} eq $dspgpposes [$key1]{'ID'}) {
                    $isCommon += 1 ;
                }
            }
        }

        if ($isCommon eq (scalar (@dspcfgs))) {
            $commonGppOs [$commonGppOsCount] = \%{$gpposes [$key]} ;
            $commonGppOsCount += 1 ;
        }
    }

    if ($commonGppOsCount eq 0) {
        print "\n****************** ERROR !!! *************************** \n" ;
        die "There is no GPP OS that support all selected DSPs.  Press --help for all options\n" ;
    }

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--gppos/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            goto SUCCESS ;
        }
    }

#No match, print error with help msg
goto ERROR ;

SUCCESS:
    $done = 0 ;
    for ($key = 0 ; $key < (scalar (@commonGppOs)) ; $key += 1) {
        if ($commonGppOs [$key]{'ID'} eq $value) {
            %gppos = %{$commonGppOs [$key]} ;
            print "\nChosen GPP OS for selected DSP(s):\n\tIdentifier:\t" . $value . "\n" ;
            print "\tDescription:\t" . $gppos {'DESC'} . "\n" ;
            return (%gppos) ;
        }
    }

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid GPP OS!\n" ;
    print "Following GPP OS are supported by selected DSPs:\n" ;
    for (my $key = 0 ; $key < (scalar (@commonGppOs)) ; $key += 1) {
        print "<ID>" . "-->" . $commonGppOs [$key]{'ID'} . "\n" ;
        print "\t" . $commonGppOs [$key]{'DESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --gppos=<ID> or --gppos=" . $gpposes [0]{'ID'} . " or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getComponentArg
#
#   desc    Function to parse Component
#   ============================================================================
sub CFG_getComponentArg {
    my $key ;
    my (%gppos) = %{(shift)}  ;
    my ($cmd, $value)         ;
    my %components            ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--comps/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            chomp ($value) ;
            if ($value ne "") {
                goto SUCCESS ;
            }
        }
    }

#No match, print error with help msg
goto ERROR ;

SUCCESS:
    my @comps   = split(//, uc($value)) ;
    my $sComps  = uc($gppos {'COMPS'}) ; # supported components by GPP OS.
    #initialize
    $components{'USE_PROC'}   = 0 ;
    $components{'USE_MPCS'}   = 0 ;
    $components{'USE_POOL'}   = 0 ;
    $components{'USE_NOTIFY'} = 0 ;
    $components{'USE_RINGIO'} = 0 ;
    $components{'USE_MPLIST'} = 0 ;
    $components{'USE_CHNL'}   = 0 ;
    $components{'USE_MSGQ'}   = 0 ;
    for (my $i = 0 ; $i < (scalar (@comps)) ; $i += 1) {
        if (($comps [$i] eq 'P') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components {'USE_PROC'} = 1 ;
        }
        elsif (($comps [$i] eq 'N') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'}   = 1 ;
            $components{'USE_NOTIFY'} = 1 ;
        }
        elsif (($comps [$i] eq 'O') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'} = 1 ;
            $components{'USE_MPCS'} = 1 ;
            $components{'USE_POOL'} = 1 ;
        }
        elsif (($comps [$i] eq 'S') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'} = 1 ;
            $components{'USE_MPCS'} = 1 ;
            $components{'USE_POOL'} = 1 ;
        }
        elsif (($comps [$i] eq 'L') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'}   = 1 ;
            $components{'USE_MPCS'}   = 1 ;
            $components{'USE_POOL'}   = 1 ;
            $components{'USE_MPLIST'} = 1 ;
        }
        elsif (($comps [$i] eq 'R') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'}   = 1 ;
            $components{'USE_MPCS'}   = 1 ;
            $components{'USE_POOL'}   = 1 ;
            $components{'USE_NOTIFY'} = 1 ;
            $components{'USE_RINGIO'} = 1 ;
        }
        elsif (($comps [$i] eq 'M') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'}   = 1 ;
            $components{'USE_MPCS'}   = 1 ;
            $components{'USE_POOL'}   = 1 ;
            $components{'USE_MPLIST'} = 1 ;
            $components{'USE_MSGQ'}   = 1 ;
        }
        elsif (($comps [$i] eq 'C') && ($comps [$i] =~ /[($sComps)]+/i)) {
            $components{'USE_PROC'}   = 1 ;
            $components{'USE_MPCS'}   = 1 ;
            $components{'USE_POOL'}   = 1 ;
            $components{'USE_MPLIST'} = 1 ;
            $components{'USE_CHNL'}   = 1 ;
        }
        else {
            print "\n****************** ERROR !!! *************************** \n" ;
            print "undefined component value: " . $comps [$i] . "\n" ;
            goto ERROR ;
        }
    }

    print "\nChosen Components for DSPLink:\n" ;
    print "\tUSE_PROC\t= " . $components{'USE_PROC'} . "\n" ;
    print "\tUSE_NOTIFY\t= " . $components{'USE_NOTIFY'} . "\n" ;
    print "\tUSE_POOL\t= " . $components{'USE_POOL'} . "\n" ;
    print "\tUSE_MPCS\t= " . $components{'USE_MPCS'} . "\n" ;
    print "\tUSE_MPLIST\t= " . $components{'USE_MPLIST'} . "\n" ;
    print "\tUSE_RINGIO\t= " . $components{'USE_RINGIO'} . "\n" ;
    print "\tUSE_MSGQ\t= " . $components{'USE_MSGQ'} . "\n" ;
    print "\tUSE_CHNL\t= " . $components{'USE_CHNL'} . "\n" ;
    return (%components) ;

ERROR :
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide valid components!\n" ;
    print "Following COMPONENTs  are supported by " . $gppos {'ID'} . ":\n" ;
    my $sComps  = $gppos {'COMPS'} ; # supported components by GPP OS.
    my @scomps = split (//, uc($sComps)) ;
    for (my $i = 0 ; $i < (scalar (@scomps)) ; $i += 1) {
        if ($scomps [$i] eq 'P') {
            print "\t[P]ROC Component\n" ;
        }
        elsif ($scomps [$i] eq 'N') {
            print "\t[N]OTIFY Component\n" ;
        }
        elsif ($scomps [$i] eq 'O') {
            print "\tP[O]OL Component\n" ;
        }
        elsif ($scomps [$i] eq 'S') {
            print "\tMPC[S] Component\n" ;
        }
        elsif ($scomps [$i] eq 'L') {
            print "\tMP[L]IST Component\n" ;
        }
        elsif ($scomps [$i] eq 'R') {
            print "\t[R]INGIO Component\n" ;
        }
        elsif ($scomps [$i] eq 'M') {
            print "\t[M]SGQ Component\n" ;
        }
        elsif ($scomps [$i] eq 'C') {
            print "\t[C]HNL Component\n" ;
        }
        else {
            print "!!BUG at component menu!!\n" ;
            print "Report this to DSPLINK team at:\n" ;
            print "dsplink-dev\@list.ti.com\n" ;
        }
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --comps=" . $sComps ." or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getTraceArg
#
#   desc    Function to parse trace
#   ============================================================================
sub CFG_getTraceArg {
    my $key ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--trace/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if ($value eq 1) {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "To enable trace use option: --trace=1\n" ;
    print "Provided: " .$value . "\n" ;
    print "Assuming trace disable and continuing...\n" ;
    return 0 ;

SUCCESS:
    print "\nEnabling trace!!\n" ;
    return $value ;
}


#   ============================================================================
#   func    CFG_getLegacySupportArg
#
#   desc    Function to get Legacy support argument
#   ============================================================================
sub CFG_getLegacySupportArg {
    my $key ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--legacy/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if ($value eq 1) {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "To enable legacy support use option: --legacy=1\n" ;
    print "Provided: " .$value . "\n" ;
    print "Assuming legacy support disable and continuing...\n" ;
    return 0 ;

SUCCESS:
    print "\nEnabling legacy support!!\n" ;
    print "DSPLINK is now compatible with previous release for single DSP\n" ;
    return $value ;
}

#   ============================================================================
#   func    CFG_getDspMode
#
#   desc    Function to get the DSP mode SWI or TSK
#   ============================================================================
sub CFG_getDspMode {
    my $key ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--DspTskMode/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if ($value eq 1) {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "To enable DSP TSK mode select: --DspTskMode=1\n" ;
    print "Provided: " .$value . "\n" ;
    print "Assuming SWI mode enabled and continuing...\n" ;
    return 0 ;

SUCCESS:
    print "\n========================================================= \n" ;
    print "Enabling  TSK Mode !!\n" ;
    return $value ;
}


#   ============================================================================
#   func    CFG_checkAndGetLoaderArg
#
#   desc    Function to parse loader arg only if multiple loader exists.
#   ============================================================================
sub CFG_checkAndGetLoaderArg {
    my $key ;
    my (%gppos)   = %{(shift)}  ;
    my (@loaders) = @{$gppos {'LOADERS'}} ;
    my ($cmd, $value)           ;
    my %loader                  ;

    if (scalar (@loaders) > 1) {
        for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
            if ($ARGV [$key] =~ m/--loader/) {
                ($cmd, $value) = split (/=/, $ARGV [$key]) ;
                for (my $i = 0 ; $i < (scalar (@loaders)) ; $i += 1) {
                    if ($loaders [$i]{'ID'} eq $value) {
                        %loader = %{$loaders [$i]} ;
                        goto SUCCESS ;
                    }
                }
            }
        }
        goto ERROR ;
    }
    else {
        $value = 0 ;
        %loader = %{$loaders [0]} ;
        goto SUCCESS ;
    }

SUCCESS:
    print "\nChosen Loader for GPP OS:\n\tIdentifier:\t" . $loader{'ID'}. "\n" ;
    print "\tDescription:\t" . $loader{'DESC'} . "\n" ;
    return (%loader) ;

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid loader!\n" ;
    print "Following loaders are supported by GPP OS (" . $gppos {'ID'} . "):\n" ;
    for (my $key = 0 ; $key < (scalar (@loaders)) ; $key += 1) {
        print "<ID>" . " --> " . $loaders [$key]{'ID'} . "\n" ;
        print "\t" . $loaders [$key]{'DESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --loader=COFF_LAODER or --loader=<ID> or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_checkAndGetLoaderArg
#
#   desc    Function to parse filesystem arg only if multiple loader exists.
#   ============================================================================
sub CFG_checkAndGetFileSytemArg {
    my $key ;
    my (%gppos)   = %{(shift)}  ;
    my (@fss) = @{$gppos {'FSS'}} ;
    my ($cmd, $value)           ;
    my %fs                      ;

    if (scalar (@fss) > 1) {
        for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
            if ($ARGV [$key] =~ m/fs/) {
                ($cmd, $value) = split (/=/, $ARGV [$key]) ;
                for (my $i = 0 ; $i < (scalar (@fss)) ; $i += 1) {
                    if ($fss [$i]{'ID'} eq $value) {
                        %fs = %{$fss [$i]} ;
                        goto SUCCESS ;
                    }
                }
            }
        }
        goto ERROR ;
    }
    else {
        $value = 0 ;
        %fs = %{$fss [0]} ;
        goto SUCCESS ;
    }

SUCCESS:
    print "\nChosen Filesystem for GPP OS:\n\tIdentifier:\t" . $fs{'ID'}. "\n" ;
    print "\tDescription:\t" . $fs{'DESC'} . "\n" ;
    return (%fs) ;

ERROR:
    print "\n****************** ERROR !!! *************************** \n" ;
    print "Please provide a valid filesystem!\n" ;
    print "Following filesystems are supported by GPP OS (" . $gppos {'ID'} . "):\n" ;
    for (my $key = 0 ; $key < (scalar (@fss)) ; $key += 1) {
        print "<ID>" . " --> " . $fss [$key]{'ID'} . "\n" ;
        print "\t" . $fss [$key]{'DESC'} . "\n" ;
    }
    print "Provided: " . $value . "\n" ;
    die "Example: --fs=PSEUDOFS or --fs=<ID> or --help for all options\n" ;
}


#   ============================================================================
#   func    CFG_getGppTempPathArg
#
#   desc    Function to parse GPP temp Path
#   ============================================================================
sub CFG_getGppTempPathArg {
    my $key ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--gpp_temp/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if (chomp ($value) ne "") {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "Binaries for GPP can be generated at preferred location \n" ;
    print "For example: --gpp_temp=/home/dsplink/gpp/bin\n" ;
    return "" ;

SUCCESS:
    print "\nBinaries for GPP will be generated here  : ". $value ."\n" ;
    return $value ;
}


#   ============================================================================
#   func    CFG_getDspTempPathArg
#
#   desc    Function to parse DSP temp Path
#   ============================================================================
sub CFG_getDspTempPathArg {
    my $key ;
    my ($dspNo) = (shift) ;
    my $str      = sprintf ("--dsp%d_temp", $dspNo) ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/($str)/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if (chomp ($value) ne "") {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "Binaries for DSP can be generated at preferred location \n" ;
    print "For example: --dsp" . $dspNo . "_temp=/home/dsplink/dsp<#>/bin\n" ;
    return "" ;

SUCCESS:
    print "\nBinaries for DSP" . $dspNo . " will be generated here  : ". $value ."\n" ;
    return $value ;
}

#   ============================================================================
#   func  CFG_getDMASupportArg
#
#   desc    Function to parse dma flag. to find out if DSP DMA is required
#           instead of memcpy to send data to DSP.
#   ============================================================================
sub CFG_getDMASupportArg {
    my $key ;
    my ($cmd, $value) ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--dspdma/) {
            ($cmd, $value) = split (/=/, $ARGV [$key]) ;
            if ($value eq 1) {
                goto SUCCESS ;
            }
        }
    }

    print "\n****************** ADVICE !!! *************************** \n" ;
    print "To enable usage of dsp edma use option: --dspdma=1\n" ;
    print "Provided: " .$value . "\n" ;
    print "Assuming default memcpy for data transfer to and from DSP  and continuing...\n" ;
    return 0 ;

SUCCESS:
    print "\n Enabling option to use DSP EDMA instead of default memcpy!!\n" ;
    return $value ;
}

#   ============================================================================
#   func    Generate_GppFlags
#
#   desc    Generate GPP CFLAGS
#   ============================================================================
sub Generate_GppFlags {
    my (%platform)   = %{(shift)}  ;
    my ($nodsp)      =  (shift)    ;
    my (@dspcfgs)    = @{(shift)}  ;
    my (@dspos)      = @{(shift)}  ;
    my (%gppos)      = %{(shift)}  ;
    my (%components) = %{(shift)}  ;
    my (%loader)     = %{(shift)}  ;
    my (%fs)         = %{(shift)}  ;
    my ($trace)      =  (shift)    ;
    my (@dsptemps)   = @{(shift)}  ;
    my ($gpptemp)    =  (shift)    ;
    my ($legacy)     =  (shift)    ;
    my ($dmaRequired)= (shift)     ;
    my ($dspMode)   =  (shift)    ;
    my ($text)                     ;
    my ($tempStr)                  ;

# GPP include paths
    my $PROJ_INCLUDE        = "\${DSPLINK}" . $DIRSEP . "gpp" . $DIRSEP . "inc" ;
    my $PROJ_INC_GENERIC    = $PROJ_INCLUDE  ;
    my $PROJ_INC_USR        = $PROJ_INCLUDE . $DIRSEP . "usr" ;
    my $PROJ_INC_SYS        = $PROJ_INCLUDE . $DIRSEP . "sys" ;
    my $PROJ_INC_ARCH       = $PROJ_INC_SYS . $DIRSEP . "arch"  ;
    my $PROJ_INC_BUILDOS    = $PROJ_INCLUDE . $DIRSEP . "sys" . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_INC_BUILDOSVER = $PROJ_INCLUDE . $DIRSEP . "sys" . $DIRSEP . $gppos {'TYPE'} . $DIRSEP . $gppos {'VER'} ;

# Component include paths
    my $PROJ_SRC                = "\${DSPLINK}" . $DIRSEP . "gpp" . $DIRSEP . "src" ;
    my $PROJ_SRC_GEN            = $PROJ_SRC         . $DIRSEP . "gen" ;
    my $PROJ_SRC_GEN_OS         = $PROJ_SRC_GEN     . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_SRC_GEN_OS_VER     = $PROJ_SRC_GEN_OS  . $DIRSEP . $gppos {'VER'} ;
    my $PROJ_SRC_OSAL           = $PROJ_SRC         . $DIRSEP . "osal" ;
    my $PROJ_SRC_OSAL_OS        = $PROJ_SRC_OSAL    . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_SRC_OSAL_OS_VER    = $PROJ_SRC_OSAL_OS . $DIRSEP . $gppos {'VER'} ;
    my $PROJ_SRC_PMGR           = $PROJ_SRC         . $DIRSEP . "pmgr" ;
    my $PROJ_SRC_PMGR_OS        = $PROJ_SRC_PMGR    . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_SRC_PMGR_OS_VER    = $PROJ_SRC_PMGR_OS . $DIRSEP . $gppos {'VER'} ;
    my $PROJ_SRC_API            = $PROJ_SRC         . $DIRSEP . "api" ;
    my $PROJ_SRC_API_OS         = $PROJ_SRC_API     . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_SRC_API_OS_VER     = $PROJ_SRC_API_OS  . $DIRSEP . $gppos {'VER'} ;
    my $PROJ_SRC_LDRV           = $PROJ_SRC         . $DIRSEP . "ldrv" ;
    my $PROJ_SRC_LDRV_OS        = $PROJ_SRC_LDRV    . $DIRSEP . $gppos {'TYPE'} ;
    my $PROJ_SRC_LDRV_OS_VER    = $PROJ_SRC_LDRV_OS . $DIRSEP . $gppos {'VER'} ;
    my $PROJ_SRC_LDRV_DRV       = $PROJ_SRC_LDRV    . $DIRSEP . "DRV"    ;
    my $PROJ_SRC_LDRV_DATA      = $PROJ_SRC_LDRV    . $DIRSEP . "DATA"   ;
    my $PROJ_SRC_LDRV_MPCS      = $PROJ_SRC_LDRV    . $DIRSEP . "MPCS"   ;
    my $PROJ_SRC_LDRV_MPLIST    = $PROJ_SRC_LDRV    . $DIRSEP . "MPLIST" ;
    my $PROJ_SRC_LDRV_MQT       = $PROJ_SRC_LDRV    . $DIRSEP . "MQT"    ;
    my $PROJ_SRC_LDRV_IPS       = $PROJ_SRC_LDRV    . $DIRSEP . "IPS"    ;
    my $PROJ_SRC_LDRV_SMM       = $PROJ_SRC_LDRV    . $DIRSEP . "SMM"    ;
    my $PROJ_SRC_LDRV_RINGIO    = $PROJ_SRC_LDRV    . $DIRSEP . "RINGIO" ;
    my $PROJ_SRC_LDRV_POOLS     = $PROJ_SRC_LDRV    . $DIRSEP . "POOLS"  ;

    $tempStr .= $PROJ_INC_GENERIC ;
    $tempStr .= " " . $PROJ_INC_USR ;
    $tempStr .= " " . $PROJ_INC_SYS ;
    $tempStr .= " " . $PROJ_INC_ARCH ;

    for (my $key = 0 ; $key < (scalar (@dspcfgs)) ; $key +=1) {
        $tempStr .= " " . $PROJ_INC_ARCH . $DIRSEP . $dspcfgs [$key]{'DSPID'} ;
        $tempStr .= " " . $PROJ_INC_ARCH . $DIRSEP . $dspcfgs [$key]{'DSPID'} . $DIRSEP . $gppos {'TYPE'} ;
    }

    $tempStr .= " " . $PROJ_INC_BUILDOS ;
    $tempStr .= " " . $PROJ_INC_BUILDOSVER ;
    $tempStr .= " " . $PROJ_SRC_GEN ;
    $tempStr .= " " . $PROJ_SRC_GEN_OS ;
    $tempStr .= " " . $PROJ_SRC_GEN_OS_VER ;
    $tempStr .= " " . $PROJ_SRC_OSAL ;
    $tempStr .= " " . $PROJ_SRC_OSAL_OS ;
    $tempStr .= " " . $PROJ_SRC_OSAL_OS_VER ;
    $tempStr .= " " . $PROJ_SRC_PMGR ;
    $tempStr .= " " . $PROJ_SRC_PMGR_OS ;
    $tempStr .= " " . $PROJ_SRC_PMGR_OS_VER ;
    $tempStr .= " " . $PROJ_SRC_API ;
    $tempStr .= " " . $PROJ_SRC_API_OS ;
    $tempStr .= " " . $PROJ_SRC_API_OS_VER ;
    $tempStr .= " " . $PROJ_SRC_LDRV ;
    $tempStr .= " " . $PROJ_SRC_LDRV_OS ;
    $tempStr .= " " . $PROJ_SRC_LDRV_OS_VER ;
    $tempStr .= " " . $PROJ_SRC_LDRV_DRV    ;
    $tempStr .= " " . $PROJ_SRC_LDRV_DATA   ;
    $tempStr .= " " . $PROJ_SRC_LDRV_MPCS   ;
    $tempStr .= " " . $PROJ_SRC_LDRV_MPLIST ;
    $tempStr .= " " . $PROJ_SRC_LDRV_MQT    ;
    $tempStr .= " " . $PROJ_SRC_LDRV_IPS    ;
    $tempStr .= " " . $PROJ_SRC_LDRV_SMM    ;
    $tempStr .= " " . $PROJ_SRC_LDRV_RINGIO ;
    $tempStr .= " " . $PROJ_SRC_LDRV_POOLS  ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   GPP INC PATH\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export  TI_DSPLINK_GPP_INC_PATH := " . $tempStr ;
    $text .= "\n" ;
    $text .= "\n" ;

# Now defines
    $tempStr = "" ;

    if ($trace eq 1) {
        $tempStr .= "TRACE_ENABLE"  ;
    }

    if ($gppos {'TYPE'} eq 'Linux') {
        $tempStr .= " OS_LINUX"  ;
    }
    elsif ($gppos {'TYPE'} eq 'PROS') {
        $tempStr .= " OS_PROS"  ;
    }
    elsif ($gppos {'TYPE'} eq 'WINCE') {
        $tempStr .= " OS_WINCE"  ;
    }

#   ----------------------------------------------------------------------------
#   Definitions for MAX_DSP, MAX_PROCESSORS, ID_GPP, PLATFORM, PROCIDs
#   ----------------------------------------------------------------------------
    $tempStr .= " MAX_DSPS=" . $nodsp ;
    $tempStr .= " MAX_PROCESSORS=" . ($nodsp + 1) ;
    $tempStr .= " ID_GPP=" . $nodsp ;
    $tempStr .= " " . $platform {'ID'} ;
    if ($legacy eq 1) {
        $tempStr .= " DSPLINK_LEGACY_SUPPORT" ;
    }

    if ($components {'USE_PROC'} eq 1) {
        $tempStr .= " PROC_COMPONENT" ;
    }
    if ($components {'USE_POOL'} eq 1) {
        $tempStr .= " POOL_COMPONENT" ;
    }
    if ($components {'USE_NOTIFY'} eq 1) {
        $tempStr .= " NOTIFY_COMPONENT" ;
    }
    if ($components {'USE_MPCS'} eq 1) {
        $tempStr .= " MPCS_COMPONENT" ;
    }
    if ($components {'USE_RINGIO'} eq 1) {
        $tempStr .= " RINGIO_COMPONENT" ;
    }
    if ($components {'USE_MPLIST'} eq 1) {
        $tempStr .= " MPLIST_COMPONENT" ;
    }
    if ($components {'USE_MSGQ'} eq 1) {
        $tempStr .= " MSGQ_COMPONENT" ;
        $tempStr .= " MSGQ_ZCPY_LINK" ;
    }
    if ($components {'USE_CHNL'} eq 1) {
        $tempStr .= " CHNL_COMPONENT" ;
        $tempStr .= " CHNL_ZCPY_LINK" ;
    }
    if (($components {'USE_CHNL'} eq 1) || ($components {'USE_MSGQ'} eq 1)) {
        $tempStr .= " ZCPY_LINK" ;
    }

# PROC Only case
    if (   ($components {'USE_CHNL'}   eq 0)
        && ($components {'USE_MSGQ'}   eq 0)
        && ($components {'USE_MPLIST'} eq 0)
        && ($components {'USE_RINGIO'} eq 0)
        && ($components {'USE_MPCS'}   eq 0)
        && ($components {'USE_NOTIFY'} eq 0)
        && ($components {'USE_POOL'}   eq 0)
        && ($components {'USE_PROC'}   eq 1)) {
        $tempStr .= " ONLY_PROC_COMPONENT" ;
    }

    if (defined (%fs)) {
        if ($fs {'ID'} eq 'PSEUDOFS') {
            $tempStr .= " KFILE_PSEUDO " ;
        }

        if ($fs {'ID'} eq 'PRFILEFS') {
            $tempStr .= " KFILE_DEFAULT " ;
        }
    }
    else {
        $tempStr .= " KFILE_DEFAULT " ;
    }

    for (my $key = 0 ; $key < (scalar (@dspcfgs)) ; $key += 1) {
        $tempStr .= " " . $dspcfgs [$key]{'DSPID'} ;
        $tempStr .= " " . $dspcfgs [$key]{'DSPID'} . "_PHYINTERFACE=" . $dspcfgs [$key]{'PHYTYPE'} . "_INTERFACE" ;
    }
    if ($dmaRequired eq 1) {
        $tempStr .= "  LINUXPC_DSP_DMA " ;
    }
     
	if ($dspMode eq 1) {
        $tempStr .= " GPP_TSK_MODE" ;
        $text .= "\n" ;
        $text .= "\n" ;
    }
    else {
        $tempStr .= " GPP_SWI_MODE" ;
        $text .= "\n" ;
        $text .= "\n" ;
    }

    $text .= "#   =========================================================\n" ;
    $text .= "#   GPP DEFINES\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export  TI_DSPLINK_GPP_DEFINES := " . $tempStr ;
    $text .= "\n" ;
    $text .= "\n" ;

    return $text ;
}


#   ============================================================================
#   func    Generate_DspFlags
#
#   desc    Generate DSP CFLAGS
#   ============================================================================
sub Generate_DspFlags {
    my (%platform)   = %{(shift)}  ;
    my ($nodsp)      =  (shift)    ;
    my (@dspcfgs)    = @{(shift)}  ;
    my (@dspos)      = @{(shift)}  ;
    my (%gppos)      = %{(shift)}  ;
    my (%components) = %{(shift)}  ;
    my (%loader)     = %{(shift)}  ;
    my (%fs)         = %{(shift)}  ;
    my ($trace)      =  (shift)    ;
    my (@dsptemps)   = @{(shift)}  ;
    my ($gpptemp)    =  (shift)    ;
    my ($legacy)     =  (shift)    ;
    my ($dspMode)   =  (shift)    ;
    my ($text)                     ;
    my ($tempStr)                  ;

    for (my $key = 0 ; $key < (scalar (@dspcfgs)) ; $key +=1) {
    # GPP include paths
        my $PROJ_INCLUDE        = "\${DSPLINK}"     . $DIRSEP . "dsp" . $DIRSEP . "inc" ;
        my $PROJ_INC_GENERIC    = $PROJ_INCLUDE  ;
        my $PROJ_INC_CLASS      = $PROJ_INCLUDE     . $DIRSEP . $dspcfgs [$key]{'DSPCLASS'}  ;
        my $PROJ_INC_OS         = $PROJ_INCLUDE     . $DIRSEP . $dspos   [$key]{'TYPE'}  ;
        my $PROJ_INC_OS_DEV     = $PROJ_INC_OS      . $DIRSEP . $dspcfgs [$key]{'DSPID'} ;
        my $PROJ_INC_OS_VER     = $PROJ_INC_OS      . $DIRSEP . $dspos   [$key]{'VER'}  ;
        my $PROJ_INC_OS_VER_DEV = $PROJ_INC_OS_VER  . $DIRSEP . $dspcfgs [$key]{'DSPID'} ;

    # Component include paths
        my $PROJ_SRC                       = "\${DSPLINK}" . $DIRSEP . "dsp" . $DIRSEP . "src" ;
        my $PROJ_SRC_BASE                  = $PROJ_SRC                  . $DIRSEP . "base" ;
        my $PROJ_SRC_BASE_HAL              = $PROJ_SRC_BASE             . $DIRSEP . "hal" ;
        my $PROJ_SRC_BASE_HAL_OS           = $PROJ_SRC_BASE_HAL         . $DIRSEP . $dspos  [$key]{'TYPE'} ;
        my $PROJ_SRC_BASE_HAL_OS_VER       = $PROJ_SRC_BASE_HAL_OS      . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_BASE_HAL_OS_DEV       = $PROJ_SRC_BASE_HAL_OS      . $DIRSEP . $dspcfgs [$key]{'DSPID'} ;
        my $PROJ_SRC_BASE_HAL_OS_VER_DEV   = $PROJ_SRC_BASE_HAL_OS_VER  . $DIRSEP . $dspcfgs [$key]{'DSPID'} ;
        my $PROJ_SRC_BASE_DRV              = $PROJ_SRC_BASE             . $DIRSEP . "drv" ;
        my $PROJ_SRC_BASE_DRV_OS           = $PROJ_SRC_BASE_DRV         . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_BASE_DRV_OS_VER       = $PROJ_SRC_BASE_DRV_OS      . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_BASE_IPS              = $PROJ_SRC_BASE             . $DIRSEP . "ips" ;
        my $PROJ_SRC_BASE_IPS_OS           = $PROJ_SRC_BASE_IPS         . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_BASE_IPS_OS_VER       = $PROJ_SRC_BASE_IPS_OS      . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_BASE_GEN              = $PROJ_SRC_BASE             . $DIRSEP . "gen" ;
        my $PROJ_SRC_BASE_GEN_OS           = $PROJ_SRC_BASE_GEN         . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_BASE_GEN_OS_VER       = $PROJ_SRC_BASE_GEN_OS      . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_DATA                  = $PROJ_SRC                  . $DIRSEP . "data" ;
        my $PROJ_SRC_DATA_OS               = $PROJ_SRC_DATA             . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_DATA_OS_VER           = $PROJ_SRC_DATA_OS          . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_MPCS                  = $PROJ_SRC                  . $DIRSEP . "mpcs" ;
        my $PROJ_SRC_MPCS_OS               = $PROJ_SRC_MPCS             . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_MPCS_OS_VER           = $PROJ_SRC_MPCS_OS          . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_MPLIST                = $PROJ_SRC                  . $DIRSEP . "mplist" ;
        my $PROJ_SRC_MPLIST_OS             = $PROJ_SRC_MPLIST           . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_MPLIST_OS_VER         = $PROJ_SRC_MPLIST_OS        . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_MSG                   = $PROJ_SRC                  . $DIRSEP . "msg" ;
        my $PROJ_SRC_MSG_OS                = $PROJ_SRC_MSG              . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_MSG_OS_VER            = $PROJ_SRC_MSG_OS           . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_NOTIFY                = $PROJ_SRC                  . $DIRSEP . "notify" ;
        my $PROJ_SRC_NOTIFY_OS             = $PROJ_SRC_NOTIFY           . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_NOTIFY_OS_VER         = $PROJ_SRC_NOTIFY_OS        . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_POOLS                 = $PROJ_SRC                  . $DIRSEP . "pools" ;
        my $PROJ_SRC_POOLS_OS              = $PROJ_SRC_POOLS            . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_POOLS_OS_VER          = $PROJ_SRC_POOLS_OS         . $DIRSEP . $dspos [$key]{'VER'} ;
        my $PROJ_SRC_RINGIO                = $PROJ_SRC                  . $DIRSEP . "ringio" ;
        my $PROJ_SRC_RINGIO_OS             = $PROJ_SRC_RINGIO           . $DIRSEP . $dspos [$key]{'TYPE'} ;
        my $PROJ_SRC_RINGIO_OS_VER         = $PROJ_SRC_RINGIO_OS        . $DIRSEP . $dspos [$key]{'VER'} ;


        $tempStr .= $PROJ_INC_GENERIC             ;
        $tempStr .= " " . $PROJ_INCLUDE                 ;
        $tempStr .= " " . $PROJ_INC_GENERIC             ;
        $tempStr .= " " . $PROJ_INC_CLASS               ;
        $tempStr .= " " . $PROJ_INC_OS                  ;
        $tempStr .= " " . $PROJ_INC_OS_DEV              ;
        $tempStr .= " " . $PROJ_INC_OS_VER              ;
        $tempStr .= " " . $PROJ_INC_OS_VER_DEV          ;
        $tempStr .= " " . $PROJ_SRC                     ;
        $tempStr .= " " . $PROJ_SRC_BASE                ;
        $tempStr .= " " . $PROJ_SRC_BASE_HAL            ;
        $tempStr .= " " . $PROJ_SRC_BASE_HAL_OS         ;
        $tempStr .= " " . $PROJ_SRC_BASE_HAL_OS_VER     ;
        $tempStr .= " " . $PROJ_SRC_BASE_HAL_OS_DEV     ;
        $tempStr .= " " . $PROJ_SRC_BASE_HAL_OS_VER_DEV ;
        $tempStr .= " " . $PROJ_SRC_BASE_DRV            ;
        $tempStr .= " " . $PROJ_SRC_BASE_DRV_OS         ;
        $tempStr .= " " . $PROJ_SRC_BASE_DRV_OS_VER     ;
        $tempStr .= " " . $PROJ_SRC_BASE_IPS            ;
        $tempStr .= " " . $PROJ_SRC_BASE_IPS_OS         ;
        $tempStr .= " " . $PROJ_SRC_BASE_IPS_OS_VER     ;
        $tempStr .= " " . $PROJ_SRC_BASE_GEN            ;
        $tempStr .= " " . $PROJ_SRC_BASE_GEN_OS         ;
        $tempStr .= " " . $PROJ_SRC_BASE_GEN_OS_VER     ;
        $tempStr .= " " . $PROJ_SRC_DATA                ;
        $tempStr .= " " . $PROJ_SRC_DATA_OS             ;
        $tempStr .= " " . $PROJ_SRC_DATA_OS_VER         ;
        $tempStr .= " " . $PROJ_SRC_MPCS                ;
        $tempStr .= " " . $PROJ_SRC_MPCS_OS             ;
        $tempStr .= " " . $PROJ_SRC_MPCS_OS_VER         ;
        $tempStr .= " " . $PROJ_SRC_MPLIST              ;
        $tempStr .= " " . $PROJ_SRC_MPLIST_OS           ;
        $tempStr .= " " . $PROJ_SRC_MPLIST_OS_VER       ;
        $tempStr .= " " . $PROJ_SRC_MSG                 ;
        $tempStr .= " " . $PROJ_SRC_MSG_OS              ;
        $tempStr .= " " . $PROJ_SRC_MSG_OS_VER          ;
        $tempStr .= " " . $PROJ_SRC_NOTIFY              ;
        $tempStr .= " " . $PROJ_SRC_NOTIFY_OS           ;
        $tempStr .= " " . $PROJ_SRC_NOTIFY_OS_VER       ;
        $tempStr .= " " . $PROJ_SRC_POOLS               ;
        $tempStr .= " " . $PROJ_SRC_POOLS_OS            ;
        $tempStr .= " " . $PROJ_SRC_POOLS_OS_VER        ;
        $tempStr .= " " . $PROJ_SRC_RINGIO              ;
        $tempStr .= " " . $PROJ_SRC_RINGIO_OS           ;
        $tempStr .= " " . $PROJ_SRC_RINGIO_OS_VER       ;

        $text .= "#   =========================================================\n" ;
        $text .= "#   DSP INC PATH\n" ;
        $text .= "#   =========================================================\n" ;
        $text .= "export  TI_DSPLINK_DSP" . $key . "_INC_PATH := " . $tempStr ;
        $text .= "\n" ;
        $text .= "\n" ;

        $tempStr  = "" ;
        $tempStr .= " PROCID=" . $key ;
        $tempStr .= " " . $dspcfgs [$key]{'DSPID'} ;
        $tempStr .= " " . $dspcfgs [$key]{'DSPID'} . "_INTERFACE=" . $dspcfgs [$key]{'PHYTYPE'} . "_INTERFACE" ;
        $tempStr .= " PHYINTERFACE=" . $dspcfgs [$key]{'PHYTYPE'} . "_INTERFACE" ;

        if ($dspMode eq 1) {
            $tempStr .= " DSP_TSK_MODE" ;
            $text .= "\n" ;
            $text .= "\n" ;
        }
        else {
            $tempStr .= " DSP_SWI_MODE" ;
            $text .= "\n" ;
            $text .= "\n" ;
        }

        $text .= "#   =========================================================\n" ;
        $text .= "#   DSP SPECIFIC DEFINES\n" ;
        $text .= "#   =========================================================\n" ;
        $text .= "export  TI_DSPLINK_DSP" . $key . "_DEFINES := " . $tempStr ;
        $text .= "\n" ;
        $text .= "\n" ;
    }
# Now defines
    $tempStr = "" ;

    if ($trace eq 1) {
        $tempStr .= "TRACE_ENABLE"  ;
    }

#   ----------------------------------------------------------------------------
#   Definitions for MAX_DSP, MAX_PROCESSORS, ID_GPP, PLATFORM, PROCIDs
#   ----------------------------------------------------------------------------
    $tempStr .= " MAX_DSPS=" . $nodsp ;
    $tempStr .= " MAX_PROCESSORS=" . ($nodsp + 1) ;
    $tempStr .= " ID_GPP=" . $nodsp ;
    $tempStr .= " " . $platform {'ID'} ;
    if ($legacy eq 1) {
        $tempStr .= " DSPLINK_LEGACY_SUPPORT" ;
    }

    if ($components {'USE_PROC'} eq 1) {
        $tempStr .= " PROC_COMPONENT" ;
    }
    if ($components {'USE_POOL'} eq 1) {
        $tempStr .= " POOL_COMPONENT" ;
    }
    if ($components {'USE_NOTIFY'} eq 1) {
        $tempStr .= " NOTIFY_COMPONENT" ;
    }
    if ($components {'USE_MPCS'} eq 1) {
        $tempStr .= " MPCS_COMPONENT" ;
    }
    if ($components {'USE_RINGIO'} eq 1) {
        $tempStr .= " RINGIO_COMPONENT" ;
    }
    if ($components {'USE_MPLIST'} eq 1) {
        $tempStr .= " MPLIST_COMPONENT" ;
    }
    if ($components {'USE_MSGQ'} eq 1) {
        $tempStr .= " MSGQ_COMPONENT" ;
        $tempStr .= " MSGQ_ZCPY_LINK" ;
    }
    if ($components {'USE_CHNL'} eq 1) {
        $tempStr .= " CHNL_COMPONENT" ;
        $tempStr .= " CHNL_ZCPY_LINK" ;
    }
    if (($components {'USE_CHNL'} eq 1) || ($components {'USE_MSGQ'} eq 1)) {
        $tempStr .= " ZCPY_LINK" ;
    }

# PROC Only case
    if (   ($components {'USE_CHNL'}   eq 0)
        && ($components {'USE_MSGQ'}   eq 0)
        && ($components {'USE_MPLIST'} eq 0)
        && ($components {'USE_RINGIO'} eq 0)
        && ($components {'USE_MPCS'}   eq 0)
        && ($components {'USE_NOTIFY'} eq 0)
        && ($components {'USE_POOL'}   eq 0)
        && ($components {'USE_PROC'}   eq 1)) {
        $tempStr .= " ONLY_PROC_COMPONENT" ;
    }

    $text .= "#   =========================================================\n" ;
    $text .= "#   DSP COMMON DEFINES\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export  TI_DSPLINK_DSP_COMMON_DEFINES := " . $tempStr ;
    $text .= "\n" ;
    $text .= "\n" ;

    return $text ;
}


#   ============================================================================
#   func    Generate_Currentcfgmk
#
#   desc    Generate CURRENTCFG.mk
#   ============================================================================
sub Generate_Currentcfgmk {
    my ($text, $filehandle) ;
    my (%platform)    = %{(shift)}  ;
    my ($nodsp)       =  (shift)    ;
    my (@dspcfgs)     = @{(shift)}  ;
    my (@dspos)       = @{(shift)}  ;
    my (%gppos)       = %{(shift)}  ;
    my (%components)  = %{(shift)}  ;
    my (%loader)      = %{(shift)}  ;
    my (%fs)          = %{(shift)}  ;
    my ($trace)       =  (shift)    ;
    my (@dsptemps)    = @{(shift)}  ;
    my ($gpptemp)     =  (shift)    ;
    my ($legacy)      =  (shift)    ;
    my ($dspMode)     =  (shift)    ;
    my ($dmaRequired) =  (shift)    ;
    my ($ti)          =  0          ;
    my ($found)       =  0          ;
    my (@tdsps)                     ;
    my ($tempStr)                   ;

    my ($cfgfile)   = &GetCfgFile () ;

    open ($filehandle, ">$cfgfile") || die "!! Couldn't open file: $cfgfile\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   DSP/BIOS LINK Configuration file.\n" ;
    $text .= "#\n" ;
    $text .= "#   CAUTION! This is a generated file.\n" ;
    $text .= "#            All changes will be lost.\n" ;
    $text .= "#\n" ;
    $text .= "#   This file was generated on " . &GetDateString() . "\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   When this file was created.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export  TI_DSPLINK_CFGDATE            := " . &GetDateString() . "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   DSP/BIOS LINK GPP side root directory.\n" ;
    $text .= "#   =========================================================\n" ;
    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        $text .= "export TI_DSPLINK_GPPROOT             := \${DSPLINK}\$(DIRSEP)gpp\n" ;
        $text .= "export TI_DSPLINK_DSPROOT             := \${DSPLINK}\$(DIRSEP)dsp\n" ;
    }
    else {
        $text .= "export TI_DSPLINK_GPPROOT             := \${DSPLINK}/gpp\n" ;
        $text .= "export TI_DSPLINK_DSPROOT             := \${DSPLINK}/dsp\n" ;
    }
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   GPP OS for which DSP/BIOS LINK is being built.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_GPPOS               := " . $gppos{'TYPE'}  . "\n" ;
    $text .= "export TI_DSPLINK_GPPOSPREFIX         := " . $gppos{'OSPREFIX'}  . "\n" ;
    $text .= "export TI_DSPLINK_GPPDEVICE           := " . $platform{'GPPDEVICE'}  . "\n" ;
    $text .= "export TI_DSPLINK_GPPOSVERSION        := " . $gppos{'VER'}  . "\n" ;
    $text .= "export TI_DSPLINK_GPPDISTRIBUTION     := " . &GetGppMakeFile (\%platform, \%gppos) . "\n" ;
    if ($gpptemp ne "") {
        $text .= "export TI_DSPLINK_GPPTEMPPATH         := " . $gpptemp . "\n" ;
    }
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Target platform for DSP/BIOS LINK.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_PLATFORM            := " . uc($platform{'PREFIX'})  . "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   DSPs for which DSP/BIOS LINK is being built.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_DSPDEVICES          := " ;
    for (my $i = 0 ; $i < (scalar (@dspcfgs)) ; $i += 1) {
        $text .= uc($dspcfgs [$i]{'DSPID'}) . " "
    }
    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   External DSPS name for which DSP/BIOS LINK is being built.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_DSPDEVICES_EXTERNAL := " ;

    for (my $i = 0 ; $i < (scalar (@dspcfgs)) ; $i += 1) {
        if(($dspcfgs [$i]{'DSPID'}) ne ($dspcfgs [$i]{'DSPNAME'})) {
            $text .= uc($dspcfgs [$i]{'DSPNAME'}) . " "
        }
    }
    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   DSP Config files.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_DSPCFGFILES        := " ;
    for (my $i = 0 ; $i < (scalar (@dspcfgs)) ; $i += 1) {
        $found = 0 ;
        for (my $j = 0 ; $j < (scalar (@tdsps)) ; $j++) {
            if (uc ($dspcfgs [$i]{'DSPID'}) eq $tdsps [$j]) {
                $found = 1 ;
            }
        }
        if ($found == 0) {
            $text .= "CFG_" .uc($dspcfgs [$i]{'DSPNAME'}) ;
            $text .= "_" . uc ($dspcfgs [$i]{'PHYTYPE'}) . ".c " ;
            $tdsps [$ti] = uc($dspcfgs [$i]{'DSPID'}) ;
            $ti += 1 ;
        }
    }

    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Processor Counts and IDs.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_MAX_PROCESSORS     := " . ($nodsp + 1) . "\n" ;
    $text .= "export TI_DSPLINK_MAX_DSPS           := " . $nodsp . "\n" ;
    $text .= "export TI_DSPLINK_ID_GPP             := " . $nodsp . "\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Default DSP Device and its physical interfaces for DSP/BIOS LINK.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "ifndef TI_DSPLINK_BUILD_DEFAULTDSP\n";
    $text .= "export  TI_DSPLINK_DSPDEVICE         := " . $dspcfgs [0]{'DSPID'} . "\n" ;
    if($dspcfgs [0]{'DSPID'} eq $dspcfgs [0]{'DSPNAME'}) {
        $text .= "export  TI_DSPLINK_DSPDEVICE_EXTERNAL:= " ."\n";
    }
    else {
        $text .= "export  TI_DSPLINK_DSPDEVICE_EXTERNAL:= " . $dspcfgs [0]{'DSPNAME'} . "\n" ;
    }
    $text .= "export  TI_DSPLINK_DSPDISTRIBUTION   := " . &GetDspMakeFile (\%{$dspcfgs [0]}, \%{$dspos [0]}) . "\n" ;
    $text .= "export  TI_DSPLINK_DSPOS             := " . $dspos [0]{'TYPE'} . "\n" ;
    $text .= "export  TI_DSPLINK_DSPOSVERSION      := " . $dspos [0]{'PREFIX'} . "\n" ;
    $text .= "export  TI_DSPLINK_PROCID            := 0\n" ;
    $text .= "export  TI_DSPLINK_DSPDEVICECLASS    := " . $dspcfgs [0]{'DSPCLASS'} . "\n" ;
    $text .= "export  TI_DSPLINK_DSPTEMPPATH       := " . $dsptemps [0] . "\n" ;
    $text .= "endif\n" ;
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Target DSP Devices and their physical interfaces for DSP/BIOS LINK.\n" ;
    $text .= "#   =========================================================\n" ;
    for (my $i = 0 ; $i < (scalar (@dspcfgs)) ; $i += 1) {
        $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_OS                  := " . $dspos [$i]{'TYPE'}  . "\n" ;
        $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_DISTRIBUTION        := " . &GetDspMakeFile (\%{$dspcfgs [$i]}, \%{$dspos [$i]}) . "\n" ;
        $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_PHYINTERFACE        := " ;
        $text .= uc($dspcfgs [$i]{'PHYTYPE'}) . "\n" ;
        $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_CLASS               := " ;
        $text .= uc($dspcfgs [$i]{'DSPCLASS'}) . "\n" ;
        $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_PROCID              := " ;
        $text .= $i . "\n" ;
        if ($dspMode eq 1) {
            $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_MODE                := DSP_TSK_MODE" ."\n";
        }
        else {
            $text .= "export TI_DSPLINK_". $dspcfgs [$i]{'DSPID'} ."_MODE                := DSP_SWI_MODE" ."\n";
        }
    }
    $text .= "\n" ;
    $text .= "\n" ;


    $text .= "#   =========================================================\n" ;
    $text .= "#   Compile time scalability options for DSP/BIOS LINK.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_USE_PROC            := " . $components {'USE_PROC'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_POOL            := " . $components {'USE_POOL'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_NOTIFY          := " . $components {'USE_NOTIFY'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_MPCS            := " . $components {'USE_MPCS'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_RINGIO          := " . $components {'USE_RINGIO'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_MPLIST          := " . $components {'USE_MPLIST'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_MSGQ            := " . $components {'USE_MSGQ'}  . "\n" ;
    $text .= "export TI_DSPLINK_USE_CHNL            := " . $components {'USE_CHNL'}  . "\n" ;
    if (   ($components {'USE_CHNL'}   eq 0)
        && ($components {'USE_MSGQ'}   eq 0)
        && ($components {'USE_MPLIST'} eq 0)
        && ($components {'USE_RINGIO'} eq 0)
        && ($components {'USE_MPCS'}   eq 0)
        && ($components {'USE_NOTIFY'} eq 0)
        && ($components {'USE_POOL'}   eq 0)
        && ($components {'USE_PROC'}   eq 1)) {
        $text .= "export TI_DSPLINK_ONLY_PROC_COMPONENT            := 1\n" ;
    }
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Compile time physical link scalability options for device.\n" ;
    $text .= "#   =========================================================\n" ;
    if ($components {'USE_MSGQ'} eq 1) {
        $text .= "export TI_DSPLINK_USE_MSGQ_ZCPY_LINK  := 1\n" ;
    }
    else {
        $text .= "export TI_DSPLINK_USE_MSGQ_ZCPY_LINK  := 0\n" ;
    }
    if ($components {'USE_CHNL'} eq 1) {
        $text .= "export TI_DSPLINK_USE_CHNL_ZCPY_LINK  := 1\n" ;
    }
    else {
        $text .= "export TI_DSPLINK_USE_CHNL_ZCPY_LINK  := 0\n" ;
    }
    $text .= "\n" ;
    $text .= "\n" ;

    $text .= "#   =========================================================\n" ;
    $text .= "#   Enable/ Disable Trace.\n" ;
    $text .= "#   =========================================================\n" ;
    $text .= "export TI_DSPLINK_TRACE  := " . $trace . "\n";
    $text .= "\n" ;
    $text .= "\n" ;

    if ($legacy eq 1) {
        $text .= "#   =========================================================\n" ;
        $text .= "#   Enable/Diable Legacy support.\n" ;
        $text .= "#   =========================================================\n" ;
        $text .= "export TI_DSPLINK_LEGACY_SUPPORT  := " . $legacy . "\n";
        $text .= "\n" ;
        $text .= "\n" ;
    }

    if (defined (%fs)) {
        $text .= "#   =========================================================\n" ;
        $text .= "#   Filesystem used.\n" ;
        $text .= "#   =========================================================\n" ;
        $text .= "export TI_DSPLINK_FILESYSTEM  := " . $fs {'ID'} . "\n";
        $text .= "\n" ;
        $text .= "\n" ;
    }

    $text .= &Generate_GppFlags (\%platform, $nodsp, \@dspcfgs, \@dspos,
                                 \%gppos, \%components, \%loader, \%fs, $trace,
                                 \@dsptemps, $gpptemp, $legacy, $dmaRequired,
                                 \$dspMode) ;

    $text .= &Generate_DspFlags (\%platform, $nodsp, \@dspcfgs, \@dspos,
                                 \%gppos, \%components, \%loader, \%fs, $trace,
                                 \@dsptemps, $gpptemp, $legacy, $dspMode) ;

    print $filehandle $text ;

    close ($filehandle) ;
}


#   ============================================================================
#   func    Generate_multiMake
#
#   desc    Generate MulitMake script
#   ============================================================================
sub Generate_multiMake {
    my ($text, $filehandle) ;
    my (%platform)   = %{(shift)}  ;
    my ($nodsp)      =  (shift)    ;
    my (@dspcfgs)    = @{(shift)}  ;
    my (@dspos)      = @{(shift)}  ;
    my (@dsptemps)   = @{(shift)}  ;
    my ($comment)                  ;
    my ($export)                   ;
    my ($equal)                    ;
    my ($make)                     ;

    my ($cfgfile)   = &GetMultiMakeFile () ;

    open ($filehandle, ">$cfgfile") || die "!! Couldn't open file: $cfgfile\n" ;

    if ($DSPLINK_BUILDOS eq 'WINDOWS') {
        $comment = "rem" ;
        $export  = "set" ;
        $equal   = "=" ;
        $make    = "gmake -f Makefile \%1 \%2\n" ;
    }
    else {
        $comment = "#" ;
        $export  = "export" ;
        $equal   = "=" ;
        $make    = "gmake -f Makefile \$\@\n" ;
    }

    $text .= "echo off\n" ;
    $text .= $comment . "   =========================================================\n" ;
    $text .= $comment . "   DSP/BIOS LINK Configuration file.\n" ;
    $text .= $comment . "\n" ;
    $text .= $comment . "   CAUTION! This is a generated file.\n" ;
    $text .= $comment . "            All changes will be lost.\n" ;
    $text .= $comment . "\n" ;
    $text .= $comment . "   This file was generated on " . &GetDateString() . "\n" ;
    $text .= $comment . "   =========================================================\n" ;
    $text .= "echo on\n" ;
    for (my $i = 0 ; $i < $nodsp ; $i += 1) {
        $text .= "echo off\n" ;
        $text .= $comment . "   =========================================================\n" ;
        $text .= $comment . "   Build DSP side binary for " . $dspcfgs [$i]{'DSPID'} ."\n" ;
        $text .= $comment . "   =========================================================\n" ;
        $text .= $export  . " TI_DSPLINK_BUILD_DEFAULTDSP" . $equal  . $i . "\n" ;
        $text .= $export  . " TI_DSPLINK_DSPDEVICE" . $equal  . $dspcfgs [$i]{'DSPID'} . "\n" ;
        if($dspcfgs [$i]{'DSPID'} eq $dspcfgs [$i]{'DSPNAME'}) {
            $text .= $export . " TI_DSPLINK_DSPDEVICE_EXTERNAL" . $equal . "\n" ;
        }
        else {
            $text .= $export . " TI_DSPLINK_DSPDEVICE_EXTERNAL" . $equal . $dspcfgs [$i]{'DSPNAME'} . "\n" ;
        }
        $text .= $export  . " TI_DSPLINK_DSPDISTRIBUTION" . $equal  . &GetDspMakeFile (\%{$dspcfgs [$i]}, \%{$dspos [$i]}) . "\n" ;
        $text .= $export  . " TI_DSPLINK_DSPOS" . $equal  . $dspos [$i]{'TYPE'} . "\n" ;
        $text .= $export  . " TI_DSPLINK_DSPOSVERSION" . $equal  . $dspos [$i]{'PREFIX'} . "\n" ;
        $text .= $export  . " TI_DSPLINK_PROCID" . $equal  . $i . "\n" ;
        $text .= $export  . " TI_DSPLINK_DSPDEVICECLASS" . $equal  . $dspcfgs [$i]{'DSPCLASS'} . "\n" ;
        $text .= $export  . " TI_DSPLINK_DSPTEMPPATH" . $equal  . $dsptemps [$i] . "\n" ;
        $text .= "echo on\n" ;
        $text .= $make ;
        $text .= "echo off\n" ;
        $text .= $export  . " TI_DSPLINK_BUILD_DEFAULTDSP" . $equal . "\n" ;
        $text .= "echo on\n" ;
    }
    $text .= "\n" ;
    $text .= "\n" ;

    print $filehandle $text ;

    close ($filehandle) ;
}


#   ============================================================================
#   func    Generate_systemc
#
#   desc    Generate CFG_system.c
#   ============================================================================
sub Generate_systemc {
    my ($text, $filehandle) ;
    my (%platform)   = %{(shift)}  ;
    my ($nodsp)      =  (shift)  ;
    my (@dspcfgs)    = @{(shift)}  ;
    my ($gpptemp)    =  (shift)  ;

    my ($cfgfile)   = &GetSystemCFile ($gpptemp) ;

    open ($filehandle, ">$cfgfile") || die "!! Couldn't open file: $cfgfile\n" ;

    $text .= "/*   =========================================================\n" ;
    $text .= " *   DSP/BIOS LINK Configuration file.\n" ;
    $text .= " *\n" ;
    $text .= " *   CAUTION! This is a generated file.\n" ;
    $text .= " *            All changes will be lost.\n" ;
    $text .= " *\n" ;
    $text .= " *   This file was generated on " . &GetDateString() . "\n" ;
    $text .= " *   Target platform for DSP/BIOS LINK: " . $platform {'ID'} . "\n" ;
    $text .= " *   =========================================================\n" ;
    $text .= " */\n" ;
    $text .= "\n" ;


    $text .= "/*  ----------------------------------- DSP/BIOS LINK Headers       */\n" ;
    $text .= "#include <dsplink.h>\n" ;
    $text .= "#include <procdefs.h>\n" ;
    $text .= "#include <_dsplink.h>\n\n" ;

    $text .= "#if defined (__cplusplus)\n" ;
    $text .= "EXTERN \"C\" {\n" ;
    $text .= "#endif /* defined (__cplusplus) */\n\n" ;


    $text .= "extern LINKCFG_Gpp LINKCFG_gppObject ;\n" ;
    for (my $i = 0 ; $i < $nodsp ; $i += 1) {
        $text .= "extern LINKCFG_DspConfig ". $dspcfgs [$i]{'DSPID'} . "_" . $dspcfgs [$i]{'PHYTYPE'} . "_Config ;\n" ;
    }

    $text .= "\n/** ============================================================================\n" ;
    $text .= " *  \@name   LINKCFG_config\n" ;
    $text .= " *\n" ;
    $text .= " *  \@desc   Configuration object for the overall system.\n" ;
    $text .= " *  ============================================================================\n" ;
    $text .= " */\n" ;
    $text .= "LINKCFG_Object LINKCFG_config = { &LINKCFG_gppObject,\n" ;
    $text .= "                                        {" ;
    for (my $i = 0 ; $i < $nodsp ; $i += 1) {
        $text .= "&". $dspcfgs [$i]{'DSPID'} . "_" . $dspcfgs [$i]{'PHYTYPE'} ."_Config, " ;
    }
    $text .= "} ,\n                                      }  ;\n" ;
    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "#if defined (__cplusplus)\n" ;
    $text .= "}\n" ;
    $text .= "#endif /* defined (__cplusplus) */\n" ;

    print $filehandle $text ;

    close ($filehandle) ;
}


#   ============================================================================
#   func    GetGppXdcFilePath
#
#   desc    Returns the full path to Gpp side global.xdc file
#   ============================================================================
sub GetGppXdcFilePath
{
    my ($str) ;

    if (   $DSPLINK_BUILDOS eq 'LINUX'
        || $DSPLINK_BUILDOS eq 'CYGWIN') {
        $str = &GetRootDir ("gpp") . "/Global.xdc";
    }
    if (   $DSPLINK_BUILDOS eq 'WINDOWS'
        || $DSPLINK_BUILDOS eq 'MSWIN32') {
        $str = &GetRootDir ("gpp") . "\\Global.xdc" ;
    }

    return $str ;
}

#   ============================================================================
#   func    GetDspXdcFilePath
#
#   desc    Returns the full path to Dsp side global.xdc file
#   ============================================================================
sub GetDspXdcFilePath
{
    my ($str) ;

    if (   $DSPLINK_BUILDOS eq 'LINUX'
        || $DSPLINK_BUILDOS eq 'CYGWIN') {
        $str = &GetRootDir ("dsp") . "/Global.xdc";
    }
    if (   $DSPLINK_BUILDOS eq 'WINDOWS'
        || $DSPLINK_BUILDOS eq 'MSWIN32') {
        $str = &GetRootDir ("dsp") . "\\Global.xdc" ;
    }

    return $str ;
}

#   ============================================================================
#   func    GenerateRTSCGppCfgFile
#
#   desc    Generates Gpp side global.xdc file
#   ============================================================================
sub GenerateRTSCGppCfgFile
{
    my (%platform)   = %{(shift)}  ;
    my (%gppos)      = %{(shift)}  ;
    my (%components) = %{(shift)}  ;
    my ($xdcfile)   = &GetGppXdcFilePath () ;
    my ($text)      = "" ;

    my ($filehandle) ;

    open ($filehandle, ">$xdcfile") || die "!! Couldn't open file: $xdcfile\n" ;

    $text .= "/*  =========================================================\n" ;
    $text .= " *  Gpp Side Global.xdc (RTSC specific file) \n" ;
    $text .= " *\n" ;
    $text .= " *  CAUTION! This is a generated file.\n" ;
    $text .= " *           All changes will be lost.\n" ;
    $text .= " *\n" ;
    $text .= " *  This file was generated on " . &GetDateString() . "\n" ;
    $text .= " *  =========================================================\n" ;
    $text .= " */\n" ;
    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "metaonly module Global\n" ;
    $text .= "{\n" ;
    $text .= "    enum RuntimeOS {\n" ;
    $text .= "        LINUX,\n" ;
    $text .= "        PROS,\n" ;
    $text .= "        WINCE\n" ;
    $text .= "    };\n\n" ;
    $text .= "    config RuntimeOS runtimeOS = " . uc($gppos{'TYPE'}) . " ;\n\n" ;

    $text .= "    enum DeviceName{\n" ;
    for (my $i = 0 ; $i < keys (%CFG_PLATFORMS)- 1 ; $i++) {
        $text .= "\t" . $CFG_PLATFORMS{$i}{'ID'} . " ,\n" ;
    }
         $text .= "\t" . $CFG_PLATFORMS{keys (%CFG_PLATFORMS)- 1}{'ID'} . " \n" ;
    $text .= "    };\n\n" ;

    $text .= "    config DeviceName deviceName  = undefined " . " ;\n\n" ;

    $text .= "    config Bool useProc    = " . $components {'USE_PROC'}  . " ;\n" ;
    $text .= "    config Bool usePool    = " . $components {'USE_POOL'}  . " ;\n" ;
    $text .= "    config Bool useNotify  = " . $components {'USE_NOTIFY'}  . " ;\n" ;
    $text .= "    config Bool useMpcs    = " . $components {'USE_MPCS'}  . " ;\n" ;
    $text .= "    config Bool useRingio  = " . $components {'USE_RINGIO'}  . " ;\n" ;
    $text .= "    config Bool useMplist  = " . $components {'USE_MPLIST'}  . " ;\n" ;
    $text .= "    config Bool useMsgq    = " . $components {'USE_MSGQ'}  . " ;\n" ;
    $text .= "    config Bool useChnl    = " . $components {'USE_CHNL'}  . " ;\n" ;
    $text .= "    string getIncludes() ;\n" ;
    $text .= "    string getDefines() ;\n\n" ;
    $text .= "\n" ;
    $text .= "}\n" ;

    print $filehandle $text ;

    close ($filehandle) ;
}

#   ============================================================================
#   func    GenerateRTSCDspCfgFile
#
#   desc    Generates Dsp side global.xdc file
#   ============================================================================
sub GenerateRTSCDspCfgFile
{
    my (%platform)   = %{(shift)}  ;
    my (%components) = %{(shift)}  ;
    my (@dspcfgs)    = @{(shift)}  ;
    my ($xdcfile)    = &GetDspXdcFilePath () ;
    my ($text)       = "" ;
    my $status       = 0 ;
    my $index        = 0 ;
    my @dublicate_device ;


    my ($filehandle) ;

    open ($filehandle, ">$xdcfile") || die "!! Couldn't open file: $xdcfile\n" ;

    $text .= "/*  =========================================================\n" ;
    $text .= " *  Dsp Side Global.xdc (RTSC specific file) \n" ;
    $text .= " *\n" ;
    $text .= " *  CAUTION! This is a generated file.\n" ;
    $text .= " *           All changes will be lost.\n" ;
    $text .= " *\n" ;
    $text .= " *  This file was generated on " . &GetDateString() . "\n" ;
    $text .= " *  =========================================================\n" ;
    $text .= " */\n" ;
    $text .= "\n" ;
    $text .= "\n" ;
    $text .= "metaonly module Global\n" ;
    $text .= "{\n" ;
    $text .= "\n" ;
    $text .= "    string getIncludes() ;\n" ;
    $text .= "    string getDefines() ;\n\n" ;
    $text .= "\n" ;

    $text .= "    enum DeviceName{\n" ;
    for (my $i = 0 ; $i < keys (%CFG_DSPCFGS) -1 ; $i++) {
        for (my $j = ($i+1) ; $j < keys (%CFG_DSPCFGS) ; $j++) {
            if(($CFG_DSPCFGS{$i}{'DSPNAME'}) eq ($CFG_DSPCFGS{$j}{'DSPNAME'})) {
                $status = 1 ;
                $dublicate_device[$index] = $i ;
                $index++ ;
                goto EXIT ;
            }
            else {
                $status = 0 ;
            }
        }
    EXIT:
        if ($status == 0) {
            $text .= "\t" . $CFG_DSPCFGS{$i}{'DSPNAME'} . " ,\n" ;
        }
    }

    $text .= "\t" . $CFG_DSPCFGS{keys (%CFG_DSPCFGS) - 1 }{'DSPNAME'} . " \n" ;

    $text .= "    };\n\n" ;

    $text .= "    config DeviceName deviceName  = undefined " . " ;\n\n" ;

    $text .= "    config Bool useProc    = " . $components {'USE_PROC'}  . " ;\n" ;
    $text .= "    config Bool usePool    = " . $components {'USE_POOL'}  . " ;\n" ;
    $text .= "    config Bool useNotify  = " . $components {'USE_NOTIFY'}  . " ;\n" ;
    $text .= "    config Bool useMpcs    = " . $components {'USE_MPCS'}  . " ;\n" ;
    $text .= "    config Bool useRingio  = " . $components {'USE_RINGIO'}  . " ;\n" ;
    $text .= "    config Bool useMplist  = " . $components {'USE_MPLIST'}  . " ;\n" ;
    $text .= "    config Bool useMsgq    = " . $components {'USE_MSGQ'}  . " ;\n" ;
    $text .= "    config Bool useChnl    = " . $components {'USE_CHNL'}  . " ;\n" ;
    $text .= "}\n" ;

    print $filehandle $text ;

    close ($filehandle) ;
}


#   ============================================================================
#   func    help
#
#   desc    help function
#   ============================================================================
sub help {
    my $text ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        if ($ARGV [$key] =~ m/--help/) {
            $text .= "Following options are supported:\n" ;
            $text .= " Options          | Description\n" ;
            $text .= "================================================================================\n" ;
            $text .= "1. platform       | Base Platform to used by DSPLink\n" ;
            $text .= "                  | For example: --platform=DAVINCI\n" ;
            $text .= "================================================================================\n" ;
            $text .= "2. No of DSPs     | Number of dsps to controlled by the DSPLink\n" ;
            $text .= "                  | For example: --nodsp=1\n" ;
            $text .= "================================================================================\n" ;
            $text .= "3. DSPCFG#        | DSP to be controlled (DSP procId #) & its Physical Interface\n" ;
            $text .= "                  | For example: --dspcfg_1=DM6446GEMSHMEM\n" ;
            $text .= "================================================================================\n" ;
            $text .= "4. DSP OS#        | DSP OS used by DSP#\n" ;
            $text .= "                  | For example: --dspos_1=DSPBIOS5XX\n" ;
            $text .= "================================================================================\n" ;
            $text .= "5. GPP OS         | GPP OS to be used by DSPLink\n" ;
            $text .= "                  | For example: --gppos=MVL4G\n" ;
            $text .= "================================================================================\n" ;
            $text .= "6. COMPONENTS     | Components/Modules to be used by DSPLink\n" ;
            $text .= "                  | For example: --comps=lmrc\n" ;
            $text .= "================================================================================\n" ;
            $text .= "7. TRACE          | Enable tracing for DSPLink (optional)\n" ;
            $text .= "                  | For example: --trace=1\n" ;
            $text .= "================================================================================\n" ;
            $text .= "8. GPP Temp Dir   | Temporary path for GPP binaries and libraries generation\n" ;
            $text .= "                    (optional)\n" ;
            $text .= "                  | For example: --gpp_temp=/tmp/gpptemp\n" ;
            $text .= "================================================================================\n" ;
            $text .= "9. DSP# Temp Dir  | Temporary path for DSP binaries and libraries generation\n" ;
            $text .= "                    (optional)\n" ;
            $text .= "                  | For example: --dsp1_temp=/tmp/dsp1temp\n" ;
            $text .= "================================================================================\n" ;
            $text .= "10. Legacy        | Enable legacy support (optional)\n" ;
            $text .= "                  | For example: --legacy=1\n" ;
            $text .= "================================================================================\n" ;
            $text .= "11. dspMode       | Enable the TSK mode.By default SWI mode is enable.\n" ;
            $text .= "                  | For example: --DspTskMode=1\n" ;
            $text .= "================================================================================\n" ;
            $text .= "12. filesystem    | Filesystem type to be used by GPP OS (exists only on few\n" ;
            $text .= "                    platform)\n" ;
            $text .= "                  | For example: --fs=PSEUDOFS\n" ;
            $text .= "================================================================================\n" ;
            $text .= "*** Command specific helps are available by typing --command, for example type\n";
            $text .= " --platform to get a list of supported PLatforms\n" ;
            die  $text ;
        }
    }
}


#   ============================================================================
#   func    validateOptions
#
#   desc    Valudates the options provided
#   ============================================================================
sub validateOptions {
    my $terminate = 0 ;

    for (my $key = 0 ; $key < ($#ARGV + 1) ; $key += 1) {
        my $count = ($ARGV [$key] =~ tr/-//) ;
        if ($count ne 2) {
           print "invalid options \'" . $ARGV [$key] . "\'\n" ;
           $terminate = 1 ;
        }
    }

    if ($terminate eq 1) {
        exit ;
    }
}


#   ============================================================================
#   func    main
#
#   desc    Main function
#   ============================================================================
sub main {
    my (%platform, $nodsp, @dspcfgs, @dsposes, %gppos) ;
    my (%components, %loader, $trace, %fs, @dsptemps, $gpptemp) ;
    my ($legacy) ;
    my ($dmaRequired) ;
    my ($dspMode) ;

# Check if the option contains help string
    &help ;
    &validateOptions ;

    print "=========================================================\n" ;
    print "Chosen configuration is as follows:\n" ;
    %platform = &CFG_getPlatformArg ;
    $nodsp    = &CFG_getNoDspsArg   ;

    for (my $key = 0 ; $key < $nodsp ; $key +=1 ) {
        my %dspcfg  = &CFG_getDspCfgArg ($key, \%platform)   ;
        my %dspos   = &CFG_getDspOsArg  ($key, \%platform, \%dspcfg)   ;
        my $dsptemp = &CFG_getDspTempPathArg ($key) ;

         $dspcfgs [$key] = \%dspcfg ;
         $dsposes [$key] = \%dspos ;
         $dsptemps [$key] = $dsptemp ;
    }

     %gppos      = &CFG_getGppOsArg (\%platform, \@dspcfgs) ;
     $gpptemp     = &CFG_getGppTempPathArg () ;
     %components = &CFG_getComponentArg (\%gppos) ;
     if (defined ($gppos {'FSS'})) {
         %fs         = &CFG_checkAndGetFileSytemArg (\%gppos) ;
     }
     $trace      = &CFG_getTraceArg ;
     $legacy     = &CFG_getLegacySupportArg ;
     $dspMode    = &CFG_getDspMode ;
     if ($platform {'ID'} eq 'LINUXPC') {
         $dmaRequired     = &CFG_getDMASupportArg ;
     }
    print "=========================================================\n" ;
    print "\nConfiguration done successfully!!\n" ;
    print "Generating CURRENTCFG.MK file...\n" ;
    &Generate_Currentcfgmk (\%platform, $nodsp, \@dspcfgs, \@dsposes,
                            \%gppos, \%components, \%loader, \%fs, $trace,
                            \@dsptemps, $gpptemp, $legacy, $dspMode, $dmaRequired ) ;
    print "Generating multimake script...\n" ;
    &Generate_multiMake(\%platform, $nodsp, \@dspcfgs, \@dsposes, \@dsptemps) ;
    print "Generating CFG_system.c File...\n" ;
    &Generate_systemc (\%platform, $nodsp, \@dspcfgs, $gpptemp) ;
    print "Generating GPP RTSC xdc file...\n" ;
    &GenerateRTSCGppCfgFile (\%platform, \%gppos, \%components) ;
    print "Generating DSP RTSC xdc file...\n" ;
    &GenerateRTSCDspCfgFile (\%platform, \%components, \@dspcfgs) ;
    print "=========================================================\n" ;

    print "Please edit the following files for toolchains, kernel sources, etc changes.\n" ;
    print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "make" . $DIRSEP . $gppos {'TYPE'} . $DIRSEP .&GetGppMakeFile (\%platform, \%gppos) . ".mk\n" ;
    if ($platform {'ID'} eq 'OMAP3530') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'OMAPL138') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'DA850') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'DAVINCI') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'DAVINCIHD') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'DA8XX') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    if ($platform {'ID'} eq 'OMAPL1XX') {
        print "GPP side distribution file: \$DSPLINK" . $DIRSEP . "gpp" . $DIRSEP. "src" . $DIRSEP. "Rules.mk\n" ;
    }
    for (my $key = 0 ; $key < $nodsp ; $key +=1 ) {
        print "DSP side distribution file: \$DSPLINK" . $DIRSEP . "make" . $DIRSEP. "DspBios" . $DIRSEP . &GetDspMakeFile (\%{$dspcfgs [$key]}, \%{$dsposes [$key]}) . ".mk\n" ;
    }
    print "==========================================================\n" ;
    print "Users consuming DSPLINK as XDC package, Need to do the following.\n" ;
    print "cd into the \$(DSPLINK)" . $DIRSEP . "dsp directory and run: \n" ;
    print " \$(XDC_INSTALL_DIR)" . $DIRSEP . "xdc clean \n \$(XDC_INSTALL_DIR)" . $DIRSEP . "xdc .interfaces \n" ;
    print "cd into the \$(DSPLINK)" . $DIRSEP . "gpp directory and run: \n" ;
    print " \$(XDC_INSTALL_DIR)" . $DIRSEP . "xdc clean \n \$(XDC_INSTALL_DIR)" . $DIRSEP . "xdc .interfaces \n" ;
    print "==========================================================\n" ;
}

&main ;
