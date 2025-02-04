#This makefile for the creation of client side executable
CC = gcc
CFLAGS = -Wall
TARGET = client

all: $(TARGET)

$(TARGET): client_side.c
	$(CC) $(CFLAGS) -o $(TARGET) client_side.c

clean:
	rm -f $(TARGET)