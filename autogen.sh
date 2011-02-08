#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir

libtoolize -cf
aclocal -I m4 || exit 1
exec autoconf
