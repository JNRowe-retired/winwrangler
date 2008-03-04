# ******************************************************************************
# Images and icon
# ******************************************************************************
artdir = $(pkgdatadir)/art
art_DATA = winwrangler.png

16icondir = $(datadir)/icons/hicolor/16x16/apps
16icon_DATA = 16x16/winwrangler.png

22icondir = $(datadir)/icons/hicolor/22x22/apps
22icon_DATA = 22x22/winwrangler.png

32icondir = $(datadir)/icons/hicolor/32x32/apps
32icon_DATA = 32x32/winwrangler.png

scalableicondir = $(datadir)/icons/hicolor/scalable/apps
scalableicon_DATA = scalable/winwrangler.svg

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
	$(art_DATA) \
	$(16icon_DATA) \
	$(22icon_DATA) \
	$(32icon_DATA) \
	$(scalableicon_DATA)
