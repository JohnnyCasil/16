# do not run directly, use make.sh

CFLAGS_1 =
!ifndef DEBUG
DEBUG = -d0
DSUFFIX =
!else
DSUFFIX = d
!endif

!ifndef CPULEV0
CPULEV0 = 3
!endif
!ifndef CPULEV2
CPULEV2 = 3
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
TARGET86 = 386
!endif

# Watcom does not have -fp4 because there's really nothing new to the 486 FPU to code home about
!ifndef CPULEV3F
CPULEV3F=$(CPULEV3)
!ifeq CPULEV3F 4
CPULEV3F=3
!endif
!endif

!ifndef CPULEV4F
CPULEV4F=$(CPULEV4)
!ifeq CPULEV4F 4
CPULEV4F=3
!endif
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

TARGET_MSDOS = 32
TARGET_WINDOWS = 31
WIN386   = 1
SUBDIR   = win38631
CC       = wcc386
RC       = wrc
LINKER   = wcl386
LDFLAGS  = # -ldos32a
WLINK_SYSTEM = win386
WLINK_CON_SYSTEM = win386
WLINK_DLL_SYSTEM = win386
RCFLAGS  = -q -r -31 -bt=windows $(WIN_INCLUDE)
CFLAGS   = -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
CFLAGS386= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
# a 586 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_586= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV5) -$(CPULEV5)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
# a 686 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_686= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV6) -$(CPULEV6)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
AFLAGS   = -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3) -dTARGET_MSDOS=32 -dMSDOS=1 -dWIN386=1 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
NASMFLAGS= -DTARGET_MSDOS=32 -DMSDOS=1 -DWIN386=1 -DTARGET86=$(TARGET86) -DTARGET_WINDOWS=$(TARGET_WINDOWS) -DMMODE=$(MMODE) -dWIN386=1

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

RCFLAGS_DLL  = -q -r -31 -bt=windows $(WIN_INCLUDE)
CFLAGS_DLL = -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
CFLAGS386_DLL= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
# a 586 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_586_DLL= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV5) -$(CPULEV5)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
# a 686 version of the build flags, so some OBJ files can target Pentium or higher instructions
CFLAGS386_TO_686_DLL= -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -oilrtfm -bt=windows -wx -fp$(CPULEV6) -$(CPULEV6)r -dTARGET_MSDOS=32 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dMSDOS=1 -dWIN386=1 -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
AFLAGS_DLL = -e=2 -zq -zw -m$(MMODE) $(DEBUG) $(CFLAGS_1) -bt=windows -wx -fp$(CPULEV3F) -$(CPULEV3) -dTARGET_MSDOS=32 -dMSDOS=1 -dWIN386=1 -dTARGET_WINDOWS=$(TARGET_WINDOWS) -dTARGET86=$(TARGET86) -DMMODE=$(MMODE) -q $(WIN_INCLUDE) -dWIN386=1
NASMFLAGS_DLL = -DTARGET_MSDOS=32 -DMSDOS=1 -DWIN386=1 -DTARGET86=$(TARGET86) -DTARGET_WINDOWS=$(TARGET_WINDOWS) -DMMODE=$(MMODE) -dWIN386=1

# macro to patch the EXE to the proper version
WIN_NE_SETVER_BUILD = ../../tool/chgnever.pl 3.1

# needed to rename ,exe back to .rex if the "EXE" was actually a .rex file
WIN386_EXE_TO_REX_IF_REX = ../../tool/win386-exe-to-rex-if-rex.pl

!include "$(REL)$(HPS)mak$(HPS)bcommon.mak"
!include "common.mak"
!include "$(REL)$(HPS)mak$(HPS)dcommon.mak"

