CLEANFILES = *~

adapters_LTLIBRARIES = gluster.la
adaptersdir = $(libdir)/bigfiles/$(PACKAGE_VERSION)/adapters/

gluster_la_SOURCES = gluster.c

gluster_la_LDFLAGS = -module -avoid-version
gluster_la_LIBADD = $(GFAPI_LIBS)

noinst_HEADERS = gluster.h

AM_CPPFLAGS =  -I$(top_srcdir)/libbigfiles/src
AM_CFLAGS = -Wall $(GFAPI_CFLAGS)

uninstall-local:
	rm -f $(DESTDIR)$(adaptersdir)/gluster.so
