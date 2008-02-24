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
