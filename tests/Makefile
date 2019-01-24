
INPUTS_DIR ?= ./
OUTPUTS_DIR ?= $(INPUTS_DIR)
OUTPUTS_POSTFIX ?= .out
SOLUTIONS_DIR ?= $(OUTPUTS_DIR)
SOLUTIONS_POSTFIX ?= .sol
RUNNER ?= ../traffic_sim.dbg
JQ ?= jq
DIFF ?= diff
MKTEMP ?= mktemp

# Remove trailing slashes to not mess with find
CLEAN_INPUTS_DIR := $(patsubst %/,%,$(INPUTS_DIR))
CLEAN_OUTPUTS_DIR := $(patsubst %/,%,$(OUTPUTS_DIR))
CLEAN_SOLUTIONS_DIR := $(patsubst %/,%,$(SOLUTIONS_DIR))

# Find inputs and compute outputs
INPUTS := $(shell find $(CLEAN_INPUTS_DIR) -name "*.json")
OUTPUTS := $(INPUTS:$(CLEAN_INPUTS_DIR)/%=$(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX))

# Arbitrarily define solutions
SOLUTIONS_OUTS := $(OUTPUTS:$(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX)=/nonexisting/%$(SOLUTIONS_POSTFIX))

$(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX): $(CLEAN_INPUTS_DIR)/%
	$(RUNNER) <$< >$@

/nonexisting/%$(SOLUTIONS_POSTFIX): $(CLEAN_OUTPUTS_DIR)/%$(OUTPUTS_POSTFIX)
	$(eval NORMALISED_OUTPUT := $(shell $(MKTEMP)))
	$(eval NORMALISED_SOLUTION := $(shell $(MKTEMP)))
	$(JQ) -S . $< >$(NORMALISED_OUTPUT)
	$(JQ) -S . $(@:/nonexisting/%=$(CLEAN_SOLUTIONS_DIR)/%) >$(NORMALISED_SOLUTION)
	$(DIFF) $(NORMALISED_SOLUTION) $(NORMALISED_OUTPUT) || true
	$(RM) $(NORMALISED_OUTPUT) $(NORMALISED_SOLUTION)

all:

run: $(OUTPUTS)

verify: $(SOLUTIONS_OUTS)

clean:
	$(RM) $(OUTPUTS)

.PHONY: run verify clean
