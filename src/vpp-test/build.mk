CC := $(CROSS_COMPILE)gcc

CFLAGS += -I$(VPP_INCLUDE) -DCLIB_ASSERT_ENABLE -DCLIB_DEBUG=4 -g
LDFLAGS += -L$(VPP_LIB) -lvlibmemoryclient -lsvm -lpthread -lvppinfra -L$(OPENSSL_CRYPTO_LIBRARY) -lcrypto


SRCS = $(wildcard test_*.c)
BINS = $(patsubst %.c,%,$(SRCS))

all: $(BINS)

$(BINS): % : %.c

clean:
	@-rm -rf $(BINS) *.o *.lo .libs *.log

.PHONY: clean