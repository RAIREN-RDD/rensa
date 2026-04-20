PROJECT_NAME = rensa
CXX := g++

CXX_STD      := -std=c++23
CXX_WARN     := -Wall -Wextra

CXX_ARCH     := -march=x86-64

CXX_INCLUDES := \
	-Iinclude                            \
	-Ithird_party/yaml-cpp/include       \
	-Ithird_party/xz/src/liblzma/api     \
	-Ithird_party/libarchive/libarchive  \

GIT_HASH     := $(shell git rev-parse HEAD)
CXX_DEFINES  := -DRENSA_COMMIT_HASH=\"$(GIT_HASH)\"

CXX_FLAGS    := $(CXX_STD) $(CXX_WARN) $(CXX_ARCH) \
                $(CXX_INCLUDES) \
                $(CXX_DEFINES)

LD_FLAGS  := -static libs/libyaml-cpp.a libs/liblzma.a libs/libarchive.a

SRC_DIR    := src
BUILD_DIR  := build

SRCS  := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS  := $(OBJS:.o=.d)

TARGET := $(BUILD_DIR)/$(PROJECT_NAME)

dev: CXX_FLAGS += -DRENSA_DEV
dev: all

release: all

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

$(BUILD_DIR):
	mkdir -p $@

INSTALL_DIR = ${HOME}/.apps/rairen/${PROJECT_NAME}
INSTALL_TARGET = $(INSTALL_DIR)/entry

install: release
	mkdir -p $(INSTALL_DIR)
	bash install.sh
	cp $(TARGET) $(INSTALL_TARGET)
	chmod +x $(INSTALL_TARGET)
	ln -sfn $(INSTALL_TARGET) /usr/local/bin/rensa

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean dev release install