/*
** Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "lipegtk.h"

void
error_gtk(enum error_gtk what)
{
	GtkWidget *dialog;
	dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			error_gtk_messages[what]);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

gboolean
callback_delete_event(GtkWidget *widget,gpointer data)
{
	return FALSE;
}

void
callback_destroy_event(GtkWidget *widget,gpointer data)
{
	if(data_needs_save) if(handle_unsaved_data()) return;
	gtk_main_quit();
}

void
find_devices()
{
	int i, j;
	char dletter;
	char dev [10];
	char cmd [1024];
	char *cmd1 = "hdparm -i %s 2>&1 | grep 'Model=' | sed -e 's/.*Model=\\([^ ]* [^ ]*\\).*/\\1/'";
	char *cmd2 = "hdparm -i %s 2>&1 | grep CHS | sed -e 's/.*RawCHS=\\(.[^, ]*\\).*/physical\\t\\1/' -e 's/.*CurCHS=\\(.[^, ]*\\).*/logical\\t\\1/'";
	FILE* pp;
	char buf[81];
	int buflen;
	
	device_list=(struct device *)malloc(sizeof(struct device));
	struct device *temp=device_list;
	struct device *temp1=NULL;
	for(i=0;i<4;i++)
	{
		for (j=0; j<2; j++)
		{
			if (j == 0) dletter = 's';
			else dletter = 'h';
			snprintf(dev,10,"%cd%c", dletter,'a'+i);
			if(device_media_type(dev)) 
			{
				snprintf (temp->dev, 80, "/dev/%cd%c", dletter, 'a' + i);
				
				temp->capacity = device_capacity_in_bytes (temp->dev);
				
				buf [80] = '\0';
				snprintf (cmd, 1023, cmd1, temp->dev);
				if ((pp = popen (cmd, "r")) && (buflen = fread (buf, sizeof(char), 80, pp)) > 0)
				{
					buf [buflen] = '\0';
					temp->model = (char *) malloc (sizeof (char) * strlen (buf) + 1);
					strcpy (temp->model, buf);
				}
				else
				{
					temp->model = (char *) malloc (sizeof (char) * strlen("Unknown") + 1);
					strcpy (temp->model, "Unknown");
				}
				pclose (pp);
				pp = NULL;
				
				buf [80] = '\0';
				snprintf (cmd, 1023, cmd2, temp->dev);
				if ((pp = popen (cmd, "r")) && (buflen = fread (buf, sizeof(char), 80, pp)) > 0)
				{
					buf [buflen] = '\0';
					temp->geometry = (char *) malloc (sizeof (char) * strlen (buf) + 1);
					strcpy (temp->geometry, buf);
				}
				else
				{
					temp->geometry = (char *) malloc (sizeof (char) * strlen("Unknown") + 1);
					strcpy (temp->geometry, "Unknown");
				}
				pclose (pp);
				pp = NULL;
				
				temp->next=(struct device *)malloc(sizeof(struct device));
				temp1=temp;
				temp=temp->next;
			}
		}
	}
	if(temp==device_list)
	{
		free(device_list);
		device_list=NULL;
	}
	else
	{
		temp1->next=NULL;
		if(temp) free(temp);
		current_device=device_list;
	}
}

void
callback_disk_change(GtkWidget *widget,gpointer data)
{
	int i;
	struct device *new_device=(struct device *)data;
	if(strcmp(new_device->dev,current_device->dev))
	{
		if(data_needs_save) if(handle_unsaved_data()) return;
		gtk_widget_set_sensitive(save_button,FALSE);
		gtk_widget_set_sensitive(reset_button,FALSE);
		gtk_widget_set_sensitive(item_save,FALSE);
		gtk_widget_set_sensitive(item_reset,FALSE);
		for(i=0;i<4;i++)
		{
			gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
		}
		data_needs_save=0;
		current_device=new_device;
		gtk_label_set_text(GTK_LABEL(disk_label),current_device->geometry);
		sector[0]=0;
		sector[1]=0;
		sector_level=0;
		first_sector_not_first_pt=0;
		fill_gwindow();
	}
}

int
handle_unsaved_data()
{
	return callback_save(NULL,0);
}

void
callback_goto_parent(GtkWidget *widget,gpointer data)
{
	int i;
	GtkWidget *dialog;
	if(!sector_level)
	{
		dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"Already at the top level partition table for current disk!");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return;
	}
	if(data_needs_save) if(handle_unsaved_data()) return;
	gtk_widget_set_sensitive(save_button,FALSE);
	gtk_widget_set_sensitive(reset_button,FALSE);
	gtk_widget_set_sensitive(item_save,FALSE);
	gtk_widget_set_sensitive(item_reset,FALSE);
	for(i=0;i<4;i++)
	{
		gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
	}
	data_needs_save=0;
	sector[sector_level]=0;
	sector_level-=1;
	fill_gwindow();
}

void
callback_goto_child(GtkWidget *widget,gpointer data)
{
	int i, found;
	int type;
	char *chtype;
	int base_sectorno;
	GtkWidget *dialog;
	if(sector_level==MAX_PTS-1)
	{
		dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"You have reached the limit for maximum allowed partition tables for this program!");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return;
	}
	if(data_needs_save) if(handle_unsaved_data()) return;
	gtk_widget_set_sensitive(save_button,FALSE);
	gtk_widget_set_sensitive(reset_button,FALSE);
	gtk_widget_set_sensitive(item_save,FALSE);
	gtk_widget_set_sensitive(item_reset,FALSE);
	for(i=0;i<4;i++)
	{
		gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
	}
	data_needs_save=0;
	found=0;
	if(first_sector_not_first_pt) base_sectorno=0;
	else base_sectorno=1;
	for(i=0;i<4;i++)
	{
		chtype = gtk_entry_get_text(GTK_ENTRY(part_widgets[i].type));
		type = hex (chtype[0]) * 10 + hex (chtype [1]);
		if(IS_EXTENDED(type))
		{
			sector[sector_level+1]=sector[base_sectorno]+
				ATOLL(gtk_entry_get_text(GTK_ENTRY(part_widgets[i].sectors_before)));
			sector_level+=1;
			fill_gwindow();
			found=1;
			break;
		}
	}
	if(!found)
	{
		dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"No extended partition exists in this partition table!");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
}

void
callback_goto_first(GtkWidget *widget,gpointer data)
{
	int i;
	GtkWidget *dialog;
	if(!sector_level)
	{
		dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"Already at the top level partition table for current disk!");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return;
	}
	if(data_needs_save) if(handle_unsaved_data()) return;
	gtk_widget_set_sensitive(save_button,FALSE);
	gtk_widget_set_sensitive(reset_button,FALSE);
	gtk_widget_set_sensitive(item_save,FALSE);
	gtk_widget_set_sensitive(item_reset,FALSE);
	for(i=0;i<4;i++)
	{
		gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
	}
	data_needs_save=0;
	sector_level=0;
	fill_gwindow();
}

void
callback_goto_last(GtkWidget *widget,gpointer data)
{
	int i,j,type;
	int more=1;
	int base_sectorno;
	char *chtype;
	GtkWidget *dialog;

	if(data_needs_save) if(handle_unsaved_data()) return;
	gtk_widget_set_sensitive(save_button,FALSE);
	gtk_widget_set_sensitive(reset_button,FALSE);
	gtk_widget_set_sensitive(item_save,FALSE);
	gtk_widget_set_sensitive(item_reset,FALSE);
	for(i=0;i<4;i++)
	{
		gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
		gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
	}
	data_needs_save=0;
	if(first_sector_not_first_pt) base_sectorno=0;
	else base_sectorno=1;
	for(j=sector_level;j<MAX_PTS;j++)
	{
		more=0;
		for(i=0;i<4;i++)
		{
			chtype = gtk_entry_get_text(GTK_ENTRY(part_widgets[i].type));
			type = hex (chtype[0]) * 10 + hex (chtype [1]);
			if(IS_EXTENDED(type))
			{
				more=1;
				sector[sector_level+1]=sector[base_sectorno]+
					ATOLL(gtk_entry_get_text(GTK_ENTRY(part_widgets[i].sectors_before)));
				sector_level+=1;
				fill_gwindow();
			}
		}
		if(!more) return;
	}
	dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"You have reached the limit for maximum allowed partition tables for this program!");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return;
}

void
callback_reset(GtkWidget *widget,gpointer data)
{
	GtkWidget *dialog;
	int i;
	dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,
			"Reset Data?");
	switch(gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_YES:
			gtk_widget_set_sensitive(save_button,FALSE);
			gtk_widget_set_sensitive(reset_button,FALSE);
			gtk_widget_set_sensitive(item_save,FALSE);
			gtk_widget_set_sensitive(item_reset,FALSE);
			data_needs_save=0;
			for(i=0;i<4;i++)
			{
				gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
			}
			fill_gwindow();
			break;
	}
	gtk_widget_destroy(dialog);
}

int
callback_save(GtkWidget *widget,gpointer data)
{
	GtkWidget *dialog;
	int ret;
	int res,i;
	char str[80];
	snprintf(str,80,"Save Partition Table at sector " PRINTF_LLD " in disk %s %s (" PRINTF_LLD ")?",
			sector[sector_level],
			current_device->model,
			current_device->dev,
			current_device->capacity/((ull_off_t)SECTOR_SIZE));
	dialog=gtk_message_dialog_new(GTK_WINDOW(gwindow),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,str);
	gtk_dialog_add_button(GTK_DIALOG(dialog),
			GTK_STOCK_CANCEL,
			GTK_RESPONSE_CANCEL);
	ret=0;
	switch(gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_YES:
			disable_partition_table_edit_signals=1;
			for(i=0;i<4;i++)
			{
				gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
				gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
				set_partition(partitions+i,i,&partition_table);
			}
			if((res=write_partition_table(current_device->dev,sector[sector_level],&partition_table))!=1)
			{
				if(res==-1) error_gtk(deviceOpen);
				if(res==-2) error_gtk(deviceSeek);
				error_gtk(deviceWrite);
			}
			disable_partition_table_edit_signals=0;
			gtk_widget_set_sensitive(save_button,FALSE);
			gtk_widget_set_sensitive(reset_button,FALSE);
			gtk_widget_set_sensitive(item_save,FALSE);
			gtk_widget_set_sensitive(item_reset,FALSE);
			data_needs_save=0;
			break;
		case GTK_RESPONSE_CANCEL:
			ret=1;
			break;
	}
	gtk_widget_destroy(dialog);
	return ret;
}

void
callback_type_selected_for_partition(GtkWidget *widget,gpointer data)
{
	char str[3];
	char *p=(char*)data;
	str[0]=p[2];
	str[1]=p[3];
	str[2]='\0';
	disable_partition_table_edit_signals=1;
	gtk_entry_set_text(GTK_ENTRY(part_widgets[current_partition].type),str);
	disable_partition_table_edit_signals=0;
}

void
callback_show_type_list(GtkWidget *widget,gpointer data)
{
	int i;
	//char *current_type;
	GtkWidget *dialog;
	GtkWidget *scrolled;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *button;
	char str[80];

	dialog=gtk_dialog_new_with_buttons("Select Type",
			GTK_WINDOW(gwindow),
			GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
			NULL);

	gtk_window_resize(GTK_WINDOW(dialog),480,300);

	snprintf(str,80,"Select type for partition %hd on disk %s %s (" PRINTF_LLD "):",
			current_partition,current_device->model,
			current_device->dev,
			current_device->capacity/((ull_off_t)SECTOR_SIZE));
	label=gtk_label_new(str);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,0);
	gtk_widget_show(label);

	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),scrolled);

	vbox=gtk_vbox_new(FALSE,10);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),vbox);

	button=gtk_radio_button_new_with_label(NULL,i386_type_names[1]);
	g_signal_connect(G_OBJECT(button),
			"toggled",
			G_CALLBACK(callback_type_selected_for_partition),
			i386_type_names[1]);
	callback_type_selected_for_partition(button,i386_type_names[1]);
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,0);
	gtk_widget_show(button);

	for(i=2;i386_type_names[i];i++)
	{
		button=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button),
				i386_type_names[i]);
		gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,0);
		g_signal_connect(G_OBJECT(button),
				"toggled",
				G_CALLBACK(callback_type_selected_for_partition),
				i386_type_names[i]);
		gtk_widget_show(button);
	}

	gtk_widget_show(vbox);
	gtk_widget_show(scrolled);

	switch(gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_OK:
			callback_type_edited(part_widgets[current_partition].type,partitions+current_partition);
			break;
		default:
			snprintf(str,80,"%02x",
					partitions[current_partition].type);
			disable_partition_table_edit_signals=1;
			gtk_entry_set_text(GTK_ENTRY(part_widgets[current_partition].type),str);
			disable_partition_table_edit_signals=0;
	}
	gtk_widget_destroy(dialog);
}

void
fill_parttype()
{
	int j;
	char str[80];
	char str2[5];
	char str3[5];
	char size[20];
	char name[20];
	name[0]='\0';
	if(sector_level==0||current_partition==0)
		snprintf(name,20,"<b>%s%d::</b>",
				current_device->dev,
				sector_level>0?sector_level+4:current_partition+1);
	if(partitions[current_partition].tt_sector*((ull_off_t)SECTOR_SIZE)<ONE_KB)
		snprintf(size,20,PRINTF_LLD " bytes",
				partitions[current_partition].tt_sector*((ull_off_t)SECTOR_SIZE));
	else if(partitions[current_partition].tt_sector*((ull_off_t)SECTOR_SIZE)<ONE_MB)
		snprintf(size,20,"%.3f KB",
				partitions[current_partition].tt_sector*((double)SECTOR_SIZE)/((double)ONE_KB));
	else if(partitions[current_partition].tt_sector*((ull_off_t)SECTOR_SIZE)<ONE_GB)
		snprintf(size,20,"%.3f MB",
				partitions[current_partition].tt_sector*((double)SECTOR_SIZE)/((double)ONE_MB));
	else
		snprintf(size,20,"%.3f GB",
				partitions[current_partition].tt_sector*((double)SECTOR_SIZE)/((double)ONE_GB));
	snprintf(str,80,"%s %#02x: Unknown type, (%s)",name,partitions[current_partition].type,size);
	gtk_label_set_markup(GTK_LABEL(partition_label),str);
	snprintf(str3,80,"0x%02x",partitions[current_partition].type);
	for(j=1;i386_type_names[j];j++)
	{
		str2[0]=i386_type_names[j][0];
		str2[1]=i386_type_names[j][1];
		str2[2]=i386_type_names[j][2];
		str2[3]=i386_type_names[j][3];
		str2[4]='\0';
		if(strcmp(str2,str3)==0)
		{
			snprintf(str,80,"%s %s, (%s)",name,i386_type_names[j],size);
			gtk_label_set_markup(GTK_LABEL(partition_label),str);
			break;
		}
	}
}

void
fill_gwindow_noread()
{
	int i;
	char str[80];
	gtk_widget_set_sensitive(child_button,FALSE);
	gtk_widget_set_sensitive(item_forward,FALSE);
	gtk_widget_set_sensitive(item_last,FALSE);
	if(sector_level==0)
	{
		gtk_widget_set_sensitive(parent_button,FALSE);
		gtk_widget_set_sensitive(item_back,FALSE);
		gtk_widget_set_sensitive(item_first,FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(parent_button,TRUE);
		gtk_widget_set_sensitive(item_back,TRUE);
		gtk_widget_set_sensitive(item_first,TRUE);
	}
	for(i=0;i<4;i++)
	{
		partitions[i]=get_partition(&partition_table,i);
		if(IS_EXTENDED(partitions[i].type))
		{
			gtk_widget_set_sensitive(child_button,TRUE);
			gtk_widget_set_sensitive(item_forward,TRUE);
			gtk_widget_set_sensitive(item_last,TRUE);
		}
		snprintf(str,80,"%02X",partitions[i].type);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].type),str);
		snprintf(str,80,"%02X",partitions[i].bootable);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].boot),str);
		snprintf(str,80,"%u",partitions[i].st_c);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].cs),str);
		snprintf(str,80,"%u",partitions[i].st_h);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].hs),str);
		snprintf(str,80,"%u",partitions[i].st_s);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].ss),str);
		snprintf(str,80,"%u",partitions[i].en_c);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].ce),str);
		snprintf(str,80,"%u",partitions[i].en_h);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].he),str);
		snprintf(str,80,"%u",partitions[i].en_s);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].se),str);
		snprintf(str,80,"%u",partitions[i].st_sector);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].sectors_before),str);
		snprintf(str,80,"%u",partitions[i].tt_sector);
		gtk_entry_set_text(GTK_ENTRY(part_widgets[i].sectors),str);

		fill_parttype();
	}
	snprintf(str,80,"Partition Table at sector " PRINTF_LLD ":",sector[sector_level]);
	gtk_frame_set_label(GTK_FRAME(disk_frame),str);
}

void
fill_gwindow()
{
	int res;
	disable_partition_table_edit_signals=1;
	if((res=read_partition_table(current_device->dev,sector[sector_level],&partition_table))!=1)
	{
		if(res==-1) error_gtk(deviceOpen);
		if(res==-2) error_gtk(deviceSeek);
		error_gtk(deviceRead);
	}
	fill_gwindow_noread();
	disable_partition_table_edit_signals=0;
}

int
edited_common(char *data,int ishex)
{
	int i,j,incr;
	int newdata=0;
	int (*func) (int);
	if(ishex)
	{
		incr=16;
		func=isxdigit;
	}
	else
	{
		incr=10;
		func=isdigit;
	}
	for(i=strlen(data)-1,j=1;i>=0;i--,j*=incr)
	{
		if(!(*func)(data[i]))
		{
			gdk_beep();
			return 0;
		}
		newdata+=j*hex(data[i]);
	}
	return newdata;
}

void
callback_type_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->type=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),1);
		if(IS_EXTENDED(p->type))
		{
			gtk_widget_set_sensitive(child_button,TRUE);
			gtk_widget_set_sensitive(item_forward,TRUE);
			gtk_widget_set_sensitive(item_last,TRUE);
		}
		fill_parttype();
	}
}

void
callback_boot_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->bootable=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),1);
	}
}

void
callback_cs_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->st_c=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_hs_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->st_h=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_ss_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->st_s=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_ce_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->en_c=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_he_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->en_h=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_se_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->en_s=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_sectors_before_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->st_sector=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_sectors_edited(GtkWidget *widget,gpointer data)
{
	partition *p=(partition*)data;
	if(!disable_partition_table_edit_signals)
	{
		gtk_widget_set_sensitive(save_button,TRUE);
		gtk_widget_set_sensitive(reset_button,TRUE);
		gtk_widget_set_sensitive(item_save,TRUE);
		gtk_widget_set_sensitive(item_reset,TRUE);
		gtk_widget_modify_text(widget,GTK_STATE_NORMAL,&colorRed);
		data_needs_save=1;
		p->tt_sector=edited_common((char*)gtk_entry_get_text(GTK_ENTRY(widget)),0);
	}
}

void
callback_grab_focus(GtkWidget *widget,gpointer data)
{
	if(!disable_partition_table_edit_signals)
	{
		if(current_partition!=(long)data)
		{
			current_partition=(long)data;
			fill_parttype();
		}
	}
}

void
callback_statusbar_in(GtkWidget *widget,gpointer data)
{
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),statusbar_context_id,(char*)data);
}

void
callback_statusbar_out(GtkWidget *widget,gpointer data)
{
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar),statusbar_context_id);
}

void
callback_about(GtkWidget *widget,gpointer data)
{
	char *str[]={PACKAGE_BUGREPORT, NULL};
	gtk_show_about_dialog(GTK_WINDOW(gwindow),/*"authors",*/
			"name", PACKAGE_NAME,
			"version", PACKAGE_VERSION,
			"comments", PACKAGE_NAME " is Partition-Table Editor. It is GNU/Linux clone of ptedit program.",
			"copyright", "Copyright (c) 2004-2008 " PACKAGE_BUGREPORT ".",
			"license",
#include "gpl.h"
			"website-label","LiPE Home",
			"website",PACKAGE_URL,
			"authors",str,
			"documenters",str,
			NULL);
}

void
callback_jumpto_sector(GtkWidget *widget,gpointer data)
{
	int i;
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *chkbutton;
	char str[80];
	
	dialog=gtk_dialog_new_with_buttons("Select Sector",
			GTK_WINDOW(gwindow),
			GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK,GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
			NULL);

	label=gtk_label_new("Type in a sector number:");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,0);
	gtk_widget_show(label);

	entry=gtk_entry_new_with_max_length(10);
	gtk_entry_set_width_chars(GTK_ENTRY(entry),10);
	snprintf(str,80,PRINTF_LLD,sector[sector_level]);
	gtk_entry_set_text(GTK_ENTRY(entry),str);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),entry,FALSE,FALSE,0);
	gtk_widget_show(entry);
	
	chkbutton=gtk_check_button_new_with_label("Don't treat the partition table at this sector as first partition table on the disk");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkbutton),TRUE);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),chkbutton,FALSE,FALSE,0);
	gtk_widget_show(chkbutton);
	
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		if(data_needs_save) if(handle_unsaved_data()) return;
		gtk_widget_set_sensitive(save_button,FALSE);
		gtk_widget_set_sensitive(reset_button,FALSE);
		gtk_widget_set_sensitive(item_save,FALSE);
		gtk_widget_set_sensitive(item_reset,FALSE);
		for(i=0;i<4;i++)
		{
			gtk_widget_modify_text(part_widgets[i].type,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].boot,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].cs,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].hs,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].ss,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].ce,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].he,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].se,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].sectors_before,GTK_STATE_NORMAL,NULL);
			gtk_widget_modify_text(part_widgets[i].sectors,GTK_STATE_NORMAL,NULL);
		}
		data_needs_save=0;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkbutton))==TRUE)
			first_sector_not_first_pt=1;
		else
			first_sector_not_first_pt=0;
		sector_level=0;
		sector[1]=0;
		sector[0]=atol(gtk_entry_get_text(GTK_ENTRY(entry)));
		fill_gwindow();
	}
	gtk_widget_destroy(dialog);
}

int
lipegtk(int argc,char **argv)
{
	long i;
	int tempargc=0;
	char str[200];
	char size[20];
	GtkWidget *windowbox;
	GtkWidget *mainbox;
	GtkWidget *topbox;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *entry;
	GtkWidget *hbox,*vbox;
	GtkWidget *button;
	GtkWidget *opt,*menu,*item;
	GtkWidget *menu_bar;
	GtkWidget *file_menu,*edit_menu,*disk_menu,*help_menu,*go_menu;
	GtkTooltips *tips;

	struct device *temp;
	find_devices();
	if(!device_list)
	{
		fprintf(stderr,"No usable disks found! Probably you should run as root!\n");
		exit(1);
	}
	temp=device_list;

	sector[0]=0;
	current_partition=0;
	colorRed.red=65535;
	colorRed.blue=0;
	colorRed.green=0;

	gtk_set_locale();
	gtk_init(&tempargc,NULL);
	gwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(gwindow),
			PACKAGE_NAME " v " PACKAGE_VERSION);
	gtk_window_set_default_icon(gdk_pixbuf_new_from_file(SYSTEM_DATA_DIR"/pixmaps/lipe.xpm",NULL));
	g_signal_connect(G_OBJECT(gwindow),
			"delete_event",
			G_CALLBACK(callback_delete_event),
			NULL);
	g_signal_connect(G_OBJECT(gwindow),
			"delete_event",
			G_CALLBACK(callback_destroy_event),
			NULL);

	tips=gtk_tooltips_new();

	windowbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(gwindow),windowbox);

	menu_bar=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(windowbox),menu_bar,FALSE,FALSE,0);

	file_menu=gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),file_menu);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu),menu);

	item=gtk_image_menu_item_new_with_mnemonic("_Save Partition Table");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect_swapped(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_save),
			NULL);
	gtk_widget_set_sensitive(item,FALSE);
	gtk_widget_show(item);
	item_save=item;

	item=gtk_image_menu_item_new_with_mnemonic("_Reset Partition Table");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_REVERT_TO_SAVED,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect_swapped(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_reset),
			NULL);
	gtk_widget_set_sensitive(item,FALSE);
	gtk_widget_show(item);
	item_reset=item;

	item=gtk_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	gtk_widget_show(item);

	item=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect_swapped(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_destroy_event),
			NULL);
	gtk_widget_show(item);

	gtk_widget_show(file_menu);

	edit_menu=gtk_menu_item_new_with_mnemonic("_Edit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),edit_menu);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu),menu);

	item=gtk_image_menu_item_new_with_label("Partition Type");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_PROPERTIES,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_show_type_list),
			NULL);
	gtk_widget_show(item);

	item=gtk_image_menu_item_new_with_label("Jump to Partition Table at sector...");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_jumpto_sector),
			NULL);
	gtk_widget_show(item);

	gtk_widget_show(edit_menu);

	go_menu=gtk_menu_item_new_with_mnemonic("_Goto");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),go_menu);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(go_menu),menu);

	item=gtk_image_menu_item_new_with_label("Parent Partition");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_goto_parent),
			NULL);
	gtk_widget_show(item);
	item_back=item;

	item=gtk_image_menu_item_new_with_label("Child Partition");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_goto_child),
			NULL);
	gtk_widget_show(item);
	item_forward=item;

	item=gtk_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	gtk_widget_show(item);

	item=gtk_image_menu_item_new_with_label("First Partition");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_GOTO_FIRST,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_goto_first),
			NULL);
	gtk_widget_show(item);
	item_first=item;

	item=gtk_image_menu_item_new_with_label("Last Partition");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
			gtk_image_new_from_stock(GTK_STOCK_GOTO_LAST,GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_goto_last),
			NULL);
	gtk_widget_show(item);
	item_last=item;

	gtk_widget_show(go_menu);

	disk_menu=gtk_menu_item_new_with_mnemonic("_Disk");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),disk_menu);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(disk_menu),menu);

	temp=device_list;
	while(temp)
	{
		if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_KB)
			snprintf(size,200,PRINTF_LLD " bytes",temp->capacity);
		else if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_MB)
			snprintf(size,200,PRINTF_LLD " KB",temp->capacity/((ull_off_t)ONE_KB));
		else if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_GB)
			snprintf(size,200,PRINTF_LLD " MB",temp->capacity/((ull_off_t)ONE_MB));
		else
			snprintf(size,200,PRINTF_LLD " GB",temp->capacity/((ull_off_t)ONE_GB));

		snprintf(str,200,"%s %s (%s)",temp->model,temp->dev,size);
		item=gtk_image_menu_item_new_with_label(str);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
				gtk_image_new_from_stock(GTK_STOCK_HARDDISK,GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
		g_signal_connect(G_OBJECT(item),
				"activate",
				G_CALLBACK(callback_disk_change),
				(gpointer)temp);
		gtk_widget_show(item);
		temp=temp->next;
	}

	gtk_widget_show(disk_menu);

	help_menu=gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_item_right_justify(GTK_MENU_ITEM(help_menu));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),help_menu);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu),menu);

	item=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect_swapped(G_OBJECT(item),
			"activate",
			G_CALLBACK(callback_about),
			NULL);
	gtk_widget_show(item);

	gtk_widget_show(help_menu);

	gtk_widget_show(menu_bar);

	mainbox=gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(mainbox),10);
	gtk_container_add(GTK_CONTAINER(windowbox),mainbox);

	topbox=gtk_hbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(mainbox),topbox);
	gtk_widget_show(topbox);

	label=gtk_label_new("Hard Disk: ");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_label_set_pattern(GTK_LABEL(label),"____ ____  ");
	gtk_box_pack_start(GTK_BOX(topbox),label,FALSE,FALSE,0);
	gtk_widget_show(label);

	opt=gtk_option_menu_new();
	menu=gtk_menu_new();
	i=0;
	temp=device_list;
	while(temp)
	{
		i++;
		if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_KB)
			snprintf(size,200,PRINTF_LLD " bytes",temp->capacity);
		else if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_MB)
			snprintf(size,200,PRINTF_LLD " KB",temp->capacity/((ull_off_t)ONE_KB));
		else if(temp->capacity*((ull_off_t)SECTOR_SIZE)<ONE_GB)
			snprintf(size,200,PRINTF_LLD " MB",temp->capacity/((ull_off_t)ONE_MB));
		else
			snprintf(size,200,PRINTF_LLD " GB",temp->capacity/((ull_off_t)ONE_GB));

		snprintf(str,200,"%s %s (%s)",temp->model,temp->dev,size);
		item=gtk_menu_item_new_with_label(str);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
		g_signal_connect(G_OBJECT(item),
				"activate",
				G_CALLBACK(callback_disk_change),
				(gpointer)temp);
		gtk_widget_show(item);
		temp=temp->next;
	}
	gtk_option_menu_set_menu(GTK_OPTION_MENU(opt),menu);
	gtk_box_pack_start(GTK_BOX(topbox),opt,FALSE,FALSE,0);
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),opt,
			"Select a Disk",
			"Requests for a new disk to be selected, so that "
			"its Partition Table can be edited.");
	gtk_widget_show(opt);
	if(i==1) gtk_widget_set_sensitive(opt,FALSE);

	snprintf(str,200,"%s",current_device->geometry);
	disk_label=gtk_label_new(str);
	gtk_box_pack_start(GTK_BOX(topbox),disk_label,FALSE,FALSE,0);
	gtk_widget_show(disk_label);

	gtk_widget_show(topbox);

	snprintf(str,200,"Partition Table at sector " PRINTF_LLD ":",sector[sector_level]);
	disk_frame=gtk_frame_new(str);
	gtk_container_add(GTK_CONTAINER(mainbox),disk_frame);

	vbox=gtk_vbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(disk_frame),vbox);

	table=gtk_table_new(6,11,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table),5);
	gtk_table_set_col_spacings(GTK_TABLE(table),5);
	gtk_container_add(GTK_CONTAINER(vbox),table);

	label=gtk_label_new("Starting");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,3,6,0,1);
	gtk_widget_show(label);

	label=gtk_label_new("Ending");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,6,9,0,1);
	gtk_widget_show(label);

	label=gtk_label_new("Sectors");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,9,11,0,1);
	gtk_widget_show(label);

	label=gtk_label_new("Type");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Boot");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Cyl");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,3,4,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Head");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Sector");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,5,6,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Cyl");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,6,7,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Head");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,7,8,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Sector");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,8,9,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Before");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,9,10,1,2);
	gtk_widget_show(label);

	label=gtk_label_new("Sectors");
	gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table),label,10,11,1,2);
	gtk_widget_show(label);

	for(i=1;i<=4;i++)
	{
		snprintf(str,200,"%ld",i);
		label=gtk_label_new(str);
		gtk_label_set_justify(GTK_LABEL(label),GTK_JUSTIFY_LEFT);
		gtk_label_set_pattern(GTK_LABEL(label),"_");
		gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,i+1,i+2);
		gtk_widget_show(label);

		entry=gtk_entry_new_with_max_length(2);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),2);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_type_edited),
				partitions+i-1);
		snprintf(str,200,"Sets type for partition %ld. \n"
				"Possible values: All two digit hexadecimal numbers",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,1,2,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].type=entry;

		entry=gtk_entry_new_with_max_length(2);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),2);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_boot_edited),
				partitions+i-1);
		snprintf(str,200,"Sets bootable flag for partition %ld. \n"
				"Possible values: 00 and 80",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,2,3,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].boot=entry;

		entry=gtk_entry_new_with_max_length(4);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),4);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_cs_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Starting Cylinder value for partition %ld. \n"
				"Possible values: 0 through 1023",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,3,4,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].cs=entry;

		entry=gtk_entry_new_with_max_length(3);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),3);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_hs_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Starting Head value for partition %ld. \n"
				"Possible values: 0 through 255",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,4,5,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].hs=entry;

		entry=gtk_entry_new_with_max_length(2);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),2);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_ss_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Starting Sector value for partition %ld. \n"
				"Possible values: 1 through 63",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,5,6,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].ss=entry;

		entry=gtk_entry_new_with_max_length(4);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),4);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_ce_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Ending Cylinder value for partition %ld. \n"
				"Possible values: 0 through 1023",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,6,7,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].ce=entry;

		entry=gtk_entry_new_with_max_length(3);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),3);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_he_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Ending Head value for partition %ld. \n"
				"Possible values: 0 through 255",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,7,8,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].he=entry;

		entry=gtk_entry_new_with_max_length(2);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),2);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_se_edited),
				partitions+i-1);
		snprintf(str,200,"Sets Ending Sector value for partition %ld. \n"
				"Possible values: 1 through 63",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,8,9,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].se=entry;

		entry=gtk_entry_new_with_max_length(10);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),10);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_sectors_before_edited),
				partitions+i-1);
		snprintf(str,200,"Sets the Number of Sectors before partition %ld. \n"
				"Possible values: Only digits 0-9 are allowed",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,9,10,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].sectors_before=entry;

		entry=gtk_entry_new_with_max_length(10);
		gtk_entry_set_width_chars(GTK_ENTRY(entry),10);
		g_signal_connect(G_OBJECT(entry),
				"grab-focus",
				G_CALLBACK(callback_grab_focus),
				(void*)(i-1));
		g_signal_connect(G_OBJECT(entry),
				"changed",
				G_CALLBACK(callback_sectors_edited),
				partitions+i-1);
		snprintf(str,200,"Sets the Number of Sectors in partition %ld. \n"
				"Possible values: Only digits 0-9 are allowed",i);
		gtk_tooltips_set_tip(GTK_TOOLTIPS(tips),entry,str,NULL);
		gtk_table_attach_defaults(GTK_TABLE(table),entry,10,11,i+1,i+2);
		gtk_widget_show(entry);
		part_widgets[i-1].sectors=entry;
	}

	gtk_widget_show(table);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(vbox),hbox);

	button=gtk_button_new_with_mnemonic("Goto _Parent");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_goto_parent),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Goto Parent Partition Table");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);
	parent_button=button;

	button=gtk_button_new_with_mnemonic("Goto _Child");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_goto_child),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Goto Child Partition Table");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);
	child_button=button;

	button=gtk_button_new_with_mnemonic("Set _Type");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_show_type_list),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Set Type for currently selected partition");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);

	button=gtk_button_new_with_mnemonic("_Jump to Sector");
	gtk_widget_set_sensitive(button,TRUE);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_jumpto_sector),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Read Partition Table from specified sector");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);
	jumpto_button=button;

	button=gtk_button_new_with_mnemonic("_Discard Changes");
	gtk_widget_set_sensitive(button,FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_reset),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Discard all chages made to the current Partition Table from last save");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);
	reset_button=button;

	button=gtk_button_new_with_mnemonic("_Save Changes");
	gtk_widget_set_sensitive(button,FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button),
			"clicked",
			G_CALLBACK(callback_save),
			NULL);
	g_signal_connect(G_OBJECT(button),
			"enter",
			G_CALLBACK(callback_statusbar_in),
			"Save all chages to the current Partition Table");
	g_signal_connect(G_OBJECT(button),
			"leave",
			G_CALLBACK(callback_statusbar_out),
			NULL);
	gtk_widget_show(button);
	save_button=button;

	gtk_widget_show(hbox);

	gtk_widget_show(vbox);

	gtk_widget_show(disk_frame);

	frame=gtk_frame_new("Partition Information");
	gtk_container_add(GTK_CONTAINER(mainbox),frame);

	partition_label=gtk_label_new("No partition selected");
	gtk_label_set_justify(GTK_LABEL(partition_label),GTK_JUSTIFY_LEFT);
	gtk_container_add(GTK_CONTAINER(frame),partition_label);
	gtk_widget_show(partition_label);

	gtk_widget_show(frame);

	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),"<span foreground=\"blue\" size=\"x-large\">"
			PACKAGE_NAME"</span> v <i>"PACKAGE_VERSION
			"</i> Copyright<sup>(c)</sup> 2004-2008 Neeraj Jakhar.");
	gtk_box_pack_start(GTK_BOX(mainbox),label,FALSE,FALSE,0);
	gtk_widget_show(label);

	gtk_widget_show(mainbox);

	gtk_widget_grab_focus(part_widgets[0].type);

	statusbar=gtk_statusbar_new();
	statusbar_context_id=gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
			PACKAGE_NAME " v" PACKAGE_VERSION);
	gtk_box_pack_start(GTK_BOX(windowbox),statusbar,FALSE,FALSE,0);
	gtk_widget_show(statusbar);

	gtk_widget_show(windowbox);

	gtk_widget_show_all(gwindow);

	fill_gwindow();

	gtk_main();
	return 0;
}

