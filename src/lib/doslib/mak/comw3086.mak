# do not run directly, use make.sh

CFLAGS_1 =
!ifndef DEBUG
DEBUG = -d0
DSUFFIX =
!else
DSUFFIX = d
!endif

!ifndef CPULEV0
CPULEV0 = 0
!endif
!ifndef CPULEV2
CPULEV2 = 2
!endif
!ifndef CPULEV3
CPULEV3 = 3
!endif
!ifndef CPULEV4
CPULEV4 = 4
!endif
!ifndef CPULEV5
CPULEV5 = 5
!endif
!ifndef CPULEV6
CPULEV6 = 6
!endif
!ifndef TARGET86
TARGET86 = 86
!endif

!ifeq TARGET86 86
TARGET86_1DIGIT=0
!endif
!ifeq TARGET86 186
TARGET86_1DIGIT=1
!endif
!ifeq TARGET86 286
TARGET86_1DIGIT=2
!endif
!ifeq TARGET86 386
TARGET86_1DIGIT=3
!endif
!ifeq TARGET86 486
TARGET86_1DIGIT=4
!endif
!ifeq TARGET86 586
TARGET86_1DIGIT=5
!endif
!ifeq TARGET86 686
TARGET86_1DIGIT=6
!endif

# why is this even necessary? why does dumbshit Watcom insist on including the WINNT headers for Windows 3.1 builds?
WIN_INCLUDE=-i="$(%WATCOM)/h/win"

# NOTE TO SELF: If you compile using naive flags, your code will work under Windows 3.1, but will crash horribly under Windows 3.0.
#               Wanna know why? Because apparently Windows 3.0 doesn't maintain SS == DS, which Watcom assumes. So you always need
#               to specify the -zu and -zw switches. Even for Windows 3.1

TARGET_MSDOS = 16
TARGET_WINDOWS = 30
SUBDIR   = win30$(TARGET86_1DIGIT)$(MMODE)$(DSUFFIX)
RC       = wrc
CC       = wcc
LINKER   = wcl
WLINK_SYSTEM = windows
WLINK_CON_SYSTEM = windows
WLINK_DLL_SYSTEM = windows_dll
RCFLAGS  = -q -r -30 -bt=windows $(WIN_INCLUDE)
CFLAGS   = -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -$(CPULEV0) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bg
CFLAGS386= -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -$(CPULEV3) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bg
CFLAGS386_TO_586= -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -fp$(CPULEV5) -$(CPULEV5) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bg
CFLAGS386_TO_686= -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -fp$(CPULEV6) -$(CPULEV6) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bg
AFLAGS   = -e=2 -zq -zw -zu -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -wx -$(CPULEV0) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bg
NASMFLAGS= -DTARGET_MSDOS=16 -DTARGET_WINDOWS=$(TARGET_WINDOWS) -DMSDOS=1 -DTARGET86=$(TARGET86) -DMMODE=$(MMODE)

# NTS: Win16 apps do not have a console mode
RCFLAGS_CON = $(RCFLAGS)
CFLAGS_CON = $(CFLAGS)
CFLAGS386_CON = $(CFLAGS386)
# a 586 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_586_CON = $(CFLAGS386_TO_586)
# a 686 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_686_CON = $(CFLAGS386_TO_686)
AFLAGS_CON = $(AFLAGS)
NASMFLAGS_CON = $(NASMFLAGS)

RCFLAGS_DLL = -q -r -30 -bt=windows $(WIN_INCLUDE)
CFLAGS_DLL = -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -$(CPULEV0) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bd
CFLAGS386_DLL = -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -$(CPULEV3) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bd
CFLAGS386_TO_586_DLL = -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -fp$(CPULEV5) -$(CPULEV5) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bd
CFLAGS386_TO_686_DLL = -e=2 -zq -zu -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -oilrtfm -wx -fp$(CPULEV6) -$(CPULEV6) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bd
AFLAGS_DLL = -e=2 -zq -zw -zu -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -wx -$(CPULEV0) -dTARGET_MSDOS=16 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -D_WINDOWS_16_ -bd
NASMFLAGS_DLL = -DTARGET_MSDOS=16 -DTARGET_WINDOWS=$(TARGET_WINDOWS) -DMSDOS=1 -DTARGET86=$(TARGET86) -DMMODE=$(MMODE)

# macro to patch the EXE to the proper version
WIN_NE_SETVER_BUILD = ../../tool/chgnever.pl 3.0

!include "$(REL)$(HPS)mak$(HPS)bcommon.mak"
!include "common.mak"
!include "$(REL)$(HPS)mak$(HPS)dcommon.mak"

