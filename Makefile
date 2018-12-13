# adapted from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

BUILD_DIR ?= ./build
SRC_DIRS ?= ./source

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(CXXFLAGS) $(INC_FLAGS) -MMD -MP -std=c++17 -O3 -Wall -Wextra

.PHONY: all

all = traffic_sim

# link all to traffic_sim
$(BUILD_DIR)/traffic_sim: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# compile .cpp source files
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
