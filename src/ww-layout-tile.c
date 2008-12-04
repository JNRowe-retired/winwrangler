/*
   -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>

#include "winwrangler.h"

/**
 * get_grid_size
 * @windows: A list of all windows to be arranged 
 *
 * Calculate a minimal grid containing the number of windows
 * found in @windows
 *
 * Return value: A newly allocated integer array with two values (x,y)
 */
int*
get_grid_size (GList	*windows)
{
	int		*result;
	int		count;
	GList   *next;
	
	result = g_new0(int, 2);
	count = 0;
	
	/* Count the windows */
	for (next = windows; next; next = next->next)
	{
		count++;
	}
	
	result[0] = ceilf(sqrt(count));
	
	/* Check if we have an exact square */
	if (result[0]*result[0] == count)
	{
		result[1] = result[0];
		return result;
	}
	
	g_debug ("Num windows: %d", count);
	
	result[1] = floorf(sqrt(count));
	
	/* Adjust for odd cases (like count=3) */
	if (result[0]*result[1] < count)
		result[1]++;
	
	return result;
}

#define is_high(w, h) (h > w)
#define is_broad(w, h) (w > h)
/* Calculate the maximal rect within a set of blocking windows.
 * For simplicity this method assumes that all struts are along the screen
 * edges and expand over the entire screen edge. Ie a standard panel setup. */
void
calculate_bounds (WnckScreen *screen, GList *struts, int *x, int *y, int *bottom_right, int *bottom)
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
	
	*x = edge_l;
	*y = edge_t;
	*bottom_right = edge_r;
	*bottom = edge_b;
}

/**
 * ww_layout_tile
 * @screen: The screen to work on
 * @windows: A list of all windows on the @screen
 * @active: The currently active window
 * @error: %GError to set on failure
 *
 * A %WwLayoutHandler tiling all visible windows
 */
void
ww_layout_tile (WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	GList   *next;
	int		*dim;
	int		cell_w, cell_h;
	int		edge_l, edge_t, edge_r, edge_b;
	
	g_return_if_fail (WNCK_IS_SCREEN(screen));
	if (g_list_length(windows) == 0)
		return;
	
	dim = get_grid_size (windows);
	
	calculate_bounds (screen, struts, &edge_l, &edge_t, &edge_r, &edge_b);
	
	cell_w = (edge_r - edge_l) / dim[0];
	cell_h = (edge_b - edge_t) / dim[1];

	g_debug ("Grid is %dx%d, with cell size %dx%d\n",
			 dim[0], dim[1], cell_w, cell_h);
	
	int row = 0, col = 0;
	for (next = windows; next; next = next->next)
	{
		g_debug ("set_geom(%d, %d, %d, %d)",
				 col*cell_w + edge_l, row*cell_h + edge_t,
				 cell_w, cell_h);
		wnck_window_set_geometry (next->data, WNCK_WINDOW_GRAVITY_STATIC,
								  WW_MOVERESIZE_FLAGS, 
								  col*cell_w + edge_l, row*cell_h + edge_t,
								  cell_w, cell_h);
		
		col++;
		
		/* Check if we should start a new row */
		if (col == dim[0])
		{
			col = 0;
			row++;
			
		}		
	}
	
	g_free (dim);
}
