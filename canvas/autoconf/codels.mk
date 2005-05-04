
###### GenoM flags
include $(top_srcdir)/$(autoconfdir)/genom.mk

CPPFLAGS += $(ENDIANNESS) $(GENOM_DEFINES) $(GENOM_INCLUDES)
CPPFLAGS += -I. -I$(top_builddir) -I$(srcdir) -I$(top_srcdir)
CPPFLAGS += $(GENOM_INC) $(POCOLIBS_INC) $(EXTRA_INCLUDES)

codels_obj= $(codels_src:%.c=$(OBJDIR)/%.lo)


###### Build
all: $(OBJDIR) $(OBJDIR)/$(MODULE_BIN)

$(OBJDIR)/$(MODULE_BIN): $(OBJDIR)/$(USER_LIB)
$(OBJDIR)/$(MODULE_BIN): $(top_builddir)/$(serverdir)/$(OBJDIR)/$(MODULE_LIB)
$(OBJDIR)/$(MODULE_BIN): $(top_builddir)/$(serverdir)/$(OBJDIR)/$(POSTERWRITE_LIB)

$(OBJDIR)/$(MODULE_BIN):
	$(LTLD) $(BINEXPORT) $(CFLAGS) -o $@ $(LDFLAGS) $(LIBTOOL_LINKOPT)		\
		$(EXTRA_LIBS) \
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(CLIENT_LIB)	\
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(MODULE_LIB)	\
		$(OBJDIR)/$(USER_LIB)					\
		$(top_builddir)/$(serverdir)/$(OBJDIR)/$(POSTERWRITE_LIB)	\
		$(LTLK_GENOM_LIB)

$(OBJDIR)/$(USER_LIB): $(codels_obj)
	$(LTLD) -static $(CFLAGS) $(codels_obj) -o $@ $(LDFLAGS) \
		$(LIBTOOL_LINKOPT)

$(OBJDIR)/%.lo: %.c
	$(LTCC) -c $(CPPFLAGS) $(CODELS_CPPFLAGS) $(CFLAGS) -o $@ $< $(LIBTOOL_COPT)

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


# --- dependencies ------------------------------------------------------

.PHONY: depend
depend $(OBJDIR)/dependencies:: $(OBJDIR);

depend $(OBJDIR)/dependencies:: $(codels_src)
	$(MKDEP) -c$(CC) -o$(OBJDIR)/dependencies -d$(OBJDIR) -t.lo \
		$(CPPFLAGS) $?

ifneq (${MAKECMDGOALS},clean)
-include $(OBJDIR)/dependencies
endif

