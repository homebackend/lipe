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


#ifndef LIPE_COMMON_H
#define LIPE_COMMON_H


#if HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE		/* For using lseek64() */
#endif

#ifndef __USE_LARGEFILE64
# define __USE_LARGEFILE64
#endif



/*

#ifndef LIPE_PROG_NAME
#warning Program name is undefined: Using GNU/LiPE
#define LIPE_PROG_NAME "GNU/LiPE"
#endif

#ifndef LIPE_PROG_VER
#warning Program version is undefined: Using Unkown
#define LIPE_PROG_VER "Unknown"
#endif
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/unistd.h>
#include <syscall.h>
#include <errno.h>
#include <stdlib.h>



#define SECTOR_SIZE		512
/* 
 * Stores the default size of a sector on the disk, this value can be 
 * overridden by command-line options 
 */
#define PT_START_BYTE		446
/*
 * Starting byte for partition-data in the MBR, data is read starting 
 * from this byte in the sector containing MBR.
 */
#define PT_BYTES		64
/* 
 * The number of bytes containing all the partition-table data.
 */
#define PARTITION_BYTES		16
/* 
 * The number of bytes containing the data of a single partition.
 */
#define SIGN_BYTES		2
/* 
 * The number of bytes containing the signature of partition-table.
 */
#define MAX_PARTITIONS		4
/* 
 * Maximum number of partitions that can be stored in a 
 * partition-table.
 */

#define uint			unsigned int
#define uchar			unsigned char

#if defined(__GNUC__) || defined(HAS_LONG_LONG)
typedef unsigned long long ull_off_t;
#else
#warning unsigned long long support not found
typedef unsigned long ull_off_t;
#endif

#define EXTENDED		0x05
#define EXTENDED_HIDDEN		0x15
#define WIN95_EXTENDED		0x0f
#define WIN95_EXTENDED_HIDDEN	0x1f
#define LINUX_EXTENDED		0x85

#define IS_EXTENDED(X)		(((X)==EXTENDED) \
					|| ((X)==EXTENDED_HIDDEN) \
					|| ((X)==WIN95_EXTENDED) \
					|| ((X)==WIN95_EXTENDED_HIDDEN) \
					|| ((X)==LINUX_EXTENDED))

#define IS_VALID_PT(X,Y)	((0x55==(X))&&(0xAA==(Y)))

#define RWMODENO		0	/* LSB = 00000000 */
#define RWMODERO		16	/* LSB = 00010000 */
#define RWMODEWO		32	/* LSB = 00100000 */
#define RWMODERW		48	/* LSB = 00110000 */

typedef struct partition	/* structure to store partition data */
{
	uint	bootable:8,
		/* bootable flag - possible values 0x00 and 0x80 */
		type:8,
		/* partition type */
		st_sector:32,
		/* starting sector of the partition */
		tt_sector:32,
		/* total sectors in the partition */
		st_c:10,
		/* starting cylinder value (as in CHS) for the partition */
		st_h:8,
		/* starting head value (as in CHS) for the partition */
		st_s:6,
		/* starting sector value (as in CHS) for the partition */
		en_c:10,
		/* ending cylinder value (as in CHS) for the partition */
		en_h:8,
		/* ending head value (as in CHS) for the partition */
		en_s:6;
		/* ending sector value (as in CHS) for the partition */
}partition;

typedef struct pt
/* structure to store partition-table data */
{
	uchar partition_data[PT_BYTES];
	/* partition data */
	uchar signbytes[SIGN_BYTES];
	/* sign bytes (the 0x55aa stuff) */
}pt;

extern int read_partition_table(char*,off64_t,pt*);
extern int write_partition_table(char*,off64_t,pt*);
extern partition get_partition(pt*,uint);
extern void set_partition(partition*,uint,pt*);
extern void set_sign_bytes(pt*);
extern int check_device(char*);
extern uchar hex(uchar);
extern int device_media_type(char*);
extern ull_off_t device_capacity_in_bytes(char*);
extern char *device_model(char*);
extern char *device_geometry(char*);

#endif	/* COMMON_H */
