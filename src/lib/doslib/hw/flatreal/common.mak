# this makefile is included from all the dos*.mak files, do not use directly
# NTS: HPS is either \ (FLATREAL) or / (Linux)
NOW_BUILDING = HW_FLATREAL_LIB
CFLAGS_THIS = -fr=nul -fo=$(SUBDIR)$(HPS).obj -i=.. -i..$(HPS)..

C_SOURCE =    flatreal.c
OBJS =        $(SUBDIR)$(HPS)flatreal.obj $(SUBDIR)$(HPS)flatmode.obj
TEST_EXE =    $(SUBDIR)$(HPS)test.exe

$(HW_FLATREAL_LIB): $(OBJS)
	wlib -q -b -c $(HW_FLATREAL_LIB) -+$(SUBDIR)$(HPS)flatreal.obj -+$(SUBDIR)$(HPS)flatmode.obj

# NTS we have to construct the command line into tmp.cmd because for MS-FLATREAL
# systems all arguments would exceed the pitiful 128 char command line limit
.C.OBJ:
	%write tmp.cmd $(CFLAGS_THIS) $(CFLAGS) $[@
	@$(CC) @tmp.cmd

.ASM.OBJ:
	nasm -o $@ -f obj $(NASMFLAGS) $[@

all: lib exe
	
lib: $(HW_FLATREAL_LIB) .symbolic
	
exe: $(TEST_EXE) .symbolic

$(TEST_EXE): $(HW_FLATREAL_LIB) $(SUBDIR)$(HPS)test.obj $(HW_CPU_LIB) $(HW_CPU_LIB_DEPENDENCIES) $(HW_DOS_LIB) $(HW_DOS_LIB_DEPENDENCIES)
	%write tmp.cmd option quiet system $(WLINK_SYSTEM) file $(SUBDIR)$(HPS)test.obj $(HW_FLATREAL_LIB_WLINK_LIBRARIES) $(HW_CPU_LIB_WLINK_LIBRARIES) $(HPS_DOS_LIB_WLINK_LIBRARIES) name $(TEST_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe

clean: .SYMBOLIC
          del $(SUBDIR)$(HPS)*.obj
          del $(HW_FLATREAL_LIB)
          del tmp.cmd
          @echo Cleaning done

