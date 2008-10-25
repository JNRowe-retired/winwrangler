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
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "winwrangler.h"

#include <gtkhotkey.h>

#define HOTKEY_APP_ID "winwrangler"

static void
on_hotkey_activated (GtkHotkeyInfo *hotkey, guint event_time, WwLayout *layout)
{
	ww_apply_layout_by_name (layout->name);
}

gboolean
ww_hotkey_bind_layout (WwLayout *layout)
{
	GtkHotkeyInfo *hotkey;
	GtkHotkeyRegistry *hotkey_registry;
	GError *error;
	
	g_return_val_if_fail(layout != NULL, FALSE);
	
	error = NULL;
	hotkey = NULL;
	hotkey_registry = gtk_hotkey_registry_get_default ();
	hotkey = gtk_hotkey_registry_get_hotkey (hotkey_registry,
						 HOTKEY_APP_ID,
						 layout->name,
						 &error);
	
	if (error)
	{
		if (error->code != GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_APP &&
		    error->code != GTK_HOTKEY_REGISTRY_ERROR_UNKNOWN_KEY)
		{
			g_critical ("Error binding hotkey for '%s': %s",
				    layout->name, error->message);
			goto clean_up;
		}
		
		/* It's OK. The error is just that the app or key is unknown */
		g_error_free (error);
		error = NULL;
	}
	
	/* If the hotkey is not stored, create it and and store it */
	if (hotkey == NULL) {
		hotkey = gtk_hotkey_info_new (HOTKEY_APP_ID,
					      layout->name,
					      layout->default_hotkey,
					      NULL);
		gtk_hotkey_info_set_description (hotkey,
						 layout->desc);
		
		if (hotkey == NULL) {
			g_critical ("Error creating hotkey %s for '%s'",
				    layout->default_hotkey,
				    layout->name);
			goto clean_up;
		}
		
		gtk_hotkey_registry_store_hotkey (hotkey_registry,
						  hotkey,
						  &error);
		if (error) {
			g_critical ("Error storing hotkey %s for '%s': %s",
				    layout->default_hotkey,
				    layout->name,
				    error->message);
			goto clean_up;
		}
	}
	
	/* Bind the hotkey */
	gtk_hotkey_info_bind (hotkey, &error);
	if (error) {
		g_critical ("Error binding hotkey %s for '%s': %s",
			    gtk_hotkey_info_get_signature(hotkey),
			    layout->name,
			    error->message);
		goto clean_up;
	}
	
	/* Attach callbacks to hotkey events */
	g_signal_connect (hotkey, "activated", on_hotkey_activated, layout);
	
	g_debug("Bound hotkey %s for '%s'",
		gtk_hotkey_info_get_signature(hotkey),
		layout->name);
	
	clean_up:
	    if (hotkey_registry) g_object_unref (hotkey_registry);
	    if (hotkey) g_object_unref (hotkey);
	    if (error) {
		    g_error_free (error);
		    return FALSE;
	    }
	
	return TRUE;
}
