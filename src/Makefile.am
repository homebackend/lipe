# Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AM_CPPFLAGS		=	-DSYSTEM_DATA_DIR=\""$(datadir)"\"
LDADD			=	-lcurses

if WITH_CURSES
LIPE_CURSES		=	lipecurse.c \
				lipecurse.h
endif

if WITH_GTK2
LIPE_GTK2		=	lipegtk.c \
				lipegtk.h \
				gpl.h
endif

sbin_PROGRAMS		=	lipe
lipe_SOURCES		=	\
				common.c \
				common.h \
				token.c \
				token.h \
				i386types.h \
				lipe.c \
				lipe.h $(LIPE_CURSES) $(LIPE_GTK2)

man_MANS		=	lipe.8
lipepixmapdir		=	$(datadir)/pixmaps
lipepixmap_DATA		=	lipe.xpm
appsdir			=	$(datadir)/applications
apps_DATA		=	lipe.desktop
EXTRA_DIST		=	lipecurse.c \
				lipecurse.h \
				lipegtk.c \
				lipegtk.h \
				gpl.h \
				lipe.xpm \
				$(man_MANS) $(apps_DATA)

#BUILT_SOURCES		=	manpage


#manpage : 
#	@${ECHO} "Making manpage"
#	${SED} "s%lipe%${BINARY}%g" lipe.8 | ${SED} "s%LiPE%${LIPE_PROG_NAME}%g" > __${BINARY}.8

