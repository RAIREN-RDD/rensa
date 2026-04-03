CXX := g++
CXX_FLAGS := -std=c++23 -Wall -Wextra -Iinclude
LD_FLAGS := libs/libyaml-cpp.a

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := $(BUILD_DIR)/rensa

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
