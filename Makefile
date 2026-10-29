
TARGET=simulator
BUILD_DIR=build
SRC_DIR=src

CXX = g++
CXXFLAGS = -std=c++23 -Iinclude -Iexternal/fmt/include -Iexternal/glfw/include -Wall

SRC_FILES=$(shell find $(SRC_DIR)/ -name "*.cpp")
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
DEPS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRC_FILES))

BUILD_DIRS=$(sort $(dir $(OBJS)))

$(BUILD_DIR)/$(TARGET): $(OBJS) external/fmt/build/libfmt.a | $(BUILD_DIR)
	$(CXX) -o $(BUILD_DIR)/$(TARGET) $(OBJS) -Lexternal/fmt/build -lfmt -Lexternal/glfw/build/src -lglfw3 -lGLEW

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(BUILD_DIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD

run: $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)

$(BUILD_DIRS):
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	echo $(BUILD_DIRS)
	for d in $(BUILD_DIRS); do
		mkdir -p $d
	done
	chmod 777 $(BUILD_DIR)

external/fmt/build/libfmt.a: external/fmt
	cd external/fmt && mkdir -p build && cd build && cmake .. && make fmt

.PHONY: clean
clean: $(BUILD_DIR)
	rm -rf $(BUILD_DIR)/*

.PHONY: clean-deps
clean-deps:
	rm -rf external/fmt/build
