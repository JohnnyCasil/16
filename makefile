!ifdef __LINUX__
REMOVECOMMAND=rm -f
COPYCOMMAND=cp -f
DIRSEP=/
OBJ=o
!else
REMOVECOMMAND=del
COPYCOMMAND=copy /y
DIRSEP=\
OBJ=obj
!endif
!ifndef INCLUDE
IN=..$(DIRSEP)..$(DIRSEP)fdos$(DIRSEP)watcom2$(DIRSEP)h
IFLAGS=-i=$(IN)
!endif

TARGET_OS = dos
#-zk0u = translate kanji to unicode... wwww
#-zk0 = kanji support~
#-zkl = current codepage

#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
SRC=src$(DIRSEP)
SRCLIB=$(SRC)lib$(DIRSEP)
JSMNLIB=$(SRCLIB)jsmn$(DIRSEP)
NYANLIB=$(SRCLIB)nyan$(DIRSEP)
#EXMMLIB=$(SRCLIB)exmm$(DIRSEP)
VGMSNDLIB=$(SRCLIB)vgmsnd$(DIRSEP)
DOSLIB=$(SRCLIB)doslib$(DIRSEP)
WCPULIB=$(SRCLIB)wcpu$(DIRSEP)

16FLAGS=-fh=16.hed
BAKAPIFLAGS=-fh=bakapi.hed
SFLAGS=-sg -st -of+ -zu -zdf -zff -zgf -k51200#60000#49152#32768#24576
DFLAGS=-DTARGET_MSDOS=16 -DMSDOS=1 $(SFLAGS)
ZFLAGS=-zk0 -zq -zc -zp8# -zm
CFLAGS=$(IFLAGS) -mc -lr -l=dos -wo -x# -d2##wwww
OFLAGS=-obmiler -out -oh -ei -zp8 -0 -fpi87  -onac -ol+ -ok####x
FLAGS=$(CFLAGS) $(OFLAGS) $(DFLAGS) $(ZFLAGS)

DOSLIBEXMMOBJ = himemsys.$(OBJ) emm.$(OBJ)
VGMSNDOBJ = 16_snd.$(OBJ) vgmSnd.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
16LIBOBJS = bakapee.$(OBJ) 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ)
#scroll16.$(OBJ)
#timer.$(OBJ)
#3812intf.$(OBJ)
GFXLIBOBJS = modex16.$(OBJ) bitmap.$(OBJ) planar.$(OBJ) 16text.$(OBJ)

TESTEXEC =  exmmtest.exe test.exe pcxtest.exe test2.exe palettec.exe maptest.exe fmemtest.exe fonttest.exe fontgfx.exe inputest.exe scroll.exe vgmtest.exe
#tsthimem.exe
#testemm.exe testemm0.exe fonttes0.exe miditest.exe sega.exe sountest.exe
EXEC = 16.exe bakapi.exe $(TESTEXEC)

all: $(EXEC)

#
#game and bakapi executables
#
16.exe: 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib
	wcl $(FLAGS) $(16FLAGS) 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib -fm=16.map

bakapi.exe: bakapi.$(OBJ) 16.lib
	wcl $(FLAGS) $(BAKAPIFLAGS) bakapi.$(OBJ) 16.lib -fm=bakapi.map
#
#Test Executables!
#
scroll.exe: scroll.$(OBJ) scroll16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib#gfx.lib 16_head.$(OBJ) bakapee.$(OBJ) 16_in.$(OBJ) wcpu.$(OBJ)
	wcl $(FLAGS) scroll.$(OBJ) scroll16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib -fm=scroll.map#gfx.lib 16_head.$(OBJ) bakapee.$(OBJ)  16_in.$(OBJ) wcpu.$(OBJ)
scroll.$(OBJ): $(SRC)scroll.c
	wcl $(FLAGS) -c $(SRC)scroll.c

#sega.exe: sega.$(OBJ)
#	wcl $(FLAGS) sega.$(OBJ)
#sega.$(OBJ): $(SRC)sega.c
#	wcl $(FLAGS) -c $(SRC)sega.c

test.exe: test.$(OBJ) gfx.lib
	wcl $(FLAGS) test.$(OBJ) gfx.lib -fm=test.map

test2.exe: test2.$(OBJ) gfx.lib
	wcl $(FLAGS) test2.$(OBJ) gfx.lib -fm=test2.map

fonttest.exe: fonttest.$(OBJ) 16.lib
	wcl $(FLAGS) fonttest.$(OBJ) 16.lib# -fm=fonttest.map

#fonttes0.exe: fonttes0.$(OBJ) 16.lib
#	wcl $(FLAGS) fonttes0.$(OBJ) 16.lib

fontgfx.exe: fontgfx.$(OBJ) 16.lib
	wcl $(FLAGS) fontgfx.$(OBJ) 16.lib -fm=fontgfx.map

inputest.exe: inputest.$(OBJ) 16.lib
	wcl $(FLAGS) -D__DEBUG_InputMgr__=1 inputest.$(OBJ) 16.lib -fm=inputest.map

#sountest.exe: sountest.$(OBJ) 16.lib
#	wcl $(FLAGS) sountest.$(OBJ) 16.lib

#miditest.exe: miditest.$(OBJ) 16.lib $(DOSLIBEXMMOBJ) midi.$(OBJ)
#	wcl $(FLAGS) miditest.$(OBJ) 16.lib $(DOSLIBEXMMOBJ) midi.$(OBJ)

tsthimem.exe: tsthimem.$(OBJ) 16.lib $(DOSLIBEXMMOBJ)
	wcl $(FLAGS) tsthimem.$(OBJ) 16.lib $(DOSLIBEXMMOBJ) -fm=tsthimem.map

#testemm.exe: testemm.$(OBJ) 16.lib $(DOSLIBEXMMOBJ)
#	wcl $(FLAGS) testemm.$(OBJ) 16.lib $(DOSLIBEXMMOBJ)

#testemm0.exe: testemm0.$(OBJ) 16.lib $(DOSLIBEXMMOBJ)
#	wcl $(FLAGS) testemm0.$(OBJ) 16.lib $(DOSLIBEXMMOBJ)

pcxtest.exe: pcxtest.$(OBJ) gfx.lib
	wcl $(FLAGS) pcxtest.$(OBJ) gfx.lib -fm=pcxtest.map

palettec.exe: palettec.$(OBJ) 16.lib
	wcl $(FLAGS) palettec.$(OBJ) 16.lib -fm=palettec.map

maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib
	wcl $(FLAGS) maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib -fm=maptest.map

#maptest0.exe: maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)# 16.lib
#	wcl $(FLAGS) $(MFLAGS) maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)# 16.lib

#emmtest.exe: emmtest.$(OBJ) memory.$(OBJ)
#	wcl $(FLAGS) $(MFLAGS) emmtest.$(OBJ) memory.$(OBJ)

#emsdump.exe: emsdump.$(OBJ) memory.$(OBJ)
#	wcl $(FLAGS) $(MFLAGS) emsdump.$(OBJ) memory.$(OBJ)

fmemtest.exe: fmemtest.$(OBJ) 16.lib
	wcl $(FLAGS) fmemtest.$(OBJ) 16.lib -fm=fmemtest.map

exmmtest.exe: exmmtest.$(OBJ) 16.lib#16_head.$(OBJ) 16_mm.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)
	wcl $(FLAGS) exmmtest.$(OBJ) 16.lib -fm=exmmtest.map#16_head.$(OBJ) 16_mm.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)

vgmtest.exe: vgmtest.$(OBJ) vgmsnd.lib 16_in.$(OBJ) 16_head.$(OBJ)
	wcl $(FLAGS) vgmtest.$(OBJ) vgmsnd.lib -fm=vgmtest.map 16_in.$(OBJ) 16_head.$(OBJ)
	#====wcl -mc vgmtest.$(OBJ) $(VGMSNDOBJ) -fm=vgmtest.map


#
#executable's objects
#
16.$(OBJ): $(SRC)16.h $(SRC)16.c
	wcl $(FLAGS) -c $(SRC)16.c

bakapi.$(OBJ): $(SRC)bakapi.h $(SRC)bakapi.c
	wcl $(FLAGS) -c $(SRC)bakapi.c

test.$(OBJ): $(SRC)test.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test.c

test2.$(OBJ): $(SRC)test2.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test2.c

pcxtest.$(OBJ): $(SRC)pcxtest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)pcxtest.c

palettec.$(OBJ): $(SRC)palettec.c
	wcl $(FLAGS) -c $(SRC)palettec.c

maptest.$(OBJ): $(SRC)maptest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)maptest.c

#maptest0.$(OBJ): $(SRC)maptest0.c# $(SRCLIB)modex16.h
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)maptest0.c

#emmtest.$(OBJ): $(SRC)emmtest.c
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emmtest.c

#emsdump.$(OBJ): $(SRC)emsdump.c
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emsdump.c

fmemtest.$(OBJ): $(SRC)fmemtest.c
	wcl $(FLAGS) -c $(SRC)fmemtest.c

fonttest.$(OBJ): $(SRC)fonttest.c
	wcl $(FLAGS) -c $(SRC)fonttest.c

#fonttes0.$(OBJ): $(SRC)fonttes0.c
#	wcl $(FLAGS) -c $(SRC)fonttes0.c

fontgfx.$(OBJ): $(SRC)fontgfx.c
	wcl $(FLAGS) -c $(SRC)fontgfx.c

inputest.$(OBJ): $(SRC)inputest.c
	wcl $(FLAGS) -c $(SRC)inputest.c

#sountest.$(OBJ): $(SRC)sountest.c
#	wcl $(FLAGS) -c $(SRC)sountest.c

#miditest.$(OBJ): $(SRC)miditest.c
#	wcl $(FLAGS) -c $(SRC)miditest.c

#testemm.$(OBJ): $(SRC)testemm.c
#	wcl $(FLAGS) -c $(SRC)testemm.c

#testemm0.$(OBJ): $(SRC)testemm0.c
#	wcl $(FLAGS) -c $(SRC)testemm0.c

tsthimem.$(OBJ): $(SRC)tsthimem.c
	wcl $(FLAGS) -c $(SRC)tsthimem.c

exmmtest.$(OBJ): $(SRC)exmmtest.c
	wcl $(FLAGS) -c $(SRC)exmmtest.c

vgmtest.$(OBJ): $(SRC)vgmtest.c
	wcl $(FLAGS) -c $(SRC)vgmtest.c
	#====wcl -mc -c $(SRC)vgmtest.c

#
#non executable objects libraries
#
16.lib: $(16LIBOBJS) gfx.lib# doslib.lib vgmsnd.lib
	wlib -b -q 16.lib $(16LIBOBJS) gfx.lib# doslib.lib vgmsnd.lib

gfx.lib: $(GFXLIBOBJS)
	wlib -b -q gfx.lib $(GFXLIBOBJS)

doslib.lib: $(DOSLIBOBJ) # $(SRCLIB)cpu.lib
	wlib -b -q doslib.lib $(DOSLIBOBJ) # $(SRCLIB)cpu.lib

vgmsnd.lib: $(VGMSNDOBJ)
	wlib -b -q vgmsnd.lib $(VGMSNDOBJ)

modex16.$(OBJ): $(SRCLIB)modex16.h $(SRCLIB)modex16.c
	wcl $(FLAGS) -c $(SRCLIB)modex16.c

bakapee.$(OBJ): $(SRCLIB)bakapee.h $(SRCLIB)bakapee.c
	wcl $(FLAGS) -c $(SRCLIB)bakapee.c

bitmap.$(OBJ): $(SRCLIB)bitmap.h $(SRCLIB)bitmap.c
	wcl $(FLAGS) -c $(SRCLIB)bitmap.c

planar.$(OBJ): $(SRCLIB)planar.h $(SRCLIB)planar.c
	wcl $(FLAGS) -c $(SRCLIB)planar.c

scroll16.$(OBJ): $(SRCLIB)scroll16.h $(SRCLIB)scroll16.c
	wcl $(FLAGS) -c $(SRCLIB)scroll16.c

wcpu.$(OBJ): $(WCPULIB)wcpu.h $(WCPULIB)wcpu.c
	wcl $(FLAGS) -c $(WCPULIB)wcpu.c

16text.$(OBJ): $(SRCLIB)16text.c
	wcl $(FLAGS) -c $(SRCLIB)16text.c

mapread.$(OBJ): $(SRCLIB)mapread.h $(SRCLIB)mapread.c
	wcl $(FLAGS) -c $(SRCLIB)mapread.c

#fmapread.$(OBJ): $(SRCLIB)fmapread.h $(SRCLIB)fmapread.c 16.lib
#	wcl $(FLAGS) $(MFLAGS) -c $(SRCLIB)fmapread.c 16.lib

timer.$(OBJ): $(SRCLIB)timer.h $(SRCLIB)timer.c
	wcl $(FLAGS) -c $(SRCLIB)timer.c

16_in.$(OBJ): $(SRCLIB)16_in.h $(SRCLIB)16_in.c
	wcl $(FLAGS) -c $(SRCLIB)16_in.c

16_mm.$(OBJ): $(SRCLIB)16_mm.h $(SRCLIB)16_mm.c
	wcl $(FLAGS) -c $(SRCLIB)16_mm.c

16_ca.$(OBJ): $(SRCLIB)16_ca.h $(SRCLIB)16_ca.c
	wcl $(FLAGS) -c $(SRCLIB)16_ca.c

midi.$(OBJ): $(SRCLIB)midi.h $(SRCLIB)midi.c
	wcl $(FLAGS) -c $(SRCLIB)midi.c

#
# doslib stuff
#
adlib.$(OBJ): $(DOSLIB)adlib.h $(DOSLIB)adlib.c
	wcl $(FLAGS) -c $(DOSLIB)adlib.c

8254.$(OBJ): $(DOSLIB)8254.h $(DOSLIB)8254.c
	wcl $(FLAGS) -c $(DOSLIB)8254.c

8259.$(OBJ): $(DOSLIB)8259.h $(DOSLIB)8259.c
	wcl $(FLAGS) -c $(DOSLIB)8259.c

dos.$(OBJ): $(DOSLIB)dos.h $(DOSLIB)dos.c
	wcl $(FLAGS) -c $(DOSLIB)dos.c

cpu.$(OBJ): $(DOSLIB)cpu.h $(DOSLIB)cpu.c
	wcl $(FLAGS) -c $(DOSLIB)cpu.c

himemsys.$(OBJ): $(DOSLIB)himemsys.h $(DOSLIB)himemsys.c
	wcl $(FLAGS) -c $(DOSLIB)himemsys.c

emm.$(OBJ): $(DOSLIB)emm.h $(DOSLIB)emm.c
	wcl $(FLAGS) -c $(DOSLIB)emm.c

# end of doslib stuff

16_head.$(OBJ): $(SRCLIB)16_head.h $(SRCLIB)16_head.c
	wcl $(FLAGS) -c $(SRCLIB)16_head.c

16_hc.$(OBJ): $(SRCLIB)16_hc.h $(SRCLIB)16_hc.c
	wcl $(FLAGS) -c $(SRCLIB)16_hc.c

16_snd.$(OBJ): $(SRCLIB)16_snd.h $(SRCLIB)16_snd.c
	wcl $(FLAGS) -c $(SRCLIB)16_snd.c
	#====wcl -mc -c $(SRCLIB)16_snd.c

jsmn.$(OBJ): $(JSMNLIB)jsmn.h $(JSMNLIB)jsmn.c
	wcl $(FLAGS) -c $(JSMNLIB)jsmn.c

kitten.$(OBJ): $(NYANLIB)kitten.h $(NYANLIB)kitten.c
	wcl $(FLAGS) -c $(NYANLIB)kitten.c

vgmSnd.$(OBJ): $(VGMSNDLIB)vgmSnd.h $(VGMSNDLIB)vgmSnd.c
	wcl $(FLAGS) -c $(VGMSNDLIB)vgmSnd.c
	#====wcl -c -mc $(VGMSNDLIB)vgmSnd.c

#3812intf.$(OBJ): $(VGMSNDLIB)3812intf.h $(VGMSNDLIB)3812intf.c
#	wcl $(FLAGS) -c $(VGMSNDLIB)3812intf.c

#farjsmn.$(OBJ): $(JSMNLIB)farjsmn.h $(JSMNLIB)farjsmn.c
#	wcl $(FLAGS) $(MFLAGS) -c $(JSMNLIB)farjsmn.c

#memory.$(OBJ): $(EXMMLIB)memory.h $(EXMMLIB)memory.c
#	wcl $(FLAGS) $(MFLAGS) -c $(EXMMLIB)memory.c

#
#other~
#
clean: .symbolic
	@$(REMOVECOMMAND) $(EXEC)
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) *.lib
	@wlib -n 16.lib
	@wlib -n  gfx.lib
	@wlib -n  doslib.lib
	@wlib -n  vgmsnd.lib
	@$(REMOVECOMMAND) *.16
	@$(REMOVECOMMAND) *.16W
	@$(REMOVECOMMAND) *.16B
	@$(REMOVECOMMAND) *.OBJ
	@$(REMOVECOMMAND) *.o
	@$(REMOVECOMMAND) *.BCO
	@$(REMOVECOMMAND) makefi~1
	@$(REMOVECOMMAND) makefile~
	@$(REMOVECOMMAND) __wcl__.LNK
#	@$(REMOVECOMMAND) *.smp
	@$(REMOVECOMMAND) *.SMP
	@$(REMOVECOMMAND) *.hed
	@$(REMOVECOMMAND) *.MAP
	@$(REMOVECOMMAND) *.map
	@$(REMOVECOMMAND) *.err
	@$(COPYCOMMAND) .git/config git_con.fig
#	@$(COPYCOMMAND) $(SRC)exmmtest.c $(EXMMTESTDIR)$(SRC)
#	@$(COPYCOMMAND) $(SRCLIB)16_mm.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_head.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_ca.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_hc.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)types.h $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(NYANLIB)* $(EXMMTESTDIR)$(NYANLIB)
	@echo $(watcom)
	@echo $(INCLUDE)
