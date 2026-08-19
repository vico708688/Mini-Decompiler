DST = build
SRC = source
INC = include

SOURCES := $(shell find $(SRC) -type f -name '*.c')
OBJECTS := $(patsubst $(SRC)/%.c,$(DST)/%.o,$(SOURCES))

C_FLAGS = -I$(INC)
FLAGS = -Wall -g

all: $(DST)/main

$(DST)/main: $(OBJECTS)
	@mkdir -p $(dir $@)
	gcc $(FLAGS) -o $@ $(OBJECTS)

$(DST)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	gcc $(C_FLAGS) $(FLAGS) -c $< -o $@

.PHONY: clean

clean:
	@rm -rf $(DST)