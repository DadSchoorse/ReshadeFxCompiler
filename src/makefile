CXX ?= g++
CXXFLAGS ?= -O3 -Wall -Wextra -Wno-unknown-pragmas
CXXFLAGS += -std=c++2a -I../reshade/deps/spirv/include/spirv/unified1
LDFLAGS +=  -lstdc++fs

BUILD_DIR := ../build

SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(foreach file,$(patsubst %.cpp,%.o,$(SRC_FILES)),$(BUILD_DIR)/$(file))
RESHADE_SRC := $(wildcard ../reshade/source/effect*.cpp)
RESHADE_SRC := $(filter-out ../reshade/source/effect_codegen_hlsl.cpp,$(RESHADE_SRC))
RESHADE_SRC := $(filter-out ../reshade/source/effect_codegen_glsl.cpp,$(RESHADE_SRC))
RESHADE_OBJ := $(foreach file,$(patsubst %.cpp,%.o,$(notdir $(RESHADE_SRC))),$(BUILD_DIR)/reshade/$(file))

all: $(BUILD_DIR)/ReshadeFxCompiler

$(BUILD_DIR)/ReshadeFxCompiler: $(OBJ_FILES) $(RESHADE_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)
	$(CXX) $< -o $@ -c  $(CXXFLAGS)

$(BUILD_DIR)/reshade/%.o: ../reshade/source/%.cpp $(BUILD_DIR)
	$(CXX) $< -o $@ -c  $(CXXFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/reshade


