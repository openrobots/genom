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

namespace eval cs {

    # Set it to 0 if you are an enenmy of beep's (for intermed replies)
    variable beep 1

    variable mbox
    variable modules
    variable servers

    # public procedures
    namespace export replyof
    namespace export mboxInit
    namespace export mboxEnd
    namespace export abort
    namespace export kill

    # Generic function to send a request --------------------------------

    proc rqstSend { module name docString usage argNumber inputInfo argList } {
	variable mbox
	variable modules
	set block 1
	set raw 0
	set doc 0

	# Scan options
	getopt argList block raw doc

	if { $doc } {
	    wrappedputs $docString
	    wrappedputs $usage
	    return
	}

	# Prepare to replace incomplete cmd line in history
	set histrep 0
	if { [llength $argList] < $argNumber } { set histrep 1 }

	# Scan input
	set argList [mapscan $inputInfo $argList]
	if { $histrep } {
	    if { [llength [info commands ::el::history]] > 0 } {
		::el::history add "${module}::$name $argList"
	    }
	}

	# Get server
	set server [serverof $module]

	# Freeze wake up proc while sending request (we might receive the
	# answer before beeing ready to handle it).
	set sock [::server::sockReply $server]
	set s [fileevent $sock readable]
	fileevent $sock readable ""

	# Send request
	set reply [::server::rqstSend $server "${module}::$name $argList"]

	if { "[lindex $reply 0]" != "OK" } {
	    return -code error "[lrange $reply 2 end]"
	}

	# Get rqst id
	set rqstId "$server [lindex $reply 1]"
	set mbox($rqstId) "${module}::$name SENT"

	# Restore wake up proc
	fileevent $sock readable $s

	# Return result
	if { $block } {
	    # wait for TERM
	    set retCode "ok"
	    set answer [replyofPriv $rqstId]
	    if { "[lindex $answer 0]" != "OK" } {
		set retCode "error"
	    }
	    if { !$raw } {
		set p [set modules($module)]${name}FormatOutput
		set answer [interp invoke {} -global $p $answer]
	    }
	    return -code $retCode $answer
	} else {
	    # wait for ACK
	    if { ![ack $rqstId 1] } { error "internal error in ack" }
	    return $rqstId
	}	
    }

    # Generic function to recieve a reply -------------------------------

    proc replyof { args } {
	variable mbox
	variable modules

	# Scan options
	set raw 0
	set more 1
	while { $more } {
	    set more 0

	    if { "[lindex $args 0]" == "-raw" } {
		shift args
		set raw 1
		set more 1
	    }
	}
	if { [llength $args] != 1 } {
	    error "no value given for parameter \"rqstId\" to \"replyof\""
	}
	set rqstId [shift args]

	# retrieve rqst name
	set ids [array names mbox]
	set index [lsearch -exact $ids $rqstId]
	if { $index == -1 } {
	    error "Bad request id \"$rqstId\""
	}
	set name [lindex [set mbox($rqstId)] 0]

	# format output if needed
	set retCode "ok"
	set answer [replyofPriv $rqstId]
	if { "[lindex $answer 0]" != "OK" } {
	    set retCode "error"
	}
	if { !$raw } {
	    set p [namespace qualifiers $name]
	    set p [set modules($p)][namespace tail $name]FormatOutput
	    set answer [interp invoke {} -global $p $answer]
	}
	return -code $retCode $answer
    }

    # Generic function to read a poster ---------------------------------

    proc posterRead { module name docString argList } {
	variable modules
	set block 1
	set raw 0
	set doc 0

	# Scan options
	getopt argList block raw doc

	if { !$block } {
	    puts "Blocking param is meaningless in that context"
	}
	if { $doc } {
	    wrappedputs $docString
	    return
	}

	# Get server
	set server [serverof $module]

	# Send request
	set reply [::server::reval $server "${module}::${name}PosterRead"]

	if { "[lindex $reply 0]" != "OK" } {
	    return -code error "[lrange $reply 2 end]"
	}

	# Return result
	if { !$raw } {
	    set p [set modules($module)]${name}PosterFormatOutput
	    set reply [interp invoke {} -global $p $reply]
	}
	return $reply
    }


    # Wait for an event for a rqstId ------------------------------------

    proc ack { rqstId block } {
	variable mbox

	set ids [array names mbox]
	if { [lsearch -exact $ids $rqstId] == -1 } {
	    error "Bad request id \"$rqstId\""
	}

	# is 'ACK' already there?
	set status "[lindex [set mbox($rqstId)] 1]"
	if { $status != "SENT" } { return 1 }

	# no: eventually wait for it
	if (!$block) { return 0 }
	if {[catch {vwait ::cs::mbox($rqstId)}]} {
	    error "internal error: tcl cannot wait for an acknowledgment"
	}

	# no need to check the answer
	return 1
    }

    proc term { rqstId block } {
	variable mbox
	variable beep

	if { ![ack $rqstId $block] } { return 0 }

	# is 'TERM' already there?
	set status "[lindex [set mbox($rqstId)] 1]"
	if { $status != "ACK" } { return 1 }

	# no: eventually wait for it
	if (!$block) { return 0 }
	if { $beep } { puts -nonewline "\007"; flush stdout }
	if {[catch {vwait ::cs::mbox($rqstId)}]} {
	    error "internal error: tcl cannot wait for a final reply"
	}

	# no need to check the answer
	return 1	
    }


    # Tell server to free a rqstId --------------------------------------

    proc freeRqstId { rqstId } {
	variable mbox

	set ids [array names mbox]
	if { [lsearch -exact $ids $rqstId] == -1 } {
	    error "Bad request id \"$rqstId\""
	}

	if { [::server::chat \
		  [lindex $rqstId 0] "ACK [lindex $rqstId 1]" "OK"] } {
	    unset mbox($rqstId)
	    return
	}
	error "internal error in freeRqstId"
    }


    # Recieve every pending reply (tcl replies) -------------------------

    proc repliesRcv { server sock } {
	variable mbox

	while { [ gets $sock reply ] != -1 } {
	    if { "[lindex $reply 0]" == "DIE" } { exit }
	    set mbox([list $server [lindex $reply 0]]) [lrange $reply 1 end]
	}
    }

    proc replyofPriv { rqstId } {

	if { ![::cs::term $rqstId 1] } {
	    error "internal error in replyofPriv"
	}

	set status [lindex [set ::cs::mbox($rqstId)] 1]
	set reply [lrange [set ::cs::mbox($rqstId)] 2 end]

	::cs::freeRqstId $rqstId

	if { $status == "TERM" } {
	    return $reply
	} else {
	    error $reply
	}
    }

    proc mboxInit { args } {
    
	if {[llength $args] > 1} {
	    error "wrong \# args: should be \"mboxInit [server]\""
	}
	if {[llength $args] == 0} {
	    set server [::server::defaultServer]
	} else {
	    set server $args
	}

	if { ![::server::chat $server "cs::init" "OK.*" r]} {
	    error "cannot create mailbox on $server: $r"
	}
	if { ![::server::chat $server "modules::connect" "OK.*" r]} {
	    error "cannot connect modules on $server: $r"
	}
	return "csLib mailbox created on $server"
    }

    proc mboxEnd { args } {

	if {[llength $args] > 1} {
	    error "wrong \# args: should be \"mboxEnd [server]\""
	}
	if {[llength $args] == 0} {
	    set server [::server::defaultServer]
	} else {
	    set server $args
	}

	if { ![::server::chat $server "modules::disconnect" "OK.*" r]} {
	    error "cannot disconnect modules on $server: $r"
	}
	if { ![::server::chat $server "cs::exit" "OK.*" r]} {
	    error "cannot delete mailbox on $server: $r"
	}
	return "csLib mailbox removed on $server"
    }

    # Abort some activity -----------------------------------------------

    proc abort { rqstId } {
	
	::server::chat \
	    [lindex $rqstId 0] "ABORT [lindex $rqstId 1]" ".*" reply

	if { "[lindex $reply 0]" != "OK" } {
	    return -code error "[lrange $reply 2 end]"
	}

	# Get abort rqst id
	set abtRqstId "[lindex $rqstId 0] [lindex $reply 1]"
	set ::cs::mbox($abtRqstId) "[lindex $reply 2] SENT"

	# wait for TERM
	set retCode "ok"
	set answer [replyofPriv $abtRqstId]
	if { "[lindex $answer 0]" != "OK" } {
	    set retCode "error"
	}
	return -code $retCode $answer
    }

    # Kill a module -----------------------------------------------------

    proc kill { module args } {
	set server [serverof $module]

	if { [llength $args] == 0 } {
	    set action "abort"
	} else {
	    set action [shift args]
	}

	::server::chat $server "KILL $module $action" ".*" reply
	if { "[lindex $reply 0]" != "OK" } {
	    return -code error "[lrange $reply 2 end]"
	}

	# Get abort rqst id
	set abtRqstId "$server [lindex $reply 1]"
	set ::cs::mbox($abtRqstId) "[lindex $reply 2] SENT"

	# wait for TERM
	set retCode "ok"
	set answer [replyofPriv $abtRqstId]
	if { "[lindex $answer 0]" != "OK" } {
	    set retCode "error"
	}
	return -code $retCode $answer
    }

    # Get a module's server ---------------------------------------------

    proc serverof { module } {
	variable servers
	if {[catch { set server [set servers($module)] }]} {
	    error "$module not loaded"
	}
	return $server
    }
}

namespace import cs::*

package provide genom
#eof
