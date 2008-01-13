#!/bin/sh
[ -e src/Makefile ] && rm src/Makefile
rm Makefile
qmake-qt4
#cd src
#qmake-qt4
#cd ..
make
