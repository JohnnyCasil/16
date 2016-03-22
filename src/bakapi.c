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
 * You screen.heightould have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/bakapi.h"

/*
 * BAKAPEE!
 */
global_game_variables_t gvar;
static bakapee_t bakapee;
word key,d,xpos,ypos,xdir,ydir;
int ch=0x0;

void
main(int argc, char *argvar[])
{
	// main variables values
	d=4; // switch variable
	key=2; // default screensaver number
	xpos=TILEWH*2;
	ypos=TILEWH*2;
	xdir=1;
	ydir=1;

#ifdef MXLIB
	VGAmodeX(1, &gvar);
#else
	mxSetMode(3);
#endif
	bakapee.xx = rand()&0%gvar.video.page[0].width;
	bakapee.yy = rand()&0%gvar.video.page[0].height;
	bakapee.gq = 0;
	bakapee.sx=0;
	bakapee.sy=0;
	bakapee.bakax=0;
	bakapee.bakay=0;
	bakapee.coor=0;
	bakapee.tile=0;

	/* setup camera and screen~ */
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].width += (TILEWH*2);
	gvar.video.page[0].height += (TILEWH*2);
	textInit();

	//modexPalUpdate(bmp.palette); //____
	//modexDrawBmp(VGA, 0, 0, &bmp, 0); //____
	//getch(); //____

	modexShowPage(&gvar.video.page[0]);

// screen savers
#ifdef BOINK
	while(d>0)	// on!
	{
		if(!kbhit())
		{ // conditions of screen saver
			ding(&gvar.video.page[0], &bakapee, key);
		}
		else
		{
			#ifndef MXLIB
			mxChangeMode(0);
#else
			VGAmodeX(0, &gvar);
#endif
			//modexLeave();
			// user imput switch
			fprintf(stderr, "xx=%d	yy=%d\n", bakapee.xx, bakapee.yy);
			printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 0 to quit.\n", getch());  // prompt the user
//			printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 0 to quit.\n");  // prompt the user
//			clrstdin();
			scanf("%d", &key);
			//if(key==3){xx=yy=0;} // crazy screen saver wwww
			if(key==0){ d=0; }else{
				gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
				gvar.video.page[0].width += (TILEWH*2);
				gvar.video.page[0].height += (TILEWH*2);
#ifdef MXLIB
				VGAmodeX(1, &gvar);
#else
				mxChangeMode(3);
#endif
				modexShowPage(&gvar.video.page[0]);
			}
		}
	}
#else
	while(1)
	{ // conditions of screen saver
		while(!kbhit())
		{
			ding(&gvar.video.page[0], &bakapee, key);
		}
		//end of screen savers
		/*for(int x = 0; x < gvar.video.page[0].width; ++x)
		{
			modexputPixel(&page, x, 0, 15);
			mxPutPixel(x, gvar.video.page[0].height-1, 15);
			}
		for (int y = 0; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(gvar.video.page[0].width-1, y, 15);
			}
		for (int x = 0; x < VW; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, VH-1, 15);
			}
		for (int y = 240; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(VW-1, y, 15);
			}*/
		pdump(&gvar.video.page[0]);
		getch();
		//text box
		/*++++mxBitBlt(xpos, ypos+(TILEWH*12), gvar.video.page[0].width, TILEWH*BUFFMX, 0, BS); //copy background
		mxFillBox(xpos, ypos+(TILEWH*12), gvar.video.page[0].width, TILEWH*BUFFMX, 0, OP_SET); // background for text box
		//+(QUADWH*6)
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-48, "========================================");
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-40, "|    |Chikyuu:$line1");
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-32, "|    |$line2");
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-24, "|    |$line3");
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-16, "|    |$line4");
		mxOutText(xpos+1, ypos+gvar.video.page[0].height-8,  "========================================");
		mxFillBox(xpos+QUADWH, ypos+QUADWH+(TILEWH*12), TILEWH*2, TILEWH*2, 9, OP_SET); //portriat~
		getch();
		mxBitBlt(0, BS, gvar.video.page[0].width, TILEWH*BUFFMX, xpos, ypos+(TILEWH*12)); //copy background
		getch();++++*/
		while(!kbhit())
		{
			//for(int i=0;i<TILEWH;i++){
				ding(&gvar.video.page[0], &bakapee, key);
				modexPanPage(&gvar.video.page[0], xpos, ypos);
//++++mxFillBox(384, 304, 384, 304, 10, OP_SET);
//mxBitBlt(xpos, ypos, gvar.video.page[0].width, gvar.video.page[0].height, 32, (gvar.video.page[0].height+64+32));
//++++mxBitBlt(TILEWH*2, TILEWH*2, gvar.video.page[0].width, gvar.video.page[0].height, 32, (gvar.video.page[0].height+64+32));
				//for(word o = 0; o<TILEWH; o++){
					xpos+=xdir;
					ypos+=ydir;
					//if(ypos==1 || (ypos==(BH-gvar.video.page[0].height-1)))delay(500);
					//if((xpos>(VW-gvar.video.page[0].width-1)) || (xpos<1))delay(500);
					//mxWaitRetrace();
//mxBitBlt(32, (gvar.video.page[0].height+32), gvar.video.page[0].width, gvar.video.page[0].height, xpos, ypos);
//++++mxBitBlt(TILEWH*2, (gvar.video.page[0].height+64+32), gvar.video.page[0].width, gvar.video.page[0].height, TILEWH*2, TILEWH*2);
//xpos=ypos=TILEWH*2;
					//????modexPanPage(&gvar.video.page[0], 32, 32);
				//}
				if( (xpos>(VW-gvar.video.page[0].width-1))  || (xpos<1)){xdir=-xdir;}
				if( (ypos>(BH-gvar.video.page[0].height-1)) || (ypos<1)){ydir=-ydir;} // { Hit a boundry, change
			//}//    direction!
//mxBitBlt(32, (gvar.video.page[0].height+64+32), gvar.video.page[0].width, gvar.video.page[0].height, xpos, ypos);
//mxBitBlt(TILEWH*2, (gvar.video.page[0].height+64+32), gvar.video.page[0].width, gvar.video.page[0].height, TILEWH*2, TILEWH*2);
		}
	ch=getch();
	if(ch==0x71)break; // 'q'
	if(ch==0x1b)break; // 'ESC'
	}
//	VGAmodeX(0, &gvar);
#endif
	printf("bakapi ver. 1.04.13.04\nis made by sparky4�i���ց��j feel free to use it ^^\nLicence: GPL v3\n");
}
//pee!
