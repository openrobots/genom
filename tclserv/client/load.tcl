#	$LAAS$

#
# Copyright (c) 1999-2003 LAAS/CNRS
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


# Interface to load modules

# lm can be called with many syntaxes:
# lm <module> [as <alias>] [on <server>] [rpath <remotepath>] [lpath <path>]
#
# <alias> defaults to <module>
# <server> defaults to the last server we connected

proc lm { name args } {

    # scan args
    set alias $name
    set fullname "\"$name\""
    set remotepath ""
    set server [::server::defaultServer]
    while {[llength $args] > 0} {
	set arg [shift args]
	if { "$arg" == "as" } {
	    # want to load a module with a different name
	    set alias [shift args]
	    set fullname "\"$name\" as \"$alias\""
	
	} elseif { "$arg" == "on" } {
	    set server [shift args]
	} elseif { "$arg" == "rpath" } {
	    set remotepath "\"[shift args]\""
	} elseif { "$arg" == "lpath" } {
	    set localpath "[shift args]"
	} else {
	    error {lm <module> [as <alias>] [on <server>] [rpath <remotepath>] [lpath <path>]}
	}
    }

    # load client code on remote side
    if {![::server::chat $server \
	      "LM $fullname $remotepath" "^OK (.*)" r path]} {
	error "cannot load $fullname on ${server}: $r"
    }

    if { ![info exists localpath] } {
	set localpath $path
    }

    if { [ file readable ${localpath}/${name}Client.tcl] } {
	source ${localpath}/${name}Client.tcl
	interp invoke {} ${name}Install $alias
    } elseif { [ file readable ${localpath}/share/module/${name}Client.tcl] } {
	source ${localpath}/share/module/${name}Client.tcl
	interp invoke {} ${name}Install $alias
    } else {
	error "cannot find client code in ${localpath}/${name}Client.tcl"
    }

    set ::cs::modules($alias) $name
    set ::cs::servers($alias) $server

    return "$alias loaded on $server from $localpath"
}

package provide genom
#eof
