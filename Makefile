CC = cc
ALLEGRO = allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_primitives-5
CFLAGS = -Wall -Wextra -std=c11 $(shell pkg-config --cflags $(ALLEGRO))
LDLIBS = $(shell pkg-config --libs $(ALLEGRO))
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=build/%.o)
TARGET = build/game

.PHONY: all assets run clean distclean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

assets: | build
	cp assets/*.png assets/*.ttf assets/*.txt assets/audio/*.ogg build/

run: all assets
	cd build && ./game

clean:
	rm -f $(OBJS) $(TARGET)

distclean:
	rm -rf build
