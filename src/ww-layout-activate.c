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
 *
 * A %WwLayoutHandler TODO FIXME
 */

#include "winwrangler.h"
#include <math.h>

typedef enum { LEFT, RIGHT, UP, DOWN } Direction;

void
ww_layout_activate(	
				WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error,
				Direction	direction
				)
{
	GList	*to_activate, *next;
	int		ax, ay, aw, ah; /* active window geometry */
	int		wx, wy, ww, wh; 
	int		fx, fy, found;

	to_activate = NULL;

	g_return_if_fail (WNCK_IS_SCREEN(screen));
	if (g_list_length(windows) == 0)
		return;
	
	/* If there is no active window, do nothing */
	if (active == NULL
		|| wnck_window_is_skip_tasklist (active)) {
		g_debug ("No active window");
		return;
	}

	found = FALSE;
	fx = fy = 0; 

	wnck_window_get_geometry (active, &ax, &ay, &aw, &ah);
	g_debug("Active window geometry: ax: %d, ay: %d, aw: %d, ah: %d", ax, ay, aw, ah);


	if ( direction == LEFT )
	{
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			if ( wx < ax )
			{
				if ( found == FALSE )
				{
					to_activate = next;
					fx = wx;
					fy = wy;
					found = TRUE;
				}
				else if ( wx == fx )
				{
					if ( abs(wy - ay) < abs(fy - ay) )
					{
						to_activate = next;
						fy = wy;
					}
				}
				else if ( wx > fx )
				{
					to_activate = next;
					fx = wx;
					fy = wy;
				}
			} 
		}
	}
	else if ( direction == RIGHT )
	{
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			if ( wx > ax )
			{
				if ( found == FALSE )
				{
					to_activate = next;
					fx = wx;
					fy = wy;
					found = TRUE;
				}
				else if ( wx == fx )
				{
					if ( abs(wy - ay) < abs(fy - ay) )
					{
						to_activate = next;
						fy = wy;
					}
				}
				else if ( wx < fx ) {
					to_activate = next;
					fx = wx;
					fy = wy;
				}
			}
		}
	}
	else if ( direction == DOWN )
	{
		for ( next = windows; next; next = next->next )
		{
			wnck_window_get_geometry (next->data, &wx, &wy, &ww, &wh);
			if ( wy > ay )
			{
				if ( found == FALSE )
				{
					to_activate = next;
					fy = wy;
					fx = wx;
					found = TRUE;
				}
				else if ( wy == fy )
				{
					if ( abs(wx - ax) < abs(fx - ax) )
					{
						to_activate = next;
						fx = wx;
					}
				}
				else if ( wy < fy ) {
					to_activate = next;
					fy = wy;
					fx = wx;
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
				if ( found == FALSE )
				{
					to_activate = next;
					fy = wy;
					fx = wx;
					found = TRUE;
				}
				else if ( wy == fy )
				{
					if ( abs(wx - ax) < abs(fx - ax) )
					{
						to_activate = next;
						fx = wx;
					}
				}
				else if ( wy > fy ) {
					to_activate = next;
					fy = wy;
					fx = wx;
				}
			}
		}
	}

	if (to_activate) {
		wnck_window_activate(to_activate->data, 1);
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
	ww_layout_activate( screen, windows, struts, active, error, LEFT );
}

void
ww_layout_activate_right(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, struts, active, error, RIGHT );
}

void
ww_layout_activate_up(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, struts, active, error, UP );
}

void
ww_layout_activate_down(WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	ww_layout_activate( screen, windows, struts, active, error, DOWN );
}
