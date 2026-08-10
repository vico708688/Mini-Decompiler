DST = build
SRC = source
INC = include

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(DST)/%.o, $(SOURCES))

C_FLAGS = -I$(INC)
FLAGS = -Wall

all: main

main: $(OBJECTS)
	gcc $(FLAGS) -o $(DST)/main $(OBJECTS)

$(DST)/%.o: $(SRC)/%.c | $(DST)
	gcc $(C_FLAGS) $(FLAGS) -c $< -o $@

$(DST):
	@mkdir -p $(DST)

.PHONY: clean

clean:
	@rm -rf $(DST)/* $(DST)/main