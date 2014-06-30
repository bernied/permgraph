INC=.
SRC=.
OBJ=.
OUT=.

CC=clang
CFLAGS=-g -I$(INC) -Wno-initializer-overrides

DEPS=$(INC)/parse_cl.h $(INC)/types.h
OBJS=$(OBJ)/main.o $(OBJ)/parse_cl.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT)/permgraph: $(OBJS)
	$(CC) -o $(OUT)/$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/permgraph permutation*dot*
