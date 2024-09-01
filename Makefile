BINARY=sdb
OUT=out/
CODEDIRS=. src
INCDIRS=./include/
DEPFLAGS=-MP -MD

CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
OBJECTS=$(patsubst %,$(OUT)%,$(CFILES:.c=.o))
DEPFILES=$(patsubst %,$(OUT)%,$(CFILES:.c=.d))
ASAN=-fsanitize=address
#ASAN=

insert id=3 username=name-3 email=email-3
insert id=2 username=name-2 email=email-2
insert id=1 username=name-1 email=email-1

-include $(DEPFILES)

DEBUG=-g -DDEBUG
OPT=-O0

CC=gcc
CC_WARN=-Wall -Wshadow -Wextra -Werror -Wformat=2 -Wpedantic -fmax-errors=10 -Wno-unknown-pragmas
CFLAGS=${CC_WARN} $(OPT) -std=gnu11 ${DEBUG} $(foreach D,$(INCDIRS),-I$(D)) ${DEPFLAGS}

.PHONY: all clean testclean dbg_compile test mkdirs memcheck run

all: mkdirs $(OUT)$(BINARY)

##############################################
#   Create binary from object files
##############################################
$(OUT)$(BINARY): $(OBJECTS)
	$(CC) $(ASAN) -o $@ $^

##############################################
#   Create object files
##############################################
$(OUT)%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ASAN) -c -o $@ $<

run: all
	./$(OUT)${BINARY} ./$(OUT)sdb.db

memcheck: all
	valgrind --leak-check=full ./$(OUT)${BINARY}

clean: testclean
	@rm -rf $(OUT) $(OBJECTS) $(DEPFILES) 2>/dev/null || true

npm:
	@(cd ./tests; npm install) > /dev/null

test: all npm
	@(cd ./tests; npm test)

testclean:
	@rm -rf tests/$(OUT)

mkdirs:
	@mkdir -p $(OUT)
