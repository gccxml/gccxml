#!/bin/sh

cd `dirname $0`

#########################################
# Patch STL Headers (Non-STLport)
#########################################

CC_INCLUDES=`../find_flags "$@" | perl -ne '($a) = m|-I([/a-zA-Z0-9\._-]+/include/CC)|o ; print "$a\n" if $a'`

mkdir -p rw

cp $CC_INCLUDES/typeinfo .
cp $CC_INCLUDES/Cstd/istream .
cp $CC_INCLUDES/Cstd/ostream .
cp $CC_INCLUDES/Cstd/streambuf .
cp $CC_INCLUDES/Cstd/string .
cp $CC_INCLUDES/Cstd/algorithm .
cp $CC_INCLUDES/Cstd/rw/iterator rw/iterator

gpatch -p1 < Cstd.patch

#########################################
# Patch the standard headers
#########################################

if [ `uname -r` = "5.10" ]
then
  mkdir -p iso
  cp /usr/include/iso/stdlib_iso.h iso
  gpatch -p1 < std-5.10.patch
fi
