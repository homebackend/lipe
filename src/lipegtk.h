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

#ifndef LIPEGTK_H
#define LIPEGTK_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h> 	// for popen (), pclose ()

#include "common.h"
#include "i386types.h"
#include "token.h"

/*#define ONE_GB 1073741824*/
#define	ONE_GB 1000000000
#define	ONE_MB 1000000
#define ONE_KB 1000
#define	MAX_PTS 63

#define	GTK_DISABLE_DEPRECATED 1

#if defined(__GNUC__) || defined(HAS_LONG_LONG)
#define PRINTF_LLD	"%lld"
#define ATOLL		atoll
#else
#warning unsigned long long support not found
#define PRINTF_LLD	"%ld"
#define ATOLL		atol
#endif

#ifndef SYSTEM_DATA_DIR
#define SYSTEM_DATA_DIR ""
#endif

struct device
{
	char dev[80];
	char *model;
	char *geometry;
	ull_off_t capacity;
	struct device *next;
};

struct partition_widgets
{
	GtkWidget *type;
	GtkWidget *boot;
	GtkWidget *cs;
	GtkWidget *hs;
	GtkWidget *ss;
	GtkWidget *ce;
	GtkWidget *he;
	GtkWidget *se;
	GtkWidget *sectors_before;
	GtkWidget *sectors;
}part_widgets[4];

enum error_gtk
{
	deviceOpen,
	deviceSeek,
	deviceRead,
	deviceWrite
};

char *error_gtk_messages[]=
{
	"Error: Device open failed!\n",
	"Error: Device seek failed!\n",
	"Error: Device read failed!\n",
	"Error: Write failed on the device!\n"
};

extern int errno;

GdkColor colorRed;
//static GdkColor colorRed={65535,0,0};

static struct device *device_list,*current_device;
//static ull_off_t current_sector;
static int current_partition=0;
/*static char *current_partition_type;*/

static GtkWidget *gwindow;
static GtkWidget *disk_label;
static GtkWidget *disk_frame;
static GtkWidget *partition_label;
static GtkWidget *child_button;
static GtkWidget *parent_button;
static GtkWidget *save_button;
static GtkWidget *reset_button;
static GtkWidget *jumpto_button;
static GtkWidget *statusbar;
static guint statusbar_context_id;
static GtkWidget *item_save,
		*item_reset,
		*item_back,
		*item_forward,
		*item_first,
		*item_last;

static pt partition_table;
static partition partitions[4];
static ull_off_t sector[MAX_PTS];
static int sector_level=0;
static int data_needs_save=0;
static int disable_partition_table_edit_signals=1;
static int first_sector_not_first_pt=0;

extern void error_gtk(enum error_gtk what);
extern gboolean callback_delete_event(GtkWidget *widget,gpointer data);
extern void callback_destroy_event(GtkWidget *widget,gpointer data);
extern void find_devices();
extern void callback_disk_change(GtkWidget *widget,gpointer data);
extern int handle_unsaved_data();
extern void callback_goto_parent(GtkWidget *widget,gpointer data);
extern void callback_goto_child(GtkWidget *widget,gpointer data);
extern void callback_goto_first(GtkWidget *widget,gpointer data);
extern void callback_goto_last(GtkWidget *widget,gpointer data);
extern void callback_reset(GtkWidget *widget,gpointer data);
extern int callback_save(GtkWidget *widget,gpointer data);
extern void callback_type_selected_for_partition(GtkWidget *widget,gpointer data);
extern void callback_show_type_list(GtkWidget *widget,gpointer data);
extern void fill_parttype();
extern void fill_gwindow_noread();
extern void fill_gwindow();
extern int edited_common(char *data,int ishex);
extern void callback_statusbar_in(GtkWidget *widget,gpointer data);
extern void callback_statusbar_out(GtkWidget *widget,gpointer data);
extern void callback_type_edited(GtkWidget *widget,gpointer data);
extern void callback_boot_edited(GtkWidget *widget,gpointer data);
extern void callback_cs_edited(GtkWidget *widget,gpointer data);
extern void callback_hs_edited(GtkWidget *widget,gpointer data);
extern void callback_ss_edited(GtkWidget *widget,gpointer data);
extern void callback_ce_edited(GtkWidget *widget,gpointer data);
extern void callback_he_edited(GtkWidget *widget,gpointer data);
extern void callback_se_edited(GtkWidget *widget,gpointer data);
extern void callback_sectors_before_edited(GtkWidget *widget,gpointer data);
extern void callback_sectors_edited(GtkWidget *widget,gpointer data);
extern void callback_grab_focus(GtkWidget *widget,gpointer data);
extern void callback_about(GtkWidget *widget,gpointer data);
extern void callback_jumpto_sector(GtkWidget *widget,gpointer data);
extern int lipegtk(int argc,char **argv);

#endif /* lipegtk.h */

