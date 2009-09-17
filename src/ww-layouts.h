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

 
#ifndef _WW_LAYOUTS_H_
#define _WW_LAYOUTS_H_

G_BEGIN_DECLS

/* Macro to define a layout handler. Layout handlers should also be added 
 * to ww-layouts.c in the "layouts" array */
#define WW_LAYOUT_IMPL(layout) void layout (WnckScreen *screen, GList *windows, GList *struts, WnckWindow *active, GError **error);

WW_LAYOUT_IMPL(ww_layout_expand)
WW_LAYOUT_IMPL(ww_layout_tile)
WW_LAYOUT_IMPL(ww_layout_twothirds)
WW_LAYOUT_IMPL(ww_layout_activate_left)
WW_LAYOUT_IMPL(ww_layout_activate_right)
WW_LAYOUT_IMPL(ww_layout_activate_up)
WW_LAYOUT_IMPL(ww_layout_activate_down)

G_END_DECLS

#endif /* _WW_LAYOUTS_H_ */
