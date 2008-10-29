## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po data

winwranglerdocdir = ${prefix}/doc/winwrangler
winwranglerdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	INSTALL\
	NEWS \
	HACKING

EXTRA_DIST = \
	intltool-extract.in \
	intltool-merge.in   \
	intltool-update.in  \
	$(winwranglerdoc_DATA)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

