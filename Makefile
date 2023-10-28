ifeq ($(OS),linux)
  CC = gcc
  CFLAGS = $(shell pkg-config allegro-5 allegro_image-5 allegro_font-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5 --libs --cflags)
  TARGET = Lunch!
else
  CC = C:\MinGW\bin\gcc
  CFLAGS = -I include/ -L lib/ -lallegro -lallegro_image -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec
  TARGET = C:\Users\Jezura777\Desktop\Dev\C\allegro5\Lunch!\Lunch!.exe
endif

all:
	$(CC) debug.c -o $(TARGET) $(CFLAGS)

