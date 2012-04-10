installdir = $(pkglibdir)/python/Plugins/Extensions

SUBDIRS = PicturePlayer MediaScanner MediaPlayer GraphMultiEPG SocketMMI AutomaticVolumeAdjustment EPGRefresh PinguExplorer PPDisplayTime

if ENABLE_TF7700
SUBDIRS += TopfieldVFD
endif

if ENABLE_CUBEREVO
SUBDIRS += CuberevoVFD
endif

if HAVE_LIBDDVD
SUBDIRS += DVDPlayer
endif

install_PYTHON =	\
	__init__.py
