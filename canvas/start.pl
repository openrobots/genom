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


# add newline on print
$\ = "\n";

# Definition de $MAKE
# (utilisation de gnumake car problemes de compatibilite de make avec gnumake)
$MAKE = "gnumake";

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

# Creation du repertoire user
#
if (! -d "user") {
    print "Creating directory user";
    mkdir "user", 04775;
}
else {
    chmod 04775, 'user';
}

# Creation du repertoire configure
#
if (! -d "configure") {
    print "Creating directory configure";
    mkdir "configure", 04775;
}
else {
    chmod 04775, 'configure';
}

# Creation du repertoire propice
#
if ($genPropice == 1) {
  if (! -d "propice") {
    print "Creating directory propice";
    mkdir "propice", 04775;
  }
  else {
    chmod 04775, 'propice';
  }
}

# Creation du repertoire spy
#
if ($genSpy == 1) {
  if (! -d "spy") {
    print "Creating directory spy";
    mkdir "spy", 04775;
  }
  else {
      chmod 04775, 'spy';
  }
}


#---------------------------------------------------------------

