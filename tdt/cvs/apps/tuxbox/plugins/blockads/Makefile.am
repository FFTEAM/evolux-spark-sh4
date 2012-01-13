AM_CPPFLAGS =  \
	@FREETYPE_CFLAGS@ \
	-I$(top_srcdir)/include

blockads_LDADD = \
	@FREETYPE_LIBS@

bin_PROGRAMS = blockad

noinst_PROGRAMS = blockads

blockads_SOURCES = \
	blockads.c \
	gfx.c \
	io.c \
	text.c \
	globals.c \
	http.c

blockad_SOURCES = \
	blockad.c \
	globals.c \
	http.c

noinst_LTLIBRARIES = blockads.la

blockads_la_LDFLAGS = -rpath $(PLUGINDIR) -module -avoid-version

blockads_la_SOURCES = \
	start.c

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install blockads.la $(DESTDIR)$(PLUGINDIR)
	install -m 0755 $(srcdir)/blockad $(DESTDIR)$(bindir)
	install -m 0755 $(srcdir)/blockads $(DESTDIR)$(PLUGINDIR)/blockads.so
	install -m 0644 $(srcdir)/blockads.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644 $(srcdir)/blockads.conf $(DESTDIR)$(CONFIGDIR)
