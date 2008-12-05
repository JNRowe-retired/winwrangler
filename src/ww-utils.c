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

/**
 * ww_filter_user_windows
 * @windows: List of %WnckWindow<!-- -->s to filter
 * @current_workspace: Only use windows on this workspace. %NULL indicates
 *                     that all windows should be used
 *
 * Extract the user controlled visible windows from a %GList of
 * %WnckWindows.
 *
 * Return value: A newly allocated list containing only windows that are 
 * not minimized, shaded, or wnck_window_skip_task_list() on the current
 * workspace.
 */
GList*
ww_filter_user_windows (GList * windows, WnckWorkspace *current_workspace)
{
	GList       *next;
	GList       *result;
	WnckWindow  *win;
	WnckWorkspace *win_ws;

	result = NULL;    
	
	for (next = windows; next; next = next->next)
	{
		win = WNCK_WINDOW(next->data);
		if (!wnck_window_is_skip_tasklist (win) &&
			!wnck_window_is_minimized (win) &&
			!wnck_window_is_maximized (win) &&
			!wnck_window_is_shaded (win))
		{
			win_ws = wnck_window_get_workspace (win);
			
			if (current_workspace == NULL ||
				win_ws == current_workspace ||				
				win_ws == NULL)	
				result = g_list_append (result, win);
				
		}
    }
	
	return result;
}

/**
 * ww_filter_strut_windows
 * @windows: List of %WnckWindow<!-- -->s to filter
 * @current_workspace: Only use windows on this workspace. %NULL indicates
 *                     that all windows should be used
 *
 * Extract all windows that should be considered "hard edges", or windows
 * blocking the movement of other windows from %GList of
 * %WnckWindows. The prime example of a "strut" is a standard desktop panel.
 *
 * Return value: A newly allocated list containing only windows that are 
 * not minimized, maximized, or shaded, and is wnck_window_skip_task_list() and
 * wnck_window_is_pinned().
 */
GList*
ww_filter_strut_windows (GList * windows, WnckWorkspace *current_workspace)
{
	GList       *next;
	GList       *result;
	WnckWindow  *win;
	WnckWorkspace *win_ws;

	result = NULL;    
	
	for (next = windows; next; next = next->next)
	{
    win = WNCK_WINDOW(next->data);
    
    /* Debug code to print out all window geometries: */
    /*int x, y, w, h;
    wnck_window_get_geometry(win, &x, &y, &w, &h);
    g_debug ("\"%s\": (%d, %d)@%dx%d",
             wnck_window_get_name(win), x, y, w, h);*/
    		
		if (wnck_window_is_skip_tasklist (win) &&
			wnck_window_is_pinned (win) &&
			!wnck_window_is_minimized (win) &&
			!wnck_window_is_maximized (win) &&
			!wnck_window_is_fullscreen (win) &&
			!wnck_window_is_shaded (win))
		{
			/* We don't consider the desktop a strut.
       * For Gnome >= 2.24 the desktop is called "x-nautilus-desktop", in
       * earlier Gnome versions it is just called "Desktop" */
			if (g_str_equal (wnck_window_get_name(win), "Desktop") ||
                g_str_equal (wnck_window_get_name(win), "x-nautilus-desktop"))
				continue;
			
			win_ws = wnck_window_get_workspace (win);
			
			if (current_workspace == NULL ||
				win_ws == current_workspace ||				
				win_ws == NULL)	
				result = g_list_append (result, win);
		}  
  }
	
	return result;
}

/**
 * ww_apply_layout_by_name
 * @layout_name: The name of the layout to apply
 *
 * Apply a given layout to the default screen by looking up the relevant
 * #WwLayout based on its name.
 */
void
ww_apply_layout_by_name (const gchar * layout_name)
{
	WnckScreen *screen;
	WnckWorkspace   *current_ws;
	GList *windows, *struts;
	WnckWindow *active;
	const WwLayout *layout;
	GError *error;
	
	screen = wnck_screen_get_default ();
	wnck_screen_force_update (screen);
	
	current_ws = wnck_screen_get_active_workspace (screen);
	windows = wnck_screen_get_windows (screen);
	struts = ww_filter_strut_windows (windows, current_ws);
	windows = ww_filter_user_windows (windows, current_ws);
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
	layout->handler (screen, windows, struts, active, &error);
	g_list_free (windows);
	g_list_free (struts);
	
	if (error)
	{
		g_printerr ("Failed to apply layout '%s'. Error was:\n%s",
					layout_name, error->message);
		g_error_free (error);
		return;
	}	
}
