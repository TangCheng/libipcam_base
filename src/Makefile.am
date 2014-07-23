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
	message.c \
	notice_message.c \
	request_message.c \
	response_message.c \
	message_manager.c \
	timer_manager.c \
	base_service.c \
	socket_manager.c \
	service.c \
	timer_pump.c \
	config_manager.c \
	action_handler.c \
	event_handler.c \
	base_app.c

libipcam_base_la_LDFLAGS = 

libipcam_base_la_LIBADD = $(LIBIPCAM_BASE_LIBS)

ipcam_base_includedir = $(includedir)/libipcam_base-0.1.0
ipcam_base_include_HEADERS = \
	libipcam_base.h \
	message.h \
	notice_message.h \
	request_message.h \
	response_message.h \
	messages.h \
	message_manager.h \
	timer_manager.h \
	base_service.h \
	socket_manager.h \
	service.h \
	timer_pump.h \
	config_manager.h \
	action_handler.h \
	event_handler.h \
	base_app.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libipcam_base-0.1.0.pc

EXTRA_DIST = \
	libipcam_base-0.1.0.pc.in

bin_PROGRAMS =  \
	timer

timer_DEPENDENCIES = libipcam_base.la
timer_SOURCES =  \
	timer.c

timer_LDFLAGS =

timer_LDADD = $(LIBIPCAM_BASE_LIBS) -lipcam_base
