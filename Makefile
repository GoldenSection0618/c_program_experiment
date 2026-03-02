CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude

TARGET = billing_system
SRC = src/main.c \
      src/presentation/menu.c \
      src/business/billing_service.c \
      src/data/repository.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
