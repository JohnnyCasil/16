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
#ifndef __16_VRS__
#define __16_VRS__

#include "src/lib/16_head.h"
#include "src/lib/16_vl.h"
//#include <hw/cpu/cpu.h>
//#include <hw/dos/dos.h>
#include <hw/vga/vrl.h>
#include "src/lib/16_ca.h"
/*
struct vrs_container{
	// Size of a .vrs lob in memory
	// minus header
	dword data_size;
	union{
		byte far *buffer;
		struct vrs_header far *vrs_hdr;
	};
	// Array of corresponding vrl line offsets
	vrl1_vgax_offset_t **vrl_line_offsets;
};
*//*
struct vrl_container{
	// Size of a .vrl blob in memory
	// minus header
	dword data_size;
	union{
		byte far *buffer;
		struct vrl1_vgax_header far *vrl_header;
	};
	// Pointer to a corresponding vrl line offsets struct
	vrl1_vgax_offset_t *line_offsets;
};
*/
/* Read .vrs file into memory
* In:
* + char *filename - name of the file to load
* + struct vrs_container *vrs_cont - pointer to the vrs_container
* to load the file into
* Out:
* + int - 0 on succes, 1 on failure
*/
void VRS_ReadVRS(char *filename, entity_t *enti, global_game_variables_t *gvar);
void VRS_LoadVRS(char *filename, entity_t *enti, global_game_variables_t *gvar);
void VRS_OpenVRS(char *filename, entity_t *enti, boolean rlsw, global_game_variables_t *gvar);
void VRS_ReadVRL(char *filename, entity_t *enti, global_game_variables_t *gvar);
void VRS_LoadVRL(char *filename, entity_t *enti, global_game_variables_t *gvar);
void VRS_OpenVRL(char *filename, entity_t *enti, boolean rlsw, global_game_variables_t *gvar);

/* Seek and return a specified .vrl blob from .vrs blob in memory
* In:
* + struct vrs_container *vrs_cont - pointer to the vrs_container
* with a loaded .vrs file
* + uint16_t id - id of the vrl to retrive
* + struct vrl_container * vrl_cont - pointer to vrl_container to load to
* Out:
* int - operation status
* to the requested .vrl blob
*/
int get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id, struct vrl_container * vrl_cont);

#endif
