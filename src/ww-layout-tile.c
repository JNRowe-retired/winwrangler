/*
   -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * ww-layout-tile.c (winwrangler)
 * Copyright (C) Mikkel Kamstrup Erlandsen 2008 <mikkel.kamstrup@gmail.com>
 * 
 * ww-layout-expand.c is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * ww-layout-expand.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with callbacks.c.  If not, write to:
 *      The Free Software Foundation, Inc.,
 *      51 Franklin Street, Fifth Floor
 *      Boston, MA  02110-1301, USA.
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
				WnckWindow	*active,
				GError		**error)
{
	GList   *next;
	int		*dim;
	int		cell_w, cell_h;
	int		screen_w, screen_h;
	
	dim = get_grid_size (windows);
	g_debug ("Grid is %dx%d\n", dim[0], dim[1]);
	
	screen_w = wnck_screen_get_width (screen);
	screen_h = wnck_screen_get_height (screen);
	
	cell_w = screen_w / dim[0];
	cell_h = screen_h / dim[1];
	
	int row = 0, col = 0;
	for (next = windows; next; next = next->next)
	{
		wnck_window_set_geometry (next->data, WNCK_WINDOW_GRAVITY_CURRENT,
								  WW_MOVERESIZE_FLAGS, 
								  col*cell_w, row*cell_h,
								  cell_w, cell_h);
		
		col++;
		if (col == dim[0])
		{
			col = 0;
			row++;
		}		
	}
	
	g_free (dim);
}
