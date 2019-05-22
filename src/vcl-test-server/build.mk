CC = gcc

#DEBUG = -O2 
CFLAGS = -g $(DEBUG) $(AM_CFLAGS)

CFLAGS += -I$(VPP_INCLUDE)
LIBS += -L$(VPP_LIB) -lvppcom -lpthread -lvppinfra

#vppinfra svm vlibmemoryclient rt pthread

LTCOMPILE = libtool --tag=CC --mode=compile $(CC) $(CFLAGS) 
LINK = libtool --tag=CC --mode=link $(CC) $(LIBS)
LCLEAN = libtool --tag=CC --mode=clean rm -rf

BIN = main
objects = \
	main.o

all: $(BIN)

$(BIN): $(objects)
	@$(LINK) $^ -o $@

.c.o:
	@$(LTCOMPILE) -c $< -o $@

clean:
	@-$(LCLEAN) $(BIN) *.o *.lo .libs *.log

.PHONY: clean