CC		:= g++
C_FLAGS := -std=c++17 -Wall -Wextra

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=

ifeq ($(OS),Windows_NT)
EXECUTABLE_CLIENT	:= client.exe
EXECUTABLE_SERVER	:= server.exe
else
EXECUTABLE_CLIENT	:= client
EXECUTABLE_SERVER	:= server
endif

all: bin $(BIN)/$(EXECUTABLE_CLIENT) $(BIN)/$(EXECUTABLE_SERVER)

bin:
	mkdir -p bin

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE_CLIENT): $(SRC)/client.cpp
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

$(BIN)/$(EXECUTABLE_SERVER): $(SRC)/server.cpp
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)
