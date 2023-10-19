CC = clang

GTEST_DIR=/usr/src/googletest/

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
rwildcardDir=$(wildcard $1*/) $(foreach d,$(wildcard $1*),$(call rwildcardDir,$d/))


TARGET_EXEC ?= editor.out

BUILD_DIR ?= ./build
OUTPUT_DIR ?= ./output
INCLUDE_DIR ?= ./include ./vendor ./submodules/tracy/public/tracy
SRC_DIRS ?= ./src/ ./vendor
SHADERS_DIRS ?= $(wildcard ./assets/shaders/*)
TEST_DIR ?= ./tests

SHADERS = $(call rwildcard,$(SHADERS_DIRS[0]),*.frag) $(call rwildcard,$(SHADERS_DIRS[1]),*.vert)
SHADERS_BINARY = $(foreach  shader,$(SHADERS),$(shader).spv) 
SRCS += $(foreach  dir,$(SRC_DIRS),$(call rwildcard,$(dir),*.c*)) submodules/tracy/public/TracyClient.cpp
OBJS :=  $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(SRCS))))
DEPS := $(OBJS:.o=.d)

TESTS := $(call rwildcard,$(TEST_DIR),*.c*)
TEST_OBJS  := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(TESTS)))) $(filter-out ./build/./src/main.o, $(subst ./build/./src/core.h.o,./build/./src/app.cpp.o,$(OBJS)))
TEST_DEPS := $(TEST_OBJS:.o=.d)

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h


INC_DIRS := $(foreach dir,$(INCLUDE_DIR),$(call rwildcardDir,$(dir)))
TEST_INC_DIRS := $(foreach dir,$(INCLUDE_DIR),$(call rwildcardDir,$(dir)))
INC_FLAGS := $(addprefix -I,$(INC_DIRS) $(LIB_DIR))

CPPFLAGS ?=   -std=c++20 -Wno-c99-designator
CFLAGS ?= -std=c99

CXXFLAGS += $(INC_FLAGS)  -MMD -MP -g -pthread -O0 -ggdb3 

LDFLAGS =  -lstdc++ -lgflags -lglog -lGL -lrt -lm -ldl  -lwayland-client -lxkbcommon -lpulse -lEGL -lwayland-egl -lvulkan
TEST_LDFLAGS = -lgtest -lgtest_main -lgmock  


$(OUTPUT_DIR)/$(TARGET_EXEC): $(OBJS) $(SHADERS_BINARY)
	mkdir -p output
	$(CC) $(CXXFLAGS) $(OBJS)  -o $@ $(LDFLAGS)


print:	
	@echo $(AR) $(RM)


%.spv: %
	glslc $< -o $@

# c++ source
$(BUILD_DIR)/%.o: %.cpp
	@echo "building file: " $<
	$(MKDIR_P) $(dir $@)
	$(CC)  $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@echo "building file: " $<
	$(MKDIR_P) $(dir $@)
	$(CC)  $(CFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

test: $(OUTPUT_DIR)/test.out

$(OUTPUT_DIR)/test.out: $(TEST_OBJS)
	mkdir -p output
	$(CC)  $(CXXFLAGS) $(TEST_OBJS) -o $@ $(LDFLAGS) $(TEST_LDFLAGS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	$(RM) gtest.a gtest_main.a *.o
	$(RM) -r $(BUILD_DIR) 
	$(RM) -r $(OUTPUT_DIR)

setEnv: $(LIB_DIR)
	@echo $<
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$<:

-include $(DEPS)
-include $(TEST_DEPS)

MKDIR_P ?= mkdir -p
