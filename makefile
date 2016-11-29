CC = gcc
CFLAGS = -std=c11 -Wall -O3 -pthread -flto
C_FILES = bitboard.c eval.c genmoves.c magicmoves.c main.c move.c mt19937-64.c perft.c position.c random.c search.c timer.c uci.c xboard.c
SRC_PATH = src
SRC = $(patsubst %,$(SRC_PATH)/%,$(C_FILES))
DEPS = src/bitboard.h src/defs.h src/engine.h src/magicmoves.h src/position.h src/random.h src/search.h src/timer.h src/tt.h src/tune.h
OBJ_PATH = objs
_OBJS = $(C_FILES:.c=.o)
OBJS = $(patsubst %,$(OBJ_PATH)/%,$(_OBJS))
EXEC_PATH = .
EXEC = wyldchess

ifeq ($(FAST), 1)
	CFLAGS += -DTEST
endif

ifeq ($(POPCNT), 1)
	CFLAGS += -mpopcnt
endif

ifeq ($(STATS), 1)
	CFLAGS += -DSTATS
endif

ifdef RELEASE

EXEC := $(EXEC)$(RELEASE)

ifeq ($(FAST), 1)
	EXEC := $(EXEC)_fast_tc
endif

ifeq ($(POPCNT), 1)
	EXEC := $(EXEC)_popcnt
endif

ifeq ($(STATS), 1)
	EXEC := $(EXEC)_stats
endif

ifeq ("$(TARGET)", "win64")
	EXEC := $(EXEC).exe
	CFLAGS += -static
	EXEC_PATH = binaries/v$(RELEASE)/$(TARGET)
else ifeq ("$(TARGET)", "linux")
	EXEC_PATH = binaries/v$(RELEASE)/$(TARGET)
else
$(error TARGET not defined)
endif

endif

BIN = $(EXEC_PATH)/$(EXEC)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(DEPS)
	-mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

all: $(BIN)

clean:
	-rm -f $(OBJS)
