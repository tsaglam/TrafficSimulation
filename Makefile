# adapted from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

UNAME := $(shell uname)

FILE_EXTENSION =
FILE_EXTENSION_DBG = .dbg
FILE_EXTENSION_TEST = .test

ifdef OMP
	FILE_EXTENSION = .omp
	FILE_EXTENSION_DBG = .dbg.omp
	FILE_EXTENSION_TEST = .test.omp
	ifeq ($(UNAME), Darwin)
		LDFLAGS += -lomp
		PARALLEL_FLAGS = -DOMP -Xpreprocessor -fopenmp
	else
		PARALLEL_FLAGS = -DOMP -fopenmp
	endif
else ifdef CUDA
	FILE_EXTENSION = .cuda
	FILE_EXTENSION_DBG = .dbg.cuda
	FILE_EXTENSION_TEST = .test.cuda
	# TODO: set compiler and linker flags
endif

ifdef TIMER
	CXXFLAGS += -DTIMER
endif
ifdef AVX
	FILE_EXTENSION := $(FILE_EXTENSION).avx
	FILE_EXTENSION_DBG := $(FILE_EXTENSION_DBG).avx
	FILE_EXTENSION_TEST := $(FILE_EXTENSION_TEST).avx
	PARALLEL_FLAGS += -mavx
endif

BUILD_DIR ?= ./build
SRC_DIRS ?= ./source
TEST_DIRS ?= ./testcases

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp")
OBJS := $(SRCS:%=$(BUILD_DIR)/%$(FILE_EXTENSION).o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find $(TEST_DIRS) -name "*.cpp") $(shell find $(SRC_DIRS) -name "*.cpp")
TEST_OBJS := $(filter-out ./build/./source/main.cpp$(FILE_EXTENSION_TEST).o, $(TEST_SRCS:%=$(BUILD_DIR)/%$(FILE_EXTENSION_TEST).o))
TEST_DEPS := $(TEST_OBJS:.o=.d)

DBG_OBJS := $(SRCS:%=$(BUILD_DIR)/%$(FILE_EXTENSION_DBG).o)
DBG_DEPS := $(DBG_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(CXXFLAGS) $(INC_FLAGS) $(PARALLEL_FLAGS) -MMD -MP -std=c++17 -O3 -Wall -Wextra

DBG_CPPFLAGS ?= $(CXXFLAGS) $(INC_FLAGS) $(PARALLEL_FLAGS) -MMD -MP -std=c++17 -O0 -fno-omit-frame-pointer -g -fsanitize=address -Wall -Wextra
TEST_CPPFLAGS ?= $(DBG_CPPFLAGS) --coverage

# Rules regarding the primary executable

all: traffic_sim

traffic_sim: $(BUILD_DIR)/traffic_sim$(FILE_EXTENSION)
	ln -sfn $< $@ # create symlink for pipeline

# link all to traffic_sim
$(BUILD_DIR)/traffic_sim$(FILE_EXTENSION): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp$(FILE_EXTENSION).o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

# Rules regarding the debug executable

debug: traffic_sim.dbg

traffic_sim.dbg: $(BUILD_DIR)/traffic_sim$(FILE_EXTENSION_DBG)
	ln -sfn $< $@ # create symlink

# link all to traffic_sim
$(BUILD_DIR)/traffic_sim$(FILE_EXTENSION_DBG): $(DBG_OBJS)
	$(CXX) $(DBG_CPPFLAGS) $(DBG_OBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp$(FILE_EXTENSION_DBG).o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(DBG_CPPFLAGS) -c $< -o $@

# Rules regarding the test executable

test: $(BUILD_DIR)/testsuite$(FILE_EXTENSION_TEST)
	$(BUILD_DIR)/testsuite$(FILE_EXTENSION_TEST)

# link all to traffic_sim
$(BUILD_DIR)/testsuite$(FILE_EXTENSION_TEST): $(TEST_OBJS)
	$(CXX) $(TEST_CPPFLAGS) $(TEST_OBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp$(FILE_EXTENSION_TEST).o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(TEST_CPPFLAGS) -c $< -o $@

-include $(DEPS) $(DBG_DEPS) $(TEST_DEPS)

.PHONY: all debug clean test

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) traffic_sim
	$(RM) traffic_sim.dbg

MKDIR_P ?= mkdir -p
