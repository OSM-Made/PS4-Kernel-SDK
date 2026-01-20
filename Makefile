
# Color definitions
RED     := \033[0;31m
GREEN   := \033[0;32m
YELLOW  := \033[0;33m
BLUE    := \033[0;34m
CYAN    := \033[0;36m
RESET   := \033[0m

.PHONY: all clean loader lib samples install

all: lib loader samples
	@echo ""
	@echo "$(GREEN)════════════════════════════════════════$(RESET)"
	@echo "$(GREEN)     Build completed successfully!$(RESET)"
	@echo "$(GREEN)════════════════════════════════════════$(RESET)"

# Build CRT files
lib:
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
	@echo "$(CYAN)Targets:$(RESET)"
	@echo "  $(GREEN)make all$(RESET)            - Build everything (libs + samples)"
	@echo "  $(GREEN)make lib$(RESET)            - Build CRT files only"
	@echo "  $(GREEN)make loader$(RESET)         - Build loader libraries only"
	@echo "  $(GREEN)make samples$(RESET)        - Build all samples"
	@echo "  $(GREEN)make sample_kernel$(RESET)  - Build hello_world kernel module only"
	@echo "  $(GREEN)make sample_payload$(RESET) - Build userland payload only"
	@echo "  $(GREEN)make install$(RESET)        - Install loader to SDK"
	@echo "  $(GREEN)make clean$(RESET)          - Clean all build artifacts"
	@echo ""
	@echo "$(CYAN)Output:$(RESET)"
	@echo "  samples/hello_world/hello_world.elf  - Kernel module"
	@echo "  samples/userland_payload/payload.bin - Userland payload"

.PHONY: all clean lib loader samples install sample_kernel sample_payload help