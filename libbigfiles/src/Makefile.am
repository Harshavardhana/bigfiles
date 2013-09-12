CLEANFILES = *~

lib_LTLIBRARIES = libbigfiles.la
noinst_HEADERS = uri.h bigfiles-private.h
libbigfiles_HEADERS = bigfiles.h
libbigfilesdir = $(includedir)

libbigfiles_la_SOURCES = bigfiles.c uri.c driver.c bigfiles-private.c
libbigfiles_la_LDFLAGS = -version-info 1:0:0

AM_CPPFLAGS = $(BF_CPPFLAGS) -DDRIVERDIR=\"$(libdir)/bigfiles/$(PACKAGE_VERSION)/driver\"

AM_CFLAGS = -Wall $(BF_CFLAGS)
