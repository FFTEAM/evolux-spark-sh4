AC_INIT(tuxbox-plugins,0.0.1)
AM_INIT_AUTOMAKE(tuxbox-plugins,0.0.1)

TUXBOX_APPS
TUXBOX_APPS_DIRECTORY

AC_PROG_CC
AC_PROG_CXX
AC_DISABLE_STATIC
AM_PROG_LIBTOOL

TUXBOX_BOXTYPE

TUXBOX_APPS_DVB
#TUXBOX_APPS_DRIVER

# needed by enigma/movieplayer
TUXBOX_APPS_LIB_CONFIG(CURL,curl-config)

# needed by tuxmail
TUXBOX_APPS_LIB_CONFIG(FREETYPE,freetype-config)
#TUXBOX_APPS_LIB_CONFIG(FREETYPE,freetype-old-config)

TUXBOX_APPS_LIB_PKGCONFIG(PNG,libpng)

# needed by enigma-demo
#TUXBOX_APPS_LIB_PKGCONFIG(XMLTREE,tuxbox-xmltree)

# needed by vncviewer
#TUXBOX_APPS_LIB_PKGCONFIG(CONFIGFILE,tuxbox-configfile)

# needed by sysinfo
#TUXBOX_APPS_LIB_PKGCONFIG(MD5SUM,tuxbox-md5sum)
#TUXBOX_APPS_LIB_PKGCONFIG(UCODES,tuxbox-ucodes)

# needed by tuxtxt
#TUXBOX_APPS_LIB_PKGCONFIG(TUXTXT,tuxbox-tuxtxt)

AC_SUBST(LIBCS_CFLAGS)
AC_SUBST(LIBCS_LIBS)
AC_SUBST(FREETYPE_CFLAGS)
AC_SUBST(FREETYPE_LIBS)

AC_OUTPUT([
Makefile
include/Makefile
blockads/Makefile
pluginx/Makefile
aio-grab/Makefile
tuxbox-plugins.pc
])
