/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Steven van der Schoot

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "Include/VGA.h"
#include "Include/text.h"

void wait()
{
  int i = 0;
  for (; i < 0x1FFFFFFF; i++);
}

struct videoMode_s
{
  unsigned int  width;
  unsigned int  height;
  unsigned int  depth;
  bool          chain4;
};

const struct videoMode_s videoModes[3] = {
    { 320, 200, 1, true }, // 320 x 200 x 256 (linear) <-- Highest linear resolution!
    { 320, 200, 1, false}, // 320 x 200 x 256 (planar)
    { 600, 400, 1, false}, // 600 x 400 x 256 (planar) <-- Highest VGA resolution!
};
char* screenbuf; // sreen buffer, containing all pixels that should be written to the screen.
int videoMode;   // the current video mode.

/**
 * This is the initial function for the VGA driver
 *
 * @return
 *   false if failt, true if succeded
 */
bool vgaInit()
{
  /**
   * @TODO
   *   - load settings file or get setting from preloaded settings file.
   *   - get videomode from settings file.
   */
  int mode = 0; //should become a user defined videomode,  from a settings file.
  screenbuf = kmalloc(1); // while there's no timer, we cannot buffer the screen...
  if ( setVideoMode( mode ) == -1 )
    if ( setVideoMode(0) == -1 )
      return false;

  if ( !textInitG() )
    return false;

  return true;
}

/**
 * This function sets the videomode
 *
 * @param mode
 *   Number (or ID) of the videomode. This should corsponding with the videoModes array.
 *
 * @return
 *   If fails, this will be -1, else it will be or just some value (not -1) or a value
 *   returned by interupt.
 */

extern boolean pageDbg;

int setVideoMode(int mode)
{
	int size = videoModes[mode].width * videoModes[mode].height * videoModes[mode].depth;
	free(screenbuf);
	if (size != 0)
		screenbuf = kmalloc(size);
	else
		screenbuf = NULL;

	printf("Check 1\n");

	if(screenbuf==NULL)
		{screenbuf = 0xA0000;printf("kmalloc(%i) returned NULL!\n",size);return -1;}
// 	if ( 0 == setModeViaPorts(videoModes[mode].width, videoModes[mode].height, videoModes[mode].chain4?1:0))
// 		{printf("setModeViaPorts(%i,%i,%i) failed!\n",videoModes[mode].width, videoModes[mode].height, videoModes[mode].chain4?1:0);return -1;}
	printf("Check 1.1\n");
	memset(0xA0000,11,16);
	printf("Check 2\n");
	printf("Screenbuf: %X\nESP: %X\nValue: %X\n", screenbuf, getESP(), size);
	printf("Check 2.1\n");
	int i = 0;
	for (; i < 0x4FFFFFFF; i++);
	pageDbg = TRUE;
	printf("Check 2.2\n");
	memset(screenbuf,0,size); //hangs
	printf("Check 3\n");
	for(;;);
	memset(0xA0010,11,16);


	videoMode = mode;
	updateScreen();
	return 0;
}

//#define outp(port,msg) outb(port,msg)
//#define outpw(port,msg) outw(port,msg)
#define inp(port) inb(port)
//#define inpw(port,msg) inw(port,msg)
#define SZ(x) (sizeof(x)/sizeof(x[0]))

// misc out (3c2h) value for various modes

void outpw(unsigned short port, unsigned short value)
{
asm volatile ("outw %%ax,%%dx": :"dN"(port), "a"(value));
}

void outp(unsigned short port, unsigned char value)
{
asm volatile ("outb %%al,%%dx": :"dN"(port), "a"(value));
}

/**
 * For these numbers see: http://wiki.osdev.org/VGA_Hardware#List_of_register_settings
 */

#define R_COM  0x63 // "common" bits

#define R_W256 0x00
#define R_W320 0x00
#define R_W360 0x04
#define R_W376 0x04
#define R_W400 0x04

#define R_H200 0x00
#define R_H224 0x80
#define R_H240 0x80
#define R_H256 0x80
#define R_H270 0x80
#define R_H300 0x80
#define R_H360 0x00
#define R_H400 0x00
#define R_H480 0x80
#define R_H564 0x80
#define R_H600 0x80


static const byte hor_regs [] = { 0x0,  0x1,  0x2,  0x3,  0x4,
0x5,  0x13 };

static const byte width_256[] = { 0x5f, 0x3f, 0x40, 0x82, 0x4a,
0x9a, 0x20 };
static const byte width_320[] = { 0x5f, 0x4f, 0x50, 0x82, 0x54,
0x80, 0x28 };
static const byte width_360[] = { 0x6b, 0x59, 0x5a, 0x8e, 0x5e,
0x8a, 0x2d };
static const byte width_376[] = { 0x6e, 0x5d, 0x5e, 0x91, 0x62,
0x8f, 0x2f };
static const byte width_400[] = { 0x70, 0x63, 0x64, 0x92, 0x65,
0x82, 0x32 };

static const byte ver_regs  [] = { 0x6,  0x7,  0x9,  0x10, 0x11,
0x12, 0x15, 0x16 };

static const byte height_200[] = { 0xbf, 0x1f, 0x41, 0x9c, 0x8e,
0x8f, 0x96, 0xb9 };
static const byte height_224[] = { 0x0b, 0x3e, 0x41, 0xda, 0x9c,
0xbf, 0xc7, 0x04 };
static const byte height_240[] = { 0x0d, 0x3e, 0x41, 0xea, 0xac,
0xdf, 0xe7, 0x06 };
static const byte height_256[] = { 0x23, 0xb2, 0x61, 0x0a, 0xac,
0xff, 0x07, 0x1a };
static const byte height_270[] = { 0x30, 0xf0, 0x61, 0x20, 0xa9,
0x1b, 0x1f, 0x2f };
static const byte height_300[] = { 0x70, 0xf0, 0x61, 0x5b, 0x8c,
0x57, 0x58, 0x70 };
static const byte height_360[] = { 0xbf, 0x1f, 0x40, 0x88, 0x85,
0x67, 0x6d, 0xba };
static const byte height_400[] = { 0xbf, 0x1f, 0x40, 0x9c, 0x8e,
0x8f, 0x96, 0xb9 };
static const byte height_480[] = { 0x0d, 0x3e, 0x40, 0xea, 0xac,
0xdf, 0xe7, 0x06 };
static const byte height_564[] = { 0x62, 0xf0, 0x60, 0x37, 0x89,
0x33, 0x3c, 0x5c };
static const byte height_600[] = { 0x70, 0xf0, 0x60, 0x5b, 0x8c,
0x57, 0x58, 0x70 };

// the chain4 parameter should be 1 for normal 13h-type mode, but
// only allows 320x200 256x200, 256x240 and 256x256 because you
// can only access the first 64kb

// if chain4 is 0, then plane mode is used (tweaked modes), and
// you'll need to switch planes to access the whole screen but
// that allows you using any resolution, up to 400x600

int setModeViaPorts(int width, int height,int chain4)
  // returns 1=ok, 0=fail
{
   const byte *w,*h;
   byte val;
   int a;

   switch(width) {
      case 256: w=width_256; val=R_COM+R_W256; break;
      case 320: w=width_320; val=R_COM+R_W320; break;
      case 360: w=width_360; val=R_COM+R_W360; break;
      case 376: w=width_376; val=R_COM+R_W376; break;
      case 400: w=width_400; val=R_COM+R_W400; break;
      default: return 0; // fail
   }
   switch(height) {
      case 200: h=height_200; val|=R_H200; break;
      case 224: h=height_224; val|=R_H224; break;
      case 240: h=height_240; val|=R_H240; break;
      case 256: h=height_256; val|=R_H256; break;
      case 270: h=height_270; val|=R_H270; break;
      case 300: h=height_300; val|=R_H300; break;
      case 360: h=height_360; val|=R_H360; break;
      case 400: h=height_400; val|=R_H400; break;
      case 480: h=height_480; val|=R_H480; break;
      case 564: h=height_564; val|=R_H564; break;
      case 600: h=height_600; val|=R_H600; break;
      default: return 0; // fail
   }

   // chain4 not available if mode takes over 64k

   if(chain4 && (long)width*(long)height>65536L) return 0;

   // here goes the actual modeswitch

   outp(0x3c2,val);
   outpw(0x3d4,0x0e11); // enable regs 0-7

   for(a=0;a<SZ(hor_regs);++a)
      outpw(0x3d4,(word)((w[a]<<8)+hor_regs[a]));
   for(a=0;a<SZ(ver_regs);++a)
      outpw(0x3d4,(word)((h[a]<<8)+ver_regs[a]));

   outpw(0x3d4,0x0008); // vert.panning = 0

   if(chain4) {
      outpw(0x3d4,0x4014);
      outpw(0x3d4,0xa317);
      outpw(0x3c4,0x0e04);
   } else {
      outpw(0x3d4,0x0014);
      outpw(0x3d4,0xe317);
      outpw(0x3c4,0x0604);
   }

   outpw(0x3c4,0x0101);
   outpw(0x3c4,0x0f02); // enable writing to all planes
   outpw(0x3ce,0x4005); // 256color mode
   outpw(0x3ce,0x0506); // graph mode & A000-AFFF

/* ADDED */
   outpw(0x3d4,0x000C);
   outpw(0x3d4,0x000D);
/* END ADDED */

   inp(0x3da);
   outp(0x3c0,0x30); outp(0x3c0,0x41);
   outp(0x3c0,0x33); outp(0x3c0,0x00);

   for(a=0;a<16;a++) {    // ega pal
      outp(0x3c0,(byte)a);
      outp(0x3c0,(byte)a);
   }

   outp(0x3c0, 0x20); // enable video

   return 1;
}

/**
 * No info jet!
 *
 *
 */
void updateScreen()
{
	if(videoModes[videoMode].chain4)
		memcpy(
			screenbuf ,
			(void*)0xA0000 ,
			videoModes[videoMode].width * videoModes[videoMode].height * videoModes[videoMode].depth
		);
	else
	{
		int            size   = videoModes[videoMode].width * videoModes[videoMode].height * videoModes[videoMode].depth / 4;
		unsigned int   i      = 0                               ,
		               i2                                       ;
		unsigned char* plane1 = (unsigned char*)(0xA0000)       ;
		unsigned char* plane2 = (unsigned char*)(0xA0000+size)  ; //  ]
		unsigned char* plane3 = (unsigned char*)(0xA0000+2*size); //  ]-> these adress seem to be wrong.
		unsigned char* plane4 = (unsigned char*)(0xA0000+3*size); //  ]
		unsigned char* buf    = screenbuf                       ;

		for(; i < size; i++)
		{
			*plane1 = (unsigned char)0;
			*plane2 = (unsigned char)0;
			*plane3 = (unsigned char)0;
			*plane4 = (unsigned char)0;
			for(i2=0;i2<4;i2++)
			{
				*plane1 |= (unsigned char)( (*buf     ) & 0x03 );
				*plane2 |= (unsigned char)( (*buf << 2) & 0x03 );
				*plane3 |= (unsigned char)( (*buf << 4) & 0x03 );
				*plane4 |= (unsigned char)( (*buf << 6) & 0x03 );
				buf++;
			}
			plane1++;
			plane2++;
			plane3++;
			plane4++;
		}
	}
}

/**
 * Used to get the screen width.
 *
 * @return
 *   screen width in pixels.
 */
inline unsigned int getScreenWidth()
{
  return videoModes[videoMode].width;
}

/**
 * Used to get the screen height.
 *
 * @return
 *   screen height in pixels.
 */
inline unsigned int getScreenHeight()
{
  return videoModes[videoMode].height;
}

/**
 * Used to get the screen depth.
 *
 * @return
 *   screen depth in chars.
 */
inline unsigned int getScreenDepth()
{
  return videoModes[videoMode].depth;
}

/**
 * Used to get the screen buffer as image buffer.
 *
 * @return
 *   screen height in pixels.
 */
imageBuffer getScreenBuf()
{
  return (imageBuffer){screenbuf,videoModes[videoMode].width,videoModes[videoMode].height};
}
