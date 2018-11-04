CC = gcc
EXECUTABLES = PingClient

CFLAGS = -g
all:$(EXECUTABLES)
clean:
	rm -f core *.o $(EXECUTABLES) a.out *.a

PingClient:PingClient.c
	$(CC) $(CFLAGS) PingClient.c -o PingClient 
