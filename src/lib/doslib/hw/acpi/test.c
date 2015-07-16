/* test.c
 *
 * ACPI BIOS interface test program.
 * (C) 2011-2012 Jonathan Campbell.
 * Hackipedia DOS library.
 *
 * This code is licensed under the LGPL.
 * <insert LGPL legal text here>
 *
 * Compiles for intended target environments:
 *   - MS-DOS [pure DOS mode, or Windows or OS/2 DOS Box]
 */

#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <dos.h>

#include <hw/dos/dos.h>
#include <hw/cpu/cpu.h>
#include <hw/acpi/acpi.h>
#include <hw/8254/8254.h>		/* 8254 timer */
#include <hw/8259/8259.h>		/* 8259 PIC */
#include <hw/flatreal/flatreal.h>
#include <hw/dos/doswin.h>

static void help() {
	fprintf(stderr,"Test [options]\n");
	fprintf(stderr,"  /32      Use 32-bit RSDT\n");
}

int main(int argc,char **argv) {
	struct acpi_rsdt_header sdth;
	acpi_memaddr_t addr;
	unsigned long i,max;
	uint32_t tmp32,tmplen;
	char tmp[32];

	for (i=1;i < (unsigned long)argc;) {
		const char *a = argv[(unsigned int)(i++)];

		if (*a == '-' || *a == '/') {
			do { a++; } while (*a == '-' || *a == '/');

			if (!strcmp(a,"?") || !strcmp(a,"h") || !strcmp(a,"help")) {
				help();
				return 1;
			}
			else if (!strcmp(a,"32")) {
				acpi_use_rsdt_32 = 1;
			}
			else {
				fprintf(stderr,"Unknown switch '%s'\n",a);
				help();
				return 1;
			}
		}
		else {
			fprintf(stderr,"Unknown arg '%s'\n",a);
			help();
			return 1;
		}
	}

	if (!probe_8254()) {
		printf("Cannot init 8254 timer\n");
		return 1;
	}
	if (!probe_8259()) {
		printf("Cannot init 8259 PIC\n");
		return 1;
	}
	cpu_probe();
	probe_dos();
	detect_windows();
#if TARGET_MSDOS == 32
	probe_dpmi();
	dos_ltp_probe();
#endif

#if TARGET_MSDOS == 16
	if (!flatrealmode_setup(FLATREALMODE_4GB)) {
		printf("Unable to set up flat real mode (needed for 16-bit builds)\n");
		printf("Most ACPI functions require access to the full 4GB range.\n");
		return 1;
	}
#endif

	if (!acpi_probe()) {
		printf("ACPI BIOS not found\n");
		return 1;
	}
	assert(acpi_rsdp != NULL);
	printf("ACPI %u.0 structure at 0x%05lX\n",acpi_rsdp->revision+1,(unsigned long)acpi_rsdp_location);

	memcpy(tmp,(char*)(&(acpi_rsdp->OEM_id)),6); tmp[6]=0;
	printf("ACPI OEM ID '%s', RSDT address (32-bit) 0x%08lX Length %lu\n",tmp,
		(unsigned long)(acpi_rsdp->rsdt_address),
		(unsigned long)(acpi_rsdp->length));
	if (acpi_rsdp->revision != 0)
		printf("   XSDT address (64-bit) 0x%016llX\n",
			(unsigned long long)(acpi_rsdp->xsdt_address));

	printf("Chosen RSDT/XSDT at 0x%08llX\n",(unsigned long long)acpi_rsdt_location);

	if (acpi_rsdt != NULL) {
		memcpy(tmp,(void*)(acpi_rsdt->signature),4); tmp[4] = 0;
		printf("  '%s': len=%lu rev=%u\n",tmp,(unsigned long)acpi_rsdt->length,
			acpi_rsdt->revision);

		memcpy(tmp,(void*)(acpi_rsdt->OEM_id),6); tmp[6] = 0;
		printf("  OEM id: '%s'\n",tmp);

		memcpy(tmp,(void*)(acpi_rsdt->OEM_table_id),8); tmp[8] = 0;
		printf("  OEM table id: '%s' rev %lu\n",tmp,
			(unsigned long)acpi_rsdt->OEM_revision);

		memcpy(tmp,(void*)(&(acpi_rsdt->creator_id)),4); tmp[4] = 0;
		printf("  Creator: '%s' rev %lu\n",tmp,
			(unsigned long)acpi_rsdt->creator_revision);
	}

	max = acpi_rsdt_entries();
	if (acpi_rsdt_is_xsdt()) {
		printf("Showing XSDT, %lu entries\n",max);
	}
	else {
		printf("Showing RSDT, %lu entries\n",max);
	}

	for (i=0;i < max;i++) {
		addr = acpi_rsdt_entry(i);
		printf(" [%lu] 0x%08llX ",i,(unsigned long long)addr);
		if (addr != 0ULL) {
			tmp32 = acpi_mem_readd(addr);
			tmplen = 0;

			memcpy(tmp,&tmp32,4); tmp[4] = 0;
			if (acpi_probe_rsdt_check(addr,tmp32,&tmplen)) {
				acpi_memcpy_from_phys(&sdth,addr,sizeof(struct acpi_rsdt_header));

				printf("'%s' len=0x%lX rev=%u ",tmp,(unsigned long)tmplen,sdth.revision);

				memcpy(tmp,&sdth.OEM_id,6); tmp[6] = 0;
				printf("OEM id: '%s'\n",tmp);

				memcpy(tmp,&sdth.OEM_table_id,8); tmp[8] = 0;
				printf("OEM table id: '%s' rev %u ",tmp,sdth.OEM_revision);

				memcpy(tmp,&sdth.creator_id,4); tmp[4] = 0;
				printf("Creator id: '%s' rev %u",tmp,sdth.creator_revision);

				if (!memcmp(sdth.signature,"MCFG",4)) {
					struct acpi_mcfg_entry entry;
					uint64_t o = addr + 44;
					unsigned int count;

					printf("\nPCI Express map:");
					assert(sizeof(struct acpi_mcfg_entry) == 16);
					count = (unsigned int)(tmplen / sizeof(struct acpi_mcfg_entry));
					while (count != 0) {
						acpi_memcpy_from_phys(&entry,o,sizeof(struct acpi_mcfg_entry));
						o += sizeof(struct acpi_mcfg_entry);

						/* Some bioses I test against seem to return enough for 3 but fill in only 1? */
						if (entry.base_address != 0ULL || entry.start_pci_bus_number != 0 || entry.end_pci_bus_number != 0) {
							uint64_t sz;

							if (entry.start_pci_bus_number > entry.end_pci_bus_number)
								entry.start_pci_bus_number = entry.end_pci_bus_number;

							sz = (((unsigned long long)(entry.end_pci_bus_number - entry.start_pci_bus_number)) + 1ULL) << 20ULL;
							printf("\n  @0x%08llX-0x%08llX seg=%u bus=%u-%u",
								(unsigned long long)entry.base_address,
								(unsigned long long)(entry.base_address + sz - 1ULL),
								(unsigned int)entry.pci_segment_group_number,
								(unsigned int)entry.start_pci_bus_number,
								(unsigned int)entry.end_pci_bus_number);
						}

						count--;
					}
				}
			}
			else {
				printf("'%s' check failed",tmp);
			}
		}
		printf("\n");
	}

	acpi_free();
	return 0;
}

