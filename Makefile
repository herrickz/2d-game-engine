
TARGET=simulator
CC=/opt/homebrew/bin/g++-16
BUILD_DIR=build
SRC_DIR=src

SRC_FILES=$(shell find $(SRC_DIR)/ -name "*.cpp")
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
DEPS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.d, $(SRC_FILES))

BUILD_DIRS=$(sort $(dir $(OBJS)))

$(BUILD_DIR)/$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(TARGET) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(BUILD_DIRS)
	$(CC) -std=c++23 -Iinclude -c $< -o $@ -MMD

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

.PHONY: clean
clean: $(BUILD_DIR)
	rm -rf $(BUILD_DIR)/*
