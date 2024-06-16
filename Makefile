CC = g++
CFLAGS = -Wall -Wextra -pthread -std=c++11

all: main client

main: cmd/main.cpp
	$(CC) $(CFLAGS) -o main cmd/main.cpp
client: cmd/client.cpp
	$(CC) $(CFLAGS) -o client cmd/client.cpp
.PHONY: clean
clean:
	rm -f main client