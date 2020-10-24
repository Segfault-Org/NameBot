TARGET = namebot

VERSION := 1
VERSION := $(VERSION).$(shell git rev-list --count HEAD)

OBJS = main.o auth.o utils.o cmdline.o dynmodule.o
CFLAGS = -Wall -O0 -fno-omit-frame-pointer -g -DVERSION=$(VERSION)
LDFLAGS = -ltdjson -ljson-c -ldl

.PHONY: clean all

all: $(TARGET)

ifeq ($(PREFIX),)
    PREFIX := /usr/
endif

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 client/dynmodule_client.h $(DESTDIR)$(PREFIX)/include/

$(TARGET): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) -rdynamic $^

$(OBJS):%.o:%.c

clean:
	rm $(TARGET) $(OBJS)
