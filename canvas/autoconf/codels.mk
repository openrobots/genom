
###### Default target
codels-mk-all: $(OBJDIR) $(OBJDIR)/$(MODULE_BIN)

###### GenoM flags
include $(top_srcdir)/$(autoconfdir)/genom.mk

CPPFLAGS += $(ENDIANNESS) $(GENOM_DEFINES) $(GENOM_INCLUDES)
CPPFLAGS += -I. -I$(top_builddir) -I$(srcdir) -I$(top_srcdir)
CPPFLAGS += $(GENOM_CFLAGS) $(EXTRA_INCLUDES)

ifeq ($(USE_CXX),1)
codels_obj=$(patsubst %.cpp,$(OBJDIR)/%.lo,$(codels_src:%.c=$(OBJDIR)/%.lo))
else
codels_obj= $(codels_src:%.c=$(OBJDIR)/%.lo)
endif


###### Build

$(OBJDIR)/$(MODULE_BIN): $(OBJDIR)/$(USER_LIB)
$(OBJDIR)/$(MODULE_BIN): $(top_builddir)/$(serverdir)/$(OBJDIR)/$(MODULE_LIB)
$(OBJDIR)/$(MODULE_BIN): $(top_builddir)/$(serverdir)/$(OBJDIR)/$(POSTERWRITE_LIB)

$(OBJDIR)/$(MODULE_BIN):
	$(LTLD) $(BINEXPORT) $(CFLAGS) -o $@ $(LDFLAGS) $(LIBTOOL_LINKOPT)		\
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(MODULE_LIB)	\
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(CLIENT_LIB)	\
		$(OBJDIR)/$(USER_LIB) \
		$(EXTRA_LIBS) \
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(POSTERWRITE_LIB)	\
		$(GENOM_LIBS)

$(OBJDIR)/$(USER_LIB): $(codels_obj)
	$(LTLD) $(CFLAGS) $(codels_obj) -o $@ $(LDFLAGS)

$(OBJDIR)/%.lo: %.c
	$(LTCC) -c $(CPPFLAGS) $(CODELS_CPPFLAGS) $(CFLAGS) -o $@ $< $(LIBTOOL_COPT)
ifeq ($(USE_CXX),1)
$(OBJDIR)/%.lo: %.cpp
	$(LTCXX) -c $(CPPFLAGS) $(CODELS_CPPFLAGS) $(CFLAGS) -o $@ $< $(LIBTOOL_COPT)
endif

$(OBJDIR):
	mkdir -p $@

# --- install -----------------------------------------------------------

.PHONY: install install-bin

install: all install-bin user-install

user-install:
install-bin:
	$(LTINSTALL) $(OBJDIR)/$(MODULE_BIN) $(bindir)/$(MODULE_BIN)


# --- clean -------------------------------------------------------------

.PHONY: clean

clean: 
	$(top_builddir)/libtool --mode=clean rm -f $(codels_obj)
	rm -f $(OBJDIR)/$(MODULE_BIN)
	rm -f $(OBJDIR)/dependencies

distclean: clean
	rm -f Makefile

# --- dependencies ------------------------------------------------------

.PHONY: depend
depend $(OBJDIR)/dependencies:: $(OBJDIR);

depend $(OBJDIR)/dependencies:: $(codels_src)
	$(MKDEP) -c$(CC) -o$(OBJDIR)/dependencies -d$(OBJDIR) -t.lo \
		$(CPPFLAGS) $?

ifneq (${MAKECMDGOALS},clean)
-include $(OBJDIR)/dependencies
endif

