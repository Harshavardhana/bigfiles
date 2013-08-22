CLEANFILES = *~

plugin_LTLIBRARIES = gfapi.la
plugindir = $(libdir)/bigfiles/$(PACKAGE_VERSION)/plugins/glusterfs

gfapi_la_SOURCES = gfapi.c

gfapi_la_LDFLAGS = -module -avoid-version
gfapi_la_LIBADD = $(GFAPI_LIBS)

noinst_HEADERS = gfapi.h

AM_CFLAGS = -Wall $(GFAPI_CFLAGS)

uninstall-local:
	rm -f $(DESTDIR)$(plugindir)/gfapi.so
