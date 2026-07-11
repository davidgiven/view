LLVM = /opt/pkg/llvm-mos/bin

all: bin/view view.com view-b3.0.asm view-rebuild.rom

view.com: view-cpm.S
	$(LLVM)/mos-cpm65-clang -Iinclude -g -o $@ $<

view-b3.0.asm: view.py view-b3.0.rom
	PYTHONPATH=./py8dis/py8dis python3 view.py --output view-b3.0.asm

view-rebuild.rom: view-b3.0.asm
	beebasm -i $< -o $@

bin/view: src/view.c src/cli.h src/cli_stdio.c src/screen.h src/screen_ncurses.c
	mkdir -p bin
	$(CC) -g -O2 -Wall -Wextra -o $@ src/view.c src/cli_stdio.c src/screen_ncurses.c -lncurses

bin/render_number: tests/render_number.c
	mkdir -p bin
	$(CC) -g -O2 -Wall -Wextra -o $@ $<

bin/test_ruler: tests/test_ruler.c
	mkdir -p bin
	$(CC) -g -O2 -Wall -Wextra -o $@ $<

.PHONY: test test-render

test: bin/render_number bin/test_ruler bin/view
	./bin/render_number && ./bin/test_ruler && TERM=vt100 python3 tests/interact.py
