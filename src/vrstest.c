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

#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_vl.h"
#include "src/lib/16_sprit.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

extern boolean dbg_notest;

static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

void main()
{
	static global_game_variables_t gvar;
	//__segment sega;
	memptr bigbuffer;
	int i;
	word start;
	float t1, t2;
	//boolean baka;
	struct sprite spri;
	//vrl1_vgax_offset_t * off, *off1;
	struct vrs_container vrs;
	vrl1_vgax_offset_t **vrl_line_offsets;
	uint32_t far *vrl_headers_offsets;
	uint16_t far *vrl_id_iter;
	uint32_t vrl_size;
	int num_of_vrl;
	struct vrl1_vgax_header far *curr_vrl;
	byte *pal=0;
	int size=0;
	word w=0;
	dbg_notest=0;

	Startup16(&gvar);

	// What should be done by read_vrs:
	//sega = (mm.bufferseg);
	//if(
	CA_LoadFile("data/spri/chikyuu.vrs", &bigbuffer, &gvar);//) baka=1; else baka=0;

	// Insert sanity cheks later
	vrs.buffer = bigbuffer;
	vrs.data_size = size - sizeof(struct vrl1_vgax_header);
	num_of_vrl = 0;
	vrl_id_iter = (uint16_t far *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST]);
	while(vrl_id_iter[num_of_vrl]){
		num_of_vrl++;
	}

	// Allocate memory for vrl line offsets table
	vrl_line_offsets = malloc(sizeof(vrl1_vgax_offset_t *)*num_of_vrl);

	vrl_headers_offsets = (uint32_t far *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Calculate line offsets for each vrl
	for(i = 0; i < num_of_vrl; i++){
		curr_vrl = (struct vrl1_vgax_header far *)(vrs.buffer + vrl_headers_offsets[i]);

		// Calc. vrl size as (next_offset - curr_offset)
		if (i != num_of_vrl - 1){
			vrl_size = vrl_headers_offsets[i+1] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
		// If it's the last vrl, size is (next_vrs_struct_offset - curr_offset)
		else{
			vrl_size = vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
		vrl_line_offsets[i] = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), vrl_size);
	}
	vrs.vrl_line_offsets = vrl_line_offsets;


	//read_vrs(&gvar, "data/spri/chikyuu.vrs", &vrs);
	spri.spritesheet = &vrs;
	spri.sprite_vrl_cont = malloc(sizeof(struct vrl_container));
	i = set_anim_by_id(&spri, 11);
	if (i == -1)
	{
		return;
	}
	spri.x = 5;
	spri.y = 100;

//	Uncomment to see broken sprites
/*	sega = mm.bufferseg;
	if(CA_LoadFile("data/spri/CHUBACW1.vrl", &bigbuffer, &mm, &mmi)) baka=1; else baka=0;*/

	VGAmodeX(1, 1, &gvar);
	modexHiganbanaPageSetup(&gvar.video);
	modexShowPage_(&gvar.video.page[0]);

	/* non sprite comparison */
	start = *clockw;
	modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
	t1 = (*clockw-start) /18.2;

	start = *clockw;

	t2 = (*clockw-start)/18.2;

	/*modexLoadPalFile("data/spri/chikyuu.pal", &pal);
	modexPalUpdate1(pal);*/
	for (i = 0; i < 5; i++){
	spri.delay = 1; animate_spri(&spri, &gvar); spri.x += 20; /*sleep(1);*/ }

	while(!IN_KeyDown(sc_Escape))
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
FUNCTIONKEYFUNCTIONS;
	}
	VGAmodeX(0, 1, &gvar);
	MM_ShowMemory(&gvar);
	MM_DumpData(&gvar);
	MM_Report_(&gvar);
	Shutdown16(&gvar);
	free(spri.sprite_vrl_cont);
	free(vrl_line_offsets);
	MM_FreePtr(&bigbuffer, &gvar);
	//MM_FreePtr(&((void __based(sega)*)spri.spritesheet->buffer), &mm);
	//printf("CPU to VGA: %f\n", t1);
	//printf("VGA to VGA: %f\n", t2);
	heapdump(&gvar);
	printf("Project 16 vrstest.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("t1: %f\n", t1);
	printf("t2: %f\n", t2);
//0000	printf("gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
//0000	printf("gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	printf("Num %d", num_of_vrl);
//	if(baka) printf("\nyay!\n");
//	else printf("\npoo!\n");
}
