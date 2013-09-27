CLEANFILES = *~

drivers_LTLIBRARIES = gluster.la
driversdir = $(libdir)/bigobjects/$(PACKAGE_VERSION)/drivers/

gluster_la_SOURCES = gluster.c

gluster_la_LDFLAGS = -module -avoid-version
gluster_la_LIBADD = $(GFAPI_LIBS)

noinst_HEADERS = gluster.h

AM_CPPFLAGS =  -I$(top_srcdir)/libbigobjects/src
AM_CFLAGS = -Wall $(GFAPI_CFLAGS)

uninstall-local:
	rm -f $(DESTDIR)$(driversdir)/gluster.so
