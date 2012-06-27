installdir = $(DATADIR)/neutrino/locale

install_DATA = \
	bayrisch.locale \
	bosanski.locale \
	ch-baslerdeutsch.locale \
	ch-berndeutsch.locale \
	czech.locale \
	deutsch.locale \
	english.locale \
	francais.locale \
	italiano.locale \
	nederlands.locale \
	polski.locale \
	portugues.locale \
	russkij.locale \
	slovak.locale \
	suomi.locale \
	svenska.locale

install_DATA += norsk.locale

#if MAINTAINER_MODE

all: ordercheck ../../src/system/locals.h ../../src/system/locals_intern.h

locals: ordercheck locals.h locals_intern.h

ordercheck: deutsch.locale
	@cut -d' ' -f1 deutsch.locale | LC_ALL=C sort | uniq > /tmp/log
	@cut -d' ' -f1 deutsch.locale | uniq  | diff -u - /tmp/log || \
		(echo "ERROR: deutsch.locale not ordered or contains empty lines" && false)

locals.h: deutsch.locale
	@cut -d' ' -f1 deutsch.locale | LC_ALL=C sort | uniq | tr [:lower:] [:upper:] | tr \. \_  | tr \- \_ | tr -d \? | ./create.locals.h

locals_intern.h: deutsch.locale
	@cut -d' ' -f1 deutsch.locale | LC_ALL=C sort | uniq | ./create.locals_intern.h

check: locals.h locals_intern.h
	diff -u locals.h ../../src/system
	diff -u locals_intern.h ../../src/system


../../src/system/locals.h: locals.h
	cp locals.h ../../src/system/
	@echo "Consider committing .../neutrino/src/system/locals.h"

../../src/system/locals_intern.h: locals_intern.h
	cp locals_intern.h ../../src/system/
	@echo "Consider committing .../neutrino/src/system/locals_intern.h"

install-locals: locals.h locals_intern.h
	cp locals.h locals_intern.h ../../src/system
	@echo "Consider committing .../neutrino/src/system/[locals.h,locals_intern.h]"

locals-clean:
	rm -f locals.h locals_intern.h

#endif
