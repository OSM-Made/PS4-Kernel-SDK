
# Color definitions
RED     := \033[0;31m
GREEN   := \033[0;32m
YELLOW  := \033[0;33m
BLUE    := \033[0;34m
CYAN    := \033[0;36m
RESET   := \033[0m

.PHONY: all clean distclean loader lib samples install offsets check-python

# Check if offset headers exist, generate if missing
OFFSET_HEADERS := include/offsets/Offsets.h
OFFSET_YAMLS := $(wildcard offsets/firmware-*.yaml)

all: offsets lib loader samples
	@echo ""
	@echo "$(GREEN)════════════════════════════════════════$(RESET)"
	@echo "$(GREEN)     Build completed successfully!$(RESET)"
	@echo "$(GREEN)════════════════════════════════════════$(RESET)"

# Check if Python 3 and PyYAML are available
check-python:
	@command -v python3 >/dev/null 2>&1 || (echo "$(RED)Error: python3 is required but not installed$(RESET)" && exit 1)
	@python3 -c "import yaml" 2>/dev/null || (echo "$(RED)Error: PyYAML is required. Install with: pip install pyyaml$(RESET)" && exit 1)

# Generate offset headers from YAML files
offsets: check-python $(OFFSET_HEADERS)

$(OFFSET_HEADERS): $(OFFSET_YAMLS) tools/generate_offsets.py
	@echo "$(CYAN)→ Generating offset headers from YAML...$(RESET)"
	@python3 tools/generate_offsets.py && echo "$(GREEN)Offset headers generated$(RESET)" || (echo "$(RED)Offset generation failed$(RESET)" && exit 1)

# Force regenerate offsets even if headers exist
.PHONY: regenerate-offsets
regenerate-offsets: check-python
	@echo "$(CYAN)→ Regenerating offset headers from YAML...$(RESET)"
	@python3 tools/generate_offsets.py && echo "$(GREEN)Offset headers regenerated$(RESET)" || (echo "$(RED)Offset generation failed$(RESET)" && exit 1)

# Build CRT files
lib: offsets
	@echo "$(CYAN)→ Building CRT files...$(RESET)"
	@$(MAKE) -C lib && echo "$(GREEN)CRT files built$(RESET)" || (echo "$(RED)CRT build failed$(RESET)" && exit 1)

# Build loader libraries
loader:
	@echo "$(CYAN)→ Building loader libraries...$(RESET)"
	@$(MAKE) -C loader && echo "$(GREEN)Loader libraries built$(RESET)" || (echo "$(RED)Loader build failed$(RESET)" && exit 1)

# Build samples (both kernel module and userland payload)
samples: lib loader
	@echo "$(CYAN)→ Building samples...$(RESET)"
	@$(MAKE) -C samples/hello_world && echo "$(GREEN)hello_world built$(RESET)" || (echo "$(RED)hello_world build failed$(RESET)" && exit 1)
	@$(MAKE) -C samples/userland_payload && echo "$(GREEN)userland_payload built$(RESET)" || (echo "$(RED)userland_payload build failed$(RESET)" && exit 1)

# Install everything
install: all
	@echo "$(CYAN)→ Installing to SDK...$(RESET)"
	@$(MAKE) -C loader install && echo "$(GREEN)Installation complete$(RESET)" || (echo "$(RED)Installation failed$(RESET)" && exit 1)

clean:
	@echo "$(YELLOW)→ Cleaning build artifacts...$(RESET)"
	@$(MAKE) -C lib clean
	@$(MAKE) -C loader clean
	@$(MAKE) -C samples/hello_world clean
	@$(MAKE) -C samples/userland_payload clean
	@echo "$(GREEN)   Clean complete$(RESET)"

# Clean everything including generated headers
distclean: clean
	@echo "$(YELLOW)→ Cleaning generated headers...$(RESET)"
	@rm -f include/offsets/*.h
	@find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	@find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@echo "$(GREEN)   Deep clean complete$(RESET)"

# Individual sample targets
.PHONY: sample_kernel sample_payload

sample_kernel: lib
	@echo "$(CYAN)→ Building kernel module sample...$(RESET)"
	@$(MAKE) -C samples/hello_world && echo "$(GREEN)Kernel module built$(RESET)" || (echo "$(RED)Build failed$(RESET)" && exit 1)

sample_payload: lib loader sample_kernel
	@echo "$(CYAN)→ Building userland payload sample...$(RESET)"
	@$(MAKE) -C samples/userland_payload && echo "$(GREEN)Payload built$(RESET)" || (echo "$(RED)Build failed$(RESET)" && exit 1)

# Help target
help:
	@echo "$(BLUE)════════════════════════════════════════$(RESET)"
	@echo "$(BLUE)  PS4 Kernel SDK Build System$(RESET)"
	@echo "$(BLUE)════════════════════════════════════════$(RESET)"
	@echo ""
	@echo "$(CYAN)Build Targets:$(RESET)"
	@echo "  $(GREEN)make all$(RESET)                - Build everything (offsets + libs + samples)"
	@echo "  $(GREEN)make offsets$(RESET)            - Generate offset headers from YAML"
	@echo "  $(GREEN)make lib$(RESET)                - Build CRT files only"
	@echo "  $(GREEN)make loader$(RESET)             - Build loader libraries only"
	@echo "  $(GREEN)make samples$(RESET)            - Build all samples"
	@echo "  $(GREEN)make sample_kernel$(RESET)      - Build hello_world kernel module only"
	@echo "  $(GREEN)make sample_payload$(RESET)     - Build userland payload only"
	@echo "  $(GREEN)make install$(RESET)            - Install loader to SDK"
	@echo ""
	@echo "$(CYAN)Utility Targets:$(RESET)"
	@echo "  $(GREEN)make regenerate-offsets$(RESET) - Force regenerate offset headers"
	@echo "  $(GREEN)make clean$(RESET)              - Clean build artifacts"
	@echo "  $(GREEN)make distclean$(RESET)          - Clean everything including generated headers"
	@echo "  $(GREEN)make help$(RESET)               - Show this help message"
	@echo ""
	@echo "$(CYAN)Requirements:$(RESET)"
	@echo "  - Python 3 with PyYAML (pip install pyyaml)"
	@echo "  - GCC/G++ for btver2 (PS4 CPU)"
	@echo "  - GNU Make and Binutils"
	@echo ""
	@echo "$(CYAN)Output Files:$(RESET)"
	@echo "  include/offsets/*.h                  - Generated offset headers"
	@echo "  samples/hello_world/hello_world.elf  - Kernel module"
	@echo "  samples/userland_payload/payload.bin - Userland payload"

.PHONY: all clean distclean lib loader samples install sample_kernel sample_payload help offsets regenerate-offsets check-python