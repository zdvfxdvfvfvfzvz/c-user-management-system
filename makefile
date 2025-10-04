# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# File names
MAIN_SRC = main.c
UNIT_TEST_SRC = unit_test.c
E2E_TEST_SRC = e2e_test.c

# Output executables
MAIN_EXEC = review_system
UNIT_TEST_EXEC = unit_test
E2E_TEST_EXEC = e2e_test

# Default target (runs when you just type 'make')
all: $(MAIN_EXEC)

# Build main program
$(MAIN_EXEC): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN_EXEC) $(MAIN_SRC) $(LDFLAGS)
	@echo "✓ Main program compiled: ./$(MAIN_EXEC)"

# Build unit tests
$(UNIT_TEST_EXEC): $(UNIT_TEST_SRC)
	$(CC) $(CFLAGS) -o $(UNIT_TEST_EXEC) $(UNIT_TEST_SRC) $(LDFLAGS)
	@echo "✓ Unit tests compiled: ./$(UNIT_TEST_EXEC)"

# Build e2e tests
$(E2E_TEST_EXEC): $(E2E_TEST_SRC)
	$(CC) $(CFLAGS) -o $(E2E_TEST_EXEC) $(E2E_TEST_SRC) $(LDFLAGS)
	@echo "✓ E2E tests compiled: ./$(E2E_TEST_EXEC)"

# Build everything
build-all: $(MAIN_EXEC) $(UNIT_TEST_EXEC) $(E2E_TEST_EXEC)
	@echo "✓ All programs compiled!"

# Run tests
test: $(UNIT_TEST_EXEC)
	@echo "Running unit tests..."
	./$(UNIT_TEST_EXEC)

e2e: $(E2E_TEST_EXEC)
	@echo "Running E2E tests..."
	./$(E2E_TEST_EXEC)

test-all: $(UNIT_TEST_EXEC) $(E2E_TEST_EXEC)
	@echo "Running all tests..."
	./$(UNIT_TEST_EXEC)
	./$(E2E_TEST_EXEC)

# Run main program
run: $(MAIN_EXEC)
	./$(MAIN_EXEC)

# Clean up compiled files
clean:
	rm -f $(MAIN_EXEC) $(UNIT_TEST_EXEC) $(E2E_TEST_EXEC)
	@echo "✓ Cleaned up executables"

# Remove CSV files (reset data)
clean-data:
	rm -f reviews.csv
	@echo "✓ Removed data files"

# Full clean
clean-all: clean clean-data
	@echo "✓ Full cleanup done"

# Show help
help:
	@echo "Available commands:"
	@echo "  make           - Compile main program"
	@echo "  make build-all - Compile everything"
	@echo "  make test      - Run unit tests"
	@echo "  make e2e       - Run E2E tests"
	@echo "  make test-all  - Run all tests"
	@echo "  make run       - Run main program"
	@echo "  make clean     - Remove executables"
	@echo "  make clean-all - Remove executables and data"

# Prevent make from confusing targets with files
.PHONY: all build-all test e2e test-all run clean clean-data clean-all help