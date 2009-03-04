
#
# Copyright (c) 1999-2003 LAAS/CNRS                   --  Thu Apr 15 1999
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

namespace eval modules {
    variable loaded
    variable aliases

    namespace export lm
    namespace export unlm

    proc lm { name args } {
	global tclserv_moduledir
	variable loaded
	variable aliases

	while {[llength $args] > 0} {
	    set arg [shift args]
	    if { "$arg" == "as" } {
		# want to load a module with a different name
		set alias [shift args]
	    } else {
		set path $arg
	    }
	}
	if { ![info exists path] } {
	    set path "$tclserv_moduledir"
	}
	if { ![info exists alias] } {
	    # use default module name
	    set alias "$name"
	}

	set list [array names loaded]
	if { [lsearch -exact $list $alias] > -1 } {
	    return [set ::[set aliases($alias)]::datadir]
	}

	# load the encode / decode procs
	if { [lsearch -exact $list $name] == -1 } {

            set temp $path
            foreach dir $temp {
		if {![regexp /lib/tclserv/*$ ${dir}]} {
		    lappend path "${dir}/lib/tclserv"
		}
            }

	    set found 0
	    foreach dir $path {
		set file "${dir}/lib${name}TclServ[info sharedlibextension]"
		if {![catch {set file [glob $file]} m]} {
		    load $file $name
		    set found 1
		    break
		} else {
		    server::log "$m"
		}
	    }

	    if {!$found} {
		error "cannot find $name in $path"
	    }
	    # -1 means 'code loaded, not connected and don't wanna connect'
	    set loaded($name) -1
	}

	# 0 means 'loaded but not yet connected and wanna connect'
	set loaded($alias) 0
	set aliases($alias) $name

	server::log "client code in [set ::${name}::datadir]"
	return [set ::${name}::datadir]
    }

    proc unlm { name } {
	variable loaded
	variable aliases

	catch {
	    if { [set loaded($name)] == 1 } {
		${name}::disconnect
	    }
	}
	catch {
	    unset loaded($name)
	    unset aliases($name)
	}
    }

    proc connect { } {
	variable loaded
	variable aliases

	foreach module [ array names loaded ] {

	    if { [set loaded($module)] == 0 } {
		set name [set aliases($module)]
		${name}::connect $module
		set loaded($module) 1
	    }
	}
    }

    proc disconnect { } {
	variable loaded

	foreach module [ array names loaded ] {

	    if { [set loaded($module)] == 1 } {
		${module}::disconnect
		set loaded($module) 0
	    }
	}
    }
}

namespace import modules::*

package provide tclserv::load 1.0
#eof
