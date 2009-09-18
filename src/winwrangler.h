/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
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


#ifndef _WW_H_
#define _WW_H_

#include <config.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/window.h>
#include <libwnck/screen.h>
#include <libwnck/workspace.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS
/* Function prototypes */
typedef void (*WwLayoutHandler) (WnckScreen 	*screen,
				 				 GList 			*windows,
				 				 GList			*struts,
				 				 WnckWindow		*active,
								 GError			**error);

/* Structures */
typedef struct
{
  const gchar *name;
  const gchar *label;
  const gchar *desc;
  const gchar *default_hotkey;
  WwLayoutHandler handler;  
} WwLayout;

typedef enum
{
	LEFT,
	RIGHT,
	UP,
	DOWN
} WwDirection;

/* Constants */
#define WW_MOVERESIZE_FLAGS WNCK_WINDOW_CHANGE_WIDTH | WNCK_WINDOW_CHANGE_HEIGHT | WNCK_WINDOW_CHANGE_X | WNCK_WINDOW_CHANGE_Y

/* Functions implemented in ww-layouts.c */
const WwLayout*		ww_get_layouts			(void);

const WwLayout*		ww_get_layout			(const gchar *layout_name);

guint				ww_get_num_layouts		(void);


/* Functions in ww-utils.c */
GList*				ww_filter_user_windows		(GList *windows,
												 WnckWorkspace *current);

GList*				ww_filter_strut_windows		(GList *windows,
												 WnckWorkspace *current);

GtkStatusIcon*		ww_tray_icon_new			(void);

gboolean			ww_hotkey_bind_layout		(WwLayout *layout);

void				ww_apply_layout_by_name		(const gchar *layout_name);

void				ww_calc_bounds				(WnckScreen	*screen,
												 GList *struts,
												 int *left,
												 int *top,
												 int *right,
												 int *bottom);

WnckWindow*			ww_find_neighbour			(WnckScreen		*screen,
												 GList			*windows,
						                         WnckWindow		*active,
						                         WwDirection	direction);

G_END_DECLS
#endif /* _WW_H_ */
