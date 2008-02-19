## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/winwrangler/glade
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
	main.c       \
	winwrangler.h       \
	ww-layout-expand.c       \
	ww-layouts.c       \
	ww-layouts.h

winwrangler_LDFLAGS = 

winwrangler_LDADD = $(WINWRANGLER_LIBS)

EXTRA_DIST = $(glade_DATA)
