#!/bin/sh
#
# set -x
DIR=`pwd`
E2DIR=/devel/E2/922_rev1091public
cd $E2DIR

rm -rf ../BACKUP/*
find ./usr/lib/enigma2/python | egrep '\.pyo$|\.py$' > /tmp/pyweg
for i in `cat /tmp/pyweg`; do echo $i; mv $i ../BACKUP ; done
echo "enigma2 done"
find ./usr/lib/python2.6 | egrep '\.pyo$|\.py$' > /tmp/pyweg
for i in `cat /tmp/pyweg`; do echo $i; mv $i ../BACKUP ; done
cp /devel/E2/BACKUP/mytest.py $E2DIR/usr/lib/enigma2/python
echo "python2.6 done"
find ./ | egrep '\*~$|\.own$|\.old$' > /tmp/pyweg
for i in `cat /tmp/pyweg`; do echo $i; mv $i ../BACKUP ; done
cp /devel/E2/BACKUP/swapsize.old $E2DIR/etc
echo "Cleanup done"
exit

