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

use strict;
use File::stat;

#----------------------------------------------------------------------
# End of script for module generation
#----------------------------------------------------------------------

# mirror_dir source dest include exclude confirm

if ($installUserPart == 1) {
  # Install user files

  mirror_dir("codels", "../$codelsDir", "", "", 1);

  mirror_dir(".", "..", "^Makefile.in", "", 1);
  mirror_dir(".", "..", "^configure.ac.user", "", 1);
  mirror_dir("autoconf", "../$autoconfDir", "^local\.mk\.in", 1);
  mirror_dir(".", "..", "^acinclude.m4", "", 1);
} 

# Install server files
mirror_dir("server", "../$serverDir", "", "", 0);
mirror_dir("server/tcl", "../$tclDir", "", "", 0) if ($genTcl == 1); 
mirror_dir("server/openprs", "../$openprsDir", "", "", 0) if ($genOpenprs == 1);
mirror_dir("autoconf", "../$autoconfDir", "", "^(?:local\.mk\.in)", 0);
mirror_dir(".", "..", "^autogen", "", 0);
mirror_dir(".", "..", "^$module.pc.in", "", 0);


# Do +x on autogen
my $autogen = "../autogen";
my $autogen_mode = stat($autogen)->mode & 07777;

chmod ((0111|$autogen_mode), $autogen);

# Do not regenerate if one of server or codels is missing
if ( ! -d "../$serverDir" || ! -d "../$codelsDir" )
{
    print "
    One of server/ or codels/ is not generated 
    yet, I won't run autogen
    ";
    exit;
}


# Checks if configure is up to date
sub is_autoconf_fresh() {
    #  the configure script
    my $autoconf_dst = "../configure";
    #  its sources
    my @autoconf_src = 
    (
	"../autogen",
        "../$autoconfDir/configure.ac.begin",
        "../$autoconfDir/configure.ac.end",
        "../configure.ac.user", 
        "../acinclude.m4",
	"../$autoconfDir/robots.m4"
    );

    my $aconf_regen = 0;

    if ( -r $autoconf_dst)
    {
        my $configure = stat($autoconf_dst);
        foreach my $source (@autoconf_src)
        {
            next if ( ! -r $source );
            my $mtime = stat($source)->mtime;

            if ( $mtime > $configure->mtime )
            {
                $aconf_regen = 1;
                last;
            }
        }
    }
    else
    {
        $aconf_regen = 1;
    }

    return $aconf_regen;

}

my $aconf_regen = is_autoconf_fresh;
if ($aconf_regen)
{
    print "
    Some files related to the configure environment
    have changed. Running autogen\n";

    chdir "..";
    exec "./autogen";
}

