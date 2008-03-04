## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(WINWRANGLER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = winwrangler

winwrangler_SOURCES = \
	winwrangler.h	   \
	ww-layout-expand.c  \
	ww-layout-tile.c	\
	ww-layouts.c		\
	ww-layouts.h		\
	ww-utils.c			\
	main.c				\
	ww-tray.h			\
	ww-tray.c

winwrangler_LDFLAGS = 

winwrangler_LDADD = $(WINWRANGLER_LIBS) -lm

EXTRA_DIST = 
