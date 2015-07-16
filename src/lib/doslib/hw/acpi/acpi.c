/* acpi.c
 *
 * ACPI BIOS interface library.
 * (C) 2011-2012 Jonathan Campbell.
 * Hackipedia DOS library.
 *
 * This code is licensed under the LGPL.
 * <insert LGPL legal text here>
 *
 * Compiles for intended target environments:
 *   - MS-DOS [pure DOS mode, or Windows or OS/2 DOS Box]
 *
 * Modern PCs (since about 1999) have a BIOS that implements the ACPI BIOS standard.
 * This code allows your DOS program to detect the ACPI BIOS, locate the tables, and
 * make use of the ACPI BIOS. */
/* TODO: When can we begin to incorporate an AML interpreter? */
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

unsigned char				acpi_use_rsdt_32 = 0;
uint32_t				acpi_rsdp_location = 0;
struct acpi_rsdp_descriptor*		acpi_rsdp = NULL;
uint32_t				acpi_rsdt_location = 0; /* or XSDT (TODO: will become acpi_memaddr_t) */
struct acpi_rsdt_header*		acpi_rsdt = NULL; /* RSDT or XSDT */
unsigned char				acpi_probe_result = 0;
unsigned char				acpi_probed = 0;

uint32_t acpi_mem_readd(acpi_memaddr_t m) {
#if TARGET_MSDOS == 32
	/* 32-bit flat mode code does not yet have 64-bit address access, limited to 4GB */
	if ((m+3ULL) & (~0xFFFFFFFFULL))
		return ~0UL;

	/* if no paging, then we can just typecast the pointer and be done with it */
	if (!dos_ltp_info.paging)
		return *((volatile uint32_t*)((uint32_t)m));

	/* TODO: DPMI physical mem mapping method? */
	/* TODO: VCPI tricks? */

	return ~0UL;
#else
	/* 16-bit real mode code does not yet have 64-bit address access, limited to 4GB */
	if ((m+3ULL) & (~0xFFFFFFFFULL))
		return ~0UL;

	return flatrealmode_readd((uint32_t)m);
#endif
}

void acpi_mem_writed(acpi_memaddr_t m,uint32_t d) {
#if TARGET_MSDOS == 32
	/* 32-bit flat mode code does not yet have 64-bit address access, limited to 4GB */
	if ((m+3ULL) & (~0xFFFFFFFFULL))
		return;

	/* if no paging, then we can just typecast the pointer and be done with it */
	if (!dos_ltp_info.paging)
		*((volatile uint32_t*)((uint32_t)m)) = d;

	/* TODO: DPMI physical mem mapping method? */
	/* TODO: VCPI tricks? */
#else
	/* 16-bit real mode code does not yet have 64-bit address access, limited to 4GB */
	if ((m+3ULL) & (~0xFFFFFFFFULL))
		return;

	flatrealmode_writed((uint32_t)m,d);
#endif
}

void acpi_free() {
	if (acpi_rsdp != NULL) free(acpi_rsdp);
	acpi_rsdp = NULL;
	if (acpi_rsdt != NULL) free(acpi_rsdt);
	acpi_rsdt = NULL;
}

int acpi_probe_scan(uint32_t start,uint32_t end) {
	uint32_t a = start & (~0xFUL);
	unsigned int i,len;
	unsigned char sum;
	char buf[36];

	acpi_free();
	for (;(a+0xFUL) < end;a += 0x10UL) {
		((uint32_t*)buf)[0] = acpi_mem_readd(a+0x0UL);
		((uint32_t*)buf)[1] = acpi_mem_readd(a+0x4UL);
		if (memcmp(buf,"RSD PTR ",8) == 0) {
			((uint32_t*)buf)[2] = acpi_mem_readd(a+0x8UL);
			((uint32_t*)buf)[3] = acpi_mem_readd(a+0xCUL);
			((uint32_t*)buf)[4] = acpi_mem_readd(a+0x10UL);
			/* == 20 bytes */
			for (sum=0,i=0;i < 20;i++) sum += buf[i];
			if (sum == 0) {
				/* found it */
				acpi_rsdp_location = a;

				/* consider it v2.0 or higher if the extended checksum works out,
				 * else patch our copy to say v1.0 */
				((uint32_t*)buf)[5] = acpi_mem_readd(a+0x14UL);
				((uint32_t*)buf)[6] = acpi_mem_readd(a+0x18UL);
				((uint32_t*)buf)[7] = acpi_mem_readd(a+0x1CUL);
				((uint32_t*)buf)[8] = acpi_mem_readd(a+0x20UL);
				len = (int)(((uint32_t*)buf)[5]);
				if (buf[15]/*revision*/ != 0 && ((uint32_t*)buf)[5] >= 33 && ((uint32_t*)buf)[5] <= 255) {
					/* == 36 bytes */
					for (sum=0,i=0;i < min(len,36);i++) sum += buf[i];
					for (;i < len;i++) sum += acpi_mem_readb(a+i);

					if (sum == 0) {
					}
					else {
						fprintf(stderr,"ACPI v2.0 checksum fail\n");
						buf[15] = 0; /* patch to rev 0 */
					}
				}
				else {
					buf[15] = 0;
				}

				if (buf[15] == 0) {
					((uint32_t*)buf)[5] = 20; /* length 20 */
					len = 20;
				}

				if (len >= 20) {
					acpi_rsdp = malloc(len);
					if (acpi_rsdp != NULL)
						memcpy(acpi_rsdp,buf,min(len,36));
				}

				return 1;
			}
		}
	}

	return 0;
}

int acpi_probe_rsdt_check(acpi_memaddr_t a,uint32_t expect,uint32_t *length) {
	unsigned char sum=0;
	unsigned long i,len;
	uint32_t tmp;

	tmp = acpi_mem_readd(a);
	if (expect != 0UL && expect != tmp) return 0;

	len = (unsigned long)acpi_mem_readd(a+4); /* Length field */
	if (len < 36 || len >= (1UL << 20ULL)) return 0;

	for (i=0;i < (len & (~3UL));i += 4) {
		tmp = acpi_mem_readd(a + (acpi_memaddr_t)i);
		sum += (unsigned char)((tmp >> 0UL) & 0xFFUL);
		sum += (unsigned char)((tmp >> 8UL) & 0xFFUL);
		sum += (unsigned char)((tmp >> 16UL) & 0xFFUL);
		sum += (unsigned char)((tmp >> 24UL) & 0xFFUL);
	}
	if (len & 3UL) {
		tmp = acpi_mem_readd(a + (acpi_memaddr_t)i);
		sum += (unsigned char)((tmp >> 0UL) & 0xFFUL);
		if ((len & 3UL) >= 1UL) sum += (unsigned char)((tmp >> 8UL) & 0xFFUL);
		if ((len & 3UL) >= 2UL) sum += (unsigned char)((tmp >> 16UL) & 0xFFUL);
		if ((len & 3UL) == 3UL) sum += (unsigned char)((tmp >> 24UL) & 0xFFUL);
	}

	*length = (uint32_t)len;
	return (sum == 0x00);
}

void acpi_memcpy_from_phys(void *dst,acpi_memaddr_t src,uint32_t len) {
	while (len >= 4UL) {
		*((uint32_t*)dst) = acpi_mem_readd(src);
		dst = (void*)((char*)dst + 4);
		src += 4ULL;
		len -= 4UL;
	}
	if (len > 0UL) {
		uint32_t tmp = acpi_mem_readd(src);
		assert(len < 4UL);
		memcpy(dst,&tmp,(size_t)len);
	}
}

void acpi_probe_rsdt() {
	uint32_t len = 0;

	if (acpi_rsdt != NULL)
		return;

	acpi_rsdt_location = 0;

	/* TODO: Remove 4GB limit when this code *CAN* reach above 4GB */
	if (acpi_rsdp->revision != 0 && acpi_rsdp->xsdt_address != 0ULL &&
		acpi_rsdp->xsdt_address < 0x100000000ULL && !acpi_use_rsdt_32) {
		if (acpi_probe_rsdt_check(acpi_rsdp->xsdt_address,0x54445358UL,&len)) /* XSDT */
			acpi_rsdt_location = (uint32_t)(acpi_rsdp->xsdt_address);
	}

	if (acpi_rsdt_location == 0 && acpi_rsdp->rsdt_address != 0UL) {
		if (acpi_probe_rsdt_check(acpi_rsdp->rsdt_address,0x54445352UL,&len)) /* RSDT */
			acpi_rsdt_location = (uint32_t)(acpi_rsdp->rsdt_address);
	}

	if (acpi_rsdt_location != 0ULL && len >= 36UL && len <= 32768UL) {
		acpi_rsdt = malloc(len);
		if (acpi_rsdt != NULL) acpi_memcpy_from_phys((void*)acpi_rsdt,acpi_rsdt_location,len);
	}
}

int acpi_probe_ebda() {
	/* the RDSP can exist in the extended BIOS data area, or in the 0xE0000-0xFFFFF range */
	uint16_t sg;
	int ret = 0;

#if TARGET_MSDOS == 32
	sg = *((uint16_t*)(0x40E));
#else
	sg = *((uint16_t far*)MK_FP(0x40,0x0E));
#endif

	if (sg >= 0x60 && sg < 0xA000)
		ret = acpi_probe_scan((uint32_t)sg << 4UL,((uint32_t)sg << 4UL) + 0x3FF);

	if (ret == 0) ret = acpi_probe_scan(0xE0000UL,0xFFFFFUL);
	if (ret == 0) return 0;
	acpi_probe_rsdt();
	return 1;
}

int acpi_probe() {
	if (acpi_probed)
		return acpi_probe_result;

	acpi_probed=1;
	if (acpi_probe_ebda())
		return (acpi_probe_result=1);

	return (acpi_probe_result=0);
}

unsigned long acpi_rsdt_entries() {
	if (acpi_rsdt == NULL) return 0UL;
	if (acpi_rsdt->length < 36UL) return 0UL;
	if (acpi_rsdt_is_xsdt())
		return (unsigned long)((acpi_rsdt->length-36UL)/8UL);
	else
		return (unsigned long)((acpi_rsdt->length-36UL)/4UL);
}

acpi_memaddr_t acpi_rsdt_entry(unsigned long idx) {
	if (idx >= 0x40000000UL) return 0ULL;
	if (acpi_rsdt_is_xsdt()) {
		if (((idx*8UL)+36UL) >= acpi_rsdt->length) return 0ULL;
		return ((uint64_t*)((char*)acpi_rsdt + 36))[idx];
	}
	else {
		if (((idx*4UL)+36UL) >= acpi_rsdt->length) return 0ULL;
		return ((uint32_t*)((char*)acpi_rsdt + 36))[idx];
	}
}

