EXTRA_DIST = autogen.sh COPYING INSTALL README AUTHORS NEWS bigobjects.pc.in

SUBDIRS = libbigobjects drivers $(BIGOBJECT_SUBDIR) tests

CLEANFILES = *~

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = bigobjects.pc

gitclean: distclean
	find . -name Makefile.in -exec rm -f {} \;
	find . -name Makefile -exec rm -f {} \;
	rm -fr autom4te.cache
	rm -f missing aclocal.m4 config.h.in config.guess config.sub ltmain.sh install-sh configure depcomp
	rm -f test-driver

dist-hook:
	(cd $(srcdir) && git diff && echo ===== git log ==== && git log) > $(distdir)/ChangeLog
