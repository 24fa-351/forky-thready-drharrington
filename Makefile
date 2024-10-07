CC = gcc
TARGET = forky_thready
SRC = forky_thready.c
FORMAT_FILES = forky_thready.c

all: format $(TARGET)

format:
	clang-format -i $(FORMAT_FILES)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)