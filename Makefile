GENERATOR ?= Ninja
BUILD_TYPE ?= Debug
BUILD_DIR ?= build
GCOV ?= gcov

SOURCES = git ls-files -- "*.?pp"

all: ci

.PHONY: configure
configure: format ## Configure with default compiler
	cmake -S . -B "$(BUILD_DIR)" -G "$(GENERATOR)" \
		-D CMAKE_BUILD_TYPE="$(BUILD_TYPE)" \
		-D CMAKE_EXPORT_COMPILE_COMMANDS=ON

.PHONY: clang
clang: export CC=clang
clang: export CXX=clang++
clang: configure ## Configure with Clang compiler

.PHONY: gcc
gcc: export CC=gcc
gcc: export CXX=g++
gcc: configure ## Configure with GCC compiler

.PHONY: build
build: configure ## Build test executable
	cmake --build "$(BUILD_DIR)" --config "$(BUILD_TYPE)" --parallel

.PHONY: test
test: build ## Run test executable
	"./$(BUILD_DIR)/miroir_test"

.PHONY: clean
clean: ## Remove $(BUILD_DIR) and coverage info
	rm -f coverage.info
	rm -rf coverage
	rm -rf "$(BUILD_DIR)"

# Format

.PHONY: format
format: tags ## Format source code using `clang-format`
	$(SOURCES) | xargs clang-format -i --style=file

# Tags

.PHONY: tags
tags: ## Generate tags file using `universal-ctags`
	ctags \
		--recurse \
		--exclude=.git \
		--exclude=build \
		--c++-kinds=+p \
		--fields=+iaS \
		--extras=+q \
		--language-force=C++

# Local CI emulation

.PHONY: ci
ci: ## Run all CI stages locally (default)
	$(MAKE) lint
	$(MAKE) profile
	BUILD_TYPE=Debug $(MAKE) test
	BUILD_TYPE=Release $(MAKE) test

# Linting

.PHONY: lint
lint: ## Run all linters
	$(MAKE) lint/check
	$(MAKE) lint/tidy
	$(MAKE) lint/spell

.PHONY: lint/check
lint/check: configure ## Run `cppcheck`
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
		`$(SOURCES)`

.PHONY: lint/tidy
lint/tidy: configure ## Run `clang-tidy`
	clang-tidy \
		-p="$(BUILD_DIR)" \
		--warnings-as-errors=* \
		--header-filter=.* \
		`$(SOURCES)`

.PHONY: lint/spell
lint/spell: configure ## Check spelling using `codespell`
	codespell \
		`git ls-files -- "*.md" "*.txt" "*.json" "*.yml" "Makefile"` \
		`$(SOURCES)`

# Profilers

.PHONY: profile
profile: ## Run all profilers on test executable
	$(MAKE) profile/address
	$(MAKE) profile/undefined

.PHONY: profile/address
profile/address: BUILD_TYPE=Asan
profile/address: ASAN_OPTIONS=detect_leaks=1:detect_container_overflow=0 test ## Run address sanitizer on test executable

.PHONY: profile/undefined
profile/undefined: BUILD_TYPE=Ubsan
profile/undefined: test ## Run undefined behavior sanitizer on test executable

# Coverage

.PHONY: coverage
coverage: export BUILD_TYPE=Coverage
coverage: ## Collect code coverage using `lcov`
	lcov --zerocounters --directory .
	$(MAKE) test
	lcov \
		--directory . \
		--capture \
		--gcov-tool "$(GCOV)" \
		--output-file coverage.info
	genhtml --demangle-cpp --output-directory coverage coverage.info
	lcov --list coverage.info

# Submodules

.PHONY: submodules/fetch
submodules/fetch: ## Fetch git submodules
	git submodule update --init --recursive --progress

.PHONY: submodules/update
submodules/update: submodules/fetch ## Update git submodules to the latest versions
	git submodule update --remote --merge

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
