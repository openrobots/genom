
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
use File::Copy;
use File::Compare;

# add newline on print
# (we don't want to explicitely add '\n' on each print in the generated Perl code)
$\ = "\n";


sub move_if_change {
  local ($a, $b) = @_;
  if ( -f $b ) {
    if (compare($a, $b) == 0) {
      unlink $a;
    } else {
      move($a, $b);
    }
  } else {
    move($a, $b);
  }
}

sub mirror_dir {
  my ($srcdir, $dstdir, $include, $exclude, $mode) = @_;

  if (! -d "$dstdir") {
    mkdir "$dstdir", 04775;
  }

  my $found_something = 0;
  opendir(DIR, $srcdir);
  foreach(readdir(DIR)) {
    # skip unwanted files
    if ($exclude ne "") { if (/$exclude/) { next; } }
    if ($include ne "") { if (!/$include/) { next; } }
    next if ( ! -f "$srcdir/$_" );

    if (! -f "$dstdir/$_") {
      $found_something = 1;
      # file doesn't exist
      printf ("  creating $_\n");
      system("cp $srcdir/$_ $dstdir/$_");
    } elsif (system("cmp -s $srcdir/$_ $dstdir/$_") != 0) {
      $found_something = 1;
      # file exists and is different
      my $agree = 'n';
      if ($mode == $SKIP_IF_CHANGED) {
        printf ("  $_ changed, skipping\n");
        next;
      } elsif ($mode == $ASK_IF_CHANGED) {
	printf ("  $_ has changed, overwrite (y/n)? ");
	chomp($agree=<STDIN>);
        if ($agree eq 'y') {
          printf ("  saving $_ in $_.genomsave\n");
          system ("cp $dstdir/$_ $dstdir/$_.genomsave");
        }
      } else { 
        $agree = 'y';
        printf ("  overwriting $_ with new version\n");
      }

      if ($agree eq 'y') {
        system ("\\cp -f $srcdir/$_ $dstdir/$_");
      }
    }
  }
  closedir(DIR);

  return $found_something
}

# Creation du repertoire user
#
if (! -d "codels") {
    mkdir "codels", 04775;
}
else {
    chmod 04775, 'codels';
}

# Creation du repertoire configure
#
if (! -d "autoconf") {
    mkdir "autoconf", 04775;
}
else {
    chmod 04775, 'autoconf';
}

# Create server directory
#
if (! -d "server") {
    mkdir "server", 04775;
}
else {
    chmod 04775, 'server';
}

# Creation du repertoire tcl
#
if ($genTcl == 1) {
  if (! -d "$tclDir") {
    mkdir "$tclDir", 04775;
  }
  else {
    chmod 04775, "$tclDir";
  }
}

# Creation du repertoire openprs
#
if ($genOpenprs == 1) {
  if (! -d "$openprsDir") {
    mkdir "$openprsDir", 04775;
  }
  else {
    chmod 04775, "$openprsDir";
  }
}

# Creation du repertoire tclservClient
#
if ($genTclservClient == 1) {
  if (! -d "$tclservClientDir") {
    mkdir "$tclservClientDir", 04775;
  }
  else {
    chmod 04775, "$tclservClientDir";
  }
}

#---------------------------------------------------------------

