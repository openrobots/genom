
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
my $user_mode = ($installUserPart ? $ASK_IF_CHANGED : $SKIP_IF_CHANGED);

my $found_something = 0;

print "\nUpdating top directory";
mirror_dir(".", "..", "^(?:configure.ac.user|acinclude.user.m4|local.mk.in)", "", $user_mode);
print "" if (mirror_dir(".", "..", "", "^(?:configure.ac.user|acinclude.user.m4|local.mk.in|\\w+\\.pl)", $OVERWRITE));

print "Updating $codelsDir";
print "" if (mirror_dir("codels", "../$codelsDir", "", "", $user_mode));

print "Updating $autoconfDir";
print "" if (mirror_dir("autoconf", "../$autoconfDir", "", "",$OVERWRITE));

print "Updating $serverDir";
print "" if (mirror_dir("server", "../$serverDir", "", "",$OVERWRITE));

if ($genTcl == 1)
{
    print "Updating $tclDir";
    print "" if (mirror_dir("server/tcl", "../$tclDir", "", "",$OVERWRITE));
}

if ($genOpenprs == 1)
{
    print "Updating $openprsDir";
    print "" if (mirror_dir("server/openprs", "../$openprsDir", "", "",$OVERWRITE));
}

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
# XXXX miss also the files *.pc from required packages
sub is_autoconf_fresh() {
    #  the configure script
    my $autoconf_dst = "../configure";
    #  its sources
    my @autoconf_src =
    (
	"../autogen",
        "../$autoconfDir/configure.ac",
        "../configure.ac.user",
        "../acinclude.user.m4",
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

