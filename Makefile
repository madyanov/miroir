DESTDIR =
PREFIX ?= /usr/local
BUILD_TYPE ?= Debug
BUILD_DIR := build

BIN_DIR = $(DESTDIR)$(PREFIX)/bin/
MAKEFILE = $(BUILD_DIR)/Makefile

GIT_SOURCES := git ls-files -- "*.?pp"
SOURCES := $(shell $(GIT_SOURCES))

CHECK_BUILD_TYPE = $(BUILD_DIR)/BUILD.$(shell echo $(BUILD_TYPE))
CHECK_FORMAT = $(BUILD_DIR)/FORMAT

.PHONY: test
test: $(BUILD_DIR)/miroir_test ## Run test executable (default)
	"./$(BUILD_DIR)/miroir_test"

$(CHECK_BUILD_TYPE):
	rm -f "$(BUILD_DIR)"/BUILD.*
	mkdir -p "$(BUILD_DIR)"
	touch "$@"

$(MAKEFILE): CMakeLists.txt $(CHECK_BUILD_TYPE) $(CHECK_FORMAT)
	cmake \
		-S . \
		-B "$(BUILD_DIR)" \
		-G "Unix Makefiles" \
		-D CMAKE_BUILD_TYPE="$(BUILD_TYPE)" \
		-D CMAKE_EXPORT_COMPILE_COMMANDS=ON
	touch "$@"

$(BUILD_DIR)/miroir_test: $(MAKEFILE) $(SOURCES)
	cmake \
		--build "$(BUILD_DIR)" \
		--config "$(BUILD_TYPE)" \
		--target miroir_test \
		--parallel
	touch "$@"

# Helpers

.PHONY: clean
clean: ## Remove $(BUILD_DIR)
	rm -rf "$(BUILD_DIR)"

# Compilers

.PHONY: clang
clang: export CC=clang
clang: export CXX=clang++
clang: clean $(MAKEFILE) ## Configure with Clang compiler

.PHONY: gcc
gcc: export CC=gcc
gcc: export CXX=g++
gcc: clean $(MAKEFILE) ## Configure with GCC compiler

# Format

.PHONY: format
format: $(CHECK_FORMAT) ## Format source code using `clang-format`

$(CHECK_FORMAT): $(SOURCES)
	$(GIT_SOURCES) | xargs clang-format -i --style=file
	touch "$@"

.PHONY: ci
ci: ## Run all CI stages locally
	$(MAKE) lint
	$(MAKE) profile
	$(MAKE) BUILD_TYPE=Debug test
	$(MAKE) BUILD_TYPE=Release test

# Linting

.PHONY: lint
lint: ## Run all linters
	$(MAKE) lint/check
	$(MAKE) lint/tidy
	$(MAKE) lint/spell

.PHONY: lint/check
lint/check: $(MAKEFILE) ## Run `cppcheck`
	cppcheck \
		--cppcheck-build-dir="$(BUILD_DIR)" \
		--error-exitcode=1 \
		--enable=all \
		--language=c++ \
		--std=c++20 \
		--inline-suppr \
		--suppress=unmatchedSuppression \
		--suppress=missingIncludeSystem \
		--suppress=unusedStructMember \
		--suppress=unusedFunction \
		--suppress=useStlAlgorithm \
		`$(GIT_SOURCES)`

.PHONY: lint/tidy
lint/tidy: $(MAKEFILE) ## Run `clang-tidy`
	clang-tidy \
		-p="$(BUILD_DIR)" \
		--warnings-as-errors=* \
		`$(GIT_SOURCES)`

.PHONY: lint/spell
lint/spell: ## Check spelling using `codespell`
	codespell \
		`git ls-files -- "*.md" "*.txt" "*.json" "*.yml" "Makefile"` \
		`$(GIT_SOURCES)`

# Profilers

.PHONY: profile
profile: ## Run all profilers on test executable
	$(MAKE) profile/address
	$(MAKE) profile/undefined

.PHONY: profile/address
profile/address: ## Run address sanitizer on test executable
	$(MAKE) BUILD_TYPE=Asan ASAN_OPTIONS=detect_container_overflow=0 test

.PHONY: profile/undefined
profile/undefined: ## Run undefined behavior sanitizer on test executable
	$(MAKE) BUILD_TYPE=Ubsan test

# Tree

.PHONY: tree
tree: ## Show project structure using `tree`
	tree --gitignore -I external

# Help

help: ## Show this help
	@echo
	@echo 'Targets:'
	@echo
	@grep -Eh '\s##\s' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":[^:]*?## "}; {printf "  %-20s %s\n", $$1, $$2}'
	@echo
	@echo 'Default environment:'
	@echo
	@sed -e '/^$$/,$$d' $(MAKEFILE_LIST) | sed -e 's/^/  /'
