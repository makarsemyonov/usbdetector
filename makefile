CC = gcc
CFLAGS = -ludev

TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC)  $< -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET)