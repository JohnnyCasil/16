/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/lib/16_vl.h"

byte far* VGA=(byte far*) 0xA0000000;   /* this points to video memory. */

static void fadePalette(sbyte fade, sbyte start, word iter, byte *palette);
static byte tmppal[PAL_SIZE];

/////////////////////////////////////////////////////////////////////////////
//																														//
// setvideo() - This function Manages the video modes												//
//																														//
/////////////////////////////////////////////////////////////////////////////
void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv)
{
	union REGS in, out;

	switch (vq)
	{
		case 0: // deinit the video
			// change to the video mode we were in before we switched to mode 13h
			modexLeave();
			in.h.ah = 0x00;
			in.h.al = gv->video.old_mode;
			int86(0x10, &in, &out);
		break;
		default: // init the video
			// get old video mode
			//in.h.ah = 0xf;
			//int86(0x10, &in, &out);
			gv->video.old_mode = vgaGetMode();//out.h.al;
			// enter mode
			modexEnter(vq, cmem, gv);
		break;
	}
}

static void
vgaSetMode(byte mode)
{
	union REGS regs;

	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	int86(VIDEO_INT, &regs, &regs);
  //int10_setmode(mode);
}

//---------------------------------------------------
//
// Use the bios to get the current video mode
//

byte/*FIXME: why long? "long" is 32-bit datatype, VGA modes are 8-bit numbers. */
vgaGetMode()
{
	return int10_getmode();
}

/* -========================= Entry  Points ==========================- */
void modexEnter(sword vq, boolean cmem, global_game_variables_t *gv)
{
	word i;
	struct vga_mode_params cm;
	//int CRTParmCount;

	vgaSetMode(VGA_256_COLOR_MODE);
	vga_enable_256color_modex();

	update_state_from_vga();
	vga_read_crtc_mode(&cm);

	/* reprogram the CRT controller */
	//outp(CRTC_INDEX, 0x11); /* VSync End reg contains register write prot */
	//outp(CRTC_DATA, 0x7f);  /* get current write protect on varios regs */

	switch(vq)
	{
		case 1:
			//CRTParmCount = sizeof(ModeX_320x240regs) / sizeof(ModeX_320x240regs[0]);
			/*for(i=0; i<CRTParmCount; i++) {
				outpw(CRTC_INDEX, ModeX_320x240regs[i]);
			}*/
			/* width and height */
			gv->video.page[0].sw = vga_state.vga_width = 320; // VGA lib currently does not update this
			gv->video.page[0].sh = vga_state.vga_height = 240; // VGA lib currently does not update this
			/* virtual width and height. match screen, at first */
			gv->video.page[0].height = gv->video.page[0].sh;
			gv->video.page[0].width = gv->video.page[0].sw;

			// mode X BYTE mode
			cm.word_mode = 0;
			cm.dword_mode = 0;
			// 320x240 mode 60Hz
			cm.horizontal_total=0x5f + 5; /* CRTC[0]			 -5 */
			cm.horizontal_display_end=0x4f + 1; /* CRTC[1]	   -1 */
			cm.horizontal_blank_start=0x50 + 1; /* CRTC[2] */
//			cm.horizontal_blank_end=0x82 + 1;   /* CRTC[3] bit 0-4 & CRTC[5] bit 7 *///skewing ^^;
			cm.horizontal_start_retrace=0x54;/* CRTC[4] */
			cm.horizontal_end_retrace=0x80;	/* CRTC[5] bit 0-4 */
			//cm.horizontal_start_delay_after_total=0x3e; /* CRTC[3] bit 5-6 */
			//cm.horizontal_start_delay_after_retrace=0x41; /* CRTC[5] bit 5-6 */
			cm.vertical_total = 0x20D + 2;
			cm.vertical_start_retrace = 0x1EA;
			cm.vertical_end_retrace = 0x1EC;
			cm.vertical_display_end = 480;
			cm.vertical_blank_start = 0x1E7 + 1;
			cm.vertical_blank_end = 0x206 + 1;
			cm.clock_select = 0; /* misc register = 0xE3  25MHz */
			cm.vsync_neg = 1;
			cm.hsync_neg = 1;
			cm.offset = (vga_state.vga_width / (4 * 2)); // 320 wide (40 x 4 pixel groups x 2)
			break;
		case 2: // TODO: 160x120 according to ModeX_160x120regs
			return;
		case 3: // TODO: 160x120 according to ModeX_320x200regs
			return;
		case 4: // TODO: 160x120 according to ModeX_192x144regs
			return;
		case 5: // TODO: 160x120 according to ModeX_256x192regs
			return;
		default:
			return;
	}

	vga_state.vga_stride = cm.offset * 2;
	vga_write_crtc_mode(&cm,0);

	/* clear video memory */
	switch (cmem)
	{
		case 1: {
			/* clear video memory */
			dword far*ptr=(dword far*)vga_state.vga_graphics_ram;//VGA;	  /* used for faster screen clearing */
			vga_write_sequencer(2/*map mask register*/,0xf/*all 4 planes*/);
			for(i = 0;i < 0x4000; i++) ptr[i] = 0x0000; // 0x4000 x dword = 64KB
			/* fix up the palette and everything */
			modexPalBlack();	//reset the palette~
		}
		break;
	}
}

void
modexLeave() {
	/* VGAmodeX restores original mode and palette */
	vgaSetMode(TEXT_MODE);
}

page_t
modexDefaultPage(page_t *p)
{
	page_t page;

	/* default page values */
	//page.data = VGA;
	//page.data = (byte far *)(vga_state.vga_graphics_ram);
	page.data = (vga_state.vga_graphics_ram);
	page.dx = 0;
	page.dy = 0;
	page.sw = p->sw;
	page.sh = p->sh;
	page.width = p->sw+TILEWHD;
	page.height = p->sh+TILEWHD;
	page.ti.tw = page.sw/TILEWH;
	page.ti.th = page.sh/TILEWH;
	page.ti.tilesw=page.width/TILEWH;
	page.ti.tilesh=page.height/TILEWH;
	page.ti.tilemidposscreenx = page.ti.tw/2;
	page.ti.tilemidposscreeny = (page.ti.th/2)+1;
	page.stridew=page.width/4;
	page.pagesize = (word)(page.stridew)*page.height;
	page.pi=page.width*4;
	page.id = 0;

	return page;
}

/* returns the next page in contiguous memory
 * the next page will be the same size as p, by default
 */
page_t
modexNextPage(page_t *p) {
	page_t result;

	result.data = p->data + (p->pagesize);
	result.dx = p->dx;	// not used anymore we use page[0].dx
	result.dy = p->dy;	// not used anymore we use page[0].dy
	result.sw = p->sw;
	result.sh = p->sh;
	result.width = p->width;
	result.height = p->height;
	result.ti.tw = p->ti.tw;
	result.ti.th = p->ti.th;
	result.ti.tilesw = p->ti.tilesw;
	result.ti.tilesh = p->ti.tilesh;
	result.stridew=p->stridew;
	result.pagesize = p->pagesize;
	result.pi=result.width*4;
	result.id = p->id+1;

	return result;
}

//next page with defined dimentions~
page_t
modexNextPageFlexibleSize(page_t *p, word x, word y)
{
	page_t result;

	result.data = p->data + (p->pagesize);  /* compute the offset */
	result.dx = 0;	// not used anymore we use page[0].dx
	result.dy = 0;	// not used anymore we use page[0].dy
	result.sw = x;
	result.sh = y;
	result.width = x;
	result.height = y;
	result.ti.tw = result.sw/TILEWH;
	result.ti.th = result.sh/TILEWH;
	result.ti.tilesw=result.width/TILEWH;
	result.ti.tilesh=result.height/TILEWH;
	result.id = p->id+1;
	result.stridew=result.width/4;//p->sw/4;
	result.pagesize = (word)(result.stridew)*result.height;
/*	switch(result.id)
	{
		case 2:
			result.pi=p->width*4;
		break;
		case 3:
			result.pi=p->pi;
		break;
	}*/
	result.pi=result.width*4;

	return result;
}

void modexCalcVmemRemain(video_t *video)
{
	byte i;
	//printf("\n\n	1st vmem_remain=%u\n", video->vmem_remain);
	for(i=0; i<video->num_of_pages; i++)
	{
		video->vmem_remain-=video->page[i].pagesize;
		//printf("		[%u], video->page[%u].pagesize=%u\n", i, i, video->page[i].pagesize);
		//printf("		[%u], vmem_remain=%u\n", i, video->vmem_remain);
	}
}

void VL_Initofs(video_t *video)
{
	if(!video->bgps)
	{
		video->ofs.offscreen_ofs =	video->page[0].pagesize+video->page[1].pagesize;//(vga_state.vga_stride * vga_state.vga_height);
		video->ofs.pattern_ofs =	(uint16_t)video->page[2].data;
	}else{
		video->ofs.offscreen_ofs =	0;
		video->ofs.pattern_ofs =	0;//(uint16_t)video->page[0].data;
	}
}

void modexHiganbanaPageSetup(video_t *video)
{
	video->vmem_remain=65535U;
	video->num_of_pages=0;
	(video->page[0]) = modexDefaultPage(&(video->page[0]));	video->num_of_pages++;	//video->page[0].width += (TILEWHD); video->page[0].height += (TILEWHD);
	(video->page[1]) = modexNextPage(&(video->page[0]));	video->num_of_pages++;
//0000	(video->page[2]) = modexNextPageFlexibleSize(&(video->page[1]), (video->page[0]).width, TILEWH*4);		video->num_of_pages++;
//0000	(video->page[3]) = (video->page[2]);		video->num_of_pages++;
////	(video->page[2]) = modexNextPageFlexibleSize(&(video->page[1]), TILEWH*4, TILEWH*4);		video->num_of_pages++;
////	(video->page[3]) = modexNextPageFlexibleSize(&(video->page[2]), video->page[0].sw, 208);	video->num_of_pages++;
 	(video->page[2]) = modexNextPageFlexibleSize(&(video->page[1]), video->page[0].width, 96);	video->num_of_pages++;
 	(video->page[3]) = modexNextPageFlexibleSize(&(video->page[2]), video->page[0].width, 96);	video->num_of_pages++;
	modexCalcVmemRemain(video);

	video->sp=video->p =	0;	//showpage
	video->dorender =	1;			//render
	video->vh=video->page[0].height+video->page[1].height+video->page[2].height+video->page[3].height;

	VL_Initofs(video);
	//doslib origi var
	video->omemptr=			vga_state.vga_graphics_ram;
	video->vga_draw_stride=	vga_state.vga_draw_stride;
	video->vga_draw_stride_limit=	vga_state.vga_draw_stride_limit;
	//sprite render switch and bgpreservation switch
	video->rss=		1;
	video->bgps=	1;

	//setup the buffersize
	video->page[0].dx=video->page[0].dy=
		video->page[1].dx=video->page[1].dy=TILEWH;	// 1 tile size buffer
	video->page[2].dx=video->page[2].dy=
		video->page[3].dx=video->page[3].dy=0;		// cache pages are buffer wwww
}

//
// move page to appropriate part and show it
//
void
modexShowPage(page_t *page) {
	word high_address, low_address, offset;
	byte crtcOffset;

	/* calculate offset */
	offset = (word) page->data;
	offset += page[0].dy * (page->width >> 2 );
	offset += page[0].dx >> 2;

	/* calculate crtcOffset according to virtual width */
	crtcOffset = page->width >> 3;

	high_address = HIGH_ADDRESS | (offset & 0xff00);
	low_address  = LOW_ADDRESS  | (offset << 8);

	/* wait for appropriate timing and then program CRTC */
//+=+=										while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
	outpw(CRTC_INDEX, high_address);
	outpw(CRTC_INDEX, low_address);
	outp(CRTC_INDEX, 0x13);
	outp(CRTC_DATA, crtcOffset);

	/* wait for one retrace */
//+=+=										while (!(inp(INPUT_STATUS_1) & VRETRACE));

	/* do PEL panning here */
	outp(AC_INDEX, 0x33);
	outp(AC_INDEX, (page[0].dx & 0x03) << 1);
}

//args: page, vertical sync switch, screen resolution switch, page0 switch
void
VL_ShowPage(page_t *page, boolean vsync, boolean sr) {
	word high_address, low_address, offset;
	byte crtcOffset;

	// calculate offset
	offset = (word) page->data;
	offset += page->dy * (page->width >> 2 );
	offset += page->dx >> 2;

	// calculate crtcOffset according to virtual width
	switch(sr)
	{
		case 1:
			crtcOffset = page->sw >> 3;
		break;
		default:
		case 0:
			crtcOffset = page->width >> 3;
		break;
	}

	high_address = HIGH_ADDRESS | (offset & 0xff00);
	low_address  = LOW_ADDRESS  | (offset << 8);

	// wait for appropriate timing and then program CRTC
	if(vsync) while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
	outpw(CRTC_INDEX, high_address);
	outpw(CRTC_INDEX, low_address);
	outp(CRTC_INDEX, 0x13);
	outp(CRTC_DATA, crtcOffset);

	// wait for one retrace
	if(vsync) while (!(inp(INPUT_STATUS_1) & VRETRACE));

	// do PEL panning here
	outp(AC_INDEX, 0x33);
	outp(AC_INDEX, (page->dx & 0x03) << 1);
	vga_state.vga_graphics_ram = (VGA_RAM_PTR)page->data;
}

//=============================================================================

void
modexPanPage(page_t *page, int dx, int dy) {
	page[0].dx = dx;
	page[0].dy = dy;
}

void
modexSelectPlane(byte plane) {
	outp(SC_INDEX, MAP_MASK);	  /* select plane */
	outp(SC_DATA,  plane);
}

void
modexClearRegion(page_t *page, int x, int y, int w, int h, byte  color) {
	word pageOff = (word) page->data;
	word xoff=x/4;	   /* xoffset that begins each row */
	word scanCount=w/4;  /* number of iterations per row (excluding right clip)*/
	word poffset = pageOff + y*(page->stridew) + xoff; /* starting offset */
	word nextRow = page->stridew-scanCount-1;  /* loc of next row */
	byte lclip[] = {0x0f, 0x0e, 0x0c, 0x08};  /* clips for rectangles not on 4s */
	byte rclip[] = {0x00, 0x01, 0x03, 0x07};
	byte left = lclip[x&0x03];
	byte right = rclip[(x+w)&0x03];

	/* handle the case which requires an extra group */
	if((x & 0x03) && !((x+w) & 0x03)) {
	  right=0x0f;
	}

	//printf("modexClearRegion(x=%u, y=%u, w=%u, h=%u, left=%u, right=%u)\n", x, y, w, h, left, right);

	__asm {
		PUSHF
		PUSH ES
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		PUSH SI
		PUSH DI
		MOV AX, SCREEN_SEG	  ; go to the VGA memory
		MOV ES, AX
		MOV DI, poffset	 ; go to the first pixel
		MOV DX, SC_INDEX	; point to the map mask
		MOV AL, MAP_MASK
		OUT DX, AL
		INC DX
		MOV AL, color	   ; get ready to write colors
	SCAN_START:
		MOV CX, scanCount	   ; count the line
		MOV BL, AL		  ; remember color
		MOV AL, left		; do the left clip
		OUT DX, AL		  ; set the left clip
		MOV AL, BL		  ; restore color
		STOSB		   ; write the color
		DEC CX
		JZ SCAN_DONE		; handle 1 group stuff

		;-- write the main body of the scanline
		MOV BL, AL		  ; remember color
		MOV AL, 0x0f		; write to all pixels
		OUT DX, AL
		MOV AL, BL		  ; restore color
		REP STOSB		   ; write the color
	SCAN_DONE:
		MOV BL, AL		  ; remeber color
		MOV AL, right
		OUT DX, AL		  ; do the right clip
		MOV AL, BL		  ; restore color
		STOSB		   ; write pixel
		ADD DI, nextRow	 ; go to the next row
		DEC h
		JNZ SCAN_START
		POP DI
		POP SI
		POP DX
		POP CX
		POP BX
		POP AX
		POP ES
		POPF
	}
}

/* moved to src/lib/modex16/16render.c */

/* copy a region of video memory from one page to another.
 * It assumes that the left edge of the tile is the same on both
 * regions and the memory areas do not overlap.
 */
void
modexCopyPageRegion(page_t *dest, page_t *src,
			word sx, word sy,
			word dx, word dy,
			word width, word height)
{
	word doffset = (word)dest->data + dy*(dest->stridew) + dx/4;
	word soffset = (word)src->data + sy*(src->stridew) + sx/4;
	word scans   = vga_state.vga_stride;				//++++0000 the quick and dirty fix of the major issue with p16 video display wwww
	word nextSrcRow = src->stridew - scans - 1;
	word nextDestRow = dest->stridew - scans - 1;
	byte lclip[] = {0x0f, 0x0e, 0x0c, 0x08};  /* clips for rectangles not on 4s */
	byte rclip[] = {0x00, 0x01, 0x03, 0x07};
	byte left = lclip[sx&0x03];
	byte right = rclip[(sx+width)&0x03];

	/* handle the case which requires an extra group */
	if((sx & 0x03) && !((sx+width) & 0x03)) {
		right=0x0f;
	}

//	printf("modexCopyPageRegion(src->stridew=%u, dest->stridew=%u, sx=%u, sy=%u, dx=%u, dy=%u, width=%u, height=%u, left=%u, right=%u)\n", src->stridew, dest->stridew, sx, sy, dx, dy, width, height, left, right);

	__asm {
		PUSHF
		PUSH ES
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		PUSH SI
		PUSH DI

		MOV AX, SCREEN_SEG	  ; work in the vga space
		MOV ES, AX		  ;
		MOV DI, doffset	 ;
		MOV SI, soffset	 ;

		MOV DX, GC_INDEX	; turn off cpu bits
		MOV AX, 0008h	   ;
		OUT DX, AX

		MOV AX, SC_INDEX	; point to the mask register
		MOV DX, AX		  ;
		MOV AL, MAP_MASK	;
		OUT DX, AL		  ;
		INC DX		  ;

	ROW_START:
		PUSH DS
		MOV AX, ES
		MOV DS, AX
		MOV CX, scans	   ; the number of latches

		MOV AL, left		; do the left column
		OUT DX, AL		  ;
		MOVSB		   ;
		DEC CX		  ;

		MOV AL, 0fh		 ; do the inner columns
		OUT DX, AL
		REP MOVSB		   ; copy the pixels

		MOV AL, right	   ; do the right column
		OUT DX, AL
		MOVSB
		POP DS

		MOV AX, SI		  ; go the start of the next row
		ADD AX, nextSrcRow	  ;
		MOV SI, AX		  ;
		MOV AX, DI		  ;
		ADD AX, nextDestRow	 ;
		MOV DI, AX		  ;

		DEC height		  ; do the rest of the actions
		JNZ ROW_START	   ;

		MOV DX, GC_INDEX+1	  ; go back to CPU data
		MOV AL, 0ffh		; none from latches
		OUT DX, AL		  ;

		POP DI
		POP SI
		POP DX
		POP CX
		POP BX
		POP AX
		POP ES
		POPF
	}
}


/* fade and flash */
void
modexFadeOn(word fade, byte *palette) {
	fadePalette(-fade, 64, 64/fade+1, palette);
}


void
modexFadeOff(word fade, byte *palette) {
	fadePalette(fade, 0, 64/fade+1, palette);
}


void
modexFlashOn(word fade, byte *palette) {
	fadePalette(fade, -64, 64/fade+1, palette);
}


void
modexFlashOff(word fade, byte *palette) {
	fadePalette(-fade, 0, 64/fade+1, palette);
}


static void
fadePalette(sbyte fade, sbyte start, word iter, byte *palette) {
	word i;
	byte dim = start;

	/* handle the case where we just update */
	if(iter == 0) {
	modexPalUpdate(palette);
	return;
	}

	while(iter > 0) {  /* FadeLoop */
	for(i=0; i<PAL_SIZE; i++) { /* loadpal_loop */
		tmppal[i] = palette[i] - dim;
		if(tmppal[i] > 127) {
		tmppal[i] = 0;
		} else if(tmppal[i] > 63) {
		tmppal[i] = 63;
		}
	}
	modexPalUpdate(tmppal);
	iter--;
	dim += fade;
	}
}


/* save and load */
void
modexPalSave(byte *palette) {
	int  i;

	outp(PAL_READ_REG, 0);	  /* start at palette entry 0 */
	for(i=0; i<PAL_SIZE; i++) {
	palette[i] = inp(PAL_DATA_REG); /* read the palette data */
	}
}


byte *
modexNewPal() {
	byte *ptr;
	ptr = malloc(PAL_SIZE);

	/* handle errors */
	if(!ptr) {
		printf("Could not allocate palette.\n");
	}

	return ptr;
}


void
modexLoadPalFile(byte *filename, byte **palette) {
	FILE *file;
	byte *ptr;

	/* free the palette if it exists */
	if(*palette) {
	free(*palette);
	}

	/* allocate the new palette */
	*palette = modexNewPal();

	/* open the file */
	file = fopen(filename, "rb");
	if(!file) {
	printf("Could not open palette file: %s\n", filename);
	}

	/* read the file */
	ptr = *palette;
	while(!feof(file)) {
	*ptr++ = fgetc(file);
	}

	fclose(file);
}


void VL_LoadPalFile(const char *filename, byte *palette)
{
	int fd;

	fd = open(filename,O_RDONLY|O_BINARY);
	if (fd >= 0) {
		word i;

		read(fd,palette,	PAL_SIZE);
		close(fd);

		vga_palette_lseek(0);
		for (i=0;i < 256;i++) vga_palette_write(palette[(i*3)+0]>>2,palette[(i*3)+1]>>2,palette[(i*3)+2]>>2);
	}
}


void
modexSavePalFile(char *filename, byte *pal) {
	//unsigned int i;
	FILE *file;

	/* open the file for writing */
	file = fopen(filename, "wb");
	if(!file) {
	printf("Could not open %s for writing\n", filename);
	}

	/* write the data to the file */
	fwrite(pal, 1, PAL_SIZE, file);
	fclose(file);
}


/* blanking */
void
modexPalBlack() {
	fadePalette(-1, 64, 1, tmppal);
}


void
modexPalWhite() {
	fadePalette(-1, -64, 1, tmppal);
}


/* utility */
//moved to 16_vlpal.c

void
modexPalUpdate(byte *p)
{
	int i;
	//modexWaitBorder();
	vga_wait_for_vsync();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	for(i=0; i<PAL_SIZE/2; i++)
	{
		outp(PAL_DATA_REG, p[i]);
	}
	//modexWaitBorder();	  /* waits one retrace -- less flicker */
	vga_wait_for_vsync();
	for(; i<PAL_SIZE; i++)
	{
		outp(PAL_DATA_REG, p[(i)]);
	}
}

void
modexPalUpdate0(byte *p)
{
	int i;
	//modexWaitBorder();
	vga_wait_for_vsync();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	for(i=0; i<PAL_SIZE/2; i++)
	{
		outp(PAL_DATA_REG, rand());
	}
	//modexWaitBorder();	  /* waits one retrace -- less flicker */
	vga_wait_for_vsync();
	for(; i<PAL_SIZE; i++)
	{
		outp(PAL_DATA_REG, rand());
	}
}

void
modexPalOverscan(word col)
{
	//modexWaitBorder();
	vga_wait_for_vsync();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	outp(PAL_DATA_REG, col);
}

//color checker~
//i want to make another vesion that checks the palette when the palette is being appened~
void modexchkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z, word *i/*, word *offset*/)
{
		byte *pal;
		word zz=0;
		pal = modexNewPal();
		modexPalSave(pal);
		//printf("q: %02d\n", (*q));
		printf("chkcolor start~\n");
		printf("1				   (*z): %d\n", (*z)/3);
		printf("1				   (*i): %d\n", (*i)/3);
//		  printf("1 offset of color in palette	(*q): %d\n", (*q)/3);
		printf("wwwwwwwwwwwwwwww\n");
		//check palette for dups
		for(; (*z)<PAL_SIZE; (*z)+=3)
		{
			//printf("\n		z: %d\n", (*z));
			//printf("		  q: %d\n", (*q));
			//printf("		  z+q: %d\n\n", ((*z)+(*q)));
			//if((*z)%3==0)
			//{
//----			  if(pal[(*z)]==pal[(*z)+3] && pal[(*z)+1]==pal[(*z)+4] && pal[(*z)+2]==pal[(*z)+5])
				if((*z)==(*i))
				{
//					  printf("\n%d	[%02d][%02d][%02d]\n", (*z), pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
//					  printf("%d	  [%02d][%02d][%02d]\n\n", (*z)+3, pal[(*z)+3], pal[(*z)+4], pal[(*z)+5]);
//0000				  (*z)-=3;
					break;
				}
				else for(zz=0; zz<(*q); zz+=3)
				{
					//printf("zz: %02d\n", zz/3);
					if(zz%3==0)
					{
						if(pal[((*z)+(*q))]==pal[((*z)+(*q))+3] && pal[((*z)+(*q))+1]==pal[((*z)+(*q))+4] && pal[((*z)+(*q))+2]==pal[((*z)+(*q))+5])	//break if duplicate colors found in palette because it have reached the end of the current data of the palette
						{
//							  (*z)-=3;
//							  (*i)-=3;
//							  printf("\nzq1:%d[%02d][%02d][%02d]\n", (zz+q), pal[(zz+q)], pal[(zz+q)+1], pal[(zz+q)+2]);
//							  printf("zq2:%d[%02d][%02d][%02d]\n\n", (zz+q)+3, pal[(zz+q)+3], pal[(zz+q)+4], pal[(zz+q)+5]);
							break;
						}
						else if(pal[zz]==pal[((*z)+(*q))] && pal[zz+1]==pal[((*z)+(*q))+1] && pal[zz+2]==pal[((*z)+(*q))+2])
						{
//							  printf("\n\nwwwwwwwwwwwwwwww\n");
//							  printf("	zq: %d  [%02d][%02d][%02d] value that is needing to be changed~\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
//							  printf("	zz: %d  [%02d][%02d][%02d] value that the previous value is going to change to~\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
//							  //printf("	  zv: %d  [%02d][%02d][%02d] wwww\n", (zz-z+q)/3, pal[(zz-z+q)], pal[(zz-z+q)+1], pal[(zz-z+q)+2]);
//							  printf("	z : %d  [%02d][%02d][%02d] offset value~\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
//++++						  (*i)--;
//							  (*z)--;
							//expand dong here
/*
planned features that i plan to implement~
image that has values on the pallete list!
wwww
no... wait.... no wwww
*/
							//for(zzii=0; zzii<3; zzii++)
							//{
								//printf("z+q: %d\n\n", ((*z)+(*q)));
								a[(((*z)+(*q)))]=zz;
							//}
							(*aa)=(((*z)+(*q)));
							printf("!!					  a[%02d]: %d\n", (((*z)+(*q))/3), zz/3);
//							  printf("\n		  aa: %d\n\n", (*aa));
//							  printf("	a[%02d]=(%02d) offset array i think the palette should be updated again~\n", ((*z)+(*q))/3, a[((*z)+(*q))/3]);
//							  printf("wwwwwwwwwwwwwwww\n\n");
						}
						/*else
						{
							printf("================\n");
							printf("zq: %d  [%02d][%02d][%02d]\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
							printf("zz: %d  [%02d][%02d][%02d]\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
							printf("z : %d  [%02d][%02d][%02d]\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
							printf("================\n");
						}*/
						//printf("[%d]", (zz+q));
					}
				}
		}
		printf("wwwwwwwwwwwwwwww\n");
		printf("2				   (*z): %d\n", (*z)/3);
		printf("2				   (*i): %d\n", (*i)/3);
//		  printf("2 offset of color in palette	(*q): %d\n", (*q)/3);
		printf("chkcolor end~\n");
		free(pal);
}

void modexputPixel(page_t *page, int x, int y, byte color)
{
	word pageOff = (word) page->data;
	/* Each address accesses four neighboring pixels, so set
	   Write Plane Enable according to which pixel we want
	   to modify.  The plane is determined by the two least
	   significant bits of the x-coordinate: */
	modexSelectPlane(PLANE(x));
	//outp(SC_INDEX, 0x02);
	//outp(SC_DATA, 0x01 << (x & 3));

	/* The offset of the pixel into the video segment is
	   offset = (width * y + x) / 4, and write the given
	   color to the plane we selected above.  Heed the active
	   page start selection. */
	VGA[(unsigned)((page->width/4) * y) + (x / 4) + pageOff] = color;

}

byte modexgetPixel(page_t *page, int x, int y)
{
	word pageOff = (word) page->data;
	/* Select the plane from which we must read the pixel color: */
	outpw(GC_INDEX, 0x04);
	outpw(GC_INDEX+1, x & 3);

	return VGA[(unsigned)((page->width/4) * y) + (x / 4) + pageOff];

}

void modexDrawChar(page_t *page, int x/*for planar selection only*/, word t, word col, word bgcol, word addr)
{
	/* vertical drawing routine by joncampbell123.
	 *
	 * optimize for VGA mode X planar memory to minimize the number of times we do I/O write to map mask register.
	 * so, we enumerate over columns (not rows!) to draw every 4th pixel. bit masks are used because of the font bitmap.
	 *
	 * NTS: addr defines what VGA memory address we use, "x" is redundant except to specify which of the 4 pixels we select in the map mask register. */
	word rows = romFonts[t].charSize;
	word drawaddr;
	word colm, row;
	byte fontbyte;
	byte plane;
	byte m1,m2;

	plane = x & 3;
	m1 = 0x80; // left half
	m2 = 0x08; // right half
	for (colm=0;colm < 4;colm++) {
		drawaddr = addr;
		modexSelectPlane(PLANE(plane));
		for (row=0;row < rows;row++) {
			fontbyte = romFontsData.l[row];
			vga_state.vga_graphics_ram[drawaddr  ] = (fontbyte & m1) ? col : bgcol;
			vga_state.vga_graphics_ram[drawaddr+1] = (fontbyte & m2) ? col : bgcol;
			drawaddr += page->width >> 2;
		}

		m1 >>= 1;
		m2 >>= 1;
		if ((++plane) == 4) {
			addr++;
			plane = 0;
		}
	}
}

void modexprint(page_t *page, word x, word y, word t, word col, word bgcol, const byte *str)
{
	word s, o, w;
	word x_draw = x;
	//word addr = (word) romFontsData.l;
	word addrq = (page->stridew) * y + (x / 4) + ((word)page->data);
	word addrr = addrq;
	byte c;

	s=romFonts[t].seg;
	o=romFonts[t].off;
	w=romFonts[t].charSize;
	romFontsData.chw=0;

	for(; *str != '\0'; str++)
	{
	c = (*str);
	if(c=='\n')
	{
		x = x_draw;
		romFontsData.chw = 0;
		addrq += (page->stridew) * 8;
		addrr = addrq;
		y += 8;
		continue;
	}

	// load the character into romFontsData.l
	// no need for inline assembly!
	// NTS: It might even be faster to just let the modexDrawChar point directly at ROM font than to copy per char! --J.C.
		_fmemcpy(romFontsData.l,MK_FP(s,o+(w*c))/*ROM font location*/,w/*char size*/);
		modexDrawChar(page, x_draw/*for mode X planar use*/, t, col, bgcol, addrr);
		x_draw += 8; /* track X for edge of screen */
		addrr += 2; /* move 8 pixels over (2 x 4 planar pixels per byte) */
	}
}

void modexprintbig(page_t *page, word x, word y, word t, word col, word bgcol, const byte *str)
{
	word i, s, o, w, j, xp;
	byte l[1024];
	word addr = (word) l;
	word chw=0;
	byte c;

	switch(t)
	{
		case 0:
			w=14;
		break;
		case 1:
			w=8;
		break;
		case 2:
			w=8;
		break;
		case 3:
			w=16;
		break;
		default:
			t=3;
			w=16;
		break;
	}

	s=romFonts[t].seg;
	o=romFonts[t].off;

	for(; *str != '\0'; str++)
	{
	c = (*str);
	if((c=='\n'/* || c=="\
"*/)/* || chw>=page->width*/)
	{
		chw=0;
		y+=w;
		continue;
	}
	//load the letter 'A'
	__asm {
		PUSHF
		PUSH ES
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		PUSH SI
		PUSH DI

		MOV DI, addr
		MOV SI, o
		MOV ES, s
		SUB AH, AH
		MOV AL, c	; the letter
		MOV CX, w
		MUL CX
		ADD SI, AX	;the address of charcter
	L1:	MOV AX, ES:SI
		MOV DS:DI, AX
		INC SI
		INC DI
		DEC CX
		JNZ L1

		POP DI
		POP SI
		POP DX
		POP CX
		POP BX
		POP AX
		POP ES
		POPF
	}

		for(i=0; i<w; i++)
		{
			j=1<<8;
			xp=0;
			while(j)
			{
				//modexputPixel(page, x+xp+chw, y+i, l[i] & j ? col:bgcol);
				modexClearRegion(page, (x+xp+chw)*8, (y+i)*8, 8, 8, l[i] & j ? col:bgcol);
				xp++;
				j>>=1;
			}
		}
		chw += xp;
	}
}

/* palette dump on display! */
void modexpdump(page_t *pee)
{
	int mult=(QUADWH);
	int palq=(mult)*TILEWH;
	int palcol=0;
	int palx, paly;
	for(paly=0; paly<palq; paly+=mult){
		for(palx=0; palx<palq; palx+=mult){
				modexClearRegion(pee, palx+TILEWH, paly+TILEWH, mult, mult, palcol);
			palcol++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//																		 //
// cls() - This clears the screen to the specified color, on the VGA or on //
//		 the Virtual screen.											 //
//																		 //
/////////////////////////////////////////////////////////////////////////////
void modexcls(page_t *page, byte color, byte *Where)
{
	//modexClearRegion(page, 0, 0, page->width, page->height, color);
	/* set map mask to all 4 planes */
	outpw(SC_INDEX, 0xff02);
	//_fmemset(VGA, color, 16000);
	_fmemset(Where, color, page->stridew*page->height);
}

//
// pattern filler from joncampbell123's code
//
void VL_PatternDraw(video_t *video, word pn, boolean sw, boolean allsw)
{
	unsigned int i,j,o,	d,h,s;
	word w;

	switch(sw)
	{
		case 0:
			w=vga_state.vga_width;
			d=0;
			s=vga_state.vga_stride;
			switch(allsw)
			{
				case 0:
					h=vga_state.vga_height;
				break;
				case 1:
					h=video->vh;
				break;
			}
		break;
		default:
			w=video->page[pn].width;
			d=(0x10000UL - (uint16_t)video->page[pn].data);
			s=video->page[pn].stridew;
			switch(allsw)
			{
				case 0:
					h=video->page[pn].height;
				break;
				case 1:
					if(!pn) h=video->vh;
					else h=video->page[pn].height;
				break;
			}
		break;
	}

	/* fill screen/pattern with a distinctive pattern */
	for (i=0;i < w;i++) {
		o = (i >> 2) + d;
		vga_write_sequencer(0x02/*map mask*/,1 << (i&3));
		for (j=0;j < h;j++,o += s)
			vga_state.vga_graphics_ram[o] = (i^j)&15; // VRL samples put all colors in first 15!
	}
}

void
modexWaitBorder() {
	while(inp(INPUT_STATUS_1)  & 8)  {
	// spin
	}

	while(!(inp(INPUT_STATUS_1)  & 8))  {
	//spin
	}
}

void
modexWaitBorder_start()
{
	while(inp(INPUT_STATUS_1)  & 8)  {
	// spin
	}

}

void
modexWaitBorder_end()
{
	while(!(inp(INPUT_STATUS_1)  & 8))  {
	// spin
	}

}

//
// printings of video memory information
//
void VL_PrintmodexmemInfo(video_t *v)
{
	byte i;

//	printf("========================================\n");
	printf("VL_PrintmodexmemInfo:\n");
//	printf("========================================\n");
	printf("  Virtual Screen: %dx", v->page[0].width);	printf("%d	", v->page[0].height);	printf("Tile: %dx", v->page[0].ti.tilesw);		printf("%d", v->page[0].ti.tilesh);	printf("=((Virtual Screen)/16)\n");
	printf("  	  Screen: %dx", v->page[0].sw);		printf("%d	", v->page[0].sh);		printf("Tile: %dx", v->page[0].ti.tw);			printf("%d", v->page[0].ti.th);		printf("=((Screen)/16)\n");

	printf("  Free Video Memory: %u\n", v->vmem_remain);
	printf("  page");
	for(i=0; i<v->num_of_pages;i++)
	{
		printf("	[%u]=", i);
		printf("(%Fp)", (v->page[i].data));
		printf(" size=%u	", v->page[i].pagesize);
		printf("w=%-3lu  h=%-3lu ", (unsigned long)v->page[i].width, (unsigned long)v->page[i].height);
		printf("sw=%-3lu  sh=%-3lu ", (unsigned long)v->page[i].sw, (unsigned long)v->page[i].sh);
		printf("pi=%u", v->page[i].pi);
		printf("\n");
	}
}
