CC       := gcc
CSTD     := -std=c17
CFLAGS   := $(CSTD) -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
            -Wstrict-prototypes -Wmissing-prototypes \
            -I.
DBGFLAGS := -g3 -O0 -DDEBUG -fsanitize=address,undefined
RELFLAGS := -O2 -DNDEBUG

BUILDDIR := build
TARGET   := $(BUILDDIR)/app

# Recursively find all .c files from project root (excluding build/)
SRCS     := $(shell find . -name '*.c' -not -path './$(BUILDDIR)/*')

# Mirror the project tree inside build/
OBJS     := $(patsubst ./%.c, $(BUILDDIR)/%.o, $(SRCS))

# Default: debug build
all: debug

debug: CFLAGS += $(DBGFLAGS)
debug: $(TARGET)

release: CFLAGS += $(RELFLAGS)
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c, creating build subdirs as needed
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)

# Compile and run the full project
run: debug
	./$(TARGET)

# Compile and run a single file: make run-file main.c
run-file:
	$(eval FILE := $(word 2, $(MAKECMDGOALS)))
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make run-file <file.c>"; \
		exit 1; \
	fi
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(DBGFLAGS) -o $(BUILDDIR)/run_tmp $(FILE)
	./$(BUILDDIR)/run_tmp

# Absorb filename so make doesn't treat it as a target
%:: ;
	@:

.PHONY: all debug release clean run run-file