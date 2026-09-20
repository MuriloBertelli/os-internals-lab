

CC       := gcc
CFLAGS   := -std=c17 -Wall -Wextra -g -pthread
SANFLAGS := -fsanitize=address,undefined
LDLIBS   := -lm -lpthread

DIRS := 00-c-refresher 01-linux-env 02-processes 03-ipc 04-scheduling \
        05-threads 06-memory 07-filesystem 08-io

BIN := bin

.PHONY: all $(DIRS) san clean help

all: $(DIRS)


$(DIRS):
	@mkdir -p $(BIN)/$@
	@for src in $@/*.c; do \
		[ -e "$$src" ] || continue; \
		out=$(BIN)/$@/$$(basename $$src .c); \
		echo "  CC  $$src"; \
		$(CC) $(CFLAGS) $(EXTRA) "$$src" -o "$$out" $(LDLIBS) || exit 1; \
	done

san:
	@$(MAKE) --no-print-directory all EXTRA="$(SANFLAGS)"

clean:
	@rm -rf $(BIN)
	@echo "bin/ removido"

help:
	@echo "make            - compila tudo"
	@echo "make <pasta>    - compila so uma pasta (ex: make 03-ipc)"
	@echo "make san        - compila tudo com ASan/UBSan"
	@echo "make clean      - apaga os binarios"
