/*
   -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- 
 */
/*
 * This file is part of WinWrangler.
 * Copyright (C) Alessio 'molok' Bolognino <themolok@gmail.com>s
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "winwrangler.h"

/**
 * ww_layout_twothirds
 * @screen: The screen to work on
 * @windows: A list of all windows on the @screen
 * @active: The currently active window
 * @error: %GError to set on failure
 *
 * A %WwLayoutHandler resizing the active window to 2/3 of the screen
 */
void
ww_layout_twothirds (WnckScreen	*screen,
				GList		*windows,
				GList		*struts,
				WnckWindow	*active,
				GError		**error)
{
	GList	*next;
	int		dim, row;
	int		r_cell_w, r_cell_h;
	int		edge_l, edge_t, edge_b, edge_r;
	int		lg_h, lg_w, rg_h, rg_w;
	int		is_active;
	
	g_return_if_fail (WNCK_IS_SCREEN(screen));
	if (g_list_length(windows) == 0)
		return;
	
	ww_calc_bounds (screen, struts, &edge_l, &edge_t, &edge_r, &edge_b);
	
	lg_w = (edge_r - edge_l ) / 3 * 2;
	rg_w = (edge_r - edge_l) - lg_w;

	lg_h = rg_h = edge_b - edge_t;

	dim = g_list_length(windows);

	/* If there is only one window, resize it to fullscreen and exit */
	if ( dim == 1 ) {
		wnck_window_set_geometry (windows->data,
								  WNCK_WINDOW_GRAVITY_STATIC,
								  WW_MOVERESIZE_FLAGS, 
								  edge_l, edge_t,
								  edge_r - edge_l,
								  edge_b - edge_t);
		return;
	}

	dim -= 1;

	/* If there is no active window, do nothing */
	/* FIXME: is there a better way to do this? */
	is_active = FALSE;
	for (next = windows; next; next = next->next) 
	{
		if (wnck_window_is_active (next->data) == TRUE) 
		{
			is_active = TRUE;
			break;
		}
	}
	if ( is_active == FALSE ) 
	{
		g_debug("No active window!");
		return; 
	} 
	// END FIXME

	r_cell_w = rg_w;
	r_cell_h = rg_h / dim;

	row = 0;
	for (next = windows; next; next = next->next) 
	{
		if (wnck_window_is_active (next->data) == TRUE) 
		{
			wnck_window_set_geometry (next->data,
									  WNCK_WINDOW_GRAVITY_STATIC,
									  WW_MOVERESIZE_FLAGS, 
									  edge_l, edge_t , lg_w, lg_h);
		} else {
			wnck_window_set_geometry (next->data, 
									  WNCK_WINDOW_GRAVITY_STATIC,
									  WW_MOVERESIZE_FLAGS, 
									  lg_w + edge_l, row*r_cell_h + edge_t,
									  r_cell_w, r_cell_h);
			row++;
		}
	}
}

