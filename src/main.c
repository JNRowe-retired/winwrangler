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

static gchar *layout_name = NULL;
static gboolean *print_layouts = FALSE;

static GOptionEntry option_entries[] = {
  {"layout", 'l', 0, G_OPTION_ARG_STRING, &layout_name,
   "The layout function to apply"},
  {"layouts", 0, 0, G_OPTION_ARG_NONE, &print_layouts,
   "Print a list of layout functions"},
  {NULL}
};

static void
do_print_layouts (const WwLayout * layouts)
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

static void
do_apply_layout (gchar * layout_name)
{
  WnckScreen *screen;
  GList *windows;
  WnckWindow *active;
  const WwLayout *layout;
  GError *error;

  screen = wnck_screen_get_default ();
  wnck_screen_force_update (screen);

  windows = wnck_screen_get_windows (screen);
  windows = ww_filter_user_windows (windows);
  active = wnck_screen_get_active_window (screen);

  /* Check that we know the requested layout */
  layout = ww_get_layout (layout_name);
  if (!layout)
    {
      g_printerr ("No such layout: '%s'. Try running with --layouts to "
		  "list possible layouts\n", layout_name);
      return;
    }

  /* Apply the layout */
  error = NULL;
  layout->handler (screen, windows, active, &error);
  g_list_free (windows);

  if (error)
    {
      g_printerr ("Failed to apply layout '%s'. Error was:\n%s",
		  layout_name, error->message);
      g_error_free (error);
      return;
    }

  gtk_main();	
}

int
main (int argc, char *argv[])
{
  const WwLayout *layouts;
  GError *error;
  GOptionContext *options;

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
      g_print ("Bad command line: %s\n", error->message);
      return 1;
    }

  if (print_layouts)
    {
      do_print_layouts (layouts);
      return 0;
    }
  else if (layout_name)
    {
      do_apply_layout (layout_name);
      return 0;
    }

  else
    {
      gchar *help_msg = g_option_context_get_help (options, TRUE, NULL);
      g_printf (help_msg);
      g_free (help_msg);
      return 0;
    }
}
