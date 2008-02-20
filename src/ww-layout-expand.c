/*
   -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * ww-layout-expand.c (winwrangler)
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

#include "winwrangler.h"

/**
 * ww_layout_expand
 * @screen: The screen to work on
 * @windows: A list of all windows on the @screen
 * @active: The currently active window
 * @error: %GError to set on failure
 *
 * A %WwLayoutHandler expanding @active in all directions without it
 * overlapping any windows it doesn't already.
 */
void
ww_layout_expand (WnckScreen	*screen,
				  GList			*windows,
				  WnckWindow	*active,
				  GError		**error)
{
	GList *next;
	WnckWindow *win;
	int x, y, w, h;				/* coords of active */
	int wx, wy, ww, wh;			/* coords of window(s) to compare to */
	int bx, by, br, bb;			/* coord bounds x, y, top, bottom */


	wnck_window_get_geometry (active, &x, &y, &w, &h);
	bx = 0;
	by = 0;
	br = wnck_screen_get_width (screen);
	bb = wnck_screen_get_height (screen);

	
	for (next = windows; next; next = next->next)
	{	
		win = WNCK_WINDOW (next->data);
		
		if (wnck_window_is_active (win))
			continue;
		
		wnck_window_get_geometry (win, &wx, &wy, &ww, &wh);
		
		/* Expand left */
		if (x > wx+ww) {
			bx = MAX (bx, wx+ww);
		}
		
		/* Expand right */
		if (x + w < wx) {
			br = MIN (br, wx);
		}
		
		/* Expand up */
		if (y > wy + wh) {
			by = MAX (by, wy + wh);
		}
		
		/* Expand down */
		if (y + h < wy) {
			bb = MIN (bb, wy);
		}
	}
	
	g_debug ("Expanding window to (%d, %d) @ %dx%d", bx, by, br - bx, bb - by);
	
	wnck_window_set_geometry (active, WNCK_WINDOW_GRAVITY_CURRENT,
							  WW_MOVERESIZE_FLAGS, 
							  bx, by, br - bx, bb - by);
}
