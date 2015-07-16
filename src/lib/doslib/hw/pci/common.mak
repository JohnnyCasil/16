# this makefile is included from all the dos*.mak files, do not use directly
# NTS: HPS is either \ (DOS) or / (Linux)
NOW_BUILDING = HW_PCI_LIB
CFLAGS_THIS = -fr=nul -fo=$(SUBDIR)$(HPS).obj -i=.. -i..$(HPS)..
AFLAGS_THIS = -fr=nul -fo=$(SUBDIR)$(HPS).obj -i=.. -i..$(HPS)..
NASMFLAGS_THIS = 

# NTS: CPU functions here are to be moved at some point to the cpu library!
C_SOURCE =    pci.c
OBJS =        $(SUBDIR)$(HPS)pci.obj $(SUBDIR)$(HPS)pcibios1.obj
HW_PCI_LIB =  $(SUBDIR)$(HPS)pci.lib
TEST_EXE =    $(SUBDIR)$(HPS)test.exe

$(HW_PCI_LIB): $(OBJS)
	wlib -q -b -c $(HW_PCI_LIB) -+$(SUBDIR)$(HPS)pci.obj
	wlib -q -b -c $(HW_PCI_LIB) -+$(SUBDIR)$(HPS)pcibios1.obj

# NTS we have to construct the command line into tmp.cmd because for MS-DOS
# systems all arguments would exceed the pitiful 128 char command line limit
.C.OBJ:
	%write tmp.cmd $(CFLAGS_THIS) $(CFLAGS) $[@
	$(CC) @tmp.cmd

.ASM.OBJ:
	nasm -o $@ -f obj $(NASMFLAGS) $[@

all: lib exe

lib: $(HW_PCI_LIB) .symbolic

exe: $(TEST_EXE) .symbolic

$(TEST_EXE): $(HW_PCI_LIB) $(HW_PCI_LIB_DEPENDENCIES) $(SUBDIR)$(HPS)test.obj $(HW_CPU_LIB) $(HW_CPU_LIB_DEPENDENCIES) $(HW_DOS_LIB) $(HW_DOS_LIB_DEPENDENCIES)
	%write tmp.cmd option quiet system $(WLINK_SYSTEM) file $(SUBDIR)$(HPS)test.obj $(HW_PCI_LIB_WLINK_LIBRARIES) $(HW_CPU_LIB_WLINK_LIBRARIES) $(HW_DOS_LIB_WLINK_LIBRARIES) name $(TEST_EXE)
	@wlink @tmp.cmd
	@$(COPY) ..$(HPS)..$(HPS)dos32a.dat $(SUBDIR)$(HPS)dos4gw.exe

clean: .SYMBOLIC
          del $(SUBDIR)$(HPS)*.obj
          del $(HW_PCI_LIB)
          del tmp.cmd
          @echo Cleaning done

