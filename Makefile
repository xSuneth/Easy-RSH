# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS = -lpthread

# Debug vs Release build
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0 -DDEBUG
else
    CXXFLAGS += -O3 -DNDEBUG
endif

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source files
COMMON_SRC = $(SRC_DIR)/socket/Socket.cpp
SERVER_SRC = $(COMMON_SRC) $(SRC_DIR)/server/Server.cpp $(SRC_DIR)/server/CommandExecutor.cpp $(SRC_DIR)/server/server_main.cpp
CLIENT_SRC = $(COMMON_SRC) $(SRC_DIR)/client/Client.cpp $(SRC_DIR)/client/client_main.cpp

# Object files
COMMON_OBJ = $(BUILD_DIR)/Socket.o
SERVER_OBJ = $(COMMON_OBJ) $(BUILD_DIR)/Server.o $(BUILD_DIR)/CommandExecutor.o $(BUILD_DIR)/server_main.o
CLIENT_OBJ = $(COMMON_OBJ) $(BUILD_DIR)/Client.o $(BUILD_DIR)/client_main.o

# Executables
SERVER_BIN = server
CLIENT_BIN = client

# Targets
.PHONY: all clean server client test help

all: server client

# Build server
server: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $(SERVER_BIN) $^ $(LDFLAGS)
	@echo "Built server successfully!"

# Build client
client: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built client successfully!"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/socket/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/server/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/client/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Dependencies (headers)
$(BUILD_DIR)/Socket.o: $(INC_DIR)/Socket.h
$(BUILD_DIR)/Server.o: $(INC_DIR)/Server.h $(INC_DIR)/Socket.h $(INC_DIR)/CommandExecutor.h
$(BUILD_DIR)/Client.o: $(INC_DIR)/Client.h $(INC_DIR)/Socket.h
$(BUILD_DIR)/CommandExecutor.o: $(INC_DIR)/CommandExecutor.h
$(BUILD_DIR)/server_main.o: $(INC_DIR)/Server.h
$(BUILD_DIR)/client_main.o: $(INC_DIR)/Client.h

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(SERVER_BIN) $(CLIENT_BIN)
	@echo "Cleaned build artifacts"

# Run basic test
test: all
	@echo "Running basic test..."
	@bash tests/test_basic.sh || echo "Test script not found. Run 'make' to build first."

# Help message
help:
	@echo "Makefile for Remote Command Execution System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build both server and client (default)"
	@echo "  server    - Build server only"
	@echo "  client    - Build client only"
	@echo "  clean     - Remove build artifacts"
	@echo "  test      - Run basic tests"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Options:"
	@echo "  DEBUG=1   - Build with debug symbols (default)"
	@echo "  DEBUG=0   - Build optimized release version"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build with debug"
	@echo "  make DEBUG=0      # Build optimized"
	@echo "  make clean all    # Clean and rebuild"
