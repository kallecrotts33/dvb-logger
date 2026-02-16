# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -O0 -Wall -Wextra -std=c++17

# Libraries
LIBS = -lsqlite3 -lcurl

# Directories
SRC_DIR = src
APP_DIR = .

# Target executable
TARGET = $(APP_DIR)/dvb-logger

# Source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Db.cpp \
	   $(SRC_DIR)/GetWeather.cpp

# Object files (stored in src/)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link step
$(TARGET): $(OBJS) | $(APP_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile step
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure app directory exists
$(APP_DIR):
	mkdir -p $(APP_DIR)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
