/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c (winwrangler)
 * Copyright (C) Mikkel Kamstrup Erlandsen 2008 <mikkel.kamstrup@gmail.com>
 * 
 * main.c is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>



/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif



#include "winwrangler.h"

static gchar *layout_name;

static GOptionEntry option_entries[] = 
{
  { "layout", 'l', 0, G_OPTION_ARG_STRING, &layout_name, "The layout function to apply" },
  { NULL }
};

int
main (int argc, char *argv[])
{

	WnckScreen* screen;
	WnckWindow* active;
	GError *error;
	GOptionContext *options;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init (&argc, &argv);

	options = g_option_context_new (NULL);
	g_option_context_add_main_entries (options, option_entries, GETTEXT_PACKAGE);
	g_option_context_add_group (options, gtk_get_option_group (TRUE));
	
	error = NULL;
	if (!g_option_context_parse (options, &argc, &argv, &error))
		{
			g_print ("Bad command line: %s\n", error->message);
			return 1;
		}
	
	screen = wnck_screen_get_default ();
	wnck_screen_force_update (screen);
	
	active = wnck_screen_get_active_window (screen);

	
	return 0;
}
