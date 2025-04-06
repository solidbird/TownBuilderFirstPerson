export PKG_CONFIG_PATH=~/Projects/libs/raylib/build/raylib
PKGS=raylib
CFLAGS=-Wall -Wextra -ggdb -pedantic -std=c11 `pkg-config --cflags --static $(PKGS)`
LIBS=`pkg-config --libs --static $(PKGS)`
BUILD=../build
TESTS=../tests

all: build_folder main

build_folder:
	@mkdir -p $(BUILD)

main: main.c block_utils.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $^ $(LIBS) -lm
