INCLUDES = \
	-I$(top_srcdir)/include -I$(top_srcdir)/lib/gdi/ati -I$(top_srcdir)/../../cdk/graphlcd-base

noinst_LIBRARIES = libenigma_gdi.a

libenigma_gdi_a_SOURCES = \
	region.cpp grc.cpp epng.cpp erect.cpp fb.cpp font.cpp font_arabic.cpp gfbdc.cpp  \
	glcddc.cpp gpixmap.cpp lcd.cpp gfont.cpp accel.cpp picload.cpp picexif.cpp \
	compositing.cpp bcm.cpp stmfb.cpp

if WITH_SDL
libenigma_gdi_a_SOURCES += sdl.cpp
endif

#	ati/2dablt.cpp ati/2dcore.cpp ati/ati_core.cpp ati/test.cpp ati/2dutil.cpp
