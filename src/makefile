CXX ?= g++
CXXFLAGS ?= -O3 -Wall -Wextra -fpermissive -Wno-switch -Wno-missing-field-initializers
CXXFLAGS += -std=c++2a
LDFLAGS +=  -lstdc++fs

BUILD_DIR := ../build

SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(foreach file,$(patsubst %.cpp,%.o,$(SRC_FILES)),$(BUILD_DIR)/$(file))
RESHADE_SRC := $(wildcard ../reshade/source/effect*.cpp)
RESHADE_OBJ := $(foreach file,$(patsubst %.cpp,%.o,$(notdir $(RESHADE_SRC))),$(BUILD_DIR)/reshade/$(file))

all: $(BUILD_DIR)/ReshadeFxCompiler

$(BUILD_DIR)/ReshadeFxCompiler: $(OBJ_FILES64) $(RESHADE_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


$(BUILD_DIR)/reshade/%.o: ../reshade/source/%.cpp $(BUILD_DIR)
	$(CXX) $< -o $@ -c  $(CXXFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/reshade


