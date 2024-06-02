/*
 * 
 * Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
 *
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either 
 * version 2 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#ifndef LIPECURSE_H
#define LIPECURSE_H

#include <curses.h>	/* includes stdio.h, termios.h, termio.h, sgetty.h */
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "i386types.h"


#define MAX_PTS		60

#define COL_TAG_LENGTH	15

#define LARGE_PADDING	2
#define SMALL_PADDING	1
#define TABLE_HEAD	1

#define COORD_MAXX	6
#define COORD_MAXY	4

#define CURSE_ERR_START	25

#define WINDOW_MINX	1+LARGE_PADDING +1+SMALL_PADDING+4+SMALL_PADDING+1 \
			+SMALL_PADDING+COL_TAG_LENGTH+SMALL_PADDING+1 \
			+SMALL_PADDING+1+LARGE_PADDING+1+5
#define WINDOW_MINY	1+LARGE_PADDING+1+TABLE_HEAD+COORD_MAXY+1 \
			+LARGE_PADDING+1+5

static WINDOW *main_win;
static int data_needs_save=0;
static int window_needs_resize=0;
static int curse_window_needs_resize=0;
static int window_maxx;
static int window_maxy;

static int coordx=1;
static int coordy=1;
static int display_coordx1;
static int display_coordx2;

static int frame_limit_ulx,frame_limit_uly;
static int frame_limit_lrx,frame_limit_lry;

static int curse_window_currentx=0;
static int curse_window_currenty=0;
static int curse_window_mlen=0;
static int curse_window_mhgt=0;
static int curse_window_len=0;
static int curse_window_hgt=0;

static char *dev=NULL;
static int new_dev=0;
static int rwm;
static pt partition_table;
static partition partitions[4];
static off64_t sector[MAX_PTS];
static int sectorno=0;
static int toplevel=0;

static char *coltags[]=
{
	"Bootable",
	"ID",
	"Start Sector",
	"Size",
	"Start C/H/S",
	"End C/H/S"
};

enum curse_error
{
	ncurses=CURSE_ERR_START,
	minWindowSize,
	deviceOpen,
	deviceSeek,
	deviceRead,
	deviceWrite,
	maxPTs
};

char *curse_error_messages[]=
{
	"Error: Ncurses screen could not be initialised - initscr() failed!\n",
	"Error: Window size is too small. Unsaved changes (if any) have been lost!\n",
	"Error: Device open failed!\n",
	"Error: Device seek failed!\n",
	"Error: Device read failed!\n",
	"Error: Write failed on the device!\n",
	"Error: Maximum level of Partition Table depth reached!\n"
};

static char *help_text[] =
{
	" " PACKAGE_NAME " v" PACKAGE_VERSION " On-line Help:",
	" ",
	" Keys                       Corresponding Action",
	" ",
	" <Esc> or <ctrl>-x or q     Exit from program",
	" <Left> or h                Move (scroll) Left",
	" <Down> or j                Move Down",
	" <Up> or k                  Move Up",
	" <Right> or l               Move (scroll) Right",
	" <F4> or <Return> or a      Edit the highlighted value",
	" <F3> or !                  Reset whole of the partition-table",
	" <F2> or <ctrl>-s or w      Save partition-table data to device",
	" <ctrl>-f or n              Move to Next partition-table (follow EPBR)",
	" <ctrl>-b or p              Move to Previous partition-table",
	" <tab> or e                 Open a new device-file",
	" r                          Read partition-table from entered sector",
	" <F1> or ?                  Show this Help screen",
	" t                          List some well known partition types",
	0
};

static char **window_data;

extern int lipecurse(char*,int);


extern void initScreen(void);
extern void clearScreen(void);
extern void restoreScreen(void);
extern void initFrame(void);
extern void fillFrame(void);
extern void clearFrame(void);
extern void error_curse(enum curse_error);
extern void curse_window(char **);
extern void curse_window_draw(char **);
extern void curse_read(void);
extern void curse_save(void);
extern void clearScreen(void);
extern int  curse_edit(void);
extern void curse_next(void);
extern void curse_prev(void);
extern int  curse_open(void);
extern int  curse_start(void);
extern void init_lipecurse(void);


#endif /* lipecurse.h */
