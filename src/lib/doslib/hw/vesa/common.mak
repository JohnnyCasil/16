# this makefile is included from all the dos*.mak files, do not use directly
# NTS: HPS is either \ (DOS) or / (Linux)
NOW_BUILDING = HW_VESA_LIB
CFLAGS_THIS = -fr=nul -fo=$(SUBDIR)$(HPS).obj -i=.. -i..$(HPS)..

C_SOURCE =    vesa.c
OBJS =        $(SUBDIR)$(HPS)vesa.obj
TEST_EXE =    $(SUBDIR)$(HPS)test.exe
MODESET_EXE = $(SUBDIR)$(HPS)modeset.exe

!ifeq TARGET_MSDOS 16
VESA240_EXE = $(SUBDIR)$(HPS)vesa240.exe
!endif

$(HW_VESA_LIB): $(OBJS)
	wlib -q -b -c $(HW_VESA_LIB) -+$(SUBDIR)$(HPS)vesa.obj

# NTS we have to construct the command line into tmp.cmd because for MS-DOS
# systems all arguments would exceed the pitiful 128 char command line limit
.C.OBJ:
	%write tmp.cmd $(CFLAGS_THIS) $(CFLAGS) $[@
	$(CC) @tmp.cmd

all: lib exe
       
lib: $(HW_VESA_LIB) .symbolic
       
exe: $(TEST_EXE) $(MODESET_EXE) $(VESA240_EXE) .symbolic

$(TEST_EXE): $(HW_VESA_LIB) $(HW_VESA_LIB_DEPENDENCIES) $(HW_CPU_LIB) $(HW_CPU_LIB_DEPENDENCIES) $(HW_DOS_LIB) $(HW_DOS_LIB_DEPENDENCIES) $(HW_FLATREAL_LIB) $(HW_FLATREAL_LIB_DEPENDENCIES) $(SUBDIR)$(HPS)test.obj
	%write tmp.cmd option quiet system $(WLINK_SYSTEM) file $(SUBDIR)$(HPS)test.obj $(HW_VESA_LIB_WLINK_LIBRARIES) $(HW_CPU_LIB_WLINK_LIBRARIES) $(HW_DOS_LIB_WLINK_LIBRARIES) $(HW_FLATREAL_LIB_WLINK_LIBRARIES) name $(TEST_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe

$(MODESET_EXE): $(HW_VESA_LIB) $(HW_VESA_LIB_DEPENDENCIES) $(HW_CPU_LIB) $(HW_CPU_LIB_DEPENDENCIES) $(HW_VGA_LIB) $(HW_DOS_LIB) $(HW_DOS_LIB_DEPENDENCIES) $(HW_FLATREAL_LIB) $(HW_FLATREAL_LIB_DEPENDENCIES) $(SUBDIR)$(HPS)modeset.obj
	%write tmp.cmd option quiet system $(WLINK_SYSTEM) file $(SUBDIR)$(HPS)modeset.obj $(HW_VESA_LIB_WLINK_LIBRARIES) $(HW_VGA_LIB_WLINK_LIBRARIES) $(HW_CPU_LIB_WLINK_LIBRARIES) $(HW_DOS_LIB_WLINK_LIBRARIES) $(HW_FLATREAL_LIB_WLINK_LIBRARIES) name $(MODESET_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe

!ifdef VESA240_EXE
$(VESA240_EXE): $(HW_VESA_LIB) $(HW_VESA_LIB_DEPENDENCIES) $(HW_CPU_LIB) $(HW_CPU_LIB_DEPENDENCIES) $(HW_VGA_LIB) $(HW_DOS_LIB) $(HW_DOS_LIB_DEPENDENCIES) $(HW_FLATREAL_LIB) $(HW_FLATREAL_LIB_DEPENDENCIES) $(SUBDIR)$(HPS)vesa240.obj
	%write tmp.cmd option quiet system $(WLINK_SYSTEM) file $(SUBDIR)$(HPS)vesa240.obj $(HW_VESA_LIB_WLINK_LIBRARIES) $(HW_VGA_LIB_WLINK_LIBRARIES) $(HW_CPU_LIB_WLINK_LIBRARIES) $(HW_DOS_LIB_WLINK_LIBRARIES) $(HW_DOS_LIB_WLINK_LIBRARIES) $(HW_FLATREAL_LIB_WLINK_LIBRARIES) name $(VESA240_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe
!endif

clean: .SYMBOLIC
          del $(SUBDIR)$(HPS)*.obj
          del $(HW_VESA_LIB)
          del tmp.cmd
          @echo Cleaning done

