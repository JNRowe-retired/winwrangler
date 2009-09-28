/*
 * This file is part of WinWrangler.
 * Copyright (C) Mikkel Kamstrup Erlandsen 2008 <mikkel.kamstrup@gmail.com>
 *               Alessio 'molok' Bolognino <themolok@gmail.com>
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

#include <math.h>

#include "winwrangler.h"

static guint32 _event_time = 0;

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
			
			if (current_workspace == NULL
			    || (win_ws == current_workspace
			        && wnck_window_is_in_viewport(win, current_workspace))
			    || win_ws == NULL)	
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
    		
		if (wnck_window_get_window_type(win) == WNCK_WINDOW_DOCK)
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

#define is_high(w, h) (h > w)
#define is_broad(w, h) (w > h)

/**
 * ww_calc_bounds
 * @screen: The screen for which to calculate the bounds
 * @struts: A list of %WnckWindow<!---->s that should be treated as
 *          blocking elements on the desktop. Eg. panels and docks
 * @x: Return value for the left side of the bounding box
 * @y: Return value for the top of the box
 * @right: Return coordinate for the right side of the bounding box
 * @bottom: Return value for the bottom coordinate of the bounding box
 *
 * Calculate the maximal rect within a set of blocking windows.
 * For simplicity this method assumes that all struts are along the screen
 * edges and expand over the entire screen edge. Ie a standard panel setup.
 */
void
ww_calc_bounds (WnckScreen *screen,
                GList *struts, 
                int *left, 
                int *top, 
                int *right, 
                int *bottom)
{
	GList		*next;
	WnckWindow  *win;
	int wx, wy, ww, wh; /* current window geom */
	int edge_l, edge_t, edge_b, edge_r;
	int screen_w, screen_h;
	
	edge_l = 0;
	edge_t = 0;
	edge_r = wnck_screen_get_width (screen);
	edge_b = wnck_screen_get_height (screen);
	
	screen_w = edge_r;
	screen_h = edge_b;
	
	for (next = struts; next; next = next->next)
	{	
		win = WNCK_WINDOW (next->data);
		wnck_window_get_geometry (win, &wx, &wy, &ww, &wh);
		
		/* Left side strut */
		if (is_high(ww, wh) && wx == 0) {
			edge_l = MAX(edge_l, ww);
		}
		
		/* Top struct */
		else if (is_broad(ww, wh) && wy == 0) {
			edge_t = MAX (edge_t, wh);
		}
		
		/* Right side strut */
		else if (is_high(ww, wh) && (wx+ww) == screen_w) {
			edge_r = MIN(edge_r, wx);
		}
		
		/* Bottom struct */
		else if (is_broad(ww, wh) && (wy+wh) == screen_h) {
			edge_b = MIN (edge_b, wy);
		}
		
		else {
			g_warning ("Desktop layout contains floating element at "
					   "(%d, %d)@%dx%d", wx, wy, ww, wh);
		}
	}
	
	g_debug ("Calculated desktop bounds (%d, %d), (%d, %d)",
			 edge_l, edge_t, edge_r, edge_b);
	
	*left = edge_l;
	*top = edge_t;
	*right = edge_r;
	*bottom = edge_b;
}

/**
 * ww_window_center
 * @win:
 * @center_x:
 * @center_y:
 *
 * Return value: The return value is written to @center_x and @center_y and
 *               represents the center of gravity for @win
 */
static void
ww_window_center (WnckWindow *win, int *center_x, int *center_y)
{
	int x, y, w, h;

	wnck_window_get_geometry (win, &x, &y, &w, &h);
	*center_x = x + (w/2);
	*center_y = y + (h/2);	
}

static double
ww_euclidian_distance (int x1, int y1, int x2, int y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

/**
 * ww_x_weighted_distance
 * @x1:
 * @y1:
 * @x2:
 * @y2:
 *
 * Calculate a non-euclidian distance between two points that is stretched in
 * x-dimension, making differences in the x-dimension bigger.
 *
 * Return value: A non-euclidian distance measure between the two points
 *               (@x1,@y1) and (@x2,@y2) that is skewed to make differences
 *               in the x-dimension more important
 */
static double
ww_x_weighted_distance (int x1, int y1, int x2, int y2)
{
	return sqrt( (x1-x2)*(x1-x2)*2 + (y1-y2)*(y1-y2));
}

/**
 * ww_y_weighted_distance
 * @x1:
 * @y1:
 * @x2:
 * @y2:
 *
 * Calculate a non-euclidian distance between two points that is stretched in
 * y-dimension, making differences in the y-dimension bigger.
 *
 * Return value: A non-euclidian distance measure between the two points
 *               (@x1,@y1) and (@x2,@y2) that is skewed to make differences
 *               in the y-dimension more important
 */
static double
ww_y_weighted_distance (int x1, int y1, int x2, int y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)*2 );
}

/**
 * ww_find_neighbour
 * @screen:
 * @windows:
 * @active:
 * @direction:
 *
 * Return value: The neighbouring window from @windows in the given direction
 *               or %NULL in case no window is found or @active is %NULL
 */
WnckWindow*
ww_find_neighbour (WnckScreen	*screen,
                   GList		*windows,
                   WnckWindow	*active,
                   WwDirection	direction)
{
	WnckWindow	*neighbour;
	GList		*next;
	int			ax, ay, aw, ah; /* active window geometry */
	int			wx, wy; /* geometry for currently checked window */ 
	int			nx, ny; /* geometry of neighbour */
	double		wdist, ndist; /* distance to active window */

	neighbour = NULL;
	
	g_return_val_if_fail (WNCK_IS_SCREEN(screen), NULL);
	
	if (g_list_length(windows) == 0)
    {
		return NULL;
    }
	
	/* If there is no active window, do nothing */
	if (active == NULL
		|| wnck_window_is_skip_tasklist (active)) {
		g_debug ("No active window");
		return NULL;
	}

	nx = ny = 0;
	ndist = 100000;

	wnck_window_get_geometry (active, &ax, &ay, &aw, &ah);
	g_debug("Active window '%s' (%d, %d) @ %d x %d",
	        wnck_window_get_name (active), ax, ay, aw, ah);

	/* Set ax and ay to the center of grav. for active */
	ww_window_center (active, &ax, &ay);
	
	if ( direction == LEFT )
	{
		for ( next = windows; next; next = next->next )
		{
			ww_window_center (WNCK_WINDOW (next->data), &wx, &wy);
			wdist = ww_y_weighted_distance (wx, wy, ax, ay);
			if ( wx < ax )
			{
				if ( wdist < ndist )
				{
					neighbour = WNCK_WINDOW (next->data);
					ndist = wdist;
				}
			} 
		}
	}
	else if ( direction == RIGHT )
	{
		for ( next = windows; next; next = next->next )
		{
			ww_window_center (WNCK_WINDOW (next->data), &wx, &wy);
			wdist = ww_y_weighted_distance (wx, wy, ax, ay);
			if ( wx > ax )
			{
				if ( wdist < ndist )
				{
					neighbour = WNCK_WINDOW (next->data);
					ndist = wdist;
				}
			}
		}
	}
	else if ( direction == DOWN )
	{
		for ( next = windows; next; next = next->next )
		{
			ww_window_center (WNCK_WINDOW (next->data), &wx, &wy);
			wdist = ww_x_weighted_distance (wx, wy, ax, ay);
			if ( wy > ay )
			{
				if ( wdist < ndist )
				{
					neighbour = WNCK_WINDOW (next->data);
					ndist = wdist;
				}
			}
		}
	}
	else if ( direction == UP )
	{
		for ( next = windows; next; next = next->next )
		{
			ww_window_center (WNCK_WINDOW (next->data), &wx, &wy);
			wdist = ww_x_weighted_distance (wx, wy, ax, ay);
			if ( wy < ay )
			{
				if ( wdist < ndist )
				{
					neighbour = WNCK_WINDOW (next->data);
					ndist = wdist;
				}
			}
		}
	}

	if (neighbour)
		g_debug ("Found neighbour '%s'",
		         wnck_window_get_name (neighbour));
	
	return neighbour; 
}

guint32
ww_get_event_time (void)
{
	return _event_time;
}

void
ww_set_event_time (guint32 event_time)
{
	_event_time = event_time;
}