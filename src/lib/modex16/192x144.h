/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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

#ifndef _SMALLMODEXRES_H_
#define _SMALLMODEXRES_H_

#include "src/lib/types.h"

/*static const word ModeX_320x240regs[75] =
{
	0x3c2, 0x00, 0xe3,
	0x3d4, 0x00, 0x5f,
	0x3d4, 0x01, 0x4f,
	0x3d4, 0x02, 0x50,
	0x3d4, 0x03, 0x82,
	0x3d4, 0x04, 0x54,
	0x3d4, 0x05, 0x80,
	0x3d4, 0x06, 0x0d,
	0x3d4, 0x07, 0x3e,
	0x3d4, 0x08, 0x00,
	0x3d4, 0x09, 0x41,
	0x3d4, 0x10, 0xea,
	0x3d4, 0x11, 0xac,
	0x3d4, 0x12, 0xdf,
	0x3d4, 0x13, 0x28,
	0x3d4, 0x14, 0x00,
	0x3d4, 0x15, 0xe7,
	0x3d4, 0x16, 0x06,
	0x3d4, 0x17, 0xe3,
	0x3c4, 0x01, 0x01,
	0x3c4, 0x04, 0x06,
	0x3ce, 0x05, 0x40,
	0x3ce, 0x06, 0x05,
	0x3c0, 0x10, 0x41,
	0x3c0, 0x13, 0x00
};*/

static const word ModeX_192x144regs[] = {
	0x4f01,		/* horizontal display enable end */
	0x5002,		/* Start horizontal blanking */
	0x5404,		/* End horizontal blanking */
	0x8005,		/* End horizontal retrace */
	0x0d06,		 /* vertical total */
	0x3e07,		 /* overflow (bit 8 of vertical counts) */
	0x4109,		 /* cell height (2 to double-scan */
	0xea10,		 /* v sync start */
	0xac11,		 /* v sync end and protect cr0-cr7 */
	0xdf12,		 /* vertical displayed */
	0x2813,		/* offset/logical width */
	0x0014,		 /* turn off dword mode */
	0xe715,		 /* v blank start */
	0x0616,		 /* v blank end */
	0xe317		  /* turn on byte mode */
};

#endif /*_SMALLMODEXRES_H_*/
/*
void
tg::mode160x120()
{
    int crtc11;

    outp(0x3d4, 0x11); // unlock crtc
    crtc11 = inp(0x3d5) & 0x7f;
    outp(0x3d4, 0x11);
    outp(0x3d5, crtc11);

    width   = 160;
    height  = 120;
    maxx    = 159;
    maxy    = 119;
    pages   = 13;
    lineSize = 40;
    pageSize = 19200;
    modeName = "160x120";

    outp(0x3c2, 0xe3);   // mor

    outp(0x3d4, 0x00); // crtc
    outp(0x3d5, 0x32);

    outp(0x3d4, 0x01); // crtc
    outp(0x3d5, 0x27);

    outp(0x3d4, 0x02); // crtc
    outp(0x3d5, 0x28);

    outp(0x3d4, 0x03); // crtc
    outp(0x3d5, 0x20);

    outp(0x3d4, 0x04); // crtc
    outp(0x3d5, 0x2b);

    outp(0x3d4, 0x05); // crtc
    outp(0x3d5, 0x70);

    outp(0x3d4, 0x06); // crtc
    outp(0x3d5, 0x0d);

    outp(0x3d4, 0x07); // crtc
    outp(0x3d5, 0x3e);

    outp(0x3d4, 0x08); // crtc
    outp(0x3d5, 0x00);

    outp(0x3d4, 0x09); // crtc
    outp(0x3d5, 0x43);

    outp(0x3d4, 0x10); // crtc
    outp(0x3d5, 0xea);

    outp(0x3d4, 0x11); // crtc
    outp(0x3d5, 0xac);

    outp(0x3d4, 0x12); // crtc
    outp(0x3d5, 0xdf);

    outp(0x3d4, 0x13); // crtc
    outp(0x3d5, 0x14);

    outp(0x3d4, 0x14); // crtc
    outp(0x3d5, 0x00);

    outp(0x3d4, 0x15); // crtc
    outp(0x3d5, 0xe7);

    outp(0x3d4, 0x16); // crtc
    outp(0x3d5, 0x06);

    outp(0x3d4, 0x17); // crtc
    outp(0x3d5, 0xe3);

    outp(0x3c4, 0x01); // seq
    outp(0x3c5, 0x01);

    outp(0x3c4, 0x03); // seq
    outp(0x3c5, 0x00);

    outp(0x3c4, 0x04); // seq
    outp(0x3c5, 0x06);

    outp(0x3ce, 0x05); // gcr
    outp(0x3cf, 0x40);

    outp(0x3ce, 0x06); // gcr
    outp(0x3cf, 0x05);

    inp(0x3da);          // acr
    outp(0x3c0, 0x10 | 0x20);
    outp(0x3c0, 0x41);

    inp(0x3da);          // acr
    outp(0x3c0, 0x11 | 0x20);
    outp(0x3c0, 0x00);

    inp(0x3da);          // acr
    outp(0x3c0, 0x12 | 0x20);
    outp(0x3c0, 0x0f);

    inp(0x3da);          // acr
    outp(0x3c0, 0x13 | 0x20);
    outp(0x3c0, 0x00);

    inp(0x3da);          // acr
    outp(0x3c0, 0x14 | 0x20);
    outp(0x3c0, 0x00);

    outp(0x3d4, 0x11); // lock crtc
    crtc11 = inp(0x3d5) | 0x80;
    outp(0x3d4, 0x11);
    outp(0x3d5, crtc11);
}
*/