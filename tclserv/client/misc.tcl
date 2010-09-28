
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

# Miscelaneous noise...

# Generic function to scan request's args
proc getopt { varName blockVar rawVar docVar } {
    upvar $varName l $blockVar block $rawVar raw $docVar doc

    set more 1
    while { $more } {
	set more 0

	if { "[lindex $l 0]" == "-ack" } {
	    shift l
	    set block 0
	    set more 1
	}

	if { "[lindex $l 0]" == "-raw" } {
	    shift l
	    set raw 1
	    set more 1
	}

	if { "[lindex $l 0]" == "-doc" } {
	    shift l
	    set doc 1
	    set more 1
	}
    }
}

proc getOptions { argList listeOptions } {
    upvar 1 $argList list
    while 1 {
	set arg [lindex $list 0]
	set find 0
	foreach el $listeOptions {
	    if { $el == $arg } {
		upvar 1 $el option
		set find 1
		set list [lrange $list 1 end]
		set option 1
		break
	    }
	}
	if { $find == 0 } {
	    break
	}
    }
}


# -----------------------------------------------------------------------
# Generic function to format a request's output

if { [lsearch [interp hidden] rqstFormatOuput] >= 0 } {
    interp expose {} rqstFormatOuput
}

proc ::rqstFormatOuput { format outVar listVar array } {
    upvar $listVar list $outVar out
    
    if { "$array" == "" } {
	set prefix ""
    } else {
	set prefix "${array}."
    }

    foreach {dims members} $format {
	if { [llength $dims] == 0 } {
	    if { [llength $members] == 1 } {
		# basic member
		lappend out "${prefix}$members = [shift list]"
	    } else {
		# structure
		interp invokehidden {} rqstFormatOuput	\
		    [lrange $members 1 end] out list	\
		    ${prefix}[lindex $members 0]
	    }
	} else {
	    set nElts 1
	    foreach dim $dims { set nElts [expr $nElts * $dim] }

	    for {set i 0} { $i < $nElts} { incr i } {
		if { [llength $members] == 1 } {
		    # basic member
		    lappend out \
			"${prefix}$members[interp invokehidden {} indexofarray $dims $i] = [shift list]"
		} else {
		    # structure
		    interp invokehidden {} rqstFormatOuput	\
			[lrange $members 1 end] out list	\
			"${prefix}[lindex $members 0][interp invokehidden {} indexofarray $dims $i]"
		}
	    }
	}
    }
}

interp hide {} rqstFormatOuput


# -----------------------------------------------------------------------
# Returns a C-like string address of an array given the element number and
# the list of dimensions

if { [lsearch [interp hidden] indexofarray] >= 0 } {
    interp expose {} indexofarray
}

proc ::indexofarray { dims i } {
    set l [llength $dims]
    if { $l == 0 } return ""

    set dim [lindex $dims end]
    set local [expr $i % $dim]
    set rest [expr ($i - $local) / $dim]
    return "[interp invokehidden {} indexofarray [lrange $dims 0 [expr $l-2]] $rest]\[$local\]"
}

interp hide {} indexofarray

package provide genom
#eof
