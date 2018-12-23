# adapted from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

BUILD_DIR ?= ./build
SRC_DIRS ?= ./source

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

DBGOBJS := $(SRCS:%=$(BUILD_DIR)/%.dbg.o)
DBGDEPS := $(DBGOBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(CXXFLAGS) $(INC_FLAGS) -MMD -MP -std=c++17 -O3 -Wall -Wextra

DBGCPPFLAGS ?= $(CXXFLAGS) $(INC_FLAGS) -MMD -MP -std=c++17 -O0 -fno-omit-frame-pointer -g -fsanitize=address -Wall -Wextra

# Rules regarding the primary executable

all: traffic_sim

traffic_sim: $(BUILD_DIR)/traffic_sim
	ln -sfn $< $@ # create symlink for pipeline

# link all to traffic_sim
$(BUILD_DIR)/traffic_sim: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)

# Rules regarding the debug executable

debug: traffic_sim.dbg

traffic_sim.dbg: $(BUILD_DIR)/traffic_sim.dbg
	ln -sfn $< $@ # create symlink

# link all to traffic_sim
$(BUILD_DIR)/traffic_sim.dbg: $(DBGOBJS)
	$(CXX) $(DBGCPPFLAGS) $(DBGOBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp.dbg.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(DBGCPPFLAGS) -c $< -o $@

-include $(DBGDEPS)

.PHONY: all debug clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) traffic_sim
	$(RM) traffic_sim.dbg

MKDIR_P ?= mkdir -p
