## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/winwrangler/glade
glade_DATA = winwrangler.glade

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
	callbacks.c \
	callbacks.h \
	main.c

winwrangler_LDFLAGS = 

winwrangler_LDADD = $(WINWRANGLER_LIBS)

EXTRA_DIST = $(glade_DATA)
