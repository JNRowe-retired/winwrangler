/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "winwrangler.h"

/* Call a given layout given a GtkAction */
void
dispatch_layout_handler (GtkAction *action, gpointer data)
{
	const gchar		*name;
	const WwLayout	*layout;
	
	g_return_if_fail (GTK_IS_ACTION(action));
	
	name = gtk_action_get_name (action);
	layout = ww_get_layout (name);
	
	if (!layout) {
		g_critical ("Requested unknown layout '%s'", name);
		return;
	}
	
	WnckScreen  *screen;
	GList		*windows;
	WnckWindow  *active;
	GError		*error;
	
	screen = wnck_screen_get_default ();
	wnck_screen_force_update (screen);
	
	windows = wnck_screen_get_windows (screen);
	windows = ww_filter_user_windows (windows);
	active = wnck_screen_get_active_window (screen);
	
	layout->handler (screen, windows, active, &error);
	if (error) {
		g_critical ("Error applying layout '%s': %s", name, error->message);
		g_error_free (error);
		return;
	}
	
	
}

GtkActionGroup*
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
		g_debug ("Adding GtkAction '%s'", layouts[i].name);
		entries[i].name = layouts[i].name;
		entries[i].label = layouts[i].name;
		entries[i].tooltip = layouts[i].desc;
		entries[i].callback = G_CALLBACK (dispatch_layout_handler);
	}
	
	gtk_action_group_add_actions (actions, entries, num_layouts, NULL);
	
	for (i = 0; i < num_layouts; i++)
		g_free (entries + i);
	
	return actions;
}

gchar*
create_ui_def (const WwLayout *layouts)
{
	return g_strdup ("<ui>"
                     "  <popup>"
					 "      <menuitem name=\"Expand\" action=\"expand\"/>"
					 "  </popup>"
					 "</ui>");
}

int
main (int argc, char *argv[])
{
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	const WwLayout	*layouts;
	GtkStatusIcon   *tray_icon;
	GtkUIManager	*ui;
	gchar			*ui_def;
	GtkActionGroup  *actions;
	GError			*error;
	
	gtk_init (&argc, &argv);
	
	layouts = ww_get_layouts ();
	tray_icon = gtk_status_icon_new_from_stock (GTK_STOCK_OPEN);
	gtk_status_icon_set_visible (tray_icon, TRUE);
	
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
		return 1;
	}
	
	gtk_main();
	
	g_free (ui_def);
	g_object_unref (ui);
	g_object_unref (actions);
	g_object_unref (tray_icon);
	
}
