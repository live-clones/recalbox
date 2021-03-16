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

#loop system video : -1 = loop forever, 0 = no loop, 1 = loop once, 2 = loop twice, etc...
Loop=-1
#loop default system video
DefaultLogoLoop=-1

#change to your own videos by modifying each "LogoFile" variable.

selectSystemVideo()
{
#select systems -> videos to display
case "${TFTESSystem}" in
    default)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;;
    # Nintendo
    snes)           LogoFile='RBvideos/introRecalboxSNES.mp4' && Loop=-1  ;;
    nes)            LogoFile='NES/VideoSnaps/logo.mp4' && Loop=-1  ;;
    #n64)            LogoFile='N64/VideoSnaps/logo.mp4' && Loop=-1  ;;
    n64)            LogoFile='RBvideos/introRecalboxN64.mp4' && Loop=-1  ;;
    gba)            LogoFile='RBvideos/introRecalboxGBA.mp4' && Loop=-1  ;;
    gb)             LogoFile='RBvideos/introRecalboxGB.mp4' && Loop=-1  ;;
    gbc)            LogoFile='RBvideos/introRecalboxGBC.mp4' && Loop=-1  ;;
    fds)            LogoFile='FDS/VideoSnaps/logo.mp4' && Loop=-1  ;;
    virtualboy)     LogoFile='VB/VideoSnaps/logo.mp4' && Loop=-1  ;;
    gamecube)       LogoFile='RBvideos/introRecalboxGameCube.mp4' && Loop=-1  ;;
    wii)            LogoFile='Wii/VideoSnaps/logo.mp4' && Loop=-1  ;;
    nds)            LogoFile='NDS/VideoSnaps/logo.mp4' && Loop=-1  ;;
    pokemini)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    satellaview)    LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    sufami)         LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    gw)             LogoFile='GW/VideoSnaps/logo.mp4' && Loop=-1  ;;
    # Sega
    sg1000)          LogoFile='SEGA/SG-1000/VideoSnaps/logo.mp4' && Loop=-1  ;;
    #mastersystem)    LogoFile='SEGA/MasterSystem/VideoSnaps/logo.mp4' && Loop=-1  ;;
    mastersystem)    LogoFile='RBvideos/introRecalboxSMS.mp4' && Loop=-1  ;;
    megadrive)       LogoFile='RBvideos/introRecalboxMegadrive.mp4' && Loop=-1  ;;
    gamegear)        LogoFile='SEGA/Gamegear/VideoSnaps/logo.mp4' && Loop=-1  ;;
    sega32x)         LogoFile='SEGA/Sega32X/VideoSnaps/logo.mp4' && Loop=-1  ;;
    segacd)          LogoFile='SEGA/SegaCD/VideoSnaps/logo.mp4' && Loop=-1  ;;
    #dreamcast)       LogoFile='SEGA/Dreamcast/VideoSnaps/logo.mp4' && Loop=-1  ;;
    dreamcast)       LogoFile='RBvideos/introRecalboxDCBlue.mp4' && Loop=-1  ;;
    naomi)           LogoFile='RBvideos/introRecalboxNaomi.mp4' && Loop=-1  ;;
    atomiswave)      LogoFile='RBvideos/introRecalboxAtomiswave.mp4' && Loop=-1  ;;
    # Sony
    #psx)             LogoFile='PSX/VideoSnaps/logo.mp4' && Loop=-1  ;;
    psx)             LogoFile='RBvideos/introRecalboxPS1.mp4' && Loop=-1  ;;
    psp)             LogoFile='PSP/VideoSnaps/logo.mp4' && Loop=-1  ;;
    # Atari
    atarist)         LogoFile='ATARI/AtariST/VideoSnaps/logo.mp4' && Loop=-1  ;;
    atari800)        LogoFile='ATARI/Atari800/VideoSnaps/logo.mp4' && Loop=-1  ;;
    lynx)            LogoFile='ATARI/Lynx/VideoSnaps/logo.mp4' && Loop=-1  ;;
    jaguar)          LogoFile='ATARI/Jaguar/VideoSnaps/logo.mp4' && Loop=-1  ;;
    atari2600)       LogoFile='ATARI/A2600/VideoSnaps/logo.mp4' && Loop=-1  ;;
    atari5200)       LogoFile='ATARI/A5200/VideoSnaps/logo.mp4' && Loop=-1  ;;
    atari7800)       LogoFile='ATARI/A7800/VideoSnaps/logo.mp4' && Loop=-1  ;;
    # Amiga
    amiga)           LogoFile='AMIGA/VideoSnaps/logo.mp4' && Loop=-1  ;;
    amiga600)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    amiga1200)       LogoFile='AMIGA1200/VideoSnaps/logo.mp4' && Loop=-1  ;;
    amigacd32)       LogoFile='AMIGACD32/VideoSnaps/logo.mp4' && Loop=-1  ;;
    amigacdtv)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" && Loop=-1  ;; # tbd
    # NEC
    #pcengine)        LogoFile='PCE/VideoSnaps/logo.mp4' && Loop=-1  ;;
    #pcenginecd)      LogoFile='PCE/VideoSnaps/logo.mp4' && Loop=-1  ;;
    pcengine|pcenginecd)        LogoFile='RBvideos/introRecalboxPCEngine.mp4' && Loop=-1  ;;
    supergrafx)      LogoFile='SuperGrafx/VideoSnaps/logo.mp4' && Loop=-1  ;;
    pcfx)            LogoFile='PCFX/VideoSnaps/logo.mp4' && Loop=-1  ;;
    # SNK
    #neogeo)          LogoFile='NeoGeo/VideoSnaps/logo.mp4' && Loop=-1  ;;
    neogeo|neogeocd) LogoFile='RBvideos/introRecalboxNeoGeo.mp4' && Loop=-1  ;;
    #neogeocd)        LogoFile='NeoGeo/VideoSnaps/logo.mp4' && Loop=-1  ;;
    ngp)             LogoFile='NeoGeoPocket/VideoSnaps/logo.mp4' && Loop=-1 ;;
    ngpc)            LogoFile='NeoGeoPocket/VideoSnaps/logo.mp4' && Loop=-1 ;;
    # Arcade             
    mame)            LogoFile='RBvideos/introRecalboxMame.mp4' && Loop=-1  ;;
    advancemame)     LogoFile='RBvideos/introRecalboxMame.mp4' && Loop=-1  ;;
    #fba)             LogoFile='FBA/VideoSnaps/logo.mp4' && Loop=-1  ;;
    fbneo|fba|fba_libretro)             LogoFile='RBvideos/introRBCapcom.mp4' && Loop=-1  ;;
    #fba_libretro)    LogoFile='FBA/VideoSnaps/logo.mp4' && Loop=-1  ;;
    #fba_libretro)    LogoFile='RBvideos/introRBCapcom.mp4' && Loop=-1  ;;
    daphne)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    # Various computers  
    amstradcpc)      LogoFile='CPC/VideoSnaps/logo.mp4' && Loop=-1  ;;
    msx|msx1|msx2)   LogoFile='MSX/VideoSnaps/logo.mp4' && Loop=-1  ;;
    apple2)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    zxspectrum)      LogoFile='ZXSpectr/VideoSnaps/logo.mp4' && Loop=-1   ;;
    o2em|odyssey2)        LogoFile='O2EM/VideoSnaps/logo.mp4' && Loop=-1  ;;
    zx81)            LogoFile='ZX81/VideoSnaps/logo.mp4' && Loop=-1  ;;
    dos)             LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    c64)             LogoFile='C64/VideoSnaps/logo.mp4' && Loop=-1  ;;
    x68000)          LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    thomson)         LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    # Various consoles   
    vectrex)         LogoFile='Vectrex/VideoSnaps/logo.mp4' && Loop=-1  ;;
    wswan)           LogoFile='WS/VideoSnaps/logo.mp4' && Loop=-1  ;;
    wswanc)          LogoFile='WS/VideoSnaps/logo.mp4' && Loop=-1  ;;
    intellivision)   LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    channelf)        LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    colecovision)    LogoFile='ColecoVision/VideoSnaps/logo.mp4' && Loop=-1  ;;
    3do)             LogoFile='3DO/VideoSnaps/logo.mp4' && Loop=-1  ;;
    # Misc               
    ports|doom)            LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;;
    #doom)            LogoFile="recalbox.png" && Loop=0 ;;
    cavestory)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    lutro)           LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    imageviewer)     LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}" ;; # tbd
    scummvm)         LogoFile='SCUMMVM/VideoSnaps/logo.mp4' && Loop=-1  ;;
    moonlight)       LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd
    #kodi)            LogoFile='RBvideos/introRecalboxKodi.mp4' && Loop=0  ;;
    kodi)            LogoFile='RBvideos/introRecalboxKodiNoFade.mp4' && Loop=0  ;;
    *)               LogoFile="${DefaultLogo}" && Loop="${DefaultLogoLoop}"  ;; # tbd    
esac
}
