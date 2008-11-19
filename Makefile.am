SUBDIRS = src po data

winwranglerdocdir = ${prefix}/doc/winwrangler
winwranglerdoc_DATA = \
	README		\
	COPYING		\
	AUTHORS		\
	INSTALL		\
	NEWS		\
	HACKING

debian_dist = \
	debian/changelog	\
	debian/compat		\
	debian/control		\
	debian/copyright	\
	debian/rules


EXTRA_DIST = \
	intltool-extract.in 	\
	intltool-merge.in   	\
	intltool-update.in  	\
	$(debian_dist)			\
	$(winwranglerdoc_DATA)

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

# Copy all the spec files. Of course only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

