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
TEST_DIR ?= ./tests

SRCS += $(foreach  dir,$(SRC_DIRS),$(call rwildcard,$(dir),*.c*)) submodules/tracy/public/TracyClient.cpp
OBJS :=  $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TESTS := $(call rwildcard,$(TEST_DIR),*.c*)
TEST_OBJS  := $(TESTS:%=$(BUILD_DIR)/%.o) $(filter-out ./build/./src/main.cpp.o, $(subst ./build/./src/core.h.o,./build/./src/app.cpp.o,$(OBJS)))
TEST_DEPS := $(TEST_OBJS:.o=.d)

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h


INC_DIRS := $(foreach dir,$(INCLUDE_DIR),$(call rwildcardDir,$(dir)))
TEST_INC_DIRS := $(foreach dir,$(INCLUDE_DIR),$(call rwildcardDir,$(dir)))
INC_FLAGS := $(addprefix -I,$(INC_DIRS) $(LIB_DIR))

CPPFLAGS ?=   -std=c++20 -Wno-c99-designator
CFLAGS ?= -std=c99

CXXFLAGS += $(INC_FLAGS)  -MMD -MP -g -pthread -O0 -ggdb3 -DTRACY_ENABLE

LDFLAGS =  -lstdc++ -lgflags -lglog -lGL -lrt -lm -ldl  -lwayland-client -lxkbcommon -lpulse -lEGL -lwayland-egl
TEST_LDFLAGS = -lgtest -lgtest_main -lgmock  


$(OUTPUT_DIR)/$(TARGET_EXEC): $(OBJS)
	mkdir -p output
	$(CC) $(CXXFLAGS) $(OBJS)  -o $@ $(LDFLAGS)

print:
	@echo ./include/ $(wildcard ./include/*/) $(wildcard ./include/*/*/) $(wildcard ./include/*/*/*/) $(wildcard ./include/*/*/*/*/)
	@echo $(call rwildcardDir,./include)

	

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC)  $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
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
	$(RM) -f gtest.a gtest_main.a *.o
	$(RM) -r $(BUILD_DIR) 
	$(RM) -r $(OUTPUT_DIR)

setEnv: $(LIB_DIR)
	@echo $<
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$<:

-include $(DEPS)
-include $(TEST_DEPS)

MKDIR_P ?= mkdir -p
