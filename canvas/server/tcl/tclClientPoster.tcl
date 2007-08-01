#	$LAAS$

$commentbegin$
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
$commentend$

# $posterName$ ----------------------------------------------------------

if { [lsearch [interp hidden] $module$$posterName$PosterRead] >= 0 } {
    interp expose {} $module$$posterName$PosterRead
}
if { [lsearch [interp hidden] $module$$posterName$PosterFormatOutput] >= 0 } {
    interp expose {} $module$$posterName$PosterFormatOutput
}

proc ::$module$$posterName$PosterRead { name args } {

    return [ ::cs::posterRead "${name}" "$posterName$" \
	"$posterDoc$" \
	$args ]
}


proc ::$module$$posterName$PosterFormatOutput { list } {
    set format { $outputFormat$}

    set out ""
    interp invokehidden {} rqstFormatOuput $format out list ""
    return [join $out "\n"]
}

interp hide {} $module$$posterName$PosterRead
interp hide {} $module$$posterName$PosterFormatOutput


