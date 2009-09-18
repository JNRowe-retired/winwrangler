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
#include <math.h>

typedef enum { LEFT, RIGHT, UP, DOWN } Direction;

void
ww_layout_activate (WnckScreen	*screen,
                    GList		*windows,
                    WnckWindow	*active,
                    Direction	direction)
{
	WnckWindow	*neighbour;
	GList		*next;
	int			ax, ay, aw, ah; /* active window geometry */
	int			wx, wy, ww, wh; /* geometry for currently checked window */ 
	int			nx, ny, nw, nh; /* geometry of neighbour */

	neighbour = NULL;
	
	g_return_if_fail (WNCK_IS_SCREEN(screen));
	if (g_list_length(windows) == 0)
    {
		return;
    }
	
	/* If there is no active window, do nothing */
	if (active == NULL
		|| wnck_window_is_skip_tasklist (active)) {
		g_debug ("No active window");
		return;
	}

	nx = ny = nw = nh = 0; 

	wnck_window_get_geometry (active, &ax, &ay, &aw, &ah);
	g_debug("Active window '%s' (%d, %d) @ %d x %d",
	        wnck_window_get_name (active), ax, ay, aw, ah);


	if ( direction == LEFT )
	{
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			if ( wx < ax )
			{
				if ( neighbour == NULL )
				{
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
				else if ( wx == nx )
				{
					if ( abs(wy - ay) < abs(ny - ay) )
					{
						neighbour = WNCK_WINDOW (next->data);
						nx = wx; ny = wy; nw = ww; nh = wh;
					}
				}
				else if ( wx > nx )
				{
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
			} 
		}
	}
	else if ( direction == RIGHT )
	{
		int a_right = ax + aw;
		int w_right;
		
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			w_right = wx + ww;
			
			if ( w_right > a_right )
			{
				if ( neighbour == NULL )
				{
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
				else if ( w_right == (nx + nw) )
				{
					if ( abs(wy - ay) < abs(ny - ay) )
					{
						neighbour = WNCK_WINDOW (next->data);
						nx = wx; ny = wy; nw = ww; nh = wh;
					}
				}
				else if ( w_right < (nx + nw) ) {
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
			}
		}
	}
	else if ( direction == DOWN )
	{
		int a_bottom = ay + ah;
		int w_bottom;
		
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			w_bottom = wy + wh;
			
			if ( w_bottom > a_bottom )
			{
				if ( neighbour == NULL )
				{
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
				else if ( w_bottom == (ny + nh) )
				{
					if ( abs(wx - ax) < abs(nx - ax) )
					{
						neighbour = WNCK_WINDOW (next->data);
						nx = wx; ny = wy; nw = ww; nh = wh;
					}
				}
				else if ( w_bottom < (ny + nh) ) {
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
			}
		}
	}
	else if ( direction == UP )
	{
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			if ( wy < ay )
			{
				if ( neighbour == NULL )
				{
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
				else if ( wy == ny )
				{
					if ( abs(wx - ax) < abs(nx - ax) )
					{
						neighbour = WNCK_WINDOW (next->data);
						nx = wx; ny = wy; nw = ww; nh = wh;
					}
				}
				else if ( wy > ny ) {
					neighbour = WNCK_WINDOW (next->data);
					nx = wx; ny = wy; nw = ww; nh = wh;
				}
			}
		}
	}

	if (neighbour)
    {
    	g_debug ("Switching to '%s' (%d, %d) @ %d x %d",
    	         wnck_window_get_name (neighbour), nx, ny, nw, nh);
		wnck_window_activate(neighbour, 1);
	}
	else 
	{
		g_debug ("No window found");
	}
	return; 
}

void
ww_layout_activate_left(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, active, LEFT );
}

void
ww_layout_activate_right(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, active, RIGHT );
}

void
ww_layout_activate_up(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, active, UP );
}

void
ww_layout_activate_down(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, active, DOWN );
}
