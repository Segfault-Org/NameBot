OBJS = example.o
CFLAGS = -Wall -O0 -fno-omit-frame-pointer -g
LDFLAGS = -shared

.PHONY: clean all

all: example.so

example.so: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) -rdynamic $^

$(OBJS):%.o:%.c

clean:
	rm example.so $(OBJS)
