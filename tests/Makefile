
INPUTS_DIR ?= ./
OUTPUTS_DIR ?= $(INPUTS_DIR)
OUTPUTS_POSTFIX ?= .out
RUNNER ?= ../traffic_sim.dbg

# Remove trailing slashes to not mess with find
CLEAN_INPUTS_DIR := $(patsubst %/,%,$(INPUTS_DIR))
CLEAN_OUTPUTS_DIR := $(patsubst %/,%,$(OUTPUTS_DIR))

# Find inputs and compute outputs
INPUTS := $(shell find $(CLEAN_INPUTS_DIR) -name "*.json")
OUTPUTS := $(INPUTS:$(CLEAN_INPUTS_DIR)/%=$(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX))

$(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX): $(CLEAN_INPUTS_DIR)/%
	$(RUNNER) <$< >$@

all:

run: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS)

.PHONY: run clean
