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

proc bgerror { message } {
    puts "$::errorInfo"
    puts "bgerror: $message"
}

namespace eval server {

    variable connections
    variable rackAssoc

    # public procedures
    namespace export connect
    namespace export disconnect
    namespace export die

    proc connect { server } {
	variable connections

	if {[lsearch -exact [array names connections] $server] >= 0} {
	    error "already connected to $server"
	}

	# try to open a socket with the server
	if { [info exist ::env(TCLSERV_PORT)] } {
	    puts "using port $::env(TCLSERV_PORT)"
	    set sockCmd [ socket $server $::env(TCLSERV_PORT) ]
	} else {
	    set sockCmd [ socket $server 9472 ]
	}
	fconfigure $sockCmd -buffering line
	fconfigure $sockCmd -blocking 1

	# test the connection and get its id
	puts $sockCmd "HELLO"
	if { ![regexp "^HELLO (.*)" "[gets $sockCmd]" r] } {
	    close $sockCmd
	    error "error HELLO (cmd channel): got $r"
	}

	# open another socket to get csLib replies
	if { [info exist ::env(TCLSERV_PORT)] } {
	    set sockReply [ socket $server $::env(TCLSERV_PORT) ]
	} else {
	    set sockReply [ socket $server 9472 ]
	}
	fconfigure $sockReply -buffering line
	fconfigure $sockReply -blocking 1
	fileevent $sockReply readable "::cs::repliesRcv $server $sockReply"

	# test connection again, and get its id.
	puts $sockReply "HELLO"
	if { ![regexp "^HELLO (.*)" "[gets $sockReply]" r rsock] } {
	    close $sockReply
	    puts $sockCmd "BYE"
	    close $sockCmd
	    error "error HELLO (reply channel): got $r"
	}

	fconfigure $sockReply -blocking 0

	# tell the server to send replies on the right socket
	puts $sockCmd "REPLYTO $rsock"
	if { ![regexp "^OK.*" "[gets $sockCmd]" r] } {
	    puts $sockReply "BYE"
	    puts $sockCmd "BYE"
	    close $sockReply
	    close $sockCmd
	    error "error REPLYTO (cmd channel): got $r"
	}

	# remember sockets for this connection
	set connections($server) "$sockCmd $sockReply"

	# try to find a startup script; if there isn't, just do nothing
	set boot [info commands rack]
	if { "$boot" != "" } {
	    set result 0

	    set rack [rackName $server]
	    set ::server $server
	    
	    if { [catch {set boot [$boot boot $rack].tcl} m] } {
		puts "no startup script for $rack: $m"

		if { [ file readable ~/.tclservrc] } {
		    puts "sourcing ~/.tclservrc"
		    set result [catch {
			uplevel \#0 source ~/.tclservrc
		    } message]
		} else {
		    puts "no script ~/.tclservrc"
		}
	    } else {
		if { [ file readable $boot] } {
		    puts "sourcing $boot"
		    set result [catch {
			uplevel \#0 source $boot
		    } message]
		} else {
		    puts "no startup script $boot"

		    # look for a ~/.tclservrc
		    if { [ file readable ~/.tclservrc] } {
			puts "sourcing ~/.tclservrc"
			set result [catch {
			    uplevel \#0 source ~/.tclservrc
			} message]
		    }
		}
	    }

	    unset ::server
	    if {$result} {
		puts "startup script: $message"
	    }
	} elseif { [ file readable ~/.tclservrc] } {
	    puts "sourcing ~/.tclservrc"
	    set ::server $server
	    set result [catch {
		uplevel \#0 source ~/.tclservrc
	    } message]
	    unset ::server
	}


	# try to compute a prompt, depending on the client's machine
	if [catch {package present el::base}] {
	    # standard Tcl interpeter
	    set ::tcl_prompt1 "puts -nonewline \"tcl\\\[${server}\\\]-> \""
	} else {
	    # eltclsh / elwish
	    set ::el::prompt1 "format \"tcl\\\[${server}\\\]-> \""
        }
	    
	# Damned, it worked!
	return "connected to $server"
    }

    proc disconnect { server } {
	variable connections

	set sockC [sockCmd $server]
	set sockR [sockReply $server]

	# try to close the connection gracefuly
	puts $sockC "BYE"
	puts $sockR "BYE"
	close $sockC
	close $sockR

	# and try to clean up a bit
	catch { unset connections($server) }
	return "disconnected from $server"
    }

    # remote evalutation
    proc reval { server args } {
	variable connections

	set sock [sockCmd $server]
	puts $sock "$args"
	return [gets $sock]
    }

    proc rqstSend { server cmd } {
	variable connections

	set sock [sockCmd $server]
	puts $sock "RQST $cmd"
	return [gets $sock]
    }

    proc chat { server send rcv args } {
	variable connections

	set sock [sockCmd $server]
	puts $sock $send
	set reply [gets $sock]
	if { [uplevel regexp "\"$rcv\" \"$reply\" $args"] } {
	    return 1
	}
	if { [llength $args] > 0 } {
	    uplevel "set [lindex $args 0] \"$reply\""
	}
	return 0
    }

    # This one is a bit hard. (adults only). It kills every server.
    proc die { } {
	variable connections

	set list [array names connections]
	if {[llength $list] == 0} { exit }

	foreach server $list { puts [sockCmd $server] "DIE" }

	# servers will kill us
	while { 1 } { update; after 100 }
    }


    # -------------------------------------------------------------------

    # Well, there's definitely no heuristic for this...
    set rackAssoc(azathoth\[0-9\]*)	azathoth
    set rackAssoc(lama\[0-9\]*)		lama
    set rackAssoc(h2\[0-9\]*)		h2
    set rackAssoc(h2bis\[0-9\]*)	h2bis
    set rackAssoc(diligent)		diligent
    set rackAssoc(cabby)		cabby

    # returns the name of the rack for a given machine.
    proc rackName { mach } {
	variable rackAssoc

	foreach assoc [array names rackAssoc] {
	    if { [regexp $assoc $mach] } {
		return [set rackAssoc($assoc)]
	    }
	}

	if { ![regexp "^\[^0-9\]*" $mach rack] } {
	    set rack $machine
	}
	return $rack
    }

    # -------------------------------------------------------------------

    # the last server we connected to
    proc defaultServer { } {
	variable connections

	set list [array names connections]
	if {[llength $list] == 0} { error "not connected" }
	return [lindex $list end]
    }

    # the "command" socket
    proc sockCmd { serverName } {
	variable connections

	set r [catch {
	    set sock [lindex [set connections($serverName)] 0]
	}]

	if {$r} { error "not connected to $serverName" }
	return $sock
    }

    # the "reply" socket
    proc sockReply { serverName } {
	variable connections

	set r [catch {
	    set sock [lindex [set connections($serverName)] 1]
	}]

	if {$r} { error "not connected to $serverName" }
	return $sock
    }
}

namespace import server::*

package provide genom
#eof
