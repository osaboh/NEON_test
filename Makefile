TARGET = neon_test
SRCES = neon_test.c

all: $(TARGET)
$(TARGET): $(SRCES)
	arm-linux-gnueabihf-gcc -mfpu=neon  -std=gnu99 $(SRCES) -o $(TARGET)

run: $(TARGET)
	qemu-arm ./$(TARGET)

format: $(SRCES)
	clangformat.sh -linux $(SRCES)

clean:
	rm -f $(TARGET) *~
