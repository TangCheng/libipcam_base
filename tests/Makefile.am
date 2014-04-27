## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(LIBIPCAM_BASE_CFLAGS)

LDADD = $(top_builddir)/src/libipcam_base.la \
	$(LIBIPCAM_BASE_LIBS)

noinst_PROGRAMS =  \
	test_service \
	test_timer_pump \
	test_notice_message \
	test_request_message 

test_service_SOURCES =  \
	test_service.c

test_timer_pump_SOURCES = \
	test_timer_pump.c

test_notice_message_SOURCES =  \
	test_notice_message.c

test_request_message_SOURCES =  \
	test_request_message.c
