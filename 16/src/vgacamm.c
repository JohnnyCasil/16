/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/lib/16_vl.h"
#include "src/lib/16_sprit.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

void main() {
	static global_game_variables_t gvar;
	__segment sega;
	memptr bigbuffer;
	int i;
	word start;
	int plane;
	float t1, t2;
	boolean baka;
	byte *pal;
	word w=0;

	gvar.mm.mmstarted=0;

	MM_Startup(&gvar.mm, &gvar.mmi);
	CA_Startup(&gvar);
	printf("loading\n");
	if(CA_LoadFile("data/spri/chikyuu.vrs", &bigbuffer, &gvar)) baka=1; else baka=0;

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//      So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//      parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//      the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	VGAmodeX(1, 1, &gvar);
	modexHiganbanaPageSetup(&gvar.video);

	/* non sprite comparison */
	start = *clockw;
	modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
	t1 = (*clockw-start) /18.2;

	start = *clockw;

	t2 = (*clockw-start)/18.2;

	while(!kbhit())
	{
		switch(w)
		{
			case 1024:
				modexPalUpdate0(pal);
				w=0;
			default:
				w++;
			break;
		}
	}
	VGAmodeX(0, 1, &gvar);
	MM_ShowMemory(&gvar, &gvar.mm);
	MM_DumpData(&gvar.mm);
	//
	MM_FreePtr(&bigbuffer, &gvar.mm);
	//
	CA_Shutdown(&gvar);
	MM_Shutdown(&gvar.mm);
	//printf("CPU to VGA: %f\n", t1);
	//printf("VGA to VGA: %f\n", t2);
	heapdump(&gvar);
	printf("Project 16 vgacamm.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("t1: %f\n", t1);
	printf("t2: %f\n", t2);
	printf("gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	printf("gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
// 	printf("Num %d", num_of_vrl);
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
}
