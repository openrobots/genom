#	$LAAS$

#
# Copyright (c) 1999-2003 LAAS/CNRS                   --  Fri Apr  2 1999
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

namespace eval cs {

    variable csLibInit 0
    variable mbox
    variable activity

    # Create csLib mbox -------------------------------------------------

    proc init { } {
	variable csLibInit

	set size 0
	foreach module [array names ::modules::loaded] {
	    catch {
		set modSize [set ::${module}::mboxSize]
		if { $modSize > $size } { set size $modSize }
	    }
	}
	if { $size <=0 } { 
	    error "no modules loaded. Please load at least one module"
	}

	if { $csLibInit > 0 } {
	    if { $size > $csLibInit } {
		error "mbox already exists and is too small"
	    } else {
		# ok
		return "mbox size is $csLibInit"
	    }
	}

	::cs::mboxInit tclServ$::port 0 $size
	set csLibInit $size
	return "mbox size is $csLibInit"
    }

    # Delete csLib mbox -------------------------------------------------

    proc exit { } {
	variable csLibInit

	if { $csLibInit == 0 } {
	    error "no mailbox to delete"
	}

	::cs::mboxEnd
	set csLibInit 0
    }

    # Send a request ----------------------------------------------------

    proc rqstSend { client cmd args } {
	variable mbox
	
	set s [::cs::mboxEvent]
	::cs::mboxEvent ""

	set r [ catch { uplevel \#0 ${cmd}Send [join $args] } m ]
	if { $r != 0 } { return "ERROR $r $m" }

	set status [ lindex $m 0 ]
	if { "$status" != "SENT" } { return "ERROR 0 $m" }

	set id [::cs::newTclRqstId]
	set mbox($id) "$cmd $client [lindex $m 1] SENT"

	::cs::mboxEvent $s
	# we might have missed events
	eval $s

	return "OK $id"
    }

    # Send a service ----------------------------------------------------

    proc servSend { client cmd args } {
	variable mbox
	
	# no need to protect this section against a premature ACK like
	# above since it cannot happen.

	set r [ catch { eval ::service::servSend $cmd [join $args] } m ]
	if { $r != 0 } { return "ERROR $r $m" }

	set status [ lindex $m 0 ]
	if { "$status" != "SENT" } { return "ERROR 0 $m" }

	set id [::cs::newTclRqstId]
	set mbox($id) "service::serv $client [lindex $m 1] SENT"

	return "OK $id"
    }


    # Recieve every pending reply (csLib) -------------------------------

    proc repliesRcv { args } {
	variable mbox
	variable activity

	if { [llength $args] == 0 } {
	    set args [ array names mbox ]
	}

	foreach id $args {
	    set rqstInfo [ set mbox($id) ]

	    if { [lindex $rqstInfo 3 ] != "TERM" } {
		set rqstName [ lindex $rqstInfo 0 ]
		set rqstClnt [ lindex $rqstInfo 1 ]
		set rqstId   [ lindex $rqstInfo 2 ]

		if { [ catch { ${rqstName}Rcv $rqstId } r ] != 0 } {
		    set reply "$rqstName $rqstClnt $rqstId ERROR $r"
		    set msg "$id $rqstName ERROR $r"
		} else {
		    set reply "$rqstName $rqstClnt $rqstId $r"
		    set msg "$id $rqstName $r"
		}

		if { [ lindex $rqstInfo 3 ] != [ lindex $reply 3 ] } {
		    set mbox($id) $reply
		    if { "[lindex $r 0]" == "ACK" } {
			set activity($id) [lindex $r 1]
		    } elseif { "[lindex $r 0]" == "TERM" } {
			catch { unset activity($id) }
		    }
		    ::server::replyTo $rqstClnt $msg
		}
	    }
	}
    }


    # Find a new tclRqstId ----------------------------------------------

    proc newTclRqstId { } {
	variable mbox
	set l [lsort -integer [array names mbox]]

	if { [llength $l] == 0 } { return 0 }

	for { set i 0 } { $i < [ llength $l ] } { incr i } {
	    if { [lindex $l $i] != $i } {
		return $i
	    }
	}

	return $i
    }

    # Free a tclRqstId --------------------------------------------------

    proc freeTclRqstId { id } {
	variable mbox
	
	if { [lsearch [array names mbox] $id] == -1 } {
	    error "No such request id: $id"
	}

	set status [lindex [set mbox($id)] 3]
	if { "$status" != "TERM" && "$status" != "ERROR" } {
	    error "Waiting for final reply (status is \"$status\")"
	}
	
	unset mbox($id)
	return
    }

    # Abort a request ---------------------------------------------------

    proc abortRqst { client id } {
	variable mbox
	variable activity

	if { [lsearch [array names mbox] $id] == -1 } {
	    return "ERROR 1 No such request id: $id"
	}
	if { [lsearch [array names activity] $id] == -1 } {
	    return "ERROR 1 Request $id already terminated"
	}

	set actnum [set activity($id)]
	set module [lindex [set mbox($id)] 0]
	set n [string last "::" $module]
	if { $n < 0 } {
	    return "ERROR 0 internal error"
	}
	set module [string range $module 0 [expr $n -1]]

	return "[rqstSend $client ${module}::Abort $actnum] ${module}::Abort"
    }


    # Send genom special abort requests (abort and resume) --------------

    proc killModule { client module action } {
	variable mbox

	if { [lsearch -exact [array names ::cs::genom] $action] == -1 } {
	    return "ERROR 1 Bad action: $action"
	}

	set list [array names ::modules::loaded]
	if { [lsearch -exact $list $module] == -1 } {
	    return "ERROR 1 No such module: $module"
	}

	set actnum [set ::cs::genom($action)]
	return "[rqstSend $client ${module}::Abort $actnum] ${module}::Abort"
    }
}

# Set the default mbox event script
::cs::mboxEvent { ::cs::repliesRcv }

package provide tclserv::cs 1.0
# eof
