#!/bin/sh

cd `dirname $0`

#########################################
# Patch C++ Compiler Includes
#########################################

XLC_INCLUDES=`../find_flags "$@" | perl -ne '($a) = m|-I([/a-zA-Z0-9\._-]+)|o ; print "$a\n" if $a'`

for f in $XLC_INCLUDES/*
do
  b=`basename $f`

  sed \
  -e 's/\(extern[[:space:]]\{1,\}\)"builtin"/\1"C"/g' \
  -e 's/^\([[:space:]]\{1,\}\)??=/\1#/g' \
  -e 's/__IBMCPP__ < 400/!defined(__GCCXML__)/g' \
  -e 's/IBM VisualAge C++ v4 and later compilers/GCCXML/g' \
  -e '/#pragma[[:space:]]\{1,\}implementation/d' \
  $f > $b.tmp

  if ! cmp -s $f $b.tmp
  then
    echo "modifying $b"
    mv $b.tmp $b
  else
    rm $b.tmp
  fi

done

patch -s -i stdlib.h.patch
patch -s -i xstring.patch
