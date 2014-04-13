## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(LIBIPCAM_BASE_CFLAGS)

LDADD = $(top_builddir)/src/libipcam_base.la \
	$(LIBIPCAM_BASE_LIBS)

noinst_PROGRAMS =  \
	test_service

test_service_SOURCES =  \
	test_service.c


