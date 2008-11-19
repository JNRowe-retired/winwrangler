/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * This file is part of WinWrangler.
 * Copyright (C) Mikkel Kamstrup Erlandsen 2008 <mikkel.kamstrup@gmail.com>
 *
 *  WinWrangler is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  WinWrangler is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WinWranger.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <glib/gi18n.h>

#include "winwrangler.h"

static gchar *layout_name = NULL;
static gboolean print_layouts = FALSE;
static gboolean run_tray = FALSE;
static gboolean run_daemon = FALSE;

static GOptionEntry option_entries[] = {
	{ "layout", 'l', 0, G_OPTION_ARG_STRING, &layout_name,
	  N_("The layout function to apply") },
	{ "layouts", 0, 0, G_OPTION_ARG_NONE, &print_layouts,
	  N_("Print a list of layout functions") },
	{ "tray", 't', 0, G_OPTION_ARG_NONE, &run_tray,
	  N_("Add an icon in the system tray. This implies --daemon") },
	{ "daemon", 'd', 0, G_OPTION_ARG_NONE, &run_daemon,
	  N_("Run a background process listening for hotkey events") },
	{ NULL }
};

static void
do_print_layouts (const WwLayout *layouts)
{
	WwLayout *layout;
	
	g_return_if_fail (layouts != NULL);
	
	g_printf ("Known layouts:\n");
	for (layout = layouts; layout->name != NULL; layout++)
	{
		g_return_if_fail (layout != NULL);
		g_printf ("\t%s\t- %s\n", layout->name, layout->desc);
	}
}

void
do_bind_keys (void)
{
	WwLayout *iter, *layouts;
	
	layouts = ww_get_layouts ();
	
	for (iter = layouts; iter->name != NULL; iter++)
	{
		g_printf ("Bind %s %s\n", iter->name, iter->default_hotkey);
		ww_hotkey_bind_layout (iter);
	}
	
}

int
main (int argc, char *argv[])
{
	const WwLayout  *layouts;
	GError			*error;
	GOptionContext  *options;
	GtkStatusIcon	*tray_icon;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_init (&argc, &argv);
	
	layouts = ww_get_layouts ();
	
	options = g_option_context_new (NULL);
	g_option_context_add_main_entries (options, option_entries,
									   GETTEXT_PACKAGE);
	g_option_context_add_group (options, gtk_get_option_group (TRUE));
	
	error = NULL;
	if (!g_option_context_parse (options, &argc, &argv, &error))
	{
		g_printerr (_("Invalid command line: %s\n"), error->message);
		return 1;
	}
	
	if (print_layouts)
	{
		do_print_layouts (layouts);
	}
	else if (layout_name)
	{
		ww_apply_layout_by_name (layout_name);
	}
	
	if (run_tray) {
		run_daemon = TRUE;
		tray_icon = ww_tray_icon_new ();
	}
	
	if (run_daemon) {
		do_bind_keys();
		gtk_main();
	}
	
	else if (!layout_name &&
			 !print_layouts &&
			 !run_daemon &&
			 !run_tray)
	{
		gchar *help_msg = g_option_context_get_help (options, TRUE, NULL);
		g_printf (help_msg);
		g_free (help_msg);
		return 1;
	}
	
	return 0;
}
