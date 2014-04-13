## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	tests 

libipcam_basedocdir = ${prefix}/doc/libipcam_base
libipcam_basedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(libipcam_basedoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libipcam_basedocdir)
