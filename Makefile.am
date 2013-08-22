EXTRA_DIST = autogen.sh COPYING INSTALL README AUTHORS NEWS bigfiles.pc.in

SUBDIRS = libbigfiles adapters $(BIGFILE_SUBDIR) tests

CLEANFILES = *~

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = bigfiles.pc

gitclean: distclean
	find . -name Makefile.in -exec rm -f {} \;
	find . -name Makefile -exec rm -f {} \;
	rm -fr autom4te.cache
	rm -f missing aclocal.m4 config.h.in config.guess config.sub ltmain.sh install-sh configure depcomp
	rm -f test-driver

dist-hook:
	(cd $(srcdir) && git diff && echo ===== git log ==== && git log) > $(distdir)/ChangeLog
