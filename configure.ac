# Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

dnl Process this file with autoconf to produce a configure script.
AC_INIT([LiPE],[1.13],[Neeraj Jakhar <neerajcd@gmail.com>],[lipe])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_PREFIX_DEFAULT(/usr)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

#LF_CONFIGURE_CC
#LF_CONFIGURE_CXX
#LF_HOST_TYPE
#LF_SET_WARNINGS
#AC_PROG_RANLIB


AC_ARG_ENABLE(ncurses,[  --disable-ncurses       disables ncurses interface (Default: No)],[
	disablencurses=no
	])
AC_ARG_ENABLE(gtk2,[  --disable-gtk2          disables gtk2 interface (Default: No)],[
	disablegtk2=no
	])
AC_ARG_WITH(ncurses,[  --with-ncurses          directory containing ncurses library (may be autodetected)],ncurses="$withval",ncurses="")
#AC_ARG_WITH(gtk2,[  --with-gtk2             directory containing gtk2 library (may be autodetected)],gtk2=$withval)

horl_not_found()
{
	if test "$1" = "header"
	then
		echo "Error! header file $2 not found."
	else
		if test "$1" = "lib"
		then
			echo "Error! $2 library not found."
		else
			echo "horl_not_found(): Error! error in package configuration."
			exit 1
		fi
	fi
	if test -n "$3"
	then
		echo "May be you need $3 package installed."
	fi
	if test -n "$4"
	then
		echo "For more information visit $3."
	fi
	exit -1
}

m4_warn([obsolete],
[The preprocessor macro 'STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  C89 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS(sys/types.h sys/stat.h fcntl.h unistd.h sys/unistd.h syscall.h errno.h,[],[
	echo "Error! One or more required header files are absent."
	exit -1
	])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
m4_warn([obsolete],
[your code may safely assume C89 semantics that RETSIGTYPE is void.
Remove this warning and the 'AC_CACHE_CHECK' when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    ('int' or 'void').])


#for i in sys/types.h sys/stat.h fcntl.h unistd.h sys/unistd.h syscall.h errno.h
#do
#	AC_CHECK_HEADERS($i,[],[
#		horl_not_found header "$i"
#		])
#done

#AC_CHECK_HEADERS(sys/types.h,[],[])

if test -z "$disablencurses"
then
	if test -n "$ncurses"
	then
		LIBS="$LIBS -L$ncurses"
	fi
	header_file_checking=curses.h
	AC_CHECK_HEADER($header_file_checking,[],[
		horl_not_found header "$header_file_checking" "ncurses" "http://www.gnu.org/software/ncurses/ncurses.html"
		])
	lib_checking=curses
	AC_CHECK_LIB($lib_checking,main,[],[
		horl_not_found lib curses "$lib_checking" "http://www.gnu.org/software/ncurses/ncurses.html"
		])
	AC_DEFINE(USE_CURSES,1,[Use ncurses library])
	enablencurses=yes
else
	enablencurses=no
fi

if test "x$GCC" = "xyes"
then
	CFLAGS="$CFLAGS -Wall"
fi

if test -z "$disablegtk2"
then
	AM_PATH_GLIB_2_0(2.6.0,,AC_MSG_ERROR([
*** GLib 2.6 is required to build LiPE; please make sure you have the GLib
*** development headers installed. The latest version of GLib is
*** always available at http://www.gtk.org/.]))
	AM_PATH_GTK_2_0(2.6.0,,AC_MSG_ERROR([
*** GTK+ 2.6 is required to build LiPE; please make sure you have the GTK+
*** development headers installed. The latest version of GTK+ is
*** always available at http://www.gtk.org/.]))
	CFLAGS="$CFLAGS `pkg-config --cflags gtk+-2.0`"
	LIBS="$LIBS `pkg-config --libs gtk+-2.0`"
	AC_DEFINE(USE_GTK2,1,[Use gtk2 library])
	enablegtk2=yes
	AC_PATH_XTRA
else
	enablegtk2=no
fi

AM_CONDITIONAL(WITH_CURSES,test x$enablencurses = xyes)
AM_CONDITIONAL(WITH_GTK2,test x$enablegtk2 = xyes)

AC_DEFINE(PACKAGE_MAIL,"neerajcd@gmail.com",[Mail address])
AC_DEFINE(PACKAGE_URL,"http://sourceforge.net/projects/ptedit",[HTTP URL])

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   src/Makefile
])

AC_OUTPUT

echo

echo
echo -n $PACKAGE
echo    " ............................. : $VERSION"
echo Build with ncurses support ....... : $enablencurses
echo Build with gtk2 support .......... : $enablegtk2
echo
eval eval echo lipe will be installed in $sbindir.

echo
echo configure complete, now type \'make\'

