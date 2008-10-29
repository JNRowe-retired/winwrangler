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
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "winwrangler.h"
#include "ww-layouts.h"

static WwLayout layouts[] = {
	{"expand",
	 "Expand active window",
	 "Expand the currently active window to fill all available space "
	 "without overlapping any new windows",
	 "<Ctrl><Super>1",
	 ww_layout_expand},
	{"tile",
	 "Tile all windows",
	 "Tile all visible windows",
	 "<Ctrl><Super>2",
	 ww_layout_tile},
	{NULL}
};

/**
 * ww_get_layouts
 *
 * Get a %NULL terminated array containing all known %WwLayout<!-- -->s
 *
 * Return value: An array containing all known WwLayouts
 */
const WwLayout *
ww_get_layouts (void)
{
	return layouts;
}

/**
 * ww_get_layout
 * @layout_name: Unique name of the layout to look up
 *
 * Get a %WwLayout describing a named layout
 *
 * Return value: The WwLayout with the given name or %NULL if none is found
 */
const WwLayout *
ww_get_layout (const gchar * layout_name)
{
	WwLayout *layout;
	
	g_return_val_if_fail (layout_name != NULL, NULL);
	
	for (layout = layouts; layout->name != NULL; layout++)
	{
		if (g_str_equal (layout_name, layout->name))
			return layout;
	}
	
	return NULL;
}

/**
 * ww_get_num_layouts
 *
 * Get the number of known layouts
 *
 * Return value: The number of known layouts
 */
guint
ww_get_num_layouts (void)
{
	WwLayout	*layout;
	guint		count;
	
	count = 0;
	for (layout = ww_get_layouts (); layout->name != NULL; layout++)
		count++;
	
	return count;
}
