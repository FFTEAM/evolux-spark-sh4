
INCLUDES = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/system \
	-I$(top_srcdir)/src/zapit/include \
	-I$(top_srcdir)/src/zapit/include/client \
	-I$(top_srcdir)/lib/connection \
	-I$(top_srcdir)/lib/libeventserver \
	-I$(top_srcdir)/lib/libeplayer3

noinst_LIBRARIES = libcoolstream.a

libcoolstream_a_SOURCES = audio_cs.cpp dmx_cs.cpp playback_cs.cpp ringbuffer.c record_cs.cpp \
	video_cs.cpp

# dvb-ci
libcoolstream_a_SOURCES += dvb-ci.cpp \
	dvbci_session.cpp dvbci_mmi.cpp dvbci_camgr.cpp dvbci_appmgr.cpp dvbci_datetimemgr.cpp dvbci_resmgr.cpp



