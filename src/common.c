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

#include <stdio.h>
#include <string.h>

#include "common.h"


extern uint sector_size;


int read_partition_table(char *device, off64_t starting_sector, pt *ptvar)
{
	int fd=open(device,O_RDONLY);	/* int open(const char *pathname, int flags); */
	if(fd>=0)
	{
		if(lseek64(fd, sector_size * starting_sector + PT_START_BYTE, SEEK_SET ) > 0)
		{
			if(read(fd, ptvar->partition_data, sizeof(ptvar->partition_data)) != sizeof(ptvar->partition_data)) return -3;
			if(read(fd, ptvar->signbytes, sizeof(ptvar->signbytes)) != sizeof(ptvar->signbytes)) return -3;
		}
		else return -2;
	}
	else return -1;
	close(fd);
	return 1;
}

int write_partition_table(char *device, off64_t starting_sector, pt *ptvar)
{
	int fd=open(device,O_WRONLY);
	if(fd>=0)
	{
		if(lseek64(fd, sector_size * starting_sector + PT_START_BYTE, SEEK_SET) > 0)
		{
			if(write(fd, ptvar->partition_data, sizeof(ptvar->partition_data)) != sizeof(ptvar->partition_data))
				return -3;
			if(write(fd, ptvar->signbytes, sizeof(ptvar->signbytes)) != sizeof(ptvar->signbytes))
				return -3;
		}
		else return -2;
	}
	else return -1;
	close(fd);
	return 1;
}

partition get_partition(pt *ptvar,uint number)
{
	int i;
	partition partvar;
	uchar partition_data[PT_BYTES];
	uint offset;
	for(i=0;i<PT_BYTES;i++)
		partition_data[i]=ptvar->partition_data[i];
	offset=number*PARTITION_BYTES;
	partvar.bootable=partition_data[offset];
	partvar.type=partition_data[offset+4];
	partvar.st_sector=partition_data[offset+8]
		+(partition_data[offset+9]<<8)
		+(partition_data[offset+10]<<16)
		+(partition_data[offset+11]<<24);
	partvar.tt_sector=partition_data[offset+12]
		+(partition_data[offset+13]<<8)
		+(partition_data[offset+14]<<16)
		+(partition_data[offset+15]<<24);
	partvar.st_c=((partition_data[offset+3])
		|((partition_data[offset+2]&0xc0)<<2));
		/* 0xC0 is 11000000 */
	partvar.st_h=partition_data[offset+1];
	partvar.st_s=(partition_data[offset+2]&0x3f);
		/* 0x3F is 00111111 */
	partvar.en_c=((partition_data[offset+7])
		|((partition_data[offset+6]&0xc0)<<2));
	partvar.en_h=partition_data[offset+5];
	partvar.en_s=((partition_data[offset+6])&0x3F);
	return partvar;
}

void set_partition(partition *partvar,uint number,pt *ptvar)
{
	uint offset=number*PARTITION_BYTES;
	ptvar->partition_data[offset]=partvar->bootable;
	ptvar->partition_data[offset+1]=partvar->st_h;
	ptvar->partition_data[offset+2]=((partvar->st_c>>2)&0xc0)
		|(partvar->st_s);
	ptvar->partition_data[offset+3]=partvar->st_c;
	ptvar->partition_data[offset+4]=partvar->type;
	ptvar->partition_data[offset+5]=partvar->en_h;
	ptvar->partition_data[offset+6]=((partvar->en_c>>2)&0xc0)
		|(partvar->en_s);
	ptvar->partition_data[offset+7]=partvar->en_c;
	ptvar->partition_data[offset+8]=partvar->st_sector;
	ptvar->partition_data[offset+9]=partvar->st_sector>>8;
	ptvar->partition_data[offset+10]=partvar->st_sector>>16;
	ptvar->partition_data[offset+11]=partvar->st_sector>>24;
	ptvar->partition_data[offset+12]=partvar->tt_sector;
	ptvar->partition_data[offset+13]=partvar->tt_sector>>8;
	ptvar->partition_data[offset+14]=partvar->tt_sector>>16;
	ptvar->partition_data[offset+15]=partvar->tt_sector>>24;
}

void set_sign_bytes(pt *ptvar)
{
	ptvar->signbytes[0]=0x55;
	ptvar->signbytes[1]=0xaa;
}

int check_device(char *device)
{
	int ret=RWMODENO;
	int fd;
	if((fd=open(device,O_RDONLY))>=0)
	{
		ret=RWMODERO;
		close(fd);
	}
	if((fd=open(device,O_WRONLY))>=0)
	{
		if(ret==RWMODENO) ret=RWMODEWO;
		else ret=RWMODERW;
		close(fd);
	}
	return ret;
}

int device_media_type(char *device)
{
/*	It is assumed that device will of type /dev/hdx. Since this is not taken
 *	as input from user it seems to be a reasonable guess.
 */

	FILE *fp;
	char fpath[40];
	char fdata[6];
	
	sprintf (fpath, "/dev/%s", device);
	if(!(fp=fopen(fpath, "r"))) return 0;
	fclose (fp);
	
	sprintf(fpath, "/proc/ide/%s/media", device);
	if(!(fp=fopen(fpath,"r"))) return 1;
	if(fread (fdata, sizeof (char), 5, fp) < 4) return 1;
	fdata[5]='\0';
	fclose(fp);
	
	if(strncmp("cdrom", fdata, 5)) return 0;
	if(strncmp("tape", fdata, 4)) return 0;
	return 1;
}

ull_off_t device_capacity_in_bytes(char *device)
{
	off64_t offset;
	int fd=open(device,O_RDONLY);	/* int open(const char *pathname, int flags); */
	if(fd>=0)
	{
		offset = lseek64(fd, 0, SEEK_END);
		close(fd);
		return offset;
	}
	return 0;
}

char *device_model(char *device)
{
	/* expects device as hda and not /dev/hda */
	FILE *fp;
	int read;
	char *ret=NULL;
	char buffer[80];
	sprintf(buffer,"/proc/ide/%s/model",device);
	if(!(fp=fopen(buffer,"r"))) return NULL;
	if((read=fread(buffer,sizeof(char),80,fp)))
	{
		buffer[read-1]='\0';
		ret=(char *)malloc(sizeof(char)*read+1);
		strcpy(ret,buffer);
	}
	return ret;
}

char *device_geometry(char *device)
{
	/* expects device as hda and not /dev/hda */
	FILE *fp;
	int read;
	char *ret=NULL;
	char buffer[80];
	sprintf(buffer,"/proc/ide/%s/geometry",device);
	if(!(fp=fopen(buffer,"r"))) return NULL;
	if((read=fread(buffer,sizeof(char),80,fp)))
	{
		buffer[read-1]='\0';
		ret=(char *)malloc(sizeof(char)*read+1);
		strcpy(ret,buffer);
	}
	return ret;
}

uchar hex(uchar ch)
{
	uchar ret=0x00;
	switch(ch)
	{
		case '0' : ret=0x00;break;
		case '1' : ret=0x01;break;
		case '2' : ret=0x02;break;
		case '3' : ret=0x03;break;
		case '4' : ret=0x04;break;
		case '5' : ret=0x05;break;
		case '6' : ret=0x06;break;
		case '7' : ret=0x07;break;
		case '8' : ret=0x08;break;
		case '9' : ret=0x09;break;
		case 'a' :
		case 'A' : ret=0x0a;break;
		case 'b' :
		case 'B' : ret=0x0b;break;
		case 'c' :
		case 'C' : ret=0x0c;break;
		case 'd' :
		case 'D' : ret=0x0d;break;
		case 'e' :
		case 'E' : ret=0x0e;break;
		case 'f' :
		case 'F' : ret=0x0f;break;
	}
	return ret;
}
