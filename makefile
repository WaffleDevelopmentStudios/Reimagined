SRC = source/main.cpp /usr/src/glad.c

CC = g++
CCWIN32 = x86_64-w64-mingw32-g++

CFLAGS = -std=c++17 -O2

LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
LDWIN32FLAGS = -Llib/ -lglfw3 -lopengl32 -Wl,--subsystem,windows -lgdi32 -static-libstdc++ -static-libgcc


OBJ_NAME = Reimagined
OBJ_PATH = bin/$(OBJ_NAME)

.PHONY: test clean build buildtest

build: $(SRC)
	$(CC) $(CFLAGS) -o $(OBJ_PATH) $(SRC) $(LDFLAGS)

buildwin32:
	$(CCWIN32) $(CFLAGS) -o $(OBJ_PATH) $(SRC) $(LDWIN32FLAGS)

test: $(OBJ_PATH)
	./$(OBJ_PATH)

buildtest:
	$(CC) $(CFLAGS) -o $(OBJ_PATH) $(SRC) $(LDFLAGS)
	./$(OBJ_PATH)

clean:
	rm -f $(OBJ_PATH)
