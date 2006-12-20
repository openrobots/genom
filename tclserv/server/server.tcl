#	$LAAS$

#
# Copyright (c) 1999-2003 LAAS/CNRS                   --  Sat Apr  3 1999
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

namespace eval server {
 
    variable channels

    # Cmd channel -------------------------------------------------------

    proc evalCommand { channel } {
	variable channels

	# Est-ce que le client s'est deconnecte ?
	if { [ gets $channel cmd ] == -1 } {
	    endClient $channel
	    return
	}

	switch -regexp $cmd {

	    "HELLO" {
		log "HELLO request"
		answer $channel "HELLO $channel"
	    }

	    "BYE" {
		log "BYE request"
		endClient $channel
	    }

	    "DIE" {
		log "DIE request"
		foreach i [array names channels] {
		    answer $i "DIE"
		}
		die
	    }

	    "CSREPLY" {
##		log "csLib wakeup"
		eval [ ::cs::mboxEvent ]
		answer $channel "OK"
	    }

	    "^REPLYTO$" {
		log "REPLYTO request"
		answer $channel "OK [set channels($channel)]"
	    }

	    "^REPLYTO" {
		log "REPLYTO request"
		set l [split [string range $cmd 7 end]]
		set channels($channel) {}
		foreach i $l {
		    if { "$i" != "" } {
			# Ben-proof
			if { [lsearch [array names channels] "$i"]==-1 } {
			    answer $channel "ERROR 1 unknown channel: $i"
			    return
			}

			lappend channels($channel) $i
		    }
		}
		answer $channel "OK [set channels($channel)]"
	    }

	    "^LM " {
		log "$cmd"
		if { ![regexp "^LM\[ \t\]*(.*)" $cmd match args] } {
		    answer $channel "ERROR 1 syntax error: $match"
		    return
		}
		set r [catch { set rval [eval lm $args] } m]
		if { $r != 0 } {
		    answer $channel "ERROR $r $m"
		} else {
		    answer $channel "OK $rval"
		}
	    }

	    "^RQST " {
		log "$cmd"
		if { ![regexp "^RQST\[ \t\]*(\[^ \t\]*)\[ \t]*(.*)" \
		       $cmd match rqst rest] } {
		    answer $channel "ERROR 1 syntax error: $match"
		    return
		}
		set r [::cs::rqstSend $channel $rqst $rest]
		answer $channel $r
	    }

	    "^ACK " {
		log "$cmd"
		if { ![regexp "^ACK\[ \t\]*(\[0-9]*)" $cmd match id] } {
		    answer $channel "ERROR 1 syntax error: $match"
		    return
		}
		set r [ catch { ::cs::freeTclRqstId $id} m]
		if { $r != 0 } {
		    answer $channel "ERROR $r $m"
		} else {
		    answer $channel "OK $m"
		}
	    }

	    "^ABORT " {
		log "$cmd"
		if { ![regexp "^ABORT\[ \t\]*(\[0-9]*)" $cmd match id] } {
		    answer $channel "ERROR 1 syntax error: $match"
		    return
		}
		set r [ ::cs::abortRqst $channel $id]
		answer $channel $r
	    }

	    "^KILL " {
		log "$cmd"
		if { ![regexp "^KILL\[ \t\]*(\[^ \t]*)\[ \t]*(\[^ \t]*)" \
			   $cmd match module action] } {
		    answer $channel "ERROR 1 syntax error: $match"
		    return
		}
		set r [ ::cs::killModule $channel $module $action]
		answer $channel $r
	    }

	    default {
		log "$cmd"
		set r [ catch { uplevel \#0 $cmd } m ]
		if { $r != 0 } {
		    answer $channel "ERROR $r $m"
		} else {
		    answer $channel "OK $m"
		}
	    }
	}
	
	return
    }
    
    proc acceptCmdClient { name address port } {
	variable channels

	fconfigure $name -buffering line
	fconfigure $name -blocking 0
	fileevent $name readable "[namespace code evalCommand] $name"

	set channels($name) {}
	log "accepting $name"
    }


    # Send a message to the reply channel -------------------------------

    proc answer { name message } {
	regsub -all "\n" $message "\\n" tmp
	set message $tmp
	regsub -all "\"" $message "\\\"" tmp

	puts $name $tmp
    }

    proc replyTo { name message } {
	variable channels

	regsub -all "\n" $message "\\n" tmp
	log "replied $tmp"
	if { "$name" == "-" } {
	    slave eval ::cs::replyRcv $message
	} else {
	    catch { 
		# client might have broken connection in the meantime
		foreach c [ set channels($name) ] {
		    puts $c $tmp
		}
	    }
	}
    }


    # Terminate a client ------------------------------------------------

    proc endClient { name } {
	variable channels

	close $name
	log "closed $name"
	unset channels($name)
	return
    }

    proc die { } {
	catch { ::modules::disconnect }
	catch { ::cs::exit }
	exit
    }

    # Log message on server console -------------------------------------

    proc log { message } {
	if { $::verbose } {
	    set time [clock format [clock seconds] -format "%H:%M:%S"]
	    puts "$time - $message"
	}
    }
    
    # Creation du serveur -----------------------------------------------

    variable sockCmd [socket -server [namespace code acceptCmdClient] $::port]
    log "Accepting connections on port $::port"
}

proc bgerror { message } {
    ::server::log "bgerror: $::errorInfo"
}

# shell's shift (should be added to tcl language :)
proc shift { varName } {
    upvar $varName l 
    set h [lindex $l 0]
    set l [lrange $l 1 end]
    return $h
}

package provide tclserv::server 1.0
# eof
