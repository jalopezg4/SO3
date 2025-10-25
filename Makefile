CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Iinclude -Wall -Wextra
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = gsea

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean dirs

all: dirs $(BIN_DIR)/$(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Link
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | dirs
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)