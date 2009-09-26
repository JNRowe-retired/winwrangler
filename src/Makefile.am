INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DG_LOG_DOMAIN=\"WinWrangler\" \
	$(WINWRANGLER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = winwrangler

winwrangler_SOURCES = \
	winwrangler.h		\
	ww-hotkeys.c		\
	ww-layout-expand.c	\
	ww-layout-tile.c	\
	ww-layout-twothirds.c	\
	ww-layout-switch-spatial.c \
	ww-layouts.c		\
	ww-layouts.h		\
	ww-utils.c		\
	ww-tray.c		\
	main.c

winwrangler_LDFLAGS = 

winwrangler_LDADD = $(WINWRANGLER_LIBS) -lm

EXTRA_DIST = 
