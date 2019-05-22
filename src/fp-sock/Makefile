OFP_INSTALL = /var/opt/ofp
CC = gcc

AM_CFLAGS = -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	-Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual \
	-Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -std=c99 \
	-D_DEFAULT_SOURCE -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE \
	-DOFP_CONFIG_DEFAULT -DOFP_DEBUG -DINET -DINET6 -DSP -DMTRIE -DOFP_USE_LIBCONFIG -DCL

#DEBUG = -O2 
CFLAGS = -g $(DEBUG) -I$(OFP_INSTALL)/include $(AM_CFLAGS)
LIBS = $(OFP_INSTALL)/lib/libodp-linux.la \
	$(OFP_INSTALL)/lib/libofp.la \
	$(OFP_INSTALL)/lib/libodphelper.la

LTCOMPILE = libtool --tag=CC --mode=compile $(CC) $(CFLAGS) 
LINK = libtool --tag=CC --mode=link $(CC) $(LIBS)
LCLEAN = libtool --tag=CC --mode=clean rm -rf

BIN = sock
objects = \
	main.o utils.o \
	tcp-server.o \
	udp-server.o \
	hook.o \
	perf.o

all: $(BIN)

$(BIN): $(objects)
	@$(LINK) $^ -o $@

.c.o:
	@$(LTCOMPILE) -c $< -o $@


clean:
	@-$(LCLEAN) $(BIN) *.o *.lo .libs *.log

.PHONY: clean