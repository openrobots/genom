# 
# Copyright (C) 1999-2010 LAAS/CNRS 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#    - Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    - Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# tkgenom.tcl
#   Daniel Sidobre
#

# tkGenomRequest is a tk interface for genom request
# tkGenomPoster is a tk interface for genom poster

# Bugs
#
# Problème avec force::InitNumberFFT  coments/default value
#
# thGenomPoster : record : ReRead manuel n'enregistre pas la valeur 
#      lorsque record est actif  --> ecriture d'un ficheir "vide"
#
# In some case variables are not all destroyed when the widget is deleted

#  Example using the demo module
#
#  lm demo
#  tkGenomRequest .g ::demo::GotoPosition
#  tkGenomRequest .gg demo::SetSpeed demo::GetSpeed
#  tkGenomPoster .ggg ::demo::MobilestatePosterRead
#
#  To record some data with .ggg: 
#    Choose the time between records (500 ms by default), 
#    Select Permanent update
#    Select Record
#    Push the button ReRead
#    To stop recording unselect permanent update
#    To save the data push the button Save.


package require editval
package require BWidget
package require Ttk

proc tkGenomBuildNameVar { prefix var } {
    return "[set prefix][string map { {[} {_}  {]} {_}  { } {_} } $var ]"
}

proc tkGenomRequest { w functionRequest 
		      { functionRequestRead "" } { nbget 0 } } {
    if { [winfo exist $w ] } { destroy $w }
    toplevel $w
    wm title $w "GenomTkRequest $functionRequest"
    ScrolledWindow $w.sw
    ScrollableFrame $w.sw.sf
    $w.sw setwidget $w.sw.sf
    set f [$w.sw.sf getframe]
    set format [eval $functionRequest -format]
    set listeVar ""
    foreach el $format {
	set var [tkGenomBuildNameVar ::$w [lindex $el 1] ]
	lappend listeVar $var
    }
    set command "tkGenomRequestGet $w $functionRequest {$listeVar}"
    set nbEl 0
    foreach el $format var $listeVar {
	set defultValue [lindex $el 2]
	switch -exact [lindex $el 0] {
	    float -
	    double {
		editval $f.$nbEl label-text "[lindex $el 1]" \
		    scale-length 200 -variable $var
		$f.$nbEl set $defultValue
	    }
	    int {
		editval $f.$nbEl label-text "[lindex $el 1]" \
		    scale-length 150 spinbox-increment 1 -variable $var
		$f.$nbEl set $defultValue
	    }
	    {char*} -
	    string {
		frame $f.$nbEl
		label $f.$nbEl.l -text [lindex $el 1]
		entry $f.$nbEl.e -textvariable $var
		foreach el [winfo children $f.$nbEl] {
		    pack $el -side left
		}
		$f.$nbEl.e insert 0 $defultValue
	    }
	    default {
		#list
		frame $f.$nbEl
		label $f.$nbEl.l -text [lindex $el 1]
		tk_optionMenu $f.$nbEl.m $var {*}[lindex $el 0]
		$f.$nbEl.m.menu configure
		foreach el [winfo children $f.$nbEl] {
		    pack $el -side left
		}
	    }
	}
	incr nbEl
    }
    foreach el [winfo children $f] {
	pack $el
    }
    frame $w.b
    if { $functionRequestRead != "" } {
	button $w.b.read -text "Read SDI" -command \
	    "tkGenomRequestGet $w $functionRequestRead [list $listeVar] forceRead $nbget"
    }
    button $w.b.get -text "Send request" -command $command
    button $w.b.print -text "Print request" -command "$command print"
    button $w.b.quit -text Close -command "destroy $w"
    foreach el [winfo children $w.b] {
	pack $el -side left -fill both -expand true
    }
    pack $w.b -fill x
    pack $w.sw -fill both -expand true
}

#proc tkGenomRequestGet { w functionRequest listeVar args } {
proc tkGenomRequestGet { w functionRequest listeVar 
			 { verbose "" } {nbget end} } {
    global tkGenomVerbose
    set params {}
    if { $nbget != "end" } { incr nbget -1 }
    foreach el [lrange $listeVar 0 $nbget] {
	lappend params [set $el]
    }
    if { $verbose != "" && $verbose != "forceRead" } {
	puts "$functionRequest -raw [set params]"
	# ??return
    }
    set status [catch { $functionRequest -raw {*}[set params] } data ]
    set f [$w.sw.sf getframe]
    set format [tkGenomRequestGetFormatOutput $functionRequest]
    set formatInput [$functionRequest -format]
    set different 0
    foreach el [lrange $format 1 end] listei $formatInput {
	if { $el != [lindex $listei 1] } {
	    set different 1
	    break;
	}
    }
    if { $verbose == "forceRead" } { set different 0 }
    if { $different } {
	if { [winfo exist $f.tree] != 1 } {
	    set columns { name = value }
	    ::ttk::treeview $f.tree -columns $columns -show headings
	    foreach el $columns { $f.tree heading $el -text $el }
	    $f.tree column 1 -width 3
	    foreach el [winfo children $f] {
		pack $el -fill both -expand true
	    }
	} else {
	    $f.tree delete [$f.tree children {} ]
	}
	if { $status } {
	    $f.tree insert {} end -values $data
	} else {
	    foreach el $format val $data {
		$f.tree insert {} end -values [list $el "=" $val ]
	    }
	}
    } else {
	if { [winfo exist $f.labelStatus] == 0 } {
	    frame $f.labelStatus
	    label $f.labelStatus.label -text Status
	    label $f.labelStatus.status -relief sunken
	    pack  {*}[winfo children $f.labelStatus] -side left
	    pack $f.labelStatus
	}
	if { $status } {
	    $f.labelStatus.status configure -background red
	    $f.labelStatus.status configure -text $data
	} else {
	    set status [lindex $data 0]
	    $f.labelStatus.status configure -text $status
	    if { $status == "OK" } {
		$f.labelStatus.status configure -background \
		    [ $f.labelStatus cget -background ]
	    } else {
		$f.labelStatus.status configure -background red
	    }
	    foreach val [lrange $data 1 end] el $listeVar {
		set $el $val
	    }
	}
    }
}

proc tkGenomRequestFormatDecode { a {prefix "" } {output "" } } {
    if {$prefix != "" } { 
	set prefix [set prefix].
    }
    foreach { dims members } $a {
	if { [llength $dims] == 0 } {
	    if { [llength $members] == 1 } {
		# basic member
		lappend output "${prefix}$members"
	    } else {
		# structure
		set output [tkGenomRequestFormatDecode [lrange $members 1 end] \
				${prefix}[lindex $members 0] $output]
	    }
	} else {
	    set nElts 1
	    foreach dim $dims { set nElts [expr $nElts * $dim] }
	    for {set i 0} { $i < $nElts} { incr i } {
		if { [llength $members] == 1 } {
		    # basic member
		    lappend output "${prefix}$members[tkGenomRequestIndex $dims $i]"
		} else {
		    # structure
		    set output [ tkGenomRequestFormatDecode [lrange $members 1 end] \
				     "${prefix}[lindex $members 0][tkGenomRequestIndex $dims $i]" \
				     $output ]
		}
	    }
	}
    }
    return $output
}

proc tkGenomRequestIndex { dims i } {
    set l [llength $dims]
    if { $l == 0 } return ""

    set dim [lindex $dims end]
    set local [expr $i % $dim]
    set rest [expr ($i - $local) / $dim]
    return "[tkGenomRequestIndex [lrange $dims 0 [expr $l - 2]] $rest]\[$local\]"
}

proc tkGenomRequestGetFormatOutput { request } {
    set l [string map { : { } } $request ]
    set function [lindex $l 0][lindex $l 1]FormatOutput
    if { [lsearch [interp hidden] $function ] >= 0 } {
	interp expose {} $function
    } else {
	return "$function is not hiden"
    }
    set body [info body $function]
    interp hide {} $function
    # the eval set the format variable
    eval [lindex [split $body "\n"] 1 ]
    return [tkGenomRequestFormatDecode $format]
}

# tkGenomRequestPath : as tk is restrictive for path name of windows,
#  this function build a valid tk name from a genom name
proc tkGenomRequestPath { var } {
    set retvar [string map { {[} ""  {]} ""  {.} {_} } $var]
    if { [ string is upper [ string index $var 0 ] ] } {
	return _$retvar
    } else {
	return $retvar
    }
}

# tkGenomPoster : create a toplevel tk window to display a genom poster
#  w: path
#  functionPosterRead: function to read the poster
#  functionElToPath: function to filter the poster, useful to change
#    tkGenomPoster behavior for poster elements
proc tkGenomPoster { w functionPosterRead {functionElToPath tkGenomRequestPath} } {
    if { [winfo exist $w ] } { 
	destroy {*}[ winfo children $w ]
    } else {
	toplevel $w
    }
    wm title $w "GenomPoster $functionPosterRead"
    frame $w.f
    scrollbar $w.f.vsb -orient vertical -command [list $w.f.tree yview]
    set f $w.f
    set lreq [eval $functionPosterRead -raw]
    ::ttk::treeview $f.tree -columns { name = val } -show headings \
	-height [llength $lreq] -yscrollcommand [list $f.vsb set] 
    foreach el { name = val } { $f.tree heading $el -text $el }
    $f.tree column 1 -width 3
    set format [tkGenomRequestGetFormatOutput \
		    [string range $functionPosterRead 0 \
			 [expr [string length $functionPosterRead] - [string length read] -1]]]
    foreach el $format val $lreq {
	if { [$functionElToPath $el] != "" } {
	    $f.tree insert {} end -values [list $el = $val ]
	}
    }
    pack $f.vsb -side right -fill y -expand true
    pack $f.tree -side left -fill both -expand true
    frame $w.f1
    button $w.f1.update -text ReRead \
	-command "tkGenomRequest_update_cb $w \"$functionPosterRead\" $functionElToPath"
    checkbutton $w.f1.auto -text "Permanent update (ms)" -variable ::tkGenomRequest_auto$w
    spinbox $w.f1.time -from 10 -to 10 -increment 10 -width 4 -value 500
    checkbutton $w.f1.record -text "Record" -variable ::tkGenomRecord_$w
    foreach el [winfo children $w.f1] {
	pack $el -side left
    }
    frame $w.f2 -height 20 -background red
    button $w.f2.save -text Save \
	-command "tkGenomSave $w $functionPosterRead $functionElToPath"
    button $w.f2.quit -text "Close" -command "destroy $w"
    foreach el [winfo children $w.f2] {
	pack $el -side left
    }
    pack $w.f1 $w.f2
    pack $w.f -fill both -expand true
}

proc tkGenomRequest_update_cb { w fget functionElToPath } {
    set ::tkGenomData_$w {}
    tkGenomRequest_update $w $fget $functionElToPath
}

# tkGenomRequest_update : fonction callback associée à tkGenomRequest
#   et appelée par le bouton d'actualisation.
proc tkGenomRequest_update { w functionPosterRead functionElToPath } {
    set lreq [eval $functionPosterRead -raw]
    set f $w.f
    $f.tree delete [$f.tree children ""]
    set format [tkGenomRequestGetFormatOutput \
		    [string range $functionPosterRead 0 \
			 [expr [string length $functionPosterRead] - [string length read] -1]]]
    if { [set ::tkGenomRecord_$w] == 1 } {
	set lvar ""
	foreach el $format val $lreq {
	    if { [$functionElToPath $el] != "" } {
		$f.tree insert {} end -values [list $el = $val ]
		lappend lvar $val
	    }
	}
	lappend ::tkGenomData_$w $lvar
    } else {
	foreach el $format val $lreq {
	    if { [$functionElToPath $el] != "" } {
		$f.tree insert {} end -values [list $el = $val ]
	    }
	}
    }
    if { [set ::tkGenomRequest_auto$w] == 1 } {
	set time [$w.f1.time get]
	after $time "tkGenomRequest_update $w $functionPosterRead $functionElToPath"
    }
}

proc tkGenomSave { w functionPosterRead functionElToPath } {
    set types {
	{ {data} {.data} }
	{ {All} {*} } 
    } 
    set fileName [ tk_getSaveFile -title "Save data as" \
		       -filetypes $types -initialfile "saHand" ]
    if { $fileName == "" } { return }
    set file [open $fileName w]
    puts -nonewline $file "\# t"
    set format [tkGenomRequestGetFormatOutput \
		    [string range $functionPosterRead 0 \
			 [expr [string length $functionPosterRead] - [string length read] -1]]]
    set format [lrange $format 1 end]
    # lprint is used to avoid saving non number values
    set lprint ""
    foreach el [lindex [set ::tkGenomData_$w] 0] {
	if { [string is double $el] } {
	    lappend lprint 1
	} else {
	    lappend lprint 0
	}
    }
    set l_lprint $lprint
    foreach el $format {
	if { [$functionElToPath $el] != "" } {
	    set elprint [lindex $l_lprint 0]
	    set l_lprint [lrange $l_lprint 1 end]
	    if { $elprint } {
		puts -nonewline $file [tkGenomBuildNameVar { } $el ]
	    }
	}
    }
    puts $file ""
    set t 0
    foreach el [set ::tkGenomData_$w] elprint $lprint {
	puts -nonewline $file "$t"
	foreach val $el elprint $lprint {
	    if $elprint {
		puts -nonewline $file " $val"
	    }
	}
	puts $file ""
	incr t
    }
    close $file
}

package provide tkgenom 2.6
