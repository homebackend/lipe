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


#include "lipe.h"


int process_error_code(enum error_type what)
{
	printf(error_messages[what-ERROR_RETURN_OFFSET]);
	exit(what);
}

void dump_partition_table(char *device, int what, off64_t starting_sector)
{
	int i,j;
	int res;
	pt ptvar;
	off64_t current_sector=starting_sector;
	off64_t temp_sector=0;
	int extended=0;
	int more_than_one_extended=0;
	partition temp;
	if(what&4)			/* check if assume-extended was specified */
		temp_sector=current_sector;	/* will not work in case of 2nd or later EPBR */
	for(j=0;extended>=0;j++)
	{
		if((res=read_partition_table(device,current_sector,&ptvar))!=1)
		{
			if(-1==res) process_error_code(device_open);
			if(-2==res) process_error_code(device_seek);
			process_error_code(device_read);
		}
		extended=-1;
		if(what&1)		/* Checks and dumps output in raw mode */
		{
			printf("# Partition Table Number %d's Raw Data for device %s at sector %lu\n",j,device,(long unsigned)current_sector);
			printf(";%lu\n",(long unsigned)current_sector);
			for(i=0;i<PT_BYTES;i++)
			{
				printf("%.2X ",ptvar.partition_data[i]);
				if((i+1)%PARTITION_BYTES==0)	printf("\n");
			}
			printf("# Sign Bytes\t\t\t");
			for(i=0;i<SIGN_BYTES;i++)
				printf("%.2X ",ptvar.signbytes[i]);
			printf("\n\n");
			for(i=0;i<MAX_PARTITIONS;i++)
			{
				temp=get_partition(&ptvar,i);
				if(IS_EXTENDED(temp.type))
				{
					if(extended==-1)
					{
						current_sector=temp_sector+temp.st_sector;
						if(temp_sector==0) temp_sector=current_sector;
						extended=i;
					}
					else process_error_code(more_than_one_extended);
				}
			}
		}
		else			/* dumps output in readable mode */
		{
			printf("# Partition Table Number %d's Data for device %s at sector %lu\n",j,device,(long unsigned)current_sector);
			printf(";%lu\n",(long unsigned)current_sector);
			printf("#%5s%5s%5s%5s%5s%5s%5s%5s%12s%12s\n","Boot","Type","ST_c","ST_h","ST_s","EN_c","EN_h","EN_s","ST_sector","TT_Sector");
			for(i=0;i<MAX_PARTITIONS;i++)
			{
				temp=get_partition(&ptvar,i);
				if(IS_EXTENDED(temp.type))
				{
					if(extended==-1)
					{
						current_sector=temp_sector+temp.st_sector;
						if(temp_sector==0) temp_sector=current_sector;
						extended=i;
					}
					else
						process_error_code(more_than_one_extended);
				}
				printf(" %5.2X%5.2X%5d%5d%5d%5d%5d%5d%12u%12u\n",temp.bootable,temp.type,temp.st_c,temp.st_h,temp.st_s,temp.en_c,temp.en_h,temp.en_s,temp.st_sector,temp.tt_sector);
			}
			if(IS_VALID_PT(ptvar.signbytes[0],ptvar.signbytes[1])) printf("# This is a valid Partition Table.\n");
			else printf("# This is not a valid Partition Table.\n");
			putchar('\n');
		}
		if(what&2) break;			/* bit 1 is set if partitions are not to be followed */
	}
	if(starting_sector&&!(what&2)) printf("# WARNING: You have chosen a starting-sector this produces incorrect output at times.\n# Better use --no-follow command line option.\n");
	if(what&4&&!(what&2)) printf("# WARNING: With assume extended output is sometimes incorrect.\n");
}

void pump_partition_table(char *device,int what)
{
	int i;
	int res;
	pt ptvar;
	partition partvar;
	off64_t starting_sector=0;
	int starting_sector_found=0;
	token tok;
	FILE *file=stdin;
	int lines=1;
	int pentries=0;
	int ptentries=0;
	for(i=0;i<PT_BYTES;i++) ptvar.partition_data[i]=0;
	if(((what&48)==RWMODERW)||((what&48)==RWMODEWO))
	{
		tok=get_token(file);
		while(1)
		{
			switch(tok.type)
			{
				case hash:
					while((tok=get_token(file)).type!=newline)
						if(tok.type==string && tok.tokenval)
							free((void*)tok.tokenval);
					/* XXX: Don't separate these two. */
					/* fall through used to the advantage: don't separate the code to prevent breakage */
				case newline:
					if(what&1)	/* checking raw or readable */
					{
						if(pentries==PARTITION_BYTES)
							ptentries++;
					}
					else
					{
						if(pentries>9)
						{
							set_partition(&partvar,ptentries,&ptvar);
							ptentries++;
						}
					}
					pentries=0;
					lines++;
					break;
				case semicolon:
					tok=get_token(file);
					if(tok.type!=string)
					{
						printf("Error: Error at line #%d in input. Expected a decimal number.\n",lines);
						process_error_code(pump_input);
					}
					for(i=0;i<strlen((char*)tok.tokenval);i++)
						if(!isdigit(((char*)tok.tokenval)[i]))
						{
							printf("Error: Error at line #%d in input. Non-numerical value encountered.\n",lines);
							process_error_code(pump_input);
						}
					starting_sector_found=1;
					starting_sector=atoll((char*)tok.tokenval);
					tok=get_token(file);
					if(tok.type!=newline)
					{
						printf("Error: Error at line #%d in input.\n",lines);
						process_error_code(pump_input);
					}
					pentries=0;
					lines++;
					break;
				case spaceandtab:
					break;
				case string:
					if(what&1)	/* checking raw or readable */
					{
						if(strlen((char*)tok.tokenval)!=2
							||!isxdigit(((char*)tok.tokenval)[0])
							||!isxdigit(((char*)tok.tokenval)[1])
							||pentries>PARTITION_BYTES
							||ptentries>MAX_PARTITIONS)
						{
							printf("Error: Error at line #%d in input. Non-hexadecimal character encountered.\n",lines);
							process_error_code(pump_input);
						}
						ptvar.partition_data[ptentries*PARTITION_BYTES+pentries]=(hex(tolower(((char*)tok.tokenval)[0]))<<4)|(hex(tolower(((char*)tok.tokenval)[1])));
					}
					else
					{
						if(pentries==0 || pentries==1)
						{
							if(strlen((char*)tok.tokenval)!=2
								||!isxdigit(((char*)tok.tokenval)[0])
								||!isxdigit(((char*)tok.tokenval)[1]))
							{
								printf("Error: Error at line #%d in input. Non-hexadecimal character encountered.\n",lines);
								process_error_code(pump_input);
							}
							if(pentries==0) partvar.bootable=(hex(tolower(((char*)tok.tokenval)[0]))<<4)|(hex(tolower(((char*)tok.tokenval)[1])));
							if(pentries==1) partvar.type=(hex(tolower(((char*)tok.tokenval)[0]))<<4)|(hex(tolower(((char*)tok.tokenval)[1])));
						}
						else
						{
							for(i=0;i<strlen((char*)tok.tokenval);i++)
								if(!isdigit(((char*)tok.tokenval)[i]))
								{
									printf("Error: Error at line #%d in input. Non-numerical value encountered.\n",lines);
									process_error_code(pump_input);
								}
							switch(pentries)
							{
								case 2 : partvar.st_c=atoi((char*)tok.tokenval);break;
								case 3 : partvar.st_h=atoi((char*)tok.tokenval);break;
								case 4 : partvar.st_s=atoi((char*)tok.tokenval);break;
								case 5 : partvar.en_c=atoi((char*)tok.tokenval);break;
								case 6 : partvar.en_h=atoi((char*)tok.tokenval);break;
								case 7 : partvar.en_s=atoi((char*)tok.tokenval);break;
								case 8 : partvar.st_sector=atol((char*)tok.tokenval);break;
								case 9 : partvar.tt_sector=atol((char*)tok.tokenval);break;
								default:
									printf("Error: Error at line #%d in input. Number of tokens for this particular partition has exceeded its maximum value.\n",lines);
									process_error_code(pump_input);
							}
						}
					}
					pentries++;
					if(tok.tokenval) free((void*)tok.tokenval);
					break;
				case eof:
					if(pentries>0 || ptentries>0)
					{
						printf("Error: Unexpected end of file encountered.\n");
						process_error_code(pump_input);
					}
					return;
					break;
				case error:
					if((enum token_errors)tok.tokenval==max_token_size)
					{
						printf("Error: Maximum token length allowed is %d.\n",MAX_TOKEN_SIZE);
						process_error_code(pump_input);
					}
					printf("Error: Unknown error in input.\n");
					process_error_code(pump_input);
					break;
				default:
					process_error_code(unknown_error);
			}
			if((what&1) && (ptentries==MAX_PARTITIONS))	/* first checking raw */
			{
				ptentries=0;
				ptvar.signbytes[0]=0x55;
				ptvar.signbytes[1]=0xAA;
				if(!starting_sector_found)
				{
					printf("Error: Starting Sector not defined in the input.\n");
					process_error_code(pump_input);
				}
				if((res=write_partition_table(device,starting_sector,&ptvar))!=1)
		                {
					if(res==-1) process_error_code(device_open);
					if(res==-2) process_error_code(device_seek);
					process_error_code(device_write);
	                        }
				starting_sector_found=0;
				if(what&2) break;
			}
			if(!(what&1) && (ptentries==MAX_PARTITIONS))	/* first checking readable */
			{
				ptentries=0;
				ptvar.signbytes[0]=0x55;
				ptvar.signbytes[1]=0xAA;
				if(!starting_sector_found) process_error_code(pump_input);
				if((res=write_partition_table(device,starting_sector,&ptvar))!=1)
		                {
					if(res==-1) process_error_code(device_open);
					if(res==-2) process_error_code(device_seek);
					process_error_code(device_write);
	                        }
				starting_sector_found=0;
				if(what&2) break;
			}
			tok=get_token(file);
		}
	}
	else process_error_code(device_write);
}

void dump_partition(partition* partvar)
{
	printf("%5X%5X%5d%5d%5d%5d%5d%5d%12u%12u\n",partvar->bootable,partvar->type,partvar->st_c,partvar->st_h,partvar->st_s,partvar->en_c,partvar->en_h,partvar->en_s,partvar->st_sector,partvar->tt_sector);
}

void help(char *path)
{
	printf("%s v%s:\n\n",PACKAGE_NAME,PACKAGE_VERSION);
	printf("Usage:\n");
	printf("\t%s --dump raw|readable [ --sector-size bytes ] [ --starting-sector sector ] --device device-file\n",path);
	printf("\t%s --pump raw|readable [ --sector-size bytes ] [ --starting-sector sector ] --device device-file\n",path);
	printf("\t%s [ --text ] [ --sector-size bytes ] [ --device device-file ] \n",path);
	printf("\t%s --graphics [ --sector-size bytes ] [ --device device-file ] \n",path);
	printf("\t%s --help\n",path);
	printf("\t%s --version\n",path);
#ifdef USE_GTK2
	printf("\n\tBy default graphics mode (gtk2) is enabled.\n");
#else
#ifdef USE_CURSES
	printf("\n\tBy default text-mode (ncurses) is enabled.\n");
#endif /* USE_CURSES */
#endif /* USE_GTK2 */
	printf("\n\tIn addition some shorter options also supported. See man page for more details.\n\n");
	exit(0);
}

void version(void)
{
	printf("%s v%s\n\n",PACKAGE_NAME,PACKAGE_VERSION);
	printf("Copyright (C) 2004-2005 %s\n\n",PACKAGE_BUGREPORT);
	printf("This is free software, and you are welcome to redistribute it and modify it\n"\
	       "under certain conditions. There is ABSOLUTELY NO WARRANTY for this software.\n"\
	       "For legal details see the GNU General Public License.\n");
	exit(0);
}

int main(int argc,char *argv[])
{
	int ch;
	int mode=DEFAULTMODE;
	char *device=NULL;
	int what=0;			/* bit 0 is used to hold whether mode is raw or readable
					   bit 1 is used to hold if extended partitions are not to be followed
					   bit 2 is used to hold if first partition is to be assumed as an extended one
					   bit 3 & 4 are used to store the rwmode
					*/
	int options_index=0;
	off64_t starting_sector=0;
	struct option long_options[]=
	{
		{"device",1,0,0},
		{"dump",1,0,0},
		{"pump",1,0,0},
		{"text",0,0,0},
		{"graphics",0,0,0},
		{"bytes-in-sector",1,0,0},
		{"starting-sector",1,0,0},
		{"help",0,0,0},
		{"version",0,0,0},
		{"no-follow",0,0,0},
		{"assume-extended",0,0,0},
		{0,0,0,0}
	};

	while((ch=getopt_long(argc,argv,"b:d:ghtvV",long_options,&options_index))!=-1)
		switch(ch)
		{
			case 0:
				/* ****************************** */
				/* Option string checking started */

				if(strcmp(long_options[options_index].name,"device")==0)
					device=optarg;
				else if(strcmp(long_options[options_index].name,"text")==0)
					mode=TEXTMODE;
				else if(strcmp(long_options[options_index].name,"graphics")==0)
					mode=GRAPHICSMODE;
				else if(strcmp(long_options[options_index].name,"bytes-in-sector")==0)
					sector_size=atoi(optarg);
				else if(strcmp(long_options[options_index].name,"dump")==0)
				{
					mode=DUMPMODE;
					if(strcmp(optarg,"readable")==0) what&=0xfffe;
					else if(strcmp(optarg,"raw")==0) what|=1;
					else process_error_code(cmd_dpump);
				}
				else if(strcmp(long_options[options_index].name,"pump")==0)
				{
					mode=PUMPMODE;
					if(strcmp(optarg,"readable")==0) what&=0xfffe;
					else if(strcmp(optarg,"raw")==0) what|=1;
					else process_error_code(cmd_dpump);
				}
				else if(strcmp(long_options[options_index].name,"starting-sector")==0)
				{
					if(optarg==NULL) starting_sector=0;
					else starting_sector=atoll(optarg);
				}
				else if(strcmp(long_options[options_index].name,"help")==0) help(argv[0]);
				else if(strcmp(long_options[options_index].name,"version")==0) version();
				else if(strcmp(long_options[options_index].name,"no-follow")==0) what|=2;
				else if(strcmp(long_options[options_index].name,"assume-extended")==0) what|=4;
				else process_error_code(cmd_invalid);

				/* ******************************* */
				/* Option string checking finished */

				if(sector_size!=512 && sector_size!=1024 && sector_size!=2048)
					process_error_code(cmd_sector_limit);
				break;
			case 'b':
				sector_size=atoi(optarg);
				if(sector_size!=512 && sector_size!=1024 && sector_size!=2048)
					process_error_code(cmd_sector_limit);
				break;
			case 'd':
				device=optarg;
				break;
			case 'g':
				mode=GRAPHICSMODE;
				break;
			case 'h':
				help(argv[0]);
				break;
			case 't':
				mode=TEXTMODE;
				break;
			case 'v':
			case 'V':
				version();
			default :
				process_error_code(cmd_invalid);
		}
	if(optind<argc) process_error_code(cmd_overflow);
	if((!device)&&((mode!=TEXTMODE)&&(mode!=GRAPHICSMODE))) process_error_code(no_device);
	if((device)&&(((what|=check_device(device))&48)==RWMODENO)) process_error_code(device_open);
	switch(mode)
	{
		case DUMPMODE:
			dump_partition_table(device,what,starting_sector);
			break;
		case PUMPMODE:
			pump_partition_table(device,what);
			break;
		case TEXTMODE:
#ifdef USE_CURSES
			return lipecurse(device,what);
#else
			printf("Ncurses support was not compiled in.\n");
			exit(1);
#endif
			break;
		case GRAPHICSMODE:
#ifdef USE_GTK2
			return lipegtk(argc,argv);
#else
			printf("Graphics mode was not compiled in.\n");
			exit(1);
#endif
			break;
	}
	return 0;
}

