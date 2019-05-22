CC := $(CROSS_COMPILE)gcc

CFLAGS += -g
LDFLAGS +=


SRCS = $(wildcard test_*.c)
BINS = $(patsubst %.c,%,$(SRCS))

all: $(BINS)

$(BINS): % : %.c

clean:
	@-rm -rf $(BINS) *.o *.lo .libs *.log

.PHONY: clean