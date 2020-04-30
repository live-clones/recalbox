###################################
#      Useful Functions           #
###################################

#some useful video constants
LogoFolder='/recalbox/share_init/system/tft_logos'
#DefaultLogo='RBvideos/recalboxIntro.mp4'
DefaultLogo='RBvideos/introRecalboxWinners.mp4'
DefaultStartLogo='RBvideos/recalboxHologram.mp4'
DefaultStopLogo='RBvideos/recalboxIntroExplode.mp4'
DefaultImage='noimage.png'
LogoFile=""
Loop=false
#defined but not actually used
DefaultLogoLoop=false

#change to your own videos by modifying each "LogoFile" variable.

selectSystemVideo()
{
#select systems -> videos to display
case "${TFTESSystem}" in
    default)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;;
    # Nintendo
    snes)           LogoFile='RBvideos/introRecalboxSNES.mp4' && Loop=false  ;;
    nes)            LogoFile='NES/VideoSnaps/logo.mp4' && Loop=true  ;;
    #n64)            LogoFile='N64/VideoSnaps/logo.mp4' && Loop=true  ;;
    n64)            LogoFile='RBvideos/introRecalboxN64.mp4' && Loop=true  ;;
    gba)            LogoFile='GBA/VideoSnaps/logo.mp4' && Loop=true  ;;
    gb)             LogoFile='GBx/VideoSnaps/logo.mp4' && Loop=false  ;;
    gbc)            LogoFile='GBx/VideoSnaps/logo.mp4' && Loop=false  ;;
    fds)            LogoFile='FDS/VideoSnaps/logo.mp4' && Loop=true  ;;
    virtualboy)     LogoFile='VB/VideoSnaps/logo.mp4' && Loop=true  ;;
    gamecube)       LogoFile='GC/VideoSnaps/logo.mp4' && Loop=true  ;;
    wii)            LogoFile='Wii/VideoSnaps/logo.mp4' && Loop=false  ;;
    nds)            LogoFile='NDS/VideoSnaps/logo.mp4' && Loop=false  ;;
    pokemini)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    satellaview)    LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    sufami)         LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    gw)             LogoFile='GW/VideoSnaps/logo.mp4' && Loop=true  ;;
    # Sega
    sg1000)          LogoFile='SEGA/SG-1000/VideoSnaps/logo.mp4' && Loop=true  ;;
    #mastersystem)    LogoFile='SEGA/MasterSystem/VideoSnaps/logo.mp4' && Loop=false  ;;
    mastersystem)    LogoFile='RBvideos/introRecalboxSMS.mp4' && Loop=false  ;;
    megadrive)       LogoFile='RBvideos/introRecalboxMegadrive.mp4' && Loop=true  ;;
    gamegear)        LogoFile='SEGA/Gamegear/VideoSnaps/logo.mp4' && Loop=true  ;;
    sega32x)         LogoFile='SEGA/Sega32X/VideoSnaps/logo.mp4' && Loop=true  ;;
    segacd)          LogoFile='SEGA/SegaCD/VideoSnaps/logo.mp4' && Loop=true  ;;
    #dreamcast)       LogoFile='SEGA/Dreamcast/VideoSnaps/logo.mp4' && Loop=true  ;;
    dreamcast)       LogoFile='RBvideos/introRecalboxDCBlue.mp4' && Loop=true  ;;
    # Sony
    #psx)             LogoFile='PSX/VideoSnaps/logo.mp4' && Loop=false  ;;
    psx)             LogoFile='RBvideos/introRecalboxPS1.mp4' && Loop=false  ;;
    psp)             LogoFile='PSP/VideoSnaps/logo.mp4' && Loop=true  ;;
    # Atari
    atarist)         LogoFile='ATARI/AtariST/VideoSnaps/logo.mp4' && Loop=true  ;;
    atari800)        LogoFile='ATARI/Atari800/VideoSnaps/logo.mp4' && Loop=true  ;;
    lynx)            LogoFile='ATARI/Lynx/VideoSnaps/logo.mp4' && Loop=true  ;;
    jaguar)          LogoFile='ATARI/Jaguar/VideoSnaps/logo.mp4' && Loop=true  ;;
    atari2600)       LogoFile='ATARI/A2600/VideoSnaps/logo.mp4' && Loop=true  ;;
    atari5200)       LogoFile='ATARI/A5200/VideoSnaps/logo.mp4' && Loop=true  ;;
    atari7800)       LogoFile='ATARI/A7800/VideoSnaps/logo.mp4' && Loop=true  ;;
    # Amiga
    amiga)           LogoFile='AMIGA/VideoSnaps/logo.mp4' && Loop=false  ;;
    amiga600)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    amiga1200)       LogoFile='AMIGA1200/VideoSnaps/logo.mp4' && Loop=true  ;;
    amigacd32)       LogoFile='AMIGACD32/VideoSnaps/logo.mp4' && Loop=true  ;;
    amigacdtv)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" && Loop=false  ;; # tbd
    # NEC
    #pcengine)        LogoFile='PCE/VideoSnaps/logo.mp4' && Loop=true  ;;
    #pcenginecd)      LogoFile='PCE/VideoSnaps/logo.mp4' && Loop=false  ;;
    pcengine|pcenginecd)        LogoFile='RBvideos/introRecalboxPCEngine.mp4' && Loop=true  ;;
    supergrafx)      LogoFile='SuperGrafx/VideoSnaps/logo.mp4' && Loop=true  ;;
    pcfx)            LogoFile='PCFX/VideoSnaps/logo.mp4' && Loop=true  ;;
    # SNK
    #neogeo)          LogoFile='NeoGeo/VideoSnaps/logo.mp4' && Loop=false  ;;
    neogeo|neogeocd) LogoFile='RBvideos/introRecalboxNeoGeo.mp4' && Loop=false  ;;
    #neogeocd)        LogoFile='NeoGeo/VideoSnaps/logo.mp4' && Loop=false  ;;
    ngp)             LogoFile='NeoGeoPocket/VideoSnaps/logo.mp4' && Loop=false ;;
    ngpc)            LogoFile='NeoGeoPocket/VideoSnaps/logo.mp4' && Loop=false ;;
    # Arcade             
    mame)            LogoFile='RBvideos/introRecalboxMame.mp4' && Loop=true  ;;
    advancemame)     LogoFile='MAME/VideoSnaps/logo.mp4' && Loop=true  ;;
    #fba)             LogoFile='FBA/VideoSnaps/logo.mp4' && Loop=false  ;;
    fba)             LogoFile='RBvideos/introRBCapcom.mp4' && Loop=false  ;;
    #fba_libretro)    LogoFile='FBA/VideoSnaps/logo.mp4' && Loop=false  ;;
    fba_libretro)    LogoFile='RBvideos/introRBCapcom.mp4' && Loop=false  ;;
    daphne)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    # Various computers  
    amstradcpc)      LogoFile='CPC/VideoSnaps/logo.mp4' && Loop=false  ;;
    msx|msx1|msx2)   LogoFile='MSX/VideoSnaps/logo.mp4' && Loop=true  ;;
    apple2)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    zxspectrum)      LogoFile='ZXSpectr/VideoSnaps/logo.mp4' && Loop=true   ;;
    o2em|odyssey2)        LogoFile='O2EM/VideoSnaps/logo.mp4' && Loop=true  ;;
    zx81)            LogoFile='ZX81/VideoSnaps/logo.mp4' && Loop=true  ;;
    dos)             LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    c64)             LogoFile='C64/VideoSnaps/logo.mp4' && Loop=false  ;;
    x68000)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    thomson)         LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    # Various consoles   
    vectrex)         LogoFile='Vectrex/VideoSnaps/logo.mp4' && Loop=true  ;;
    wswan)           LogoFile='WS/VideoSnaps/logo.mp4' && Loop=true  ;;
    wswanc)          LogoFile='WS/VideoSnaps/logo.mp4' && Loop=true  ;;
    intellivision)   LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    channelf)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    colecovision)    LogoFile='ColecoVision/VideoSnaps/logo.mp4' && Loop=false  ;;
    3do)             LogoFile='3DO/VideoSnaps/logo.mp4' && Loop=true  ;;
    # Misc               
    doom)             LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;;
    cavestory)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    lutro)           LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    imageviewer)     LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    scummvm)         LogoFile='SCUMMVM/VideoSnaps/logo.mp4' && Loop=false  ;;
    moonlight)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    kodi)            LogoFile='Kodi/logo.png' && Loop=false  ;;
    *)                   
esac
}
