#	$LAAS$

#
# Copyright (c) 2004 
#      Autonomous Systems Lab, Swiss Federal Institute of Technology.
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


# --- RTAI-specific configuration ---------------------------------------

moddir=		modules
MOD=		$(moddir)/lib$(BASENAME).o

MOD_CPPFLAGS+=	-I$(RTAI_INC) -I$(LNXKERN_INC)
MOD_CPPFLAGS+=	-D__RTAI__ -D__KERNEL__ -DMODULE
ifeq (powerpc,$(host_cpu))
MOD_CFLAGS+= -fsigned-char -msoft-float -ffixed-r2 -mmultiple -mstring
endif

modobjs=$(modsrcs:%.c=$(moddir)/%.lo)

# how to build a module
all-os:	$(moddir) $(MOD)

# relocatable object
$(MOD): $(modobjs)
	$(LTLD) -o $@ $(modobjs)

$(moddir)/%.lo: %.c
	$(LTCC) -prefer-non-pic -c $(CFLAGS) $(MOD_CFLAGS) \
		$(CPPFLAGS) $(MOD_CPPFLAGS) -o $@ $<

# object file directory
$(moddir):
	mkdir -p $@

# install
install:: all-os
	$(LTINSTALL) $(MOD) $(libdir)/$(MOD)


# module dependencies
depend $(moddir)/dependencies:: $(moddir)
depend $(moddir)/dependencies:: $(modsrcs)
	$(MKDEP) -c$(CC) -o$(moddir)/dependencies -t.lo \
		$(CPPFLAGS) $(MOD_CPPFLAGS) $?

ifneq (,$(modsrcs))
ifneq (distclean,$(MAKECMDGOALS))
include $(moddir)/dependencies
endif
endif
