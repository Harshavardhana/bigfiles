CLEANFILES = *~

lib_LTLIBRARIES = libbigobjects.la
noinst_HEADERS = uri.h bigobjects-private.h
libbigobjects_HEADERS = bigobjects.h
libbigobjectsdir = $(includedir)

libbigobjects_la_SOURCES = bigobjects.c uri.c driver.c bigobjects-private.c
libbigobjects_la_LDFLAGS = -version-info 1:0:0

AM_CPPFLAGS = $(BF_CPPFLAGS) -DDRIVERDIR=\"$(libdir)/bigobjects/$(PACKAGE_VERSION)/driver\"

AM_CFLAGS = -Wall $(BF_CFLAGS)
