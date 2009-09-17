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


#include "winwrangler.h"

/* Glabls var for the popup menu */
static GtkWidget *popup = NULL;

/* Call a given layout given a GtkAction */
static void
dispatch_layout_handler (GtkAction *action, gpointer data)
{
	const gchar	*name;
	const WwLayout	*layout;
	
	g_return_if_fail (GTK_IS_ACTION(action));
	
	name = gtk_action_get_name (action);
	layout = ww_get_layout (name);
	
	if (!layout) {
		g_critical ("Requested unknown layout '%s'", name);
		return;
	}
	
	WnckScreen		*screen;
	WnckWorkspace   *ws;
	GList			*windows, *struts;
	WnckWindow		*active;
	GError			*error;
	
	screen = wnck_screen_get_default ();
	wnck_screen_force_update (screen);
	
	windows = wnck_screen_get_windows (screen);
	ws = wnck_screen_get_active_workspace (screen);
	struts = ww_filter_strut_windows (windows, ws);
	windows = ww_filter_user_windows (windows, ws);
	active = wnck_screen_get_active_window (screen);
	
	error = NULL;
	layout->handler (screen, windows, struts, active, &error);
	g_list_free (windows);
	g_list_free (struts);
	
	if (error) {
		g_critical ("Error applying layout '%s': %s", name, error->message);
		g_error_free (error);
		return;
	}
	
	
}

static GtkActionGroup*
create_action_group (const WwLayout *layouts)
{
	GtkActionGroup  *actions;
	GtkActionEntry	*entries;
	guint			num_layouts;
	
	num_layouts = ww_get_num_layouts ();
	actions = gtk_action_group_new ("winwrangler-tray");
	entries = g_new0 (GtkActionEntry, num_layouts);
	
	gint i;
	for (i = 0; i < num_layouts; i++)
	{
		g_debug ("Adding GtkActionEntry '%s'", layouts[i].name);
		entries[i].name = layouts[i].name;
		entries[i].label = layouts[i].label;
		entries[i].tooltip = layouts[i].desc;
		entries[i].accelerator = layouts[i].default_hotkey; // FIXME: Hardcoded hotkey
		entries[i].callback = G_CALLBACK (dispatch_layout_handler);
	}
	
	gtk_action_group_add_actions (actions, entries, num_layouts, NULL);
	
	g_free (entries);
	
	return actions;
}

static gchar*
create_ui_def (const WwLayout *layouts)
{
	/* FIXME: This should be autogenerated */
	return g_strdup ("<ui>"
                     "  <popup>"
					 "      <menuitem action=\"expand\"/>"
					 "      <menuitem action=\"tile\"/>"
					 "      <menuitem action=\"twothirds\"/>"
					 "      <menuitem action=\"activate_left\"/>"
					 "      <menuitem action=\"activate_right\"/>"
					 "      <menuitem action=\"activate_up\"/>"
					 "      <menuitem action=\"activate_down\"/>"
					 "  </popup>"
					 "</ui>");
}

/* Callback for clicking tray icon */
static void
show_popup (GtkStatusIcon *tray_icon, guint button, guint time, gpointer data)
{
	GtkMenu	 *menu;
	
	g_return_if_fail (GTK_IS_STATUS_ICON(tray_icon));
	g_return_if_fail (GTK_IS_MENU(data));
	
	menu = GTK_MENU (data);
	gtk_menu_popup (menu, NULL, NULL, NULL, NULL, button, time);
}

/* We override the GtkStatusIcon's dispose() method with this one.
 * It allows us to do some memory management hacks.
 */
static void
override_dispose (GObject *obj)
{	
	g_return_if_fail (GTK_IS_STATUS_ICON(obj));
	
	if (popup) {
		g_object_unref (popup);
		popup = NULL;
	} else {
		if (GTK_IS_MENU(popup))
			g_critical ("Leak of 'popup' for tray icon detected");
	}
	
	/* Call the roiginal dispose() method */
	G_OBJECT_GET_CLASS(obj)->dispose(obj);
}

/**
 * ww_status_icon_new
 * @returns: A #GtkStatusIcon set up with the everything needed
 *
 * To releasse all resources related to the icon simply call g_object_unref()
 * on it.
 */
GtkStatusIcon*
ww_tray_icon_new ()
{	
	const WwLayout	*layouts;
	GtkStatusIcon   *tray_icon;
	GtkUIManager	*ui;
	gchar		*ui_def;
	GtkActionGroup  *actions;
	GError		*error;
	
	if (popup) {
		g_critical ("Global popup for tray icon already set. Ww only supports"
					" one tray icon at a time");
		return NULL;
	}
	
	layouts = ww_get_layouts ();
	tray_icon = gtk_status_icon_new_from_icon_name ("winwrangler");
	
	gtk_status_icon_set_visible (tray_icon, TRUE);
	gtk_status_icon_set_tooltip (tray_icon, "Perform advanced window layout");
	G_OBJECT_GET_CLASS(tray_icon)->dispose = override_dispose; /* To tweak memory management */
	
	ui = gtk_ui_manager_new ();
	actions = create_action_group (layouts);
	gtk_ui_manager_insert_action_group (ui, actions, 0);
	
	error = NULL;
	ui_def = create_ui_def (layouts);
	gtk_ui_manager_add_ui_from_string (ui, ui_def, -1, &error);
	
	if (error) {
		g_critical ("Failed to create ui: %s\n", error->message);
		g_printf ("%s\n", ui_def);
		g_error_free (error);
		return NULL;
	}
	
	popup = gtk_ui_manager_get_widget (ui, "/popup");
	g_return_val_if_fail (GTK_IS_MENU(popup), NULL);
	
	g_signal_connect (tray_icon, "popup-menu", G_CALLBACK(show_popup), popup);
	
	/* This should be freed by our overridden dispose() method on the
	 * tray_icon */
	g_object_ref (popup);
	
	g_free (ui_def);
	g_object_unref (ui);
	g_object_unref (actions);
	
	return tray_icon;
}
