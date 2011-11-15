#! /bin/sh
CURDIR=`pwd`
PINGUDIR=${CURDIR%/cvs/cdk}
make clean
rm .deps/*neutrino*
find "$PINGUDIR"/cvs/apps/neutrino -name "*.deps" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "*.libs" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "*.o" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "*.a" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "*.la" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "*.lo" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "Makefile" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "Makefile.in" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "config.*" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "autom4te.cache" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "configure" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "stamp-h1" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "missing" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "configure" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "INSTALL" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "install-sh" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "libtool" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "ltmain.sh" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "depcomp" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "COPYING" -exec rm -rf {} \;
find "$PINGUDIR"/cvs/apps/neutrino -name "aclocal.m4" -exec rm -rf {} \;


#find "$PINGUDIR"/cvs/apps/neutrino -name "*.deps" -a "*.libs" "*.o" "*.a" "*.la" "Makefile" -exec rm -rf {} \;
