# 
# Copyright (C) 1999-2007 LAAS/CNRS 
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
# editval.tcl
#   Daniel SIDOBRE
#

# Le widget editval permet d'afficher et de modifier un double

# Voir documentation en fin de fichier (variable help_editval)

#??? bug lorsque on passe de spinbox a scale : les configs de 
#    spinbox ... n'est pas conserve'e


# fonction à definir en fonction du système utilisé
switch -exact [set env(OSTYPE)] {
    "linux" {
	proc editval_man { page { section "" } } {
	    set cmd "exec /usr/bin/gnome-help man:$page"
	    if { $section == "" } {
		set cmd "[set cmd] &"
	    } else {
		set cmd "[set cmd]([set section]) &"
	    }
	    eval $cmd
	}
    }
    "solaris" {
	proc editval_man { page { section "" } } {
	    if { $section == "" } {
		exec /usr/openwin/bin/xterm -e /usr/bin/man $section $page &
	    } else {
		exec /usr/openwin/bin/xterm -e /usr/bin/man -s $section $page &
	    }
	}
    }
    default { puts "Attention pb configuration help" }
}

proc editval { chemin args } {
    #puts "editval chemin= \"$chemin\" args= \"$args\""
    frame $chemin
    label $chemin.label
    spinbox $chemin.spinbox
    array set ::[set chemin] [array get ::_editval_init]
    set variable ::[set chemin]._var
    array set ::[set chemin] "-variable $variable"
    set $variable 0
    foreach { el val } [array get $chemin] {
	#puts "editval_configure $chemin $el $val"
	editval_configure $chemin $el $val
    }    
    $chemin.spinbox configure -command "editval_spinbox_set $chemin" \
	-from -1e20 -to 1e20 -increment 1 -exportselection 1 
    #-validate key -validatecommand "editval_spinbox_validate $chemin %P"
    $chemin.spinbox set [set $variable]
    editval_scale_roller_init $chemin
    bind $chemin.spinbox <Return> "editval_spinbox_return $chemin"
    trace variable [set variable] w "editval_trace $chemin"
    bind $chemin.label   <3> "editval_menu3 $chemin %X %Y"
    bind $chemin.spinbox <3> "editval_menu3 $chemin %X %Y"
    bind $chemin         <3> "editval_menu3 $chemin %X %Y"
    bind $chemin <Destroy> "editval_destroy_f $chemin"
    eval editval_configure $chemin $args
    editval_scale_center $chemin
    rename $chemin w$chemin
    proc $chemin { option args } "
	switch \[set option\] {
	    \"configure\" { eval editval_configure $chemin \[set args\] }
	    \"cget\"      { eval editval_cget $chemin \[set args\] }
	    \"get\"       { eval editval_get $chemin \[set args\] }
	    \"set\"       { eval editval_set $chemin \[set args\] }
            default { error {Pb editval : syntaxe :\n\
		 \t$chemin configure ?option value? ?option value ...?\n\
		 \t$chemin cget -option\n\
		 \t$chemin get\n\
		 \t$chemin set val}}
        }
    "
}

### -- fin interface -- ###

proc editval_define_images { } {
##    set output [open pipotmp w]
##    package require base64 
##    set rimage "roueimages/roue"
##    set nbimage 0
##    while {1 == 1} {
##        if { [catch {set f [open "$rimage[set nbimage].gif" r]}  inutile] } {
##	    break
##	}
##        fconfigure $f -translation binary 
##        set img [base64::encode [read $f]] 
##        close $f 
##	puts $output "image create photo [set rimage][set nbimage] -data {\n[set img]\n}"
##	incr nbimage
##    }
##    close $output
image create photo roueimages/roue0 -data {
R0lGODlheAAXAMYAAAYGWqamxlJSkkpKlgYGbkZGigYGgv7+/goKksrK7goK
poaGusLC5iYmth4ewnp6vgoKlkZGlhoapgYGegoKmgYGZvLy+goKtg4OjgoK
gmZmvgoKopqazkZGphISskpKtrq60goKqk5OwgoKhhISugoKjjY20kZGqgYG
igYGdgoKngYGYg4OpkpKvg4OqkZGrgYGagoKigYGfgYGci4u0hoasq6uyhIS
lkpKwkZGsg4OtgYGhgoKstLS4oKCtg4Ooj4+yhYWtkZGmkZGtg4OssbG5iIi
xlJSvgoKagoKfkpKxhoaukZGngYGjvb2+p6eyhYWsgYGXtLS6h4exk5OxkZG
okpKugoKrkZGuhISvhoavv//////////////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAHCUdWLYY4iFYvL0OIjo+QOEqTJ0wvLZNUmlQinY+GoKGGiidYVqcf
Q6qrQzmurzlWiK6LLye3uB26u1VVjUo4HVVMTEIRHYUtDwcWNU0hHiEhVzwk
DSMxLjoX3N3eFzwXOjokHhAVSQomJlMO7lPtDlo1QUEe9RdXCgohz00qM0o0
GdiEAgUICBEoVFiiIQtwQDBgG0ExicUkSCymmEGg4wwEREjwMFABRoUoUVbs
2KCAgg8GGyA04eHvSogbM2T86Eez58wQ+6QBBRojCgwUP1iEYMGU6Y8fCjao
+LHhaRMFA1X8QwgDRomDCErEiDFih1kDaGWohbBPQpIU/hs5di1ZYQXKu1Eq
GFDhQoEMAABQAq4wYmCBIlhVhNgQguYGFDMyPLtCmRrloPsUXLmqYIfRESxZ
Rt1AWoVpFQZVaC5IoYnMgRUqyBiIYkftHSMyZDCgVq2BJpspwJ1BfIbJ2Cvs
Cka5QgYEqRPuBs5rgAKKAgxUQKCwgbNNFASSKJ75kx/QzPxsbtixAsYO1aTj
T/1BwbRBzgO7H5RpMkMMsWOVhZZuGVg0wWzSqIBREh0RMFddyQkGWHMUKKDC
BIBFARgAskFwXXYDLcbZBjHkpBhljT0TFE1AbbaPZzDEoEJo8f1w2o0zAqVV
awMhENtsKNyGAkW78WbABElk/gBBcHClgIRcyEWIV0q/RRWdhndVsEMTH2qn
mGrkNZFTheTlY5N50pTXWXsokDYaVVLdeF9U/xQEAQKwofNfgGbtUKQMvB3I
VggqbJQCgw4+GOGGVJL5l4YbrvBbE9hp1YRUQoVQwgy/TXPFBeTxowBNNtEU
FYzvRaUAnPLd2N0+BRGkUGy/BRkkkTugxZtakkVFwQQpEFDccRBOCcAKSVBw
xQbRTYeSlh5WCgGYWG3WRHhk8kCZtplKk95iMI4AX41PyWkacN1px2MTtO45
wm26XaTWBEoCR0ESxQlLbF0SModgEzJAmqGkhoFYoWJCbbqlTZuxeJ5P6MGI
woz7/lQVJ46mlbYuQuwiy+WtFOWqq0UyLOzaofnOlZyUkFKoD4Z4cWhAtCAC
dxW6MRCQAQVCVdatp0CRxp57M27AQnwX23dfPxvc5xq7eo0Vg60U6dobvTyH
INyTHCVqkpTONsdWEwZsOJ1eBaOmmmKrQTbbBptlug+pNos2glEyVjxaaaZh
9c9m3Q3EcRMmATlkyFbzlsRaLF14qLAExFYXhIFF2qkCV0qoF82ukYkVD/3M
kARw0/Dkrac+R2WAUe+Ra6OrWEUFlNN3NrGCbGLdRlFuu6mVhKDdbRBXg8et
vIKzVPrzV4Yaok0pTCGyTVqJBmA1TTicpehiaJ3hrdqq/kjjKFV9/Qj+GtQZ
oNAEnyIbkKQMB17+a1xyFa+c2eioAGrmGW7epUwuqhh4JpC1mVTGPAxbjKpQ
RbEaxak+TgOOhQSnEKgZYGq3ikH7dvW7Tl0KWBypX5SM1RyblW06g5kUdrJC
qM1spkSSmQaoRAUUuqFnPZ+BT8WSFruKle8gsqrL6MjyLj9ZLQPwswjPFAAB
fBGnI5IznsBSsiWapKBf1KGZ3/qRKfD8Kx+nk0YCM4NDoq3qKa2a0VVSw5KB
7GcghfsYRYb0JyT25irdmcATO0Is5UgHJXuRxqME5rzDFORL/dgHZDKAMKGI
KijbqxgDacS3jKHGIHD7x7q6/tOusdRmBANKwq4OlIHuEEqPe1QZvyClIY9t
5orMy8sOolUEgnCxH1cYoD96IirSCUU0ZWxdVKgyoxvl51J+045MZEIrLmGj
T2kxABIncKAl/qo4HKELy7CUvM2sTmDHUiEDWsIz1XgLMnupjE2kATrHkJEl
4dJhGouWGNGsi0d1kYEncQPKAflucUlAgLWGQ7wo3q9lW9pfLLN4nQQgIEh4
Yg0ESEKASV3qoq9a46UuGTB0IOAgbwSLaxZCkCY0RCAVnEEFJqArxcEPSU0i
Tgpm1h0R2s+PyJvBRZvFqBVMAAUGEMABIjAXGMwALhM4SV4OBVAZNPV3MG3S
8Y7VYKSqHnU42DROg8KzERgERgaRkxyEpIgXGGzkeH/EUuXWOh0CAIuby4EB
AZAAggM4wQYByKte98rXvvo1r0/4q2D9qoEFFHYBC+BAYAf718A6NgCPjSxk
+yrZxQagBwcIBAA7
}
image create photo roueimages/roue1 -data {
R0lGODlheAAXAMYAAAYGXqamxk5OwkpKkgYGckZGjgYGgv7+/goKjtbW4goK
osLC5lZWlh4ewmZmvgoKkgYGahoaskZGmn5+ygoKtgoKlgoKqvb2+gYGekZG
tkpKlpqazgYGZgoKghYWtr6+1goKmkpKtjY2whISqkZGogoKpgoKnkZGpkpK
ukZG2hISus7O7i4uzkpKvgYGhgYGYhYWngYGfg4OqgYGbkZGqg4OpgYGdg4O
okpKxgoKhq6uysrK5g4OthISloaGuhoavgoKigoKskZGskpKwiIixgoKbhYW
ugYGig4OrlJSvhISrhoaxvr6+p6eykZGrtbW6hoauhYWvgoKrkZGuhISsoaG
vkZGng4Osk5Oxhoatv//////////////////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAHK0koKC2HLUOKQk5OKEM4ipKTkjiWkSQkGZA4WAKUQ4iiiIWljFMo
IaoZrBlCr7CwGYktjbY0uCe6upmZJyiWKCRWVhJWsy0TBxcRJgpGQRYWQUEU
RjAdDyo81RTe3+A8PCoqRh4dHAgRKSxEDe4NUFAeHlRISDL3FFIK/QoWR0DY
QHCkxJEjFSo8WLgQgUMgQBDUoMBDRI4cHTJmLNKhSBECM0DOgGDDhJEfFghw
eAEAQIcjAKssUHBQRsEjUkpYKNEBQo4aUnaWGEpU2lAZQ3cqwACAwIMRN2rU
uKHghlUFJkyA0LrV4MGDIBRCgOCCIZAcLlwYWBujLQYM/gZM7IRBQCQEDnjx
vmDZsiVZBUpAzOgL4EWMCkc0LEAIYqeUgkOXzjAAIqm0y0ItKCC6WYGNpkBu
lPDXL+tWEGET/rNgGvFBvDFcHEnrogNbt29tUB6KoK7IGSsJC+ewGwQE4RiO
PCiwwKtBg0FwSt5NtLrOyNY9Nz2ioEbVG1lNa+UO4uDjr2EfHLnb4eLF2ho1
ctQNgiaI32P1Cu/LQbYU48gdxBxjQumkVAwzEFQdZgVuxpkJTCVIVWnhZfXc
QZtJg5prRyAA20FprWUAbm9hUEQOOFnwQF0ztAjBXnzt159cR3AgXAcKMQdQ
QSZANhSEM+Twj0EUWGegkSZ8/kYAdxNS2ONo/pS3Y0AJKcceRO/d1lYMGHCZ
gwkG3WfXXXvt1xJxR0T3GWFLHjEgQtGBCdlSEBw0zU6XUQATdtUttR1pWDkz
Glc0FaRheQcRBFtasxmg1m252RCDCUEdwaKLZZoJAAdCWgBCjC3ZkOMCj1Uw
FExGKYCgC0MuKEWlQfTZj5IIfOdPeOUpsBVWOIEVFkIeEneWCxhhFF8HJVK2
2RE24IeXppvuZqlwbTJnAmJCQRYdBjO4YBCDQ0kRzWjWQfgnac7QKOdN1yGq
3EEvvnQEWo9y+VYM88VQWQkrgjQWmaASNmN0AWMwqgkPVNDPnmmWEAMEBhxB
AUCx/l6WZoHYmUtArYMGKmhBNH1L07XlKbTeCwag+KiIW3LZgaSVHdSsXcFp
inKva/ZV7WIHRQdQUKNhsKRcQRklhZ5CORiZdkv2Q1W66n4cslcIJcrevPSu
XKINJpqn4qVjZWpmnQpEt18MAi5gAWI5JRUZty6AmZlO4gKtNJS0Ngl1Z6g9
GRlqCKl3MtpYH4ssl142bOmYNZuJcsw5t2SwmzMh9M9B4R7BLeZHShOdkf2U
oPERztxqmnPcwZR61cohcCVajrqAW0YY2NBB3NGt6GLYNmpKnAJ6Fpx2jwor
LbHD3baa2Z1pKr1ZkudiRaGPN6VYQnkgCF4jB2iH6Cik/h0QYIOkOD1mQ4sj
vRgwYY8DRAByiFm7EFYG2W0DxM/hqf+4nEHJlFOiSRdABvUt7tBkR655gENe
ZAAgzGZluLHdW+J2Khb9q3EyigvBAkS5rwAkW/yAm8SAdh2K8akz5prBEfSG
Qh71iDvXIZ27BpeDsxgrI1t6C0fQlhNmoS8/6+tL+5gFP8qVoAKNOQ/QgES6
5ZWgGj7CzvOU9IDS9eOD3KHRwnr1wuyp5y7dgyAGcDjG2sVFJ4tL30qC6Bfz
HOFsaasaua4jOgyQpTFzs4C4ftYZcmknQaWRnnTsxC406uorX+QebcSIrNq9
JWI7sdQP1QetF7DqMe8jzOSY8EMThV3oiZpTIWSMIpQ9zjFDmoue3EJXgscY
5EmRJJ3JXPeCl4RILTlspInO6L7d3QVaAIAAQR4TMMIVYAUVYJRrLhQDDtgA
c+yC5isBdxOmQCwh6TFZ9jj0lYBo7wFqedEYN9KR2s1sZnVxARKPkL67YNBM
BqPJcdhEEAYcQAP/Et+98MWSF0CgS4jT0hi5FgPa5Swk5xzfOR05PgJAoFvI
+lczC7OXvKyRosLx5/2ACS0C2HE//izCBw7ABB0E4KQoTalKV8rSlDahpTBV
qQN8MFMfVGEDG3BpAF4aU52+9Kc7DSpQeYrSoaI0AQcIBAA7
}
image create photo roueimages/roue2 -data {
R0lGODlheAAXAMYAAAYGXqamxk5OwkpKkgYGckZGmgYGgv7+/goKjtLS6goK
miYmesLC5lJSvgoKsmZmvgoKpgYGaiYmxnp6xjIyhhoasvLy+pqazgoKggYG
dkpKlh4eqkZGtgoKngoKlhYWsgYGZkpKtgYGfhISprq60kZGogoKogoKqg4O
skpKwgoKhkZGpkZG1goKkgYGYgYGeg4OqgYGboKCukZGqgYGig4Opg4OogoK
isrK7jIyxq6uyhISshoaugYGhkpKukZGrvr6+h4evhYWvkpKvhISukpKxsbG
5goKdg4OlgoKbg4OikZGsioq0oaGugoKriYmfhISqtbW4p6eygoKthYWug4O
uhoawh4exg4OrhIStg4OtkZGnhYWthoavhISvhISrh4ewv//////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAHOA0+hUOHQymKKT8zP0NFRYuTlCmRkT5bKz6KAgKTiKGihaQ+Sz8c
ISEcrEuur64/sj+cKRyzMzMru7wlviVbP5ZFM1sFWxyFEwcWFSc0X0QnTtQO
U1NZKhgeXVrW1+DXWlpeWkRU6F1cBBEeOSwSYEFdFVxcOztYMBD8EDDXNJ49
o6HgSAsaNDy0WLgQgcMbECF6QDFFyAYMGI5gPMLxSJIYICOAiKCiBhMwHkC4
AEAgIA0ZDGh08ABBIA0n/WwQiNEDhpMT/U4IHQp0H4x9EHS6yOABSo0aNiCY
6GCjQwcFWD14oMHPhEyCWmnEiHADYY8eBtKKeMH2RYYM/i9o1DgBQ0mMnRFE
rgTAty9fAghgQFGxd+lBCgwgIOxwwgQEJ1wfiyXZgd/QfpilSjXBWUEMABla
cLZa9aqCDjS8glVsYutBhAhiuDBwQxtGEbjXss1AwCBXEwbGilTptzjL1BB6
+G3Z4gkDEy0QKFBsGWjjDDwrK3ZAtGZmzB0IgEbAuSoSq1hRT5XpwcQz1Apf
yxaBNi3GjBk7EsiAoGvwCMIRZ9xfN3QAnF8x0IDAAkagJhNQLvXTwVgI3MQd
UEH141hXnX2WgUylTWUVeiMitGFCCCkY22wq2Jabbm29JZNUwQW414AAJOiY
ActV6Bx0M6J2IT80ZECWe9Zd/laTUDdtKFV4oLU32lXoKaBVely5FB9s852l
1ou78fYhDQ4AJ9xwOPKlY3II+vgcipI9s2R4EfQgJ4b8OMCVUJhx9oyHCJA2
YnqKJeQgPzOl6JBYLGKgzZdtvXDEW6LRGFJeNw4IWGUiIEhDCwu8WaljQ9Ew
hQLs9GDqT0RNw6eT/NRgQgagtRDioKd5ZcJpLp3QgkIIldWlAT3cp1F+bvGG
kBNmjgWCgDgmWBmPfSVIg3OJ3pSlq5CBhABkePIzRUB9SmXDVB6KZkJpVnnV
7lfTQZbiQgrKZkCLGIAZ46QvkGdpXpimmaO/1P6FAKgxKSDaMzgFNStJJlDT
3apA/sE6mnihTTniiB5gpQBC/ViJkHyzoVWfAbnF+NYNjpkgQkjPZmqctBAU
nCNCzhWKZISSjWVnZPw4MW64mrXsGWg0VDUVoQgpoNpMBqpGLwI3zNfil7ix
NSlvcf32X14gCDzwMzYniDCQTt+0ZE00vJyaA6wOBbeGmp07FcZcsTsidWmD
HNBWn6YYAwj03WcsBlqL2Z+lNgocwZ40LIdzgwQFxPCeTtAJMmYnjPvdZnMx
ypTe6XEmE2fUSZXQQQ/Zq0J9+h6RbNf/Phu2wC1x2ubZnzp2k5JFxnDDknyu
2rBmXSUFZUubbXzaVSmipthAgNNQNYtpofyibm/FJVSz/mDLXNyaZePMQLYQ
yvmY5uQ+Nq6en5f351LtbVwZQawZiD9kYTVEQwQuoE/2viS7t4hpK//Si9hi
gACyeQphBGkB23BCDSJhJzVEcYID4HY8x9jNdy8wjIhIxBX9pQYh1kERbFbk
KMPdRlLJIgDzBhKg26XpcTVRTl9acq3EpIhJkLEMqiJwEAwBRU9Ec4ysIFCD
CbFENINCHUGkaCWBfIwhCBmcCLIHpnyFiSnk+hoAxTammu0uVDNpj2Ia9gwH
DBGDSXLVdzxoOlplbFBUctdXUKgYB30KIiIhFtZ0I7tkfagmLquh2IjIj/Jd
ywiqCcj95OiZCFTIOnpCYp/K4WOZCbmAAFC0H3WyNBWv/GR1r2EUBtSiPS+K
wIBwWVbbwGbDNLUEMuUDVQJaAJEU+S1Lg4vLKHHikl2ZRiZW2koEcqSC+LxG
K2GpXoVOiJCriUAlL8gPR95yhLvIECQxwECWemA7aOGIcB/TIV8ikJYBAKEA
AALJC9ayxbXQap3zpGfKdpO47olHTQaMoTfFJMOWwMVZL3sWX1zgAtuJrzgR
YEstxbbOtRSnoUkgATN0EICOevSjAZACSEdK0pKatKQPaEJKm9CEC4j0oyJ9
6UlhGtKaxtSmOPXoTT0ahQMEAgA7
}
image create photo roueimages/roue3 -data {
R0lGODlheAAXAMYAAAYGWqamxlJSlkpKtgYGckZGjgYGguLi8goKkgoKpjY2
zmpqpt7e6h4evhoarv7+/gYGagoKnoaGukZGmtLS4hYWrgYGdgoKtgYGhpqa
zgYGZgoKlhISpgYGfkZGogoKqlpaxgoKjkpKugoKora2zhYWbgoKmkZGpgYG
YkpKvg4Oqu7u+gYGehISugYGbpKSzg4OpkZGqmZmpg4OnioqskpKxmZmvgoK
ig4OokZGrhoaug4OtgoKsgoKhhYWtgYGXkpKwgoKgqKiyiYmwkZGsgoKdgoK
rhoavhISrjo6zhYWup6eyg4Ort7e8jo61hoawg4OukZGtkZGnhISshYWvv//
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAPTSAihSmHQImKKTExOSk1ipKTiTWWlgMTj5KHnZ2FoCKHoSIDREQD
A1GnrKw5r7BEkEA5jY0nuB66ulJSJylANQO9OYUvDw80OxgRSjxG0EY8PBcX
RkFFJkfV3Nw7UFAtT09KSjo6R0dILgQRSQoN5z5TTEwqHwn5+SM71h8YIwBi
2BAEAwYECBGEWHij4Y0eEHvAaMHDwQxs2IoQYOfCBQQNH12YGJJk3Q8UGBJg
EHIAw78NMEbIlIkvwYcIBCDc4PChZs+fQFXc+wCjqAkIKDrMgIFDZoQIJqJG
NWgwQtOAzAYiwLDRoIGvHcKyYGGBLAECLEzcm+ERwkcN/ih+yJULQC4KAxEq
sK3rAsEGAQeg3kAwwghAwzbxHYXADF9PfZD3JZgpcwOEHyw2PIUKNeqGgE4j
GMy3YQOGEAZvuNDw1UDYDmPJWjh71gICmxE8atgdl+5cAABQpDV8+YcLgwUC
i04p2SaGCyotMI6gr2bk5pMrX8682cRTgFOxQsVn0LRCDKo1BFm/vkjGIvAJ
FHFhgXnutyh6/wBed7/wDZO5AJwLCyUXUAiE/YOYET0ZEcFHPUTwk00UWjdZ
dpPhcNQPFny2mUoRBKSSSiYYNAI+5qWGwWoduPZabGPJd5YBKRmW025w7def
jidlQ5xcx2GQ3HIu5fPYc8+Z/uBRhEY6dwGS0F144Qg4WPbfZiGKmJJAWBkR
UInnhdDDaq29BptsZ81XBAYMYtDBWznq2B9wGmQWHV83CHmATAj9o1JPz3xA
HGMp4SPNM9UwN+WUG9KIZZYjjMglQORtdZpBLLrm4lhlWTAbARZ0AKCDusGl
H139CWeCERsCQMBpyZVo0GP5YMCDkSN4dENi5D1pzTOSRTrThkWEuBmXJUoq
Gj4BbbBVCHmuxh5G71mgUW1FYsACfnHxh2pwdnK1X5AGjoBQYYXyUKRhHo2W
wKG+KiqlsCNY9oOoI0AaKUAqgWaiYaZdil6mL7JwZhGfEkDju/fheCpwc6Hk
0oN8/hV4gKzy2gqNc9uGUOE/1QRq67w4NGUvd8fmu2W/Au3j7EGoYfBRmWLF
Zu188rEZ3Uco5AixnCd1YIJKBPCF3J4jIGjYCE8u+KRbswqaqLrSWCfTPv9s
t8E/TtG0pb/8qnRQaum1CJZYnib8ao04uWXqbzvW1cPEl7l6NMY9RcogDz+N
kJPHNT13QaD7RJrdTBhYgNlnToEX4sotVwYzVQSo1x5GRbCAcMJrGhGdw91+
a5fQRI+LWgFNyITgiS7ZytygoxmRaAJPRmZ4pDCosCF3XD8u6ZdYyTT2adB+
ZKbBBm8O6lmxt41UfnX9rKPE6kJQ19rJCZRSpD8B+8+2/rv+9Jy61jxXeAJX
BUQAZt4Ju+UHWgYkGnTNWoopa2fDBlt8tHX+OW+h85Z/CqIuFgzodAcQG2EM
o6BCPakjs5Kd7JrGN2GdLx+70wzXrgY58TCHKguBFgGSsinkcQph7GjejfKD
gujB7Qca6EAEngakoz1OUYLKYU+0pZPEQMkawbod+gLigh9kI0STaRnkRsQs
DGDsftPChuY4tTy0ZEs6AOTPz4CTFAAlrmJ6Usnq/tFAmzwtSIdClLwgQyUc
YPAyotqg136XFWGZJoQNIdhYOhCEDqRtNilMyZMqB5c4zSlVLJihzMZlQ6pU
B1CO4WH4dugr8l1QJk0RV2YQ/icpP00Kfk6kClVY1KIzaS5zswEkC9gEPyyy
UIuoOkm4FGccBPKJUmzKpRllxhjDSCNR0ICf4Q6Xjx8lEiCUYR2J/EUhmDFE
NSQ83v4QthGFHWZFDvsNj/wjtCdpYEDIWYFB5mai1nlpS6upDygdEz/weEYq
i3wVVkRpmoCJUnsY0FRYPgKf+HyqIwD1SJCIZqoAxlIuBLiBaIr4AwjMbQEP
kEHlIMAOmxksLIqrCwSQ56mLvuiP19JIXTRQloJ9Km2rdE1HFEZIHNklPyeZ
i0ynhwIXsGB9AtzPNvmDguVNTwMloAAySCCEohr1qEhNalIDIIQAOLWpTD1q
VJ0aINWmQtWpNpBAViUggQxkQKlGrSpSmSpWsJrVrAx4QCAAADs=
}
image create photo roueimages/roue4 -data {
R0lGODlheAAXAMYAAAYGXqamxmZmvlJSukpKrgYGckZGmv7+/gYGhgoKkioq
pvb2+goKmm5uygoKqnZ2zt7e7h4etgYGboaGuhoaqtLS4goKnpqazjIyfgYG
dhYWsgYGZgoKogoKhgYGfr6+4jIy0hISqkpKwkZGogoKpg4Ouj4+pra20goK
tgoKikZGpgoKlg4OpgYGYgYGeg4OqhISokpK2kZGsgYGgkpKtgoKjq6uyp6e
yhoaujIyqhISng4Odg4OogoKggoKskpKugYGagYGig4OtiYmwkZGqhISukpK
xkpKvhYWth4ewqqqygoKfhISpg4OjqKixgoKrqamzg4OskZGrioqqhYWuhIS
riIiwhoawkZGtgYGjkZGnhYWvhISshoavvb2/v//////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAHXg9HNDQ/RyKKi4tHRCpSR0ZGjIuTlJaXRgQGMomKR6GiP6SlpaGl
hjQyMoasrzJSsrOzP5NYRLmPKiO9I1rAWlgiRkdaBlIDHwcHDVEvWSxFPk/V
1D4o2U9LSytUKNjZKEJCJUVUVFcgSV047u4hG0s8IFYaRVwvLyT8/A5Z/7Jw
EYIii8GDCZYEyZKghsMUEFN06NCjB7cEVEr40MGNWwYJIIGI3NBiQwocMYYs
KZmlCYQDOTisQLCCBMAsT3Bm8bHTgoQNHWA4GErUgT+i+lgoJcGBQwKSHnhY
mDqVgdUVBhlkscDAQj8GK1YkSBAEAcgZMzy4WOsiQ4YC/m8LyEXAY1+HFi0A
6M2rt6/eAjU0sHCh10MHJQcUcKiBIIEDnkOfOKg2OYuEFjMsGO0HuV8/Dvw4
8ODhdAMADyu4XrWaVStWgxY4WAhCuyzFyx5yq23rNq4EkZmZzthAkq9fvxJS
jM6gd8kMJ4ktME7AT2dOyVkKZtgwg0HR7JI9h27alHQWqKmtat3KGqvYsU2l
S+yAAMGMyx3zcwMit8BZCzghQBxexx0nwVY8MAdABs8dMIVBjfEDWUHVVMPB
Zd35I1lOm4nHVHwWPAXAEl1NJZt6K7h20Fay1RbERB38lNtaboHkln8gJcdB
QcORVKCBNchWgF4uzBAATFkw/kATUyT848NQOJHgUws9MNDkUeF9xhQL5DV1
XguoqXaVe1iBNVYWoFkAEX323beBbmy11V+OEiDg3RMCEvejX0CgyYGCzkE3
BQdZNNbUlddFlgUQ3HnXJE8k5CQeaKKR1pSIHlilGoBbGfSaQdR1SptE9cmo
1m69zQmcVxzkadyegAlJpJEOLjZDTQZtBuWTWWzXnVGRbdghk0yNFtt5p5UI
4ImsgSWWQU0ulkJ9E7mpX0dwzSlBdzj1+OqPEgT5p14MQqcAgAsN1eROOEVm
ARCYebUZpB5++OGl6Cm7WpIrjmUTuqTOYBZ3at2YrVw5ApFCtAISuKdeB0ql
YJFH/p5bQwdZSKkTCkX946uVu2bpIaWyEWpapuqBlaRWziYAapo10EZfDzPA
m9ZuvPkWEgIAPuHtw3oBEYRsCpZbaxZBOEbUdRlP9m68TUqmrmegbdmlTCen
dmyn/CLk8j8AQlRWqW/Cmaq2dXLAU49AQ1yDVEOOSKtihSYgG0BRZ5lFAVR6
VZ3I4xVLGlVfoqYpiOyFZVANdjtA6IvTTvTTtb3pXKdmDgRRXNsSDD3ugg1a
nAK0TTHt7k8z2EQZvZOGFltTK5g2w1ZiqtevyzfJNzZaptKYwRJx4ShSDxbk
NNy34Ga896wVA9hB8TYBO9kT62LIQTU2UTNs4FwSblqV/lUliZWYoDKeZhCR
1zfwjKgGTyfPQzXcNgDh8kACYaAL6lRjPDHdz1Dv4k70JMM6Ld2LPIXDlfK4
5qlPceAfMaNNfdwEp5y57zfEy4mA5ke/m8RtCR5oXt380SQnqWtvG8hgyDpE
snuZCFmouVrtsrICh4AmSdOaj5sswg3fGSxHdgqIj9omNNLgz2iDYkzGHpgx
6gGLJzbTDGW2R7WrmQwzK9PUirIQFk6hyXG1UV+MyqaWJdAIbdu6HgmGw8Fw
/SNuFEvMYjrwtXU94YEl7BVmtAI4Yh2KKVQ51sm6wp6unOggDDhTbGbDJvUV
gIwuAJ7BhFcnKxnEYUBTGGiO/egBQaGLAf6T1FAKwqgOPLAyIxuPFb+0BADZ
hIHi86LdZBPBCbopLWypHNoWQr0ZIC950IAjrUxQKA/0ACuEyhin7iaBBS3k
Jv/ggBZV8ylnISAvGUhAWMIyFsUhbSwuQ8BBENARD+ynBdyQQH+EJxJGbcAF
nfLALwvEHYMULQMnOIATdgAv4hQgl5E0Y1v6IgEQmjOSuZRkWzoSFyDsZZ1o
nIFb2ukBubgTL/Ns2wYy4IKMFkgCA9XLBjCwAGZUwAkoDYATAsBSlK5UpS5N
qUphOtOUxvSlN5XpTVkaAAFMwKcTmMAFbgDTlea0qDl9KU2TylSbujQANihp
IAAAOw==
}
image create photo roueimages/roue5 -data {
R0lGODlheAAXAMYAAAYGXqamxk5OxkpKrgYGcv7+/kZGmgYGggoKjgoKplJS
wvb2+vLy9ioqymZmvgoKktra8goKnhoargYGanJyvhYWps7O3pqazioqgjY2
jgoKtgYGegoKgoaGur6+6hISpgYGZkpKwgoKlkZGogoKqkJC2goKboqKzhYW
dgoKokZGrgYGYgoKmg4OqpaW1hYWqgYGdgoKigYGbmJirg4OtkpK2gYGhgYG
fkpKthYWrq6uyg4Opg4OogoKsqqq2hoatiIiwkpKvhYWtgoKhg4OnqKixkZG
skZGphISuhYWwhISrhoavgYGig4OmgoKfkpKxkpKug4Osh4ewhYWukZGqi4u
xp6eygoKrkZGtg4Org4OukZGnhYWvhISshoawkpKsvLy+v//////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAFBR4KUDg4UEEhi4yNIVRHKkFPT44hlAKZlYuZAosDW1hBo6RQpqeo
p6OGh4dGRl9fr0YqtbYqVLlUWJdBuUfAI8JbxMS8i1sGFAuCEC1cPEw7ND1X
Vz3Y1xrbV0MECF3b2zRa5UhTXF5eVSVTS1NTQvE8IE47UkJZLS0kCf4pJJgk
EMiEiRBqAQsWfOCkIIIYEGMMGcKBg5OKTmJ8SNKDhBMTHwnImACC5IqTHHLU
qBIDAIgZglxEaPHABgt/A68k6EeiZ8AbAG4Q2dmT6E5//HbkSLAjRQIeUB+A
WMGBhdWrCplE0Mrk6r8HYB8gsHGAAIwbNzZsgLEWBgGR/jJGbmiShQQLGScB
rADAty9fEDEqvAAKAIWgDlqH2BDor1+CK4xJXElBAMAGETgzZ3b6LwWPCKAj
IJh6w+rW0xGsdmXyQGGEgDFskLVRcYIMtWrdvo07kmTDFJCn7vVLHMQB0IRN
CDrRbyLBgDp7QtZAIkLlDUwcO9acgnN3Ht1BMyHNgkkK1QrLlxchFgHnhxQP
yLf90clHE/hhxJ3gWwRAJgTkRZxfIAwB2gZ8KVdABwMxYdMVPfXABGSS9SNh
gKU1phl33XVHBGgpPLDXEFqlYF546jEhQlavFSSbDRRxEFdabOWmH28kwYCZ
hFMNWJwNEfCQ3HI7OUgQhNZA/siEBkfBYFl2AzG5oT87JNAhVOJNsMIBpmXF
1YrsifWAUyQgMJF8B9wwUlo2EmCCSLblmF1AAfro1wpARoAgAAp2AFoMx0U3
4ZzRSSgDAAdMqAETEjrWnZWPdhhaiHs5YaJTKK62IospbOVgfBzYZiMMbrm1
H3+XJcDjcHa6ZGAKeyp4QnaAHimZkhJ2lICTN0xIQjYB9WAlTph22OlWIATF
QmrlFbSVeg+wh4B5E0okGwcHyCiDffc5IcNu+4EAw5wAsmonnizAmuBy3TnI
gmSDPvZrhAACwAGj+M7L3T/gRfChiaRtdal463kpQj9bwYjtfLeplZZZIsGJ
6o7j/pnrY4HIrbtgAhEoBqV00PEU0AFBMZrkdpmBZyVU4SG7ZXkNUvusCGEW
1GFsMKIpalswvAnXqeMWWWerehmop8azmjiECBQKdI1AESaAV6IJLCrhZpxV
KWkE3UmFKNfFfuklAg8EtNWZQ8g3Y25q+XwqCJf9Oh7Rf5U4ZAEnWCnbnJJF
6ZM/TADVEITVWDOQsJDyAGnLyAYlQrNczUxza0y4109BaVcU6gTcmkCqbqdO
EHRAh9INwBDLxnoY10xw+Vh2Tx9lTQp4LYZvoz1h+k/iYAO8wg2Xtvvs2NOa
baZsaMa1Vps/jwT3wRJOYPoEQKZw96wk1KqhyUddjmBD/hoUbjjWxR7LerJc
mkitiRHQzJ5DlivNAVlprqmWE6S+Bafz4/bEBF50W4HRrteuggTkMaqKzK9S
AIOX5Yoxu0uB1hZ3LBOd5AYDcQq1ILcQygGncsjT2W1ItQH8gKs3E0gVj0wH
Apuoi0+H4djeIJSTJfEEcAjCzjWsgbKVRQo84TGR9EpDEIIwKzULQQACIjAQ
FsCIIjc4AH3whxv94Uhc5KpMAAeItHRBJDI8bNROFlUZ4OGLWMOiUrEmZcEt
tYh1nqIWWBRyFCYoRj4cUFMKmRcx/kwsgRa7mAv3JIMYKi07iIxOhYqCoF6J
j1HE6s4OFLc18UwlUeq7lGo8/tWeEpnoRfJR023w97ndSExcmCEdC1+lOrxp
xXY9WRRkINMYQylLQjwclnciJSkNXjKD7HNNVyjHmg45iH7YkgsM7OOz3fgx
hQKRUCAH1MKMAWACgvCBeQ7gBNshMiAHXBYTnEQ9RGZwWVdJ52pYABYR2GAv
MKDcitq5GtZIRD4FYQG3bAQCEMBFf88kyVRksJiBJMt0l6kXADAgCDBkIEAr
2Bla0MK2tPAlohPFzfI+x9G1QIwA0rvmR39mA7Y4T4/8GclJ9jLNABJgAyGd
nln6EgBBFIABOiiCTncagCL09Kc+3WlQdfrTABi1pzw1KlGFWtSh+tQBHYBq
BzrgFgIrIJWnThUqU6+q1a56dalCtYAgAgEAOw==
}
image create photo roueimages/roue6 -data {
R0lGODlheAAXAMYAAAYGXrKyzqamxk5OikpKmgYGckZGmgYGggoKkr6+0iYm
cgoKnj4+0oaGrs7O6h4extra5goKsgYGagoKogoKjgYGfhoasvLy+hYWpgoK
qqam2goKgmZmvpqazgYGZhYWskpKtk5OxgoKpgoKmkZGogYGekpKuhISshIS
qjY2fkpKvgoKlkZGrv7+/gYGYgYGdgYGigYGbhISog4Oqg4Opg4OsgoKhgYG
hlZWxkpKsg4Ooqam1kpKxhoaura24goKig4OniYmwhYWtkZGpkZGtoKCtgoK
YkpKwhoawoaGuiIiwhoarhISukJC1goKtkZGshYWukZGqp6eyg4OukZGngoK
rhIStg4OthYWvhYWqiIixj4+1s7O7v//////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAtLVw4RyYgICZHi4yNjCxDUSY8PIyUlCGZmZWOi1FURCqiKialpqeI
iCYqhqkgOU+xTyy0tSxRuLlPR5VRQ78kJFTDVAagiypURYItFw8ZDzIwCxHV
1tdO2U4IMRsiV05XV1NMUFBYWEgPD1oMW1Y9UEJCJyc6Hhs6JzMi/SJVMEQE
hDFwmpAIVwQSJPhjw4+HP2zY2ECxYkUgWJxE+GFkQ4ECEiR48ODCBYANKLYE
sQFgWYsAC0ag+HFgRT8YVfpl0Jlhp4gbAF4goNGTJw1+ImgoRfFhgo4JC4As
WIBgpI0RWFesgDECxgSuW0dsXUFjAg0EPyhEvFFBQoES/i9KFHhRIIbdkSUl
ANGRYcILkwACCx5sA0OWCgBSCNKwAOpEBCJ8+osQsB/lA0EJ6vQnYgLnCaBB
T43Z1YWHGzBWLCC4uutWGBQIxl4A0MaNA7grVIjxli5d3iFFkvTgFWCBwcgD
ezgwtQQABYsbTzhQoTJOgRF6VumZoQrQAiuKcv7cmUZnqKNXj0TtNWbY1FsR
xP4x7SbuAxuo86Zo5HfdGCGNJMEPE+z0QnLJHTDCBM5B14IGAi1wA2qRRbad
CJT50xdmLxC0k2TneRZaZ41NNcJqpv3gXldcwYCAbAzBNgFANFFXwQYlSMDb
C3R9BGCAI8UwDWXHISiYC8wt/oCYgxp4JsJEA2WAE07afSgCZgVQOd6WoaGX
Hgwk1aQVVi2u8CJaMNDnVT8UUKTbBm31tgFvdP4YpFcGGjlYkg0uJtAEE8LA
XRUXSrbTdJkBtGU/XeoQoonqLbdai64R9GKaaVKwgJQw3JZbjnP55qOdJEmg
KWUx6BkYkidu8Nxiod2wgaACAURZBBUWhWVAEeS0KKMifilQmKrFNA1Yqb2o
5poCyfrmBm559NF/dgnnwYCbilCkqgou4CqT0gXKXXfbZbjTZZlVBqKTXZ4n
nUIl1dRaa5bCGBEFotWYXwk5xsBjj3UFJ5yQE6CqKgDLndjng1NNN2ut10XG
3T8Z/nxHJQy4kseoo6KN9pUESK5AZldiJQvbQmu6iBucukU717QBB+gCwTtt
q+dVE3zrp1fi9pShhhVemRnF47HrVLDpfVUSaqvRm1q9D9lAAaOdUmejjjxO
Wy2AI137ImUSHMzqAgs32Y+sgl63HcYYB73rr52FKKKXXoHpwgZkNu3amQuh
lu/KFbncY7VAXjt1TjYbeUOrr7awQ8Pi/iMooTkVtROHHnLpz4iepd0YmKdx
9RWLr72o1kP4AnSDDdTx22/WWv8okgtZsh22qmOXHRCgs0qZne9t+y4CYlkS
yqWTOuiwwHn+TAVDvFl5RXJ8fft9E+u5xQkXwHbZOfPh/toeDECSOj8opYQK
ZkAZuVU4gdN2lwfVFdxdiijlgseOtOykpbt4elrSgUH2Whaql3WvcEJimwcO
ljCyNW4HTmKLuiDmhO5IBjMxgMy64gYszzRGIFKSEklu8LkJdMVYryHI6qYB
GhhYpATaA9h/ghMkyADEOQfjU+MgNI3brEBRlMuA+9ZHGcR0iFbkaZSIFIWe
593tK19p0QTiQ4HTycgx2YOTW/41KhqW6gcicB9g9JSwCSwJVp2hjqB2MiWA
aCSIWAoPiDiYPKjMTTQnspuC2nOsFqHMBg8J4ABjmLUDWksCxcnAgXKosAf+
iXWrsRCherU+f2CQWRyU29FI3MQpeLnAb6yZxmoQIB+1dMorUBHgALcoQ0OW
6kUAEd/dGukgB7zmAC9gj2Z2qSiCYMYD1RnIV0gjFpFpRStc0RsMAHA3+Lio
Xr68T34IMoKKFGBOL7hW90jVtZIAM22JQ1AJSJmqATCDABXIpmkKoJt2VgCG
FbhPCQAjgRLgaAMvwBG/uAg7Ln5kgQijVgxuwC+uVaAAXbuWjsQnthiUIFXi
88BHXGCEBDDjJQLIqEY3ylGNSqGjIA1pRz/aUQ4kwaRJSEIHSDrSjH70pQKA
qUxjytGZ2pSjEBBEIAAAOw==
}
image create photo roueimages/roue7 -data {
R0lGODlheAAXAMYAAAYGXqamxlJSokpKsgYGbkZGhv7+/gYGfgoKjtLS4goK
moaGusLC2iYmygoKrgoKnoKC0h4ewgYGZgoKohoaugoKghYWqtra5gYGemZm
vkZGngoKpr6+3pqazhYWtvb2+gYGlkJC1gYGhkZGphISpgYGdhISrkpKuhYW
ZgYGYk5OxkpKwg4OpkZGrgoKqgoKhgYGkoqK2s7O4rKyzgYGagYGghISogYG
cjIy0g4OrgoKihISjg4OdlZWtkZGtkZGmg4Ooh4ephISsgoKkgYGiiIiwgoK
thISug4OtsbG2goKegoKlkpKvkpKxg4OqhoavkZGshYWvkZGqp6eyvr6+kZG
ogoKskpKtg4OshoawhISvh4exhYWuhISqiIixg4Ouv//////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAGBhBMKysnUFBXhoyNjidVIz4rTYZNTSqZmpqOjExSVSdMoyelpqU+
qaqphSdXA7BQLbIttbVSuLlSIy2UKy0jI1VVGsU/PxqThwIygjE5Di5FQQgK
VtfYVkZG19vbEyU3ClhHX19HXFFPWVkREV5FDSENWB5CQiZOTkA0NRMb//6B
GDjwgYKDCpYA4WIlChAYEGEgeFFBxwuKFTJmVMKR4xIP3BDcIEBSQoqTAABU
6IKDghIUHwyoUABiQxcgSnRMIGhlYE8QVlwIdbFEggQRD1xs2KBUKYsNTqCy
IGGhywQgD7IaHJJCwguaA5ccXDIEAQIdaHWI0LGUhY4a/jUOyMVAo4TdkSRp
0DB68igQpQdSCh4M4KgNICUAJDFw4sFOBRUwwHBhxYERoJcdLN3sAobJGgo2
i156dYLp01rHek5RA8aQshEhEoFBhIgIES9gOJ5AUUmFAxjohht5Q+/evSdT
VHgAMDDhwawP3gDAwMCV3RtESGY+YaiLaCCiKd1QNLrogP8Ant454aDBhDBO
1iBY1qzZtBZxIzANgkhc38HRcMOABIykl1EmdVVDUhM491xKKYggHXWMKbDb
Czml50JNDoTH1FJCrQbaBh2SRtpVQJQGUFYIKRCfcmQNEdtsstX2QmsBvSAX
cEoINyBeBBxoEgDKhbZBYg9C/liBhdNVdwVNpmlHxEBKAbVhT9GAKOJAHVox
GkAslFaTVgYt8eIBILhWX1loXaQDERQRIRACOx7QY10EkhTkXnyZt4GDSYKm
QJMGMFETCLwdMMR34WGZ5VIlFnVUd9GkZ1qK6l214nsHvVjDEGbGJhttuN22
H0Ai/AZcgDcMtyefQ0bX4XRJEinhAwRQ2Fh6L2Cg03hCbRBeeEIBJaKRJm62
HmosqiYfDDHadx9aRKglgm6oZoSBnXT9SKBxCMqnwIYi1JoSaA8QeoIC7GkH
Q3ccEtvUdz91VcFOHaYHEBAppjgBC4haaBAILn4W6oy00WZtDTqZpuMBkR1Q
AgGu/urJZ3KsJbUBrUlGN6iu7NbUKxHMEQtUeN3NC8JnDwD1KGlPhXkVCC4I
3LKL8i0RLQL1UWvRCyLIGaVckSlRgoB50vBqghIsKSygDwqqLqJRKjEEzR1+
J1SJH24oX3eZBpQppiu2aOazoo5abW2mCjuBCHEFF+BweAmJ8YJK5VprkbhS
yMRub2OAAGfCdgnUUsMadQCyJi572qYHgWCmwTvfh0C1b94mJ8061hAZBkfT
bbEEyIkrbA3mAiDhx9WdAEJWgb9LorzBbtBTTyLu9GWY/vY7QWrOKpd2wmvf
BvQQpKUaHLcChlPg0kNKgPcEEpgbXd+tD4SqEiQLhGWw/ls70KHiLXupbMyX
mgaQha9L7qnOr5lVH+YL7xflbz2ymueeCFYf3YaoMxe6CMUE0zimVzrY0FC+
JJoXiaA7j0PPvwy4E8e8zllNGx5tLmebUtnvASKo09x+ZLGLKUhj1dvbkhSQ
KydBqT8HkFOxOnQ4pgxrfBJYnIckuCx+jallLTMTkWoAP1DN7z4dZNhpUrWq
EZbQbkMMDQig9hyp6Qo72hlcTb6TpZ644DJKWQ1SxiMaf/HOUsBLk0kqEJs0
zYaDcPKgsB5ABLkAqFsk5J9J9rignqSwY4JqIWNaZpoa+ApfNOOMlbrkKXaV
cQMy8xd/5njBJRAhZ2QRC2zS/sK2GohgPy4A4W+CYxc8AQlcKJHeuEAQQEDS
hFA+AFyvYMAUYGENRJRRoyoBwkv1yMxxBnlPmk7CRhgQBCI6IB7QbvSuoW0r
f817nh77NJ8OpcB66BJkDxBgzCneoCIEK8jvgPi6lsUHADdAABBbRLCE6Ax+
ZpIRz2AggmsKDi1IrABc5BI3JRxATaAb0HBI9ypYJYdIJVAnCJBUKwzIiAYA
SIABOKAEgV6zMK3iiF1AZ7Qe2QmiKaGY0ewSjpGWwKTE0VNJUkK6fjhvLxWY
ZnIElKCLpu45EsCA3syltFwVQBAGuEAAhjrUKRD1qEg9qlGTytSmKhWpGVhA
VCGwG4AOLNWpTzWqVgOw1a0itatcDetVAzADKhggEAA7
}
image create photo roueimages/roue8 -data {
R0lGODlheAAXAMYAAAYGXqamxk5OwkpKrgYGbkZGmgYGguLi8goKkoaGugoK
piYmxiIixmZmmh4evgoKlv7+/tbW4hoavgoKmgYGagYGjgYGehYWqgoKng4O
igoKggoKsgYGZl5expqaxnp6qkZGohISqgYGcrq60hoaajIyugoKqkZGqkpK
uvLy+hoamnZ2sgoKogYGYhYWog4OqhISsg4OpgoKhkpKvkZGsgoKigYGfjo6
1gYGdhYWrgYGhg4OskpKwgoKjmZmvhYWtubm7g4Ong4Ooqqqxmpqng4OlkpK
xkpKsgoKtkZGrp6eykZGthYWvh4ewkZGpjY2uk5OxpqazgoKrubm9g4OrkZG
nh4exuLi9gYGikpKtg4OthoawhISrhIStv//////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAQVx0zMzw8MzQ0S4aHjo+QTiBJM0ZGPEZQmptQAp6OhYeFMydOKDMo
qaooWa1Lr7BLWYZZR4qKSbm5J7y9Tr9OlJdJIMVVVQXJpoUDHhAQJTFUUlJd
JTU9MUhIG93b297fGzUUMiZbTEw/TFtNVk3w700LN0wbO1QvLyYKGBQ6GBSY
qDCwgkGDEx4ofIAgiJYNDEJU6NGjRg0ZGjJq3JgRhwgEXbYhEEGAAoUWLQAA
aIEFhgMhHIYAYTEBA8EYLnAAVFBhw8ANPX+aGGqChQ0AOCrs2ycwxosYUKMK
CRGDpk0MEyYg4MBBR4UHFbJiTYigogwdOgz0YKHgQgYL/hZsWMCBgwDJkhQ4
pFSpsoWNIAJ18B2skkIPIQ8oEJkSli0LFj3iYpAylBplE1IEKtgspcLRj2w3
iw79mAUG06Z51vxaAaWBCRUQHDSIBS1ajQgeYzBgQy5cHBzsUijJFeVgGw+A
GiA8uOuEHi0aXGFRwWbRyAYIVuh8eWhQogo+Vwit4HH50iyEYAiiG8PYrBX0
2pDNkCI2ixfRGshOXcFGEXQNJ4JJJumlV0p+PTCUYMz1hQVsHEhXnWMs1KCB
ASxgdllnQ4m2WVEWACACWB6Wdx56qYV11UEtdMUabRVgIaMOWGAkgwEPbGYU
XThoMBcFdxVY3F4AZLfZcg0C/sBBDbFFeAVP1hkUl3ad+USNUPyUF+JHWY5W
nhBCsBCEe6GNVxNWrbWQXX0V3ZdfWheO95gBv2kA4HAE5CkkSgi+NhAWSSrJ
ZAUUSIeaQCb0oIEOGV4JVGWaKQAUCyEmVUGJJp6o20CnfQVbfBxoANZsMcqI
xUU3GoAAp7zNxSOQJO2pF18aEGRCiElSMF5rDRxgU3/UaZDUQJ0hQU1PGRKV
2ZYK6mhiaWGOaV4/7lUHW4s4MlRWm/nVhpGqArGQEVx2DpgnXnnxuVd2nQGa
ZAsy2BThAWzJWaEFjGr46GXkYVZBpeN5SWFp7rkHZXUH6ZXdbFiUeqoMqdqg
HQsG/vToKg5AlkQcV7OuZAAGQ+EQKAcPxtcrVtQVZEF2Ql3ZE6I8IRGeiAF7
yFaYYaKWcsHVxqemQtvaV4NtZ+3Xg446WCCssAOKYG6B6qok8Z+BsiTvyROw
NZSFGJoAFIfUACUQZZ1tuZaXzz6mXkD1rhZWa6GSauqp+WnUA8ie/fbquUIW
19fHSFQgcq4yQHgywpsJPnVlx/qEWZZDiUeeeSxUlbNpZP66IrZABz00WkUb
HRqdS9/JN9R8Sr2dCUg2aLXJV1x66UAWAlQlQVIEXtSH/FAqYm7OPntewQNx
uqLPtcpNdw01pmXD2eLq7RGsw+XF8V5qTtCZCIFSMKiT/qhtZxAOEm84kLFS
ZMjTo1vWTFp6BJPJ1q+7+qxqWUHPGHqcj1VQg9I9CpCe8GIgItlgAgsaWQ2o
U6jYZQ1RijoI2DIDOcsAhVmXCp6JcuaegZQGYRDigAxIFaOLDA1iF1JLZyh2
sekFyXoHWpf4KDAyFvXKIOWxyb9qhYTH9eRlmvnOZxAQrvMIQQGXK1j/aFI/
+z1AKxTpQYx0wDzbxEkzGrCBsHjEgRcWp2MAKJ8UWsecFiyQUNIxQdYIogBF
YYEn3JGC4/YVuM4cxVKOGY3aTuMeDhmEOmhqUa2ocxD86ABi+xHdQOjUQjxV
z3oIWokGtFcBAnQvYSd7gE06/iO47GzgMj/0iWY0VAHB4KBZz4qBjlBDJh3J
LmEcyA4UpVhCKxogi1LMjAyUZie62AVdX1yJ6ihzlHedsYF/FM2/DDJBUT7q
Q4ED2KXyqLbhyY8ngAxkCyY5G/zQrWhanJ1neOQqPOWJA5DE3gE7Q8NcYTIF
IUCAbLKiAQJYSjuXItNsTGCTo4jwUlkJ6EIGKpu3jcogOkAJcraVFt70pjc+
UpoF3laucxEAnTCMIZEI4JUKkJE5yWvBByCwgjwBqC4r4QAO4iIXusQFYjKQ
i1z6QgEe0UUEW7yp03xpF6cFSWoBMokNBpRO7AUHB0QK1LtEMFSltgBIFCBB
BJ4xMYIAWPWqWM2qVq+qhK169atctaoPEjDWBCQgCl0Fq1a7ytYAtPWtbsUq
XNuK1QNAIBAAOw==
}
image create photo roueimages/roue9 -data {
R0lGODlheAAXAMYAAAYGXqamxlJSjkpKmgYGckZGhgYGgv7+/goKktLS6goK
ppqazsLC5iYmyoaGsh4ewgoKlkZGmgYGagoKmhoapnp6ugYGivb2+goKtgYG
fgoKnmZmvgYGZkZGor6+1k5OwhISpkZGqgoKqkpKtgYGegoKogoKggYGYkZG
rjIyxs7O7g4OpkpKuhoaqg4OqgYGjgYGdkpKvgYGhgYGbgoKigoKshoasg4O
tq6uyhISkhISmg4OokZGtkpKwiIiwgoKfhoaug4OssbG5g4OngoKZlJSwgoK
jkpKxkZGsoaGuhoawkZGphISrtbW4p6eyioq0vr6+jY2ygoKrkZGuh4exk5O
xgoKhg4OuhoavkZGniIixv//////////////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ywAAAAAeAAX
AAAH/oAHKkUxhT2HSChILIeNjo9HRyMdHYyRVZiZVR+PPYWfnyEhUyylLFM8
qaqqSDxIi4eur4koKKK3IUu6uyg9Rz0hlB1ZEcVIhSwVBxc2JSI3UlI1KTk0
EzcYGDXZ3Nnb3DcKMzAvQE8PSkoPVFpaD+8PQEA+DVc1LiIiCgolGjM0L0S8
eDFBw4SDEyBAQIBgIYIdNaZVo2Gl4o8fJi5q1EiAwAwILrAZmSGBAwcAKD8y
cWHCAYMSEDQIlKKAwg8YCPYJrPGCZw0FIpzpAzqhpIEJLhSsUKp0xYodJXbs
0DAERIkXMCEMnICAgwQZEBIaGUujbEUZBtJmQHDVhhUS/nBhwPBIsuQJlHjv
GtCwzwDevwBIxMxQgAHBgFJeKJggV3G+aNH0ieCpcx8ECScyTNi3L6rnfv34
Ki5x0KDCFyYNDFxtwYKM1zJMqFWtr0RcuR4l6DZp8u7fHxD6AsZLoCGJAkJe
aHhBM98EEjCMXPXZc+hjBcyVYyaxmTM/fiVCa9BQYt9y5Vq1nuBgYqyRsvBp
vDZBn/5oDRtn0K3L4YR/wIIF5NdwABBAEGEMCMSXM8o1JoIUD0Ko02SIRXZZ
ZhCUF14JK0QFlXhXKZhQQgv1pxprrb1mhQFoGZBBBhn2Q0JHBNxUV0m9/YcX
bbYRWKBWxzGgQUzYQagBXFhl/ldDPj0Blc9M2M2Q2QTldbYhaOI1ed4Eq6WG
gHvwWfFaWmplMJAUGmQA3Vy58dffcJoJZIGPMyAwAWFCXCWTAokxRk6EGFQI
FE8PSjZBf4JxFt4O/EA1HnnhkTceQlqZZMJqL7TW2llkGmDCZv1kIBebN3rV
n44AnPBDQArA4CM5LwR53kwiHElCkhBKaN2TQ70wAwdmDmXlho+OdxVWWnHJ
5XoGfElWWa6ZYAV9P7zIVj8XETAqARKQxJtvfwWbqY8EhIUgac5852dAD/ak
mJNMJrZPUVOGpwBUO3RILHYCBUWpVqidcGKmKYrZolqXKijqTR3diOOpeWWw
JwHk/g4UJEGglvccOdNFSJNOL2AQGXYlaabolcSGuM9Ax3KJGgfNgkmDBQYf
nIFqnS1MI7f8oYrXrcxlQK6d5yoWkGIawHCrCIES6mQ0QHl3IXDfMerZoynr
Y5BBL1R6wqUDaTrzmGlVC1x5aY46V6nf/iVwCTy5SiCssmIFoXNICjVyZe7+
RHJmAWkoeKSTDlQrxsoGHLN7FphVkWwumhmpmrjlttup4KKU6LgEFncncl1P
0G+DOMHtLq+F6roYZiY32uGHkAoEHmlbDsQsivLBxqIB1cKo4E0zstnt5RDj
dSlNFM9tcWGrLZlP0hw/JuGEzCn6AuvBzS441uEZrpyy/uoJ7OzMZemuVrXv
vmCF0mzyvJvPKIkrw9BpIreYTNn5ySVPiaEuzbvzMgnVOmM1SGGHPFGj3WEG
YilMjQ02MliRCTLwg5wEhXI0YlvxUJIZDQRqBhWLFfMGwhOR2QYnIYOMdTgz
vb+ZCTzauxLLlnNAjKlnBjJwzwveQ5Fplclak9uW5S4Hv6WJYEDD8VyQFpMu
gTDmhfxjl2SExRkI9MdkTvGQeORlrw0tkIEZkAHBZuYaCMrmRati0A+gk0Hd
POxN4ZpAYkCoPBEa5kwgU5MReqLCqHXGOy6M0YZmxxd+eI8030sIaggQsxdA
yzVpQcsEM3Cp8mRqTQ1zo12K/giBfAitcwh4gQwKQ7v+1QoGL5SXk+AFSAVY
cUqNQlmkulilLw6kW1YYCBlzF0HeUdBaQCmBqNqnwcyhJGG+ehVBYFCABIQN
YMvRj5nOM5DFHIQgBnEZagpkAS6ZhiAKAdhpQqmVL31pZtwygHxW5KmLwOWd
a1INl9Y2AzdhzpgAmIEFBoJEwEgAIDAQwAEGUM9xxAUvEsjA49YIFwrKhQSU
/AEdCwSDm8jFotpiYwb1U89uzSADu9FNBgjAm5L6jAPjwKePAHMCRiaPQCfo
FhE8cAAo4CAAOM2pTnfKU546oadADWpON5AEoiYhCQtYgFB9GoCfOrWpUH3q
U3UqCFWo6jQBBwgEADs=
}

}


proc editval_load_images { chemin } {
    #puts "editval_load_images $chemin"
    set nbimage 0
    set limages [image names]
    set rimage  [set ::[set chemin](-rimage)]
    while 1 {
	if { [lsearch -exact $limages [set rimage][set nbimage]] == -1 } {
	    break
	}
	incr nbimage
    }
    if { $nbimage > 0 } {
	editval_configure $chemin -nbimage $nbimage
	return
    }
    while 1 {
	if { [file exist $rimage[set nbimage].gif] == 1 } {
	    image create photo [set rimage][set nbimage] \
		-format GIF -file $rimage[set nbimage].gif
	    incr nbimage
	} else {
	    break
	}
    }
    editval_configure $chemin -nbimage $nbimage
}

proc editval_scale_roller_init { chemin } {
    editval_define_images
    #    editval_load_images $chemin
    switch -exact -- [set ::[set chemin](-display)] {
	"scale" {
	    if { [winfo exist $chemin.scale] != 1 } {
		scale $chemin.scale
		$chemin.scale configure -digits 0 -resolution -1 \
		    -orient horizontal -width 6 \
		    -label "" -command "editval_scale_set $chemin" \
		    -showvalue no -variable ::[set chemin]._scalevar
	    }
	    if { [winfo exist $chemin.roller] == 1 } {
		destroy $chemin.roller
	    }
	    bind $chemin.scale <3> "editval_menu3 $chemin %X %Y"
	}
	"roller" {
	    if { [winfo exist $chemin.scale] == 1 } {
		destroy $chemin.scale
	    }
	    if { [winfo exist $chemin.roller] != 1 } {
		label $chemin.roller -image [set ::[set chemin](-rimage)]0
		bind $chemin.roller <Button-1> "editval_roller_button1 $chemin %x"
		bind $chemin.roller <B1-Motion> "editval_roller_move $chemin %x"
	    }
	    bind $chemin.roller <3> "editval_menu3 $chemin %X %Y"
	}
	"default" { error "editval_scale_roller_init $chemin" }
    }
    set l [pack slaves $chemin]
    if { $l != {} }  {
	eval pack forget $l
    }
    update
    if { [winfo exist $chemin.scale] == 1 } {
	pack $chemin.label $chemin.spinbox $chemin.scale -side left
    } else {
	pack $chemin.label $chemin.spinbox $chemin.roller -side left
    }
}

# editval_roller_move : calcul et affichage de la nouvelle image, puis
#  de la nouvelle valeur newval et appel de editval_scale_set pour 
#  les vérifications et traitements associés.
proc editval_roller_move { chemin x } {
    #puts "editval_roller_move $chemin $x"
    if { [$chemin cget -state] == "disabled" } { return }
    set x0 [set ::[set chemin](-roller_x0)]
    set val0 [set ::[set chemin](-roller_val0)]
    # ATTENTION /3 et %10  codent la vitesse d'évolution des images du roller
    $chemin.roller configure -image \
	[set ::[set chemin](-rimage)][expr (($x - $x0) /3) % 10 ]
    set incr [set ::[set chemin](-roller_increment)]
    set newval [expr $val0 - $incr * ($x0 - $x)]
    editval_scale_set $chemin $newval
}

proc editval_roller_button1 { chemin x } {
    set ::[set chemin](-roller_x0) $x
    set ::[set chemin](-roller_val0) [$chemin get]
}



# _editval_loption  definit la liste des options acceptees par 
#     editval_set_arg qui est appele pour chaque option par
#     editval_configure.  la valeur du tableau sert a la verification 
#     du type
# ATTENTION les options "element-option" peuvent etre appelees sans faire 
#   partie de cette liste.
array set _editval_loption {
    -command texte
    -f_from_to positive
    -max double_empty
    -min double_empty
    -mod non_negative
    -variable texte
    -state { normal disabled }
    -display { scale roller }
    -rimage texte
    -nbimage integer
    -roller_increment positive
    label-text texte
    label-width integer
    spinbox-width integer
    spinbox-increment positive
    scale-from double
    scale-to double
    scale-length double
}

set editval_l_fromto { 1000 100 10 3.1416 0.5*3.1416 2 1 0.1 0.01 0.001 }

array set _editval_init {
    label-text ""
    -variable ""
    -min ""
    -max ""
    -command ""
    -state normal
    -display scale
    -rimage "roueimages/roue"
    -nbimage 0
    -roller_increment 1
    -roller_x0 0
    -roller_val0 0
    -mod 0
}
array set _editval_init "-f_from_to [lindex $editval_l_fromto 1]"

proc editval_spinbox_return { chemin } {
    editval_spinbox_set $chemin
}


proc editval_spinbox_set { chemin } {
    if { [catch { uplevel \#0 "expr [$chemin.spinbox get]" } newval ] } {
	error "editval_spinbox_set \"[$chemin.spinbox get]\" est une expression invalide"
    }
    #puts "editval_spinbox_set newval= $newval"
    editval_scale_set $chemin $newval
}

proc editval_scale_set { chemin val } {
    set variable [set ::[set chemin](-variable)]
    set $variable $val
    set commande [set ::[set chemin](-command)]
    if { $commande != "" } {
	#puts "editval_scale_set $chemin $val"
	eval $commande [set [set variable]]
    }
}

proc editval_modoffset_verif { chemin newval } {
    #puts "editval_modoffset_verif $chemin $newval"
    set mod [set ::[set chemin](-mod)]
    if { $mod > 0 } {
	#puts -nonewline "editval_modoffset_verif mod= $mod $newval"
	set offset [set ::[set chemin](-min)]
	if { $offset == "" } { set offset 0 }
	set o_val [ expr { $newval - $offset } ]
	set newval [expr { $offset + 
			   $o_val - $mod * floor($o_val * 1. / $mod) } ]
	#puts " $newval"
    }
    return $newval
}
    
# editval_set_minmax : positionne la valeur du editval à la valeur du min 
#   ou du max suivant minmax
proc editval_set_minmax { chemin minmax } {
    if { $minmax == "-min" } {
	set min [set ::[set chemin](-min) ]
	if { $min != "" } { 
	    editval_scale_set $chemin $min 
	} else {
	    editval_scale_set $chemin [$chemin.scale cget -from]
	}	
    } else {
	set max [set ::[set chemin](-max) ]
	if { $max != "" } {
	    editval_scale_set $chemin $max
	} else {
	    editval_scale_set $chemin [$chemin.scale cget -to]
	}
    }

}

proc editval_trace { chemin nom nom_ext wru } {
#    puts "++ editval_trace $chemin - $nom - $nom_ext - $wru "
    editval_trace_actualise $chemin
#    puts --
}

proc editval_trace_actualise { chemin {force ""} } {
    set newval [set [set ::[set chemin](-variable)]]
    # on verifie les bornes eventuelles
    set min [set ::[set chemin](-min)]
    set max [set ::[set chemin](-max)]
    set mod [set ::[set chemin](-mod)]
    if { $max < $min } {
	set max ""
	set ::[set chemin](-max) ""
    }
    if { $mod != "" && $mod > 0  } {
	set newval [editval_modoffset_verif $chemin $newval]
    }
    if { $min != "" && $newval < $min } { set newval $min }
    if { $max != "" && $newval > $max } { set newval $max }
    # On verifie s'il est necessaire de modifier les bornes du scale
    if { [winfo exist $chemin.scale] } {
	set s_min [$chemin.scale cget -from]
	set s_max [$chemin.scale cget -to]
	if { $newval < $s_min || $newval > $s_max
	     || $force != "" 
	     || $min != "" && $s_min < $min 
	     || $max != "" && $s_max > $max } {
	    set from_to [set ::[set chemin](-f_from_to) ]
	    set varr [expr ($newval*10 - fmod($newval*10, $from_to)) / 10.]
	    set n_min [expr $varr - $from_to ]
	    set n_max [expr $varr + $from_to ]
	    if { $min != "" && $n_min < $min } { set n_min $min }
	    if { $max != "" && $n_max > $max } { set n_max $max }
	    $chemin.scale configure -from $n_min -to $n_max \
		-tickinterval [expr ($n_max - $n_min) * 0.99999999]
	}
    }
    set ::[set chemin]._scalevar $newval
    $chemin.spinbox set $newval
}

proc editval_scale_center { chemin } {
    editval_trace_actualise $chemin -force
}


# traitement des configuration des options
#   largs est une suite de couples  -option valeur
proc editval_configure { chemin args } {
    global $chemin
    if {[llength $args] == 0 || $args == {{}} } {
	return [array get $chemin]
    }
    set ajuste no
    set ajuste_force no
    foreach { para val } $args {
	set retour [editval_set_arg $chemin $para $val]
	if { $retour == "TCL_ERROR" } {
	    set message "Pb option \"$para\" non valide. Options possibles:\n"
	    global _editval_loption
	    set loption ""
	    foreach { el lval } [array get _editval_loption ] {
		append loption " { $el $lval }"
	    }
	    append message $loption
	    error $message
	}
	if { $retour == "TCL_AJUSTE_FORCE" } {
	    set ajuste_force TCL_AJUSTE_FORCE
	}
	if { $retour != "TCL_OK" } { set ajuste TCL_AJUSTE }
    }
    if { $ajuste_force == "TCL_AJUSTE_FORCE" } {
	editval_trace_actualise $chemin -force
    } else {
	if { $ajuste == "TCL_AJUSTE"} { 
	    editval_trace_actualise $chemin
	}
    }
}


# traitement de l'acces a la valeur d'une option
proc editval_cget { chemin args } {
    global $chemin
    if {[llength $args] != 1} {
	error "Pb syntaxe, doit être: chemin cget -option"
    }
    set para $args
    global _editval_loption
#    if { [string index $para 0] == "-" } 
    if { [lsearch -exact [array names $chemin] $para] != -1 } {
	set el [array get $chemin $para]
	if { $el != "" } {
	    return [lindex $el 1]
        }
	error "editval_cget strange..."
    } else {
	set lpara [split $para "-"]
	if { [llength $lpara] == 2 } {
	    set element [lindex $lpara 0]
	    set e_para  [lindex $lpara 1]
	    return [$chemin.$element cget -$e_para]
	}
    }
    set message "Pb option \"$args\" doit etre: "
    foreach { el lval } [lsort [array get _editval_loption ]] {
	append message " $el"
    }
    append message {
	ou : element-option avec element=label, spinbox ou scale 
	    et option une option valide de l'element}
    error $message
}
    

# fonction utilitaire pour editval_configure : affectation de la valeur
#       val à l'option para.
proc editval_set_arg { chemin para val } {
    #puts "editval_set_arg $chemin $para $val"
    global $chemin _editval_loption
    if { $para == "-variable" } {
	editval_new_variable $chemin $val
	return TCL_AJUSTE
    }
    if { [string index $para 0] == "-" } {
	#puts "editval_set_arg if -option"
	if { [lsearch -exact [array names ::_editval_loption ] $para] == -1 } {
	    puts "editval_set_arg lsearch option"
	    return TCL_ERROR
	}
	if { [editval_option_test \
		  [set ::_editval_loption($para)] $val] == 0 } {
	    puts "editval_set_arg editval_option_test $para $val"
	    return TCL_ERROR
	}
	set ::[set chemin]($para) $val
	switch -exact -- $para {
	    "-f_from_to" {
		return TCL_AJUSTE_FORCE
	    }
	    "-min" - "-max" - "-mod" {
		return TCL_AJUSTE
	    }
	    "-state" {
		$chemin.spinbox configure -state $val
		if { [winfo exist $chemin.scale] == 1 } {
		    $chemin.scale configure -state $val
		}
		set ::[set chemin]($para) $val
		return TCL_AJUSTE
	    } 
	    "-display" {
		editval_scale_roller_init $chemin
		return TCL_AJUSTE
	    }
	    "-nbimage" {
		set ::[set chemin]($para) $val		
		return TCL_AJUSTE
		# modif 22 nov 2007  a vérifier
	    }
	}
	return TCL_OK
    } else {
	switch -exact -- $para {
	    "scale-from" {
		$chemin.scale configure -from $val
		return TCL_NEED_RECALCULER
	    }
	    "scale-to" {
		$chemin.scale configure -to $val
		return TCL_NEED_RECALCULER
	    }
	    "label-text" {
		$chemin.label configure -text $val
		set [set chemin](label-text) $val
		return TCL_OK
	    }
	    "spinbox-increment" {
		if { [editval_option_test \
			  [set ::_editval_loption($para)] $val] == 0  } {
		    set val [$chemin cget spinbox-increment]
		    if { [editval_option_test \
			      [set ::_editval_loption($para)] $val] == 0  } {
			set val 1
	          # ??? error "editval_set_arg $chemin $para $val : bad value"
		    }
		}
		$chemin.spinbox configure -increment $val
		set [set chemin](spinbox-increment) 1
		return TCL_OK
	    }
	}
	# parametres de la forme element-para
	if { [regexp {^([^-]+)(-[^-]+)$}  $para inutile wel wpara] } {
	    eval $chemin.$wel configure $wpara $val
	    return TCL_OK
	}
    }
    return TCL_ERROR
}


proc editval_get { chemin args } {
    #puts "editval_get \"$chemin\" \"$args\""
    global $chemin
    if { $args == "{}" || $args == "" } {
	set variable [set [set chemin](-variable) ]
	return [ set $variable ]
    }
    error "Pb syntaxe, must be: chemin get"
}

proc editval_set { chemin args } {
    global $chemin
    if {[llength $args] != 1} {
	error "Pb syntaxe, doit etre : editval chemin set valeur"
    }
    set variable [set [set chemin](-variable) ]
    if { [catch { expr $args } newval ] } {
	error "editval_set \"$args\" est une expression invalide"
    }
    set $variable $newval
}

#proc editval_new_variable { chemin variable } {
#    set old [set [set chemin](-variable) ]
#    if { [info exist $variable] == 0 } {
#	set $variable [set $old]
#    }
#    trace vdelete       [set old] w "editval_trace $chemin"
#    trace variable [set variable] w "editval_trace $chemin"
#    set [set chemin](-variable) $variable
#}
proc editval_new_variable { chemin variable } {
    #puts "editval_new_variable $chemin $variable"
    set old [set ::[set chemin](-variable) ]
    set new_v [namespace which -variable $variable]
    if { $new_v == "" } {
	# la variable n'esiste pas ou c'est un tableau
	set array [regsub "\\(.*\\)$" $variable "" array_base_name]
	if { $array } {
	    set new_base [namespace which -variable $array_base_name]
	    if { $new_base == "" } {
		namespace eval :: "array set $array_base_name {}"
		set new_base [namespace which -variable $array_base_name]
	    }
	    regsub $array_base_name $variable $new_base new_v
	    if { [ catch { set $new_v } ] } {
		# le tableau ne contient pas l'élément
		set [set new_v] [set old]
	    }
	} else {
	    namespace eval :: "set $variable [set $old]"
	    set new_v [namespace which -variable $variable]
	}
    }
    trace vdelete    [set old] w "editval_trace $chemin"
    trace variable [set new_v] w "editval_trace $chemin"
    set ::[set chemin](-variable) $new_v
    #puts "editval_new_variable fin"
}

# editval_destroy_f est appelé à la destruction de la fenêtre par le
#   bind <Destroy> afin de détruire les variables et les trace associées.
proc editval_destroy_f { chemin } {
    #puts "editval_destroy \"$chemin\""
    global $chemin
    set variable [set [set chemin](-variable)]
    trace vdelete  [set variable] w "editval_trace $chemin"
    if { $variable == "$chemin._var" } {
	unset $variable
    }
    unset $chemin
}

proc editval_option_test { type valeur } {
    if { [llength $type] > 1 } {
	if { [lsearch -exact $type $valeur] == -1} {
	    return 0
	} else {
	    return 1
	}
    }
    switch $type {
	texte { return 1 }
	integer { return [expr ![catch {incr valeur}]] }
	double {return [expr ![catch {expr 0 + $valeur}]] }
	boolean {
	    if { [catch "if { $valeur } { }" mess] } {
		return 0
	    } else {
		return 1
	    }
	}
	positive { return [expr {$valeur > 0} ] }
	non_negative { return [expr {$valeur >= 0}] }
	double_empty { 
	    return [ expr { $valeur == "" || ![catch {expr 0 + $valeur} ] } ]
	}
	entier_positif {
	    set u $valeur
	    return [expr { ![catch {incr u}] && ($valeur > 0) } ]
	}
 	default { error "Pb editval_option_test : type \"$type\" inconnu" }
    }
    error "Pb editval_option_test : type \"$type\" inconnu"
}

# editval_menu3 : gestion du menu sur clic bouton 3
proc editval_menu3 { chemin x y } {
    if { [winfo exist $chemin.menu3] } { destroy $chemin.menu3 }
    menu $chemin.menu3
    if { [winfo exist $chemin.scale] == 1 } {
	$chemin.menu3 add command -label Centrer \
	    -command "editval_scale_center $chemin"
	global _editval_loption
	foreach el $::editval_l_fromto {
	    $chemin.menu3 add command -label "Centrer $el" \
		-command "editval_set_arg $chemin -f_from_to $el; \
               editval_scale_center $chemin"
	}
    }
    if { [$chemin cget -min] != "" } {
	$chemin.menu3 add command -label min \
	    -command "editval_set_minmax $chemin -min" \
	    -background red -activebackground red
    }
    if { [$chemin cget -max] != "" } {
	$chemin.menu3 add command -label max \
	    -command "editval_set_minmax $chemin -max" \
	    -background red -activebackground red
    }
    $chemin.menu3 add command -label Edit \
	-command "editval_edit $chemin"
    $chemin.menu3 add command -label Help \
	-command "editval_help help_editval"
    $chemin.menu3 add command -label Fermer \
	-command "destroy $chemin.menu3"
    tk_popup $chemin.menu3 $x $y
}

proc editval_help { page { fen "" } } {
    if { [winfo exist $fen.aide] } { destroy $fen.aide }
    toplevel $fen.aide
    set largeur 80
    set hauteur 34
    text $fen.aide.t -wrap word -yscrollcommand "$fen.aide.s set" \
	-width $largeur -height $hauteur
    scrollbar $fen.aide.s -command "$fen.aide.t yview"
    pack $fen.aide.s -side right -fill y
    pack $fen.aide.t -expand yes -fill both -side top
    global $page
    $fen.aide.t insert end [set [set page]]
    button $fen.aide.fermer -text Fermer -command "destroy $fen.aide"
    button $fen.aide.man_spinbox -text "man spinbox" \
	-command "editval_man spinbox n"
    button $fen.aide.man_scale -text "man scale" -command "editval_man scale n"
    button $fen.aide.man_label -text "man label" -command "editval_man label n"
    pack $fen.aide.fermer $fen.aide.man_spinbox $fen.aide.man_scale \
	$fen.aide.man_label -side left
}


proc editval_edit { chemin { frame "" } } {
    set w $chemin.edit
    if { $frame == "" } {
	if { [winfo exist $w] } { destroy $w }
	toplevel $w
    } else {
	eval destroy [winfo children $w]
    }
    if { [winfo exist $chemin.scale] == 1 } {
	editval $w.from
	editval_set_arg $w.from -command "editval_edit_fromto $chemin -from"
	editval_set_arg $w.from label-text "scale-from"
	$w.from set [$chemin cget scale-from]
	editval $w.to
	editval_set_arg $w.to -command "editval_edit_fromto $chemin -to"
	editval_set_arg $w.to label-text "scale-to"
	$w.to set [$chemin cget scale-to]
	editval $w.scalelength
	editval_set_arg $w.scalelength label-text "scale-length"
	editval_set_arg $w.scalelength -min 0
	editval_set_arg $w.scalelength -max 500
	editval_set_arg $w.scalelength -command \
	    "editval_edit_configure $chemin configure scale-length"
	$w.scalelength set [$chemin cget scale-length]
    } else {
	editval $w.increment label-text -roller_increment -min 1e-20 \
	-command "editval_set_arg $chemin -roller_increment"
	$w.increment set [$chemin cget -roller_increment]
	editval $w.mod -min 0 label-text -mod \
	    -command "editval_edit_mod $chemin"
	$w.mod set [$chemin cget -mod]
	frame $w.rimage
	label $w.rimage.l -text rimage
	entry $w.rimage.e
	$w.rimage.e insert 0 [$chemin cget -rimage]
	bind $w.rimage.e <KeyRelease> "editval_load_images $chemin"
	eval pack [winfo children $w.rimage] -side left
    }
#    frame $w.mm
#    checkbutton $w.mm.bmin -variable ::[set chemin]_editval_edit_bmin \
#	-text " -min " -command "editval_edit_min_max $chemin toggle_min"
#    checkbutton $w.mm.bmax -variable ::[set chemin]_editval_edit_bmax \
#	-text -max -command "editval_edit_min_max $chemin toggle_max"
#    eval pack [winfo children $w.mm] -side left
    editval $w.min 
    editval_configure $w.min label-text -min -command "editval_edit_min_max $chemin -min"
    if { [$chemin cget -min] == "" } { $w.min configure -state disabled }
    editval $w.max
    editval_configure $w.max label-text -max -command "editval_edit_min_max $chemin -max"
    if { [$chemin cget -max] == "" } { $w.max configure -state disabled }
    editval_edit_min_max $chemin
    editval $w.spinboxwidth
    editval_configure $w.spinboxwidth \
	label-text "spinbox-width" -min 0 -max 500 \
	-command "editval_edit_configure $chemin configure spinbox-width"
    $w.spinboxwidth set [$chemin cget spinbox-width]
    editval $w.spinboxincrement
    editval_configure $w.spinboxincrement label-text "spinbox-increment" \
	-command "editval_edit_configure $chemin configure spinbox-increment" 
    $w.spinboxincrement set [$chemin cget spinbox-increment]
    frame $w.label
    label $w.label.l -text "label-text"
    entry $w.label.e
    bind $w.label.e <KeyRelease> \
	"$chemin configure label-text \[$w.label.e get\]"
    $w.label.e insert 0 [$chemin cget label-text]
    eval pack [winfo children $w.label] -side left
    frame $w.liste
    label $w.liste.l -text "editval_l_fromto"
    entry $w.liste.e -width 40
    $w.liste.e insert 0 [set ::editval_l_fromto]
    bind $w.liste.e <KeyRelease> "set ::editval_l_fromto \[$w.liste.e get\]"
    eval pack [winfo children $w.liste] -side left
    frame $w.nd
    foreach el [set ::_editval_loption(-state)] {
	radiobutton $w.nd.$el -text $el -variable ::[set chemin](-state) \
	    -value $el -command "$chemin configure -state $el"
    }
    eval pack [winfo children $w.nd] -side left
    frame $w.sr
    foreach el [set ::_editval_loption(-display)] {
	radiobutton $w.sr.$el -text $el -variable ::[set chemin](-display) \
	    -value $el -command \
	    "$chemin configure -display $el; editval_edit $chemin rebuild"
    }
    eval pack [winfo children $w.sr] -side left
    button $w.fermer -text Fermer -command "destroy $w"
    bind $w <Destroy> "unset ::[set chemin]_editval_edit_bmin;
       unset ::[set chemin]_editval_edit_bmax; bind $w <Destroy> {}"
    eval pack [winfo children $w]
}

proc editval_edit_mod { chemin x } {
    #puts "editval_edit_mod $chemin $x"
    set w $chemin.edit
    $chemin configure -mod $x
    set val [$chemin get]
    set newval [editval_modoffset_verif $chemin $val]
    if { $val != $newval } {
	$chemin set $newval
    }
}

proc editval_edit_configure { args } {
    eval $args
    editval_scale_center [lindex $args 0]
}

proc editval_edit_fromto { chemin fromto val } {
    $chemin.scale configure $fromto $val
    editval_trace_actualise $chemin
}

proc editval_edit_min_max { chemin {el ""} {val ""} } {
    set w $chemin.edit
    switch -exact -- $el {
	"" {
	    if {[$chemin cget -min] == "" } { 
		set ::[set chemin]_editval_edit_bmin 0
		$w.min set 0
	    } else {
		set ::[set chemin]_editval_edit_bmin 1
		$w.min set [$chemin cget -min]
	    }
	    if {[$chemin cget -max] == "" } { 
		set ::[set chemin]_editval_edit_bmax 0
		$w.max set 100
	    } else {
		set ::[set chemin]_editval_edit_bmax 1
		$w.max set [$chemin cget -max]
	    }
	}
	toggle_min {
	    if { [set ::[set chemin]_editval_edit_bmin] == 0 } {
		$chemin configure -min ""
		$w.min configure -state disabled
	    } else {
		$chemin configure -min [$w.min get]
		$w.min configure -state normal
	    }	    
	}
	toggle_max {
	    if { [set ::[set chemin]_editval_edit_bmax] == 0 } {
		$chemin configure -max ""
		$w.max configure -state disabled
	    } else {
		$chemin configure -max [$w.max get]
		$w.max configure -state normal
	    }
	}
	-min {
	    if { [set ::[set chemin]_editval_edit_bmin] == 1 } {
		$chemin configure -min [$w.min get]
	    }
	}
	-max {
	    if { [set ::[set chemin]_editval_edit_bmax] == 1 } {
		$chemin configure -max [$w.max get]
	    }
	}
	default { error Pb editval_edit_min_max $chemin $el }
    }
}

set help_editval {
              Aide pour les éléments editval

les editvals sont des widgets permettant l'affichage et la modification
    d'un nombre. Pour faire un essai : 
       editval .a -command { puts -nonewline aa= $aa; puts } -variable aa
       pack .a
    Puis clicker le bouton 3 et sélectionner Edit. 

Utilisation de la fenetre

L'éditval est composé d'un label, d'un spinbox et d'un scale ou d'un roller.
   Le roller est en fait un button à image variable. L'option -display 
    permet de choisir entre scale et roller.
** Label : affiche un titre.
** spinbox : affiche la valeur editable de la variable, l'appui sur 
    <Return> enclenche la prise en compte du texte entré. L'entrée est
    filtrée par la fonction expr, ce qui permet d'entrer une formule
    mathématique et d'utiliser les variables tcl. Les deux flèches du
    spinbox permettent d'incrémenter ou de décrémenter la variable de
    la valeur de spinbox-increment.
** scale : le deplacement du curseur enclenche la mise a jour de la
    variable.
** roller : le déplacement de la roulette entraine la modification de
    la variable de roller-increment pour chaque déplacement élémentaire.

L'éditval peut mettre à jour la variable définie par l'option -variable
    et appeler la commande définie par l'option -command à chaque
    modification de la valeur de l'editval.
L'option -display qui peut prendre les valeurs scale ou roller permet
    de choisir l'affichage d'un roller ou d'un scale.
<Button3> declenche l'ouverture d'une fenetre:
   * - - - - - permet de punaiser la fenetre menu
   * Centrer : permet de recalculer les bornes min et le max du scale à
      partir de la valeur de l'option -f_from_to.
   * min : affecte à la variable la valeur de l'option -min si elle est
     definie.
   * max : affecte à la variable la valeur de l'option -max si elle est
     definie.
   * Edit : ouvre une fenetre permettant de visualiser et d'éditer les
     options de l'éditval.
   * Help : affiche la fenetre d'aide que vous êtes en train de lire.
   * Fermer : detruit cette fenetre de dialogue.

Syntaxe de la commande editval:
    editval chemin ?options?

** editval chemin ?options?
     Permet de creer une fenetre de nom chemin les options sont les 
     mêmes que pour la commande configure de la fonction chemin.
     D'un point de vue tk chemin est un frame. Un editval permet
     d'éditer et d'afficher une variable numérique. L'affichage et
     la modification se font simultanément par un spinbox et un scale
     ou un roller. Le texte du spinbox est pris en compte par l'évènement
     <Return>. Un label permet d'afficher un titre.
     Le label a pour chemin chemin.label, le spinbox chemin.spinbox, 
     le scale chemin.scale et le roller chemin.roller.
     La commande editval cree en outre une fonction de nom chemin dont
     la syntaxe est la suivante :
       chemin_editval configure ?options?
       chemin_editval cget -option
       chemin_editval set valeur
       chemin_editval get

** chemin_editval configure ?options?
     Les options acceptées par editval sont:
   **  -command script
      Cette option définit une commande appelée lorsque la valeur
      de la variable de l'editval est modifiée par l'entry ou le
      scale.
      Le script evalué est le script auquel on ajoute la nouvelle valeur 
      de la vartiable.
   **  -f_from_to double positif
      Permet de sélectionner la valeur pour calculer les bornes min et 
      max du scale. Les bornes -from et -to du scale sont recalculées.
      On arrondi la valeur courante de l'editval au dixième de
      -f_from_to, puis on ajoute respectivement on retranche -f_from_to
      pour obtenir la valeur du -to respectivement du -from. Si ces
      bornes sont en dehors du -min et du -max on les ajuste ensuite
      en conséquence.
   **  -max double
      Définit la borne maximum que peut prendre la variable.
   **  -min double
      Définit la borne minimum que peut prendre la variable.
   **  -mod positive 
      Permet d'obtenir une variable cyclique : par exemple un angle 
      évoluant continuemant entre -pi et -pi (-min -pi -max "" -mod 2*pi).
   **  -variable variable
      Définit le nom de la variable utilisée. Si cette option n'est
      pas configurée on utilise la variable ::$chemin._var.
   **  -state { normal disable }
      Dans l'état disable, l'interaction avec l'editval est bloquée.
   **  -display { scale roller}
      Permet de choisir l'affichege du scale ou du roller. Par défaut
      affiche le scale.
   **  -rimage texte
      Racine des fichiers de définition des images utilisés par le roller.
      Par défaut roue (les fichiers ont pour nom reoueN.gif où N désigne
      le numéro de 0 à (-nbimage - 1)). Peut être changé pour le widget.      
    -nbimage integer
      Nombre d'image utilisé par le roller, ne pas modifier ce paramètre
      qui est calculé automatiquement (arrêt au premier fichier qui 
      n'existe pas).
   **  -roller_increment positive
      Définit l'incrément d'évolution de la variable à chaque mouvement
      du roller.

  **** Options des elements accessibles. On peut accéder aux options du
      label, de l'entry et du scale par la syntaxe suivante 
      "nom-option valeur" où nom représente l'élément (label, spinbox,
      scale ou roller). Les options suivantes sont plus particulièrement
      intéressantes.
   **  label-text texte
      Permet de définir le texte du label.
   **  label-width integer
      Permet de définir la largeur de la fenetre label.
   **  sinbox-width integer
      Permet de définir la largeur de la fenetre spinbox.
   **  spinbox-increment positive
      Permet de définir l'incrément d'évolution du spinbox.
   **  scale-from double
      Permet de définir le minimum du scale.
   **  scale-to double
      Permet de définir le maximum du scale.
   **  scale-length double
      Permet de définir la longueur du scale.
   ATTENTION la modification directe des options -from et -to du scale et
   -increment du spinbox est à proscrire; utiliser les options scale-from,
   scale-to et spinbox-increment de l'editval. 
   De même il est fortement déconseillé de modifier les options du scale
   pouvant modifier l'affichage.

** chemin cget -option
     Permet d'obtenir la valeur d'une option
** chemin set valeur
     Permet de positionner la valeur de la variable avec appel de la 
     commande si une commande est définie
** chemin get
     Retourne la valeur de la variable

Bugs: Le comportement du roller et du spinbox sont différent pour un même
      incrément: le roller ajoute l'incrément alors que le scale fait un
      arrondi.

Copyright (C) 1999-2007 LAAS/CNRS
}

package provide editval 2.6
