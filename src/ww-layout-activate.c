/*
   -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * This file is part of WinWrangler.
 * Copyright (C) Alessio 'molok' Bolognino <themolok@gmail.com>
 *
 *	WinWrangler is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	WinWrangler is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with WinWranger.	If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * ww_layout_activate
 * @screen: The screen to work on
 * @windows: A list of all windows on the @screen
 * @active: The currently active window
 * @error: %GError to set on failure
 * @direction: LEFT, RIGHT UP or DOWN
 *
 * A %WwLayoutHandler to activate a window on the left / right / top / bottom of
 * the current one.
 */

#include "winwrangler.h"

void
ww_layout_activate_left(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	WnckWindow *neighbour;

	neighbour = ww_find_neighbour (screen, windows, active, LEFT);
	neighbour ? wnck_window_activate (neighbour, 0) : 
				g_debug ("Unable to find left neighbour");
}

void
ww_layout_activate_right(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	WnckWindow *neighbour;

	neighbour = ww_find_neighbour (screen, windows, active, RIGHT);
	neighbour ? wnck_window_activate (neighbour, 0) : 
				g_debug ("Unable to find right neighbour");
}

void
ww_layout_activate_up(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	WnckWindow *neighbour;

	neighbour = ww_find_neighbour (screen, windows, active, UP);
	neighbour ? wnck_window_activate (neighbour, 0) : 
				g_debug ("Unable to find upper neighbour");
}

void
ww_layout_activate_down(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	WnckWindow *neighbour;

	neighbour = ww_find_neighbour (screen, windows, active, DOWN);
	neighbour ? wnck_window_activate (neighbour, 0) : 
				g_debug ("Unable to find bottom neighbour");
}
