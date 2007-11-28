#!/bin/sh

INCLUDES=/usr/include
CC_INCLUDES=`find_flags | perl -ne '($a) = m|-I([/a-zA-Z0-9_-]+/include/CC)|o ; print "$a\n" if $a'`

cd 5.8

cp $INCLUDES/math.h .
cp $CC_INCLUDES/typeinfo .
cp $CC_INCLUDES/Cstd/istream .
cp $CC_INCLUDES/Cstd/streambuf .
cp $CC_INCLUDES/Cstd/string .

mkdir -p rw
mkdir -p sys
mkdir -p iso

cp $CC_INCLUDES/Cstd/rw/iterator rw/iterator
cp $INCLUDES/iso/stdio_iso.h iso
cp $INCLUDES/iso/stdlib_iso.h iso

cp $INCLUDES/sys/regset.h sys

patch -p1 < adaptation.patch
