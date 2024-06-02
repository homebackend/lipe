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

/* 
 *	Coding start date: 15.2.2004 
 */


#include "lipecurse.h"

void init_lipecurse(void)
{
	int i;
	i=15;
	getmaxyx(main_win,window_maxy,window_maxy);
	if((window_maxx<WINDOW_MINX)||(window_maxy<WINDOW_MINY))
		error_curse(minWindowSize);
	endwin();
	initScreen();
	initFrame();
	display_coordx2=display_coordx1;
	while(((i=i+COL_TAG_LENGTH)<frame_limit_lrx-frame_limit_ulx+1)&&(display_coordx2<COORD_MAXX)) display_coordx2=display_coordx2+1;
	if((coordx<display_coordx1)||(coordx>display_coordx2)) coordx=display_coordx1;
	refresh();
	fillFrame();

}

void sigctrlcHandler(void)
{
	clearScreen();
	refresh();
	restoreScreen();
	printf("Unsaved data, not saved; but then again what were you expecting!\n");
	printf("Would you call this graceful exit ? ... Hey who cares !\n");
	exit(0);
}

void sigwinchHandler(void)
{
	signal(SIGWINCH,(void *)sigwinchHandler);
	window_needs_resize=window_needs_resize+1;
	if(window_needs_resize>0)
	{
		init_lipecurse();
		window_needs_resize=window_needs_resize-1;
	}
	if(window_needs_resize==0) init_lipecurse();
}

void sigwinchWindowHandler(void)
{
	signal(SIGWINCH,(void *)sigwinchWindowHandler);
	curse_window_needs_resize=curse_window_needs_resize+1;
	if(curse_window_needs_resize>0)
	{
		endwin();
		initScreen();
		curse_window_draw(window_data);
		curse_window_needs_resize=curse_window_needs_resize-1;
	}
	else
		curse_window_draw(window_data);
}

void initFrame(void)
{
	int i;
	char str[128];
	int str_length;

	clearScreen();
	if((window_maxx<WINDOW_MINX)||(window_maxy<WINDOW_MINY))
		error_curse(minWindowSize);

	for(i=1;i<window_maxy-1;i++)
	{
		mvaddch(i,0,ACS_VLINE);
		mvaddch(i,window_maxx-1,ACS_VLINE);
	}
	for(i=1;i<window_maxx-1;i++)
	{
		mvaddch(0,i,ACS_HLINE);
		mvaddch(window_maxy-1,i,ACS_HLINE);
	}
	mvaddch(0,0,ACS_ULCORNER);
	mvaddch(0,window_maxx-1,ACS_URCORNER);
	mvaddch(window_maxy-1,0,ACS_LLCORNER);
	mvaddch(window_maxy-1,window_maxx-1,ACS_LRCORNER);

	sprintf(str,"[%s v%s %s]",PACKAGE_NAME,PACKAGE_VERSION,"- Press Ctrl-C to Quit");
	str_length=strlen(str);
	if(str_length<window_maxx+4)
		mvaddstr(0,(window_maxx-str_length)/2,str);

	for(i=0+1+LARGE_PADDING+1;i<0+1+LARGE_PADDING+1+TABLE_HEAD+1+COORD_MAXY;i++)
	{
		mvaddch(i,0+LARGE_PADDING+1,ACS_VLINE);
		mvaddch(i,window_maxx-1-LARGE_PADDING-1,ACS_VLINE);
	}
	for(i=0+1+LARGE_PADDING+1;i<window_maxx-1-LARGE_PADDING-1;i++)
	{
		mvaddch(0+LARGE_PADDING+1,i,ACS_HLINE);
		mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1,i,ACS_HLINE);
		mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1+COORD_MAXY+1,i,ACS_HLINE);
	}
	mvaddch(0+LARGE_PADDING+1,0+LARGE_PADDING+1,ACS_ULCORNER);
	mvaddch(0+LARGE_PADDING+1,window_maxx-1-LARGE_PADDING-1,ACS_URCORNER);
	mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1+COORD_MAXY+1,0+LARGE_PADDING+1,ACS_LLCORNER);
	mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1+COORD_MAXY+1,window_maxx-1-LARGE_PADDING-1,ACS_LRCORNER);
	mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1,0+LARGE_PADDING+1,ACS_LTEE);
	mvaddch(0+LARGE_PADDING+1+TABLE_HEAD+1,window_maxx-1-LARGE_PADDING-1,ACS_RTEE);

	if(!toplevel) sprintf(str,"[Partition Table #%d at Sector #%lu of %s]",sectorno-1,(unsigned long)sector[sectorno-2],dev);
	else sprintf(str,"[Partition Table #%d at Sector #%lu of %s]",sectorno,(unsigned long)sector[sectorno-1],dev);
	str_length=strlen(str);
	if(str_length<window_maxx)
		mvaddstr(0+LARGE_PADDING+1,(window_maxx-1-str_length)/2,str);

	mvaddstr(0+LARGE_PADDING+1+1,0+LARGE_PADDING+1+SMALL_PADDING+1,"SrNo");
	for(i=1;i<=COORD_MAXY;i++)
	{
		sprintf(str,"%3d.",i);
		mvaddstr(0+LARGE_PADDING+1+TABLE_HEAD+1+i,0+LARGE_PADDING+1+SMALL_PADDING+1,str);
	}
	frame_limit_ulx=0+1+LARGE_PADDING+1+SMALL_PADDING+4+SMALL_PADDING+1+SMALL_PADDING+1;
	frame_limit_uly=0+1+LARGE_PADDING+1+TABLE_HEAD+1;
	frame_limit_lrx=window_maxx-1-1-LARGE_PADDING-1-SMALL_PADDING-1-SMALL_PADDING-1;
	frame_limit_lry=frame_limit_uly+COORD_MAXY-1;
}

void fillFrame(void)
{
	int i,j,k;
	char str[128],str2[128];
	clearFrame();
	if(display_coordx1>1) for(i=0;i<4;i++) mvaddch(frame_limit_uly+i,frame_limit_ulx-SMALL_PADDING-1,'<');
	if(display_coordx2<COORD_MAXX) for(i=0;i<4;i++) mvaddch(frame_limit_uly+i,frame_limit_lrx+SMALL_PADDING+1,'>');
	for(i=0;i<=display_coordx2-display_coordx1;i++)
	{
		sprintf(str,"%13s",coltags[i+display_coordx1-1]);
		attron(A_BOLD);
		mvaddstr(frame_limit_uly-1-TABLE_HEAD,frame_limit_ulx+i*15,str);
		attroff(A_BOLD);
		for(j=0;j<COORD_MAXY;j++)
		{
			if((coordx==i+display_coordx1)&&(coordy==j+1))
				for(k=frame_limit_ulx;k<window_maxx-1-LARGE_PADDING-1;k++)
					mvaddch(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,k,' ');
			switch(i+display_coordx1)
			{
				case 1:	sprintf(str,"%X",partitions[j].bootable);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						if(strcmp(str,"80")==0) sprintf(str2,"Partition #%d is active/bootable.",j+1);
						else sprintf(str2,"Partition #%d is not active/bootable.",j+1);
						if(strlen(str2)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str2);
					}
					sprintf(str,"           %02X",partitions[j].bootable);
					break;
				case 2:	sprintf(str,"%02x",partitions[j].type);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						for(k=0;i386_type_names[k];k++)
						{
							sprintf(str2,"%c%c",i386_type_names[k][2],i386_type_names[k][3]);
							if(strcmp(str,str2)==0)
							{
								sprintf(str2,"Partition Type for partition #%d is %s.",j+1,i386_type_names[k]);
								break;
							}
						}
						if(strcmp(str2,"ff")==0) sprintf(str2,"Partition Type for partition #%d is Unknown.",j+1);
						if(strlen(str2)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str2);
					}
					sprintf(str,"           %02X",partitions[j].type);
					break;
				case 3:	sprintf(str,"%13u",partitions[j].st_sector);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						sprintf(str2,"Partition #%d starts at sector #%u.",j+1,partitions[j].st_sector);
						if(strlen(str2)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str2);
					}
					break;
				case 4:	sprintf(str,"%13u",partitions[j].tt_sector);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						sprintf(str2,"Partition #%d has %u sectors in total.",j+1,partitions[j].tt_sector);
						if(strlen(str2)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str2);
					}
					break;
				case 5:	sprintf(str2,"%u/%u/%u",partitions[j].st_c,partitions[j].st_h,partitions[j].st_s);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						sprintf(str,"Starting C/H/S values for partition #%d are %s.",j+1,str2);
						if(strlen(str)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str);
					}
					sprintf(str,"%13s",str2);
					break;
				case 6:	sprintf(str2,"%u/%u/%u",partitions[j].en_c,partitions[j].en_h,partitions[j].en_s);
					if((coordx==i+display_coordx1)&&(coordy==j+1))
					{
						sprintf(str,"Ending C/H/S values for partition #%d are %s.",j+1,str2);
						if(strlen(str)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_uly+COORD_MAXY+1+SMALL_PADDING+1,frame_limit_ulx,str);
					}
					sprintf(str,"%13s",str2);
					break;
			}
			if((coordx==i+display_coordx1)&&(coordy==j+1)) attron(A_BOLD);
			mvaddstr(frame_limit_uly+j,frame_limit_ulx+i*15,str);
			if((coordx==i+display_coordx1)&&(coordy==j+1)) attroff(A_BOLD);
		}
	}
	if(IS_VALID_PT(partition_table.signbytes[0],partition_table.signbytes[1]))
		sprintf(str,">Partition table is valid (0x%X%X)<",partition_table.signbytes[0],partition_table.signbytes[1]);
	else
		sprintf(str,">Partition table is invalid (0x%X%X)<",partition_table.signbytes[0],partition_table.signbytes[1]);
	if(strlen(str)<frame_limit_lrx-frame_limit_ulx) mvaddstr(frame_limit_lry+1,frame_limit_lrx-strlen(str)-SMALL_PADDING,str);
	refresh();
}


void clearFrame(void)
{
	int i,j;
	for(i=frame_limit_ulx;i<frame_limit_lrx;i++) mvaddch(frame_limit_uly-1-TABLE_HEAD,i,' ');
	for(i=frame_limit_ulx-SMALL_PADDING-1;i<=frame_limit_lrx+SMALL_PADDING+1;i++)
		for(j=frame_limit_uly;j<=frame_limit_lry;j++)
			mvaddch(j,i,' ');
	refresh();
}

void initScreen(void)
{
	main_win=initscr();
	if(!main_win) error_curse(ncurses);
	curs_set(0);	/* hides the cursor */
	start_color();
	clear();
	noecho();
	cbreak();
	keypad(main_win,TRUE);

	getmaxyx(main_win,window_maxy,window_maxx);
	refresh();
}

void clearScreen(void)
{
	int i,j;
	for(i=0;i<window_maxx;i++)
		for(j=0;j<window_maxy;j++)
			mvaddch(j,i,' ');
}


void restoreScreen(void)
{
	if(new_dev) free(dev);
	endwin();
}

int curse_edit(void)
{
	int edit_linex=0+1+LARGE_PADDING+1+SMALL_PADDING+1;
	int edit_liney=window_maxy-1-1-2;
	int stat_liney=edit_liney+1;
	char str[128];
	int len;
	int i;
	int ret=1;
	for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
	{
		mvaddch(edit_liney,i,' ');
		mvaddch(stat_liney,i,' ');
	}
	switch(coordx)
	{
		case 1:
			sprintf(str,"Partition #%d: Bootable: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a hexadecimal value");
			for(i=0;i<3;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,2);
			noecho();
			if(str[1]!='\0')
			{
				if((isxdigit(str[0]))&&(isxdigit(str[1])))
					partitions[coordy-1].bootable=hex(str[0])*16+hex(str[1]);
				else
				{
					beep();
					ret=0;
				}
			}
			else
			{
				if(isxdigit(str[0]))
					partitions[coordy-1].bootable=hex(str[0]);
				else
				{
					beep();
					ret=0;
				}
			}
			break;
		case 2:
			sprintf(str,"Partition #%d: Type: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a hexadecimal value or l to list types");
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,2);
			noecho();
			if(str[1]!='\0')
			{
				if((isxdigit(str[0]))&&(isxdigit(str[1])))
					partitions[coordy-1].type=hex(str[0])*16+hex(str[1]);
				else
				{
					beep();
					ret=0;
				}
			}
			else
			{
				if(isxdigit(str[0]))
					partitions[coordy-1].type=hex(str[0]);
				else
					if((str[0]=='l')||(str[0]=='L'))
					{
						endwin();
						curse_window(i386_type_names);
						initScreen();
						initFrame();
						fillFrame();
						ret=curse_edit();
					}
					else
					{
						beep();
						ret=0;
					}
			}
			break;
		case 3:
			sprintf(str,"Partition #%d: Starting Sector/Sectors Before: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a Number (max 10 digits)");
			for(i=0;i<11;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,10);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret)	partitions[coordy-1].st_sector=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			break;
		case 4:
			sprintf(str,"Partition #%d: Total Sector: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a Number (max 10 digits)");
			for(i=0;i<11;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,10);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret)	partitions[coordy-1].tt_sector=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			break;
		case 5:
			sprintf(str,"Partition #%d: Starting Cylinder: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 4 digits)");
			for(i=0;i<5;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,4);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].st_c=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			if(!ret) break;
			for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
			{
				mvaddch(edit_liney,i,' ');
				mvaddch(stat_liney,i,' ');
			}
			sprintf(str,"Partition #%d: Starting Head: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 3 digits)");
			for(i=0;i<4;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,3);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].st_h=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			if(!ret) break;
			for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
			{
				mvaddch(edit_liney,i,' ');
				mvaddch(stat_liney,i,' ');
			}
			sprintf(str,"Partition #%d: Starting Sector: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 2 digits)");
			for(i=0;i<3;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,2);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].st_s=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			break;
		case 6:
			sprintf(str,"Partition #%d: Ending Cylinder: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 4 digits)");
			for(i=0;i<5;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,4);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].en_c=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			if(!ret) break;
			for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
			{
				mvaddch(edit_liney,i,' ');
				mvaddch(stat_liney,i,' ');
			}
			sprintf(str,"Partition #%d: Ending Head: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 3 digits)");
			for(i=0;i<4;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,3);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].en_h=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			if(!ret) break;
			for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
			{
				mvaddch(edit_liney,i,' ');
				mvaddch(stat_liney,i,' ');
			}
			sprintf(str,"Partition #%d: Ending Sector: ",coordy);
			len=strlen(str);
			attron(A_BOLD);
			mvaddstr(edit_liney,edit_linex,str);
			attroff(A_BOLD);
			mvaddstr(stat_liney,edit_linex,"Enter a number (max 2 digits)");
			for(i=0;i<3;i++) str[i]='\0';
			echo();
			mvgetnstr(edit_liney,edit_linex+len,str,2);
			noecho();
			for(i=0;i<strlen(str);i++)
				if(!isdigit(str[i]))
				{
					ret=0;
					break;
				}
			if(ret) partitions[coordy-1].en_s=atoi(str);
			else
			{
				beep();
				ret=0;
			}
			break;
}
	for(i=edit_linex;i<window_maxx-1-LARGE_PADDING-1-SMALL_PADDING-1;i++)
	{
		mvaddch(edit_liney,i,' ');
		mvaddch(stat_liney,i,' ');
	}
	if(ret) data_needs_save=1;
	return ret;
}

void curse_read(void)
{
	uint i;
	int res;
	if(sectorno==MAX_PTS)
		error_curse(maxPTs);
	if((res=read_partition_table(dev,sector[sectorno-1],&partition_table))!=1)
	{
		if(res==-1) error_curse(deviceOpen);
		if(res==-2) error_curse(deviceSeek);
		error_curse(deviceRead);
	}
	for(i=0;i<4;i++)
		partitions[i]=get_partition(&partition_table,i);
	toplevel=1;
	for(i=0;i<4;i++)
		if(IS_EXTENDED(partitions[i].type))
		{
			if(sectorno==0||sectorno==1) sector[sectorno]=partitions[i].st_sector;
			else sector[sectorno]=sector[1]+partitions[i].st_sector;
			sectorno=sectorno+1;
			toplevel=0;
			break;
		}
	/*sectorno=sectorno+1;*/
	data_needs_save=0;
}

void curse_save(void)
{
	pt temp;
	int res;
	int ch,i;
	int messagex=0+LARGE_PADDING+1;
	int messagey=window_maxy-1-2;
	attron(A_BOLD);
	mvaddstr(messagey,messagex,"Do you want to save changes ? (Y/N): ");
	attroff(A_BOLD);
	echo();
	ch=getch();
	noecho();
	if((ch=='y')||(ch=='Y'))
	{
		addstr(" Saving ... ");
		rwm=check_device(dev);
		if((rwm!=RWMODERW)&&(rwm!=RWMODEWO))
			error_curse(deviceWrite);
		for(i=0;i<4;i++)
			set_partition(partitions+i,i,&temp);
		set_sign_bytes(&temp);
		if(!toplevel)
		{
			if((res=write_partition_table(dev,sector[sectorno-2],&temp))!=1)
			{
				if(res==-1) error_curse(deviceOpen);
				if(res==-2) error_curse(deviceSeek);
				error_curse(deviceWrite);
			}
		}
		else if((res=write_partition_table(dev,sector[sectorno-1],&temp))!=1)
			{
				if(res==-1) error_curse(deviceOpen);
				if(res==-2) error_curse(deviceSeek);
				error_curse(deviceWrite);
			}
		data_needs_save=0;
	}
	for(i=messagex;i<window_maxx-1;i++) mvaddch(messagey,i,' ');
}

void curse_next(void)
{
	if(toplevel) beep();
	if(data_needs_save)
		curse_save();
	data_needs_save=0;
	curse_read();
	initFrame();
	fillFrame();
}

void curse_prev(void)
{
	if((sectorno==1)|(sectorno==2))
	{
		beep();
		return;
	}
	if(data_needs_save)
		curse_save();
	data_needs_save=0;
	if(toplevel) sectorno=sectorno-1;
	else sectorno=sectorno-2;
	curse_read();
	initFrame();
	fillFrame();
}

int curse_open(void)
{
	int i;
	int error=1;
	char str[128];
	int messagex=0+LARGE_PADDING+1;
	int messagey=window_maxy-1-2;
	if(data_needs_save)
		curse_save();
	while(error)
	{
		error=0;
		attron(A_BOLD);
		mvaddstr(messagey,messagex,"Enter the device-file to be opened: ");
		attroff(A_BOLD);
		echo();
		getnstr(str,128);
		noecho();
		if((strlen(str))&&(check_device(str)==RWMODENO))
		{
			for(i=messagex;i<window_maxx-1-LARGE_PADDING-1;i++)
				mvaddch(messagey,i,' ');
			error=1;
			mvaddstr(messagey,messagex,"Device open failed! Press any key to continue ");
			getch();
		}
		for(i=messagex;i<window_maxx-1-LARGE_PADDING-1;i++)
			mvaddch(messagey,i,' ');
	}
	if(strlen(str))
	{
		data_needs_save=0;
		if(!new_dev) dev=(char *)malloc(128);
		new_dev=1;
		sector[0]=0;
		sectorno=1;
		toplevel=0;
		strcpy(dev,str);
		initFrame();
		return 1;
	}
	return 0;
}

void curse_window_draw(char *str[])
{
	int i,j;
	int ulx=0,uly=0;
	char strt[128];
	char strx[]="[ Type <ESC> or 'q' to exit; <Left>, <Down>, <Up>, <Right> or h, j, k, l to move ]";

	clearScreen();
	refresh();
	getmaxyx(main_win,window_maxy,window_maxx);
	curse_window_mlen=strlen(strx);
	if(window_maxx<curse_window_mlen+4) curse_window_mlen=0;
	if(str[0]) if(strlen(str[0])>curse_window_mlen) curse_window_mlen=strlen(str[0])+4;
	for(i=1;str[i];i++)
		if(strlen(str[i])>curse_window_mlen) curse_window_mlen=strlen(str[i]);
	curse_window_mhgt=i;

	if(curse_window_mlen>window_maxx)
	{
		ulx=0+1+SMALL_PADDING+1;
		curse_window_len=window_maxx-SMALL_PADDING-ulx-1;
	}
	else
	{
		ulx=(window_maxx-curse_window_mlen+1+SMALL_PADDING)/2;
		curse_window_len=curse_window_mlen;
	}
	if(curse_window_mhgt>window_maxy-LARGE_PADDING)
	{
		uly=0+1+SMALL_PADDING+1;
		curse_window_hgt=window_maxy-1-uly-LARGE_PADDING-1-SMALL_PADDING;
	}
	else
	{
		uly=(window_maxy-curse_window_mhgt+1+SMALL_PADDING+1)/2;
		curse_window_hgt=curse_window_mhgt;
	}

	for(i=uly-SMALL_PADDING;i<uly+curse_window_hgt+SMALL_PADDING;i++)
	{
		mvaddch(i,ulx-SMALL_PADDING-1,ACS_VLINE);
		mvaddch(i,ulx+curse_window_len-1+SMALL_PADDING+1,ACS_VLINE);
	}
	for(i=ulx-SMALL_PADDING;i<ulx+curse_window_len+SMALL_PADDING;i++)
	{
		mvaddch(uly-SMALL_PADDING-1,i,ACS_HLINE);
		mvaddch(uly+curse_window_hgt-1+SMALL_PADDING+1,i,ACS_HLINE);
	}
	mvaddch(uly-SMALL_PADDING-1,ulx-SMALL_PADDING-1,ACS_ULCORNER);
	mvaddch(uly-SMALL_PADDING-1,ulx+curse_window_len-1+SMALL_PADDING+1,ACS_URCORNER);
	mvaddch(uly+curse_window_hgt-1+SMALL_PADDING+1,ulx-SMALL_PADDING-1,ACS_LLCORNER);
	mvaddch(uly+curse_window_hgt-1+SMALL_PADDING+1,ulx+curse_window_len-1+SMALL_PADDING+1,ACS_LRCORNER);
	if(str[0])
	{
		attron(A_BOLD);
		sprintf(strt,"[ %s ]",str[0]);
		mvaddstr(uly-SMALL_PADDING-1,(ulx+ulx+curse_window_len-strlen(strt))/2,strt);
		attroff(A_BOLD);
	}
	for(j=0;j<curse_window_hgt;j++)
	{
		if(str[curse_window_currenty+j])
		{
			for(i=0;i<curse_window_len;i++)
				if(curse_window_currentx+i<strlen(str[curse_window_currenty+j]))
					mvaddch(uly+j,ulx+i,str[curse_window_currenty+j][curse_window_currentx+i]);
				else break;
		}
	}
	attron(A_BOLD);
	mvaddstr(uly+curse_window_hgt-1+SMALL_PADDING+1,ulx+(curse_window_len-strlen(strx))/2,strx);
	attroff(A_BOLD);
	refresh();
}

void curse_window(char *str[])
{
	int ch='\0';

	window_data=str;
	curse_window_mlen=0;
	curse_window_mhgt=0;
	curse_window_len=0;
	curse_window_hgt=0;
	curse_window_currentx=0;
	curse_window_currenty=0;
	signal(SIGWINCH,(void *)sigwinchWindowHandler);
	initScreen();
	curse_window_draw(str);
	do
	{
		ch=getch();
		switch(ch)
		{
			case KEY_LEFT:
			case 'h':
				if(curse_window_currentx==0) beep();
				else
				{
					curse_window_currentx=curse_window_currentx-1;
					curse_window_draw(str);
				}
				break;
			case KEY_DOWN:
			case 'j':
				if(curse_window_currenty==curse_window_mhgt-curse_window_hgt) beep();
				else
				{
					curse_window_currenty=curse_window_currenty+1;
					curse_window_draw(str);
				}
				break;
			case KEY_UP:
			case 'k':
				if(curse_window_currenty==0) beep();
				else
				{
					curse_window_currenty=curse_window_currenty-1;
					curse_window_draw(str);
				}
				break;
			case KEY_RIGHT:
			case 'l':
				if(curse_window_currentx==curse_window_mlen-curse_window_len) beep();
				else
				{
					curse_window_currentx=curse_window_currentx+1;
					curse_window_draw(str);
				}
				break;
			case 27:
			case 'q':
				break;
			default:
				beep();
		}
	}
	while(ch!='q');
	endwin();
	signal(SIGWINCH,SIG_IGN);
}

int curse_start(void)
{
	int i;
	char str[11];
	int messagex=0+LARGE_PADDING+1;
	int messagey=window_maxy-1-2;
	for(i=0;i<11;i++) str[i]='\0';
	attron(A_BOLD);
	mvaddstr(messagey,messagex,"Enter the sector # (max 10 digits): ");
	attroff(A_BOLD);
	echo();
	getnstr(str,10);
	noecho();
	for(i=0;i<strlen(str);i++)
		if(!isdigit(str[i]))
		{
			beep();
			return 0;
		}
	for(i=messagex;i<window_maxy-1-LARGE_PADDING-1;i++) mvaddch(messagey,i,' ');
	sector[0]=atoi(str);
	sectorno=1;
	toplevel=0;
	return 1;
}

void error_curse(enum curse_error what)
{
	clearScreen();
	refresh();
	restoreScreen();
	printf(curse_error_messages[what-CURSE_ERR_START]);
	if(what==minWindowSize)
		printf("Minimum requirements are %dx%d and your's now is %dx%d\n",WINDOW_MINX,WINDOW_MINY,window_maxx,window_maxy);
	exit(what);
}

int lipecurse(char *device,int what)
{
	int i=15;
	int j;
	int ch;
	int exit=0;
	dev=device;
	rwm=what&48;

	if(!dev)
	{
		dev=malloc(128);
		exit=0;
		for(j=0;j<4&&!exit;j++)
		{
			sprintf(dev,"/dev/hd%c",'a'+j);
			if(device_media_type(dev)) exit=1;
		}
		if(exit) exit=0;
		else if(dev)
		{
			free(dev);
			dev=NULL;
		}
		/*return 0;*/
	}
	initScreen();

	signal(SIGINT,(void *)sigctrlcHandler);

	/* Reading Partition Table */
	sector[0]=0;
	sectorno=1;
	initFrame();
	/* determine if drive is set */
	while(!dev)
		curse_open();
	curse_read();
	endwin();
	initScreen();
	initFrame();
	signal(SIGWINCH,(void *)sigwinchHandler);

	coordx=1;
	coordy=1;
	display_coordx1=1;
	display_coordx2=1;
	while(((i=i+COL_TAG_LENGTH)<frame_limit_lrx-frame_limit_ulx+1)&&(display_coordx2<COORD_MAXX)) display_coordx2=display_coordx2+1;
	refresh();


	fillFrame();
	while(!exit)
	{
		switch(ch=getch())
		{
			case KEY_LEFT:
			case 'h':
				/* Move Left */
				if(coordx!=1)
				{
					if(coordx!=display_coordx1)
					{
						coordx=coordx-1;
						fillFrame();
					}
					else
					{
						coordx=coordx-1;
						display_coordx1=display_coordx1-1;
						display_coordx2=display_coordx2-1;
						fillFrame();
					}
				}
				else beep();
				break;
			case KEY_DOWN:
			case 'j':
				/* Move Down */
				if(coordy!=COORD_MAXY)
				{
					coordy=coordy+1;
					fillFrame();
				}
				else beep();
				break;
			case KEY_UP:
			case 'k':
				/* Move UP */
				if(coordy!=1)
				{
					coordy=coordy-1;
					fillFrame();
				}
				else beep();
				break;
			case KEY_RIGHT:
			case 'l':
				/* Move Right */
				if(coordx!=COORD_MAXX)
				{
					if(coordx==display_coordx2)
					{
						coordx=coordx+1;
						display_coordx1=display_coordx1+1;
						display_coordx2=display_coordx2+1;
						fillFrame();
					}
					else
					{
						coordx=coordx+1;
						fillFrame();
					}
				}
				else beep();
				refresh();
				break;
			case 6:	/* ctrl-n */
			case 'n':
				/* Move to Next Partition Table */
				curse_next();
				break;
			case 2:	/* ctrl-b */
			case 'p':
				/* Move to Previous Partition Table */
				curse_prev();
				break;
			case 9: /* tab */
			case 'e':
				/* Open a new device-file */
				if(curse_open())
				{
					initFrame();
					curse_read();
					fillFrame();
				}
				else beep();
				break;
			case 'r':
				/* start reading partition table from this sector instead of usual 0 */
				if(curse_start())
				{
					initFrame();
					curse_read();
					fillFrame();
				}
				break;
			case KEY_F(1):
			case '?':
				/* Help */
				signal(SIGWINCH,SIG_IGN);
				endwin();
				curse_window(help_text);
				signal(SIGWINCH,(void *)sigwinchHandler);
				initScreen();
				initFrame();
				init_lipecurse();
				fillFrame();
				break;
			case KEY_F(3):
			case '!':
				/* Reset data */
				initFrame();
				sectorno=sectorno-1;
				curse_read();
				fillFrame();
				break;
			case KEY_F(2):
			case 'w':
			case 19:/* ctrl-s */
				/* Save data */
				curse_save();
				break;
			case 27:/* Esc */
			case 'q':
			case 24:/* ctrl-x */
				/* Exit from Program */
				if(data_needs_save)
					curse_save();
				exit=1;
				break;
			case KEY_F(4):
			case 10:/* Return */
			case 'a':
				/* Edit a value */
				if(curse_edit())
				{
					initFrame();
					fillFrame();
				}
				else beep();
				break;
			case 't':
				/* Display some well known partition types */
				signal(SIGWINCH,SIG_IGN);
				endwin();
				curse_window(i386_type_names);
				signal(SIGWINCH,(void *)sigwinchHandler);
				initScreen();
				initFrame();
				init_lipecurse();
				fillFrame();
				break;
			default:
				/* Just annoys you :) */
				beep();
		}
	}
	clearScreen();
	refresh();
	restoreScreen();
	return 0;
}

