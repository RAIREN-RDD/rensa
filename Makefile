CXX := g++
CXX_FLAGS := -std=c++23 -Wall -Iinclude -DRENSA_COMMIT_HASH=\"$(shell git rev-parse HEAD)\"
LD_FLAGS := libs/libyaml-cpp.a

SRC_DIR := src
BUILD_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := $(BUILD_DIR)/rensa

dev: CXX_FLAGS += -DRENSA_DEV
dev: all

release: all

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

install: release
	cp $(TARGET) /bin

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean dev release install