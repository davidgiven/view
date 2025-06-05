LLVM = /opt/pkg/llvm-mos/bin

all: view.com view-b3.0.asm view-rebuild.rom

view.com: view-cpm.S
	$(LLVM)/mos-cpm65-clang -Iinclude -g -o $@ $<

view-b3.0.asm: view.py view-b3.0.rom
	PYTHONPATH=./py8dis/py8dis python3 view.py --output view-b3.0.asm

view-rebuild.rom: view-b3.0.asm
	beebasm -i $< -o $@