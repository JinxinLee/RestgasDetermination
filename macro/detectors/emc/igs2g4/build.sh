#!/bin/bash

clear
make clean
rm -rf .deps
rm -rf autom4te.cache
rm -rf bin/igs2g4.exe
rm -f Makefile
rm -f Makefile.in
rm -f aclocal.m4
rm -f config.h
rm -f config.h.in
rm -f config.log
rm -f config.status
rm -f configure
rm -f depcomp
rm -f install-sh
rm -f missing
rm -f stamp-h1
aclocal && autoconf && autoheader && automake --add-missing &&
./configure --prefix=/home/cat/work/igs2g4 &&
make && make install && 
#if [ -f test.txt ] ; then rm -f test.txt ; fi &&
time ./bin/igs2g4.exe
# > test.txt
#make distcheck
