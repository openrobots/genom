#	$LAAS$

#
# Copyright (c) 1993-2003 LAAS/CNRS
# All rights reserved.
#
# Redistribution  and  use in source   and binary forms,  with or without
# modification, are permitted provided that  the following conditions are
# met:
#
#   1. Redistributions  of  source code must  retain  the above copyright
#      notice, this list of conditions and the following disclaimer.
#   2. Redistributions in binary form must  reproduce the above copyright
#      notice,  this list of  conditions and  the following disclaimer in
#      the  documentation   and/or  other  materials   provided with  the
#      distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
# SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
# BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#----------------------------------------------------------------------
# Debut du script de generation des sources d'un module genom
#----------------------------------------------------------------------

use strict;

# add newline on print
$\ = "\n";

# Definition de $MAKE
# (utilisation de gnumake car problemes de compatibilite de make avec gnumake)
my $MAKE = "gnumake";

# move_if_change
sub move_if_change {
    local ($a, $b) = @_;
    if ( -f $b ) {
        if (system("cmp -s $a $b") == 0) {
            unlink $a;
        } else {
            print "$b changed";
            system("mv $a $b");
        }
    } else {
        print "$b created";
        system("mv $a $b");
    }
}

sub mirror_dir {
  my ($srcdir, $dstdir, $include, $exclude, $confirm) = @_;

  if (! -d "$dstdir") {
    my $agree='y';
    if ($confirm) {
      printf ("create $dstdir (y/n)? ");
      chomp($agree=<STDIN>);
    }
    if ($agree eq 'y') {
      mkdir "$dstdir", 04775;
    } else { return; }
  }

  opendir(DIR, $srcdir);
  foreach(readdir(DIR)) {
    # skip unwanted files
    if ($exclude ne "") { if (/$exclude/) { next; } }
    if ($include ne "") { if (!/$include/) { next; } }
    if (! -f "$srcdir/$_") { next; }

    if (! -f "$dstdir/$_") {
      # file doesn't exist
      system("cp $srcdir/$_ $dstdir/$_");

    } elsif (system("cmp -s $srcdir/$_ $dstdir/$_") != 0) {
      # file exists and is different
      if ($confirm) {
	printf ("overwrite $dstdir/$_ (y/n)? ");
	my $agree;
	chomp($agree=<STDIN>);
	if ($agree eq 'y') {
	  printf ("save $dstdir/$_ in $dstdir/$_.genomsave\n");
	  system ("cp $dstdir/$_ $dstdir/$_.genomsave");
	} else {
	  printf ("keeping $dstdir/$_\n");
	  next;
	}
      }
      system ("'cp' -f $srcdir/$_ $dstdir/$_");
    }
  }
  closedir(DIR);
}

# Creation du repertoire user
#
if (! -d "codels") {
    print "directory codels created";
    mkdir "codels", 04775;
}
else {
    chmod 04775, 'codels';
}

# Creation du repertoire configure
#
if (! -d "autoconf") {
    print "directory autoconf created";
    mkdir "autoconf", 04775;
}
else {
    chmod 04775, 'autoconf';
}

# Create server directory
#
if (! -d "server") {
    print "directory server created";
    mkdir "server", 04775;
}
else {
    chmod 04775, 'server';
}

# Creation du repertoire propice
#
if ($genPropice == 1) {
  if (! -d "propice") {
    print "directory propice created";
    mkdir "propice", 04775;
  }
  else {
    chmod 04775, 'propice';
  }
}

#---------------------------------------------------------------

