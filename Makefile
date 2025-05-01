CC = gcc
CFLAGS = -lm -g
SRC_FILES := $(wildcard src/*.c)
OUTPUT_FILES := $(patsubst src/%.c,build/%.o,$(SRC_FILES))

all: t-edit

t-edit: $(OUTPUT_FILES) editor.c
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@if [ ! -d "build" ]; then mkdir build; fi
	$(CC) -g -c $^ -o $@ 

clean:
	rm -rf build/ t-edit*

