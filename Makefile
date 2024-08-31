BINARY=squeel
OUT=out/
CODEDIRS=. src
INCDIRS=./include/
DEPFLAGS=-MP -MD
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPFILES=$(patsubst %.c,%.d,$(CFILES))
-include $(DEPFILES)

DEBUG=-g -DDEBUG
OPT=-O0
RT_NULL_CHECKS=-DNULL_CHECKS -DNULL_KILLS

CC=gcc
CC_WARN=-Wall -Wshadow -Wextra -Wformat=2 -Wpedantic -fmax-errors=10 -Wno-unknown-pragmas
CFLAGS=${CC_WARN} $(OPT) -std=gnu11 ${DEBUG} ${RT_NULL_CHECKS} $(foreach D,$(INCDIRS),-I$(D)) ${DEPFLAGS}

.PHONY: all clean dbg_compile test mkdirs memcheck

all: mkdirs $(OUT)$(BINARY)

##############################################
#   Create shared library from object files
##############################################
$(OUT)$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

##############################################
#   Create object files from src dir
##############################################
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	@./$(OUT)${BINARY}

memcheck: all
	@valgrind --leak-check=full ./$(OUT)${BINARY}

clean:
	@rm -rf $(OUT)/* $(OUT)$(BINARY) $(OBJECTS) $(DEPFILES) 2>/dev/null || true

mkdirs:
	@mkdir -p $(OUT)