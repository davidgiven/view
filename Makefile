LLVM = /opt/pkg/llvm-mos/bin
CC = cc
CFLAGS = -g -O0 -Wall -Wextra
LDFLAGS = -lncurses
OBJ_DIR = .obj
DEPFLAGS = -MMD -MP

all: bin/view view.com view-b3.0.asm view-rebuild.rom

view.com: view-cpm.S
	$(LLVM)/mos-cpm65-clang -Iinclude -g -o $@ $<

view-b3.0.asm: view.py view-b3.0.rom
	PYTHONPATH=./py8dis/py8dis python3 view.py --output view-b3.0.asm

view-rebuild.rom: view-b3.0.asm
	beebasm -i $< -o $@

SRC = src/view.c src/cli_stdio.c src/screen_ncurses.c
OBJ = $(addprefix $(OBJ_DIR)/, view.o cli_stdio.o screen_ncurses.o)
OBJ_TEST = $(addprefix $(OBJ_DIR)/, view.o cli_stdio.o screen_ncurses_test.o)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

$(OBJ_DIR)/screen_ncurses_test.o: src/screen_ncurses.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -DTEST_HARNESS -c -o $@ $<

$(OBJ_DIR)/view_nomain.o: src/view.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -Dmain=view_main -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

DEPFILES = $(OBJ:.o=.d) $(OBJ_TEST:.o=.d) $(OBJ_DIR)/view_nomain.d
-include $(DEPFILES)

bin/view: $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/view_for_testing: $(OBJ_TEST)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/render_number: tests/render_number.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $<

bin/test_ruler: tests/test_ruler.c $(OBJ_DIR)/view_nomain.o $(OBJ_DIR)/cli_stdio.o $(OBJ_DIR)/screen_ncurses.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/test_justify: tests/test_justify.c $(OBJ_DIR)/view_nomain.o $(OBJ_DIR)/cli_stdio.o $(OBJ_DIR)/screen_ncurses.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: test test-render clean

test: bin/render_number bin/test_ruler bin/test_justify bin/view_for_testing
	./bin/render_number && ./bin/test_ruler && ./bin/test_justify && TERM=vt100 python3 tests/interact.py

clean:
	rm -rf $(OBJ_DIR) bin/view bin/view_for_testing bin/render_number bin/test_ruler bin/test_justify
