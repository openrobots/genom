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
# End of script for module generation
#----------------------------------------------------------------------

# Copy configure script in main directory if it doesn't exist
if (! -e '../configure' || $installUserPart == 1) {
    if(system("cmp -s configure/configure ../configure") != 0) {
      system ("cp -i configure/configure ../configure");
    }
}

# Idem pour le script de demarrage de la tache d'essay */
if (! -e "../$module\_essay" || $installUserPart == 1) {
    if(system("cmp -s $module\_essay ../$module\_essay") != 0) {
	system ("cp -i $module\_essay ../");
	chmod 0777, "../$module\_essay";
    }
}

# Instale la partie codels
if ($installUserPart == 1) {

    # 1ere lettre en majuscule
    $module = ucfirst($module);
    $startFile = "start$module";
    $userDir = "../$codelsDir";

    if (! -e "../$startFile") {
	system (cp, "user/$startFile", '../');
    }

    if (! -d $userDir) {
	print "Creating directory $codelsDir";
	mkdir $userDir, 04775;
	$newdir = 1;
    }
    else {
	$newdir = 0;
    }

    # On recupere chaque fichier de auto/user
    opendir(USER, "user");
    foreach(readdir(USER)) {
	if (/\.c$/ || (/^Makefile/ && !/Makefile.all/)) {
	    # Il n'existe pas encore
	    if (! -f "$userDir/$_") {
		$agree = 'y';
		if(!$newdir) {
		    printf "Agree to create file $userDir/$_ (y/n) ? ";
		    chomp($agree=<STDIN>);
		}
		if ($agree eq 'y') {
		    system ("cp user/$_ $userDir");
		}
	    }
	    # Il existe deja
	    elsif (system("cmp -s user/$_ $userDir/$_") != 0) {
		printf ("overwrite $userDir/$_ (y/n) ? ");
		chomp($agree=<STDIN>);
		if ($agree eq 'y') {
		    printf ("save $userDir/$_ in $userDir/$_.genomsave\n");
		    system ("cp $userDir/$_ $userDir/$_.genomsave");
		    system ("cp  user/$_ $userDir");
		}
	    }
	}
    }
    closedir(USER);
    print "$codelsDir/ is installed, from now just call \"gnumake\" !";
}

