
BIN := brain

all: gz_brain rg_brain

build/Makefile: Makefile CMakeLists.txt
	mkdir -p build
	(cd build && cmake ..)

gz_brain: $(BIN)
	mv $(BIN) gz_brain

rg_brain: gz_brain
	cp gz_brain rg_brain

$(BIN): $(wildcard *.cc) build/Makefile
	(cd build && make)
	cp build/$(BIN) .

prereqs:
	sudo apt install libgtk-3-dev libcairo2-dev

clean:
	rm -rf build $(BIN) gz_brain rg_brain
