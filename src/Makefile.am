## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBIPCAM_BASE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libipcam_base.la


libipcam_base_la_SOURCES = \
	utils.c \
	message.c \
	notice_message.c \
	request_message.c \
	response_message.c \
	message_manager.c \
	base_service.c \
	socket_manager.c \
	service.c \
	base_app.c \
	app.c

libipcam_base_la_LDFLAGS = 

libipcam_base_la_LIBADD = $(LIBIPCAM_BASE_LIBS)

include_HEADERS = \
	libipcam_base.h \
	message.h \
	notice_message.h \
	request_message.h \
	response_message.h \
	message_manager.h \
	base_service.h \
	socket_manager.h \
	service.h \
	base_app.h \
	app.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libipcam_base-0.1.0.pc.in

EXTRA_DIST = \
	libipcam_base-0.1.0.pc.in
