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


#ifndef LIPE_H
#define LIPE_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "token.h"


#define DUMPMODE		0
#define PUMPMODE		1
#define TEXTMODE		2
#define GRAPHICSMODE		3
/*
#define READABLE		0x0000
#define RAW			0x0001
#define DEFAULTDPMODE		READABLE
*/

#ifdef USE_GTK2
#define DEFAULTMODE		GRAPHICSMODE
#else
#ifdef USE_CURSES
#define DEFAULTMODE		TEXTMODE
#else
#define DEFAULTMODE		DUMPMODE
#endif
#endif

#define ERROR_RETURN_OFFSET 10


enum error_type
{
	unknown_error=ERROR_RETURN_OFFSET,
	device_open,
	device_seek,
	device_read,
	device_write,
	cmd_sector_limit,
	cmd_dpump,
	more_than_one_extended,
	pump_input,
	no_device,
	cmd_overflow,
	cmd_invalid
};

char *error_messages[]=
{
	"Error: An unkown error has occured!\n",
	"Error: Device could not be opened!\n",
	"Error: Device seek failed!\n",
	"Error: Device read failed!\n",
	"Error: Device write failed!\n",
	"Error: Sector size given in command line options is invalid! Valid values are 512, 1024 and 2048.\n",
	"Error: There are only two modes available for dump and pump and they are ``raw'' and ``readable.''\n",
	"Error: More than one extended partitions exists on the device.\n Use the text or graphics mode of this program to correct that first.\n",
	"Error: There is error in standard input that prevents it from being pumped into device!\n",
	"Error: Invalid device!\n",
	"Error: Command line overflow or too many command line options!\n",
	"Error: Invalid command line option specified!\n"
};

uint sector_size=SECTOR_SIZE;

extern int process_error_code(enum error_type);
extern void dump_partition_table(char*,int,off64_t);
extern void pump_partition_table(char*,int);
extern void dump_partition(partition*);
extern void help(char*);
extern void version(void);
extern int lipecurse(char*,int);
extern int lipegtk(int,char *[]);

#endif	/* LIPE_H */
