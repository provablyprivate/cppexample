CC		:= g++
C_FLAGS := -std=c++17 -Wall -Wextra -lcrypt

BIN		:= bin
SRC		:= src

INCLUDE := -Iextern/poco/Net/include -Iextern/poco/Foundation/include -Iextern/poco/Crypto -Iextern/poco/JSON/include
LIB		:= -L/usr/local/lib

LIBRARIES	:= -lPocoNet -lPocoFoundation -lPocoCrypto -lPocoJSON

ifeq ($(OS),Windows_NT)
EXECUTABLE_CLIENT	:= client.exe
EXECUTABLE_SERVER	:= server.exe
EXECUTABLE_CRYPT	:= crypt.exe
EXECUTABLE_JSON 	:= jsonparser.exe

else
EXECUTABLE_CLIENT	:= client
EXECUTABLE_SERVER	:= server
EXECUTABLE_CRYPT	:= crypt
EXECUTABLE_JSON 	:= jsonparser

endif

build: bin $(BIN)/$(EXECUTABLE_CLIENT) $(BIN)/$(EXECUTABLE_SERVER) $(BIN)/$(EXECUTABLE_CRYPT) $(BIN)/$(EXECUTABLE_JSON)


bin:
	mkdir -p bin

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE_CLIENT): $(SRC)/client.cpp
	$(CC) $(C_FLAGS) -I $(POCO_NET_INCLUDE) $(LIB) $^ -o $@ $(LIBRARIES)

$(BIN)/$(EXECUTABLE_SERVER): $(SRC)/server.cpp
	$(CC) $(C_FLAGS) -I $(POCO_NET_INCLUDE) $(LIB) $^ -o $@ $(LIBRARIES)

$(BIN)/$(EXECUTABLE_JSON): $(SRC)/jsonparser.cpp
	$(CC) $(C_FLAGS) -I $(POCO_NET_INCLUDE) $(LIB) $^ -o $@ $(LIBRARIES)
  
$(BIN)/$(EXECUTABLE_CRYPT): $(SRC)/crypt.cpp
	$(CC) $(C_FLAGS) -I $(POCO_NET_INCLUDE) $(LIB) $^ -o $@ $(LIBRARIES)
