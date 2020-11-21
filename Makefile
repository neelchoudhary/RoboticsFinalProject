
BIN := brain

all: brain

build/Makefile: Makefile CMakeLists.txt
	mkdir -p build
	(cd build && cmake ..)


$(BIN): $(wildcard *.cc) build/Makefile
	(cd build && make)
	cp build/$(BIN) .

prereqs:
	sudo apt install libgtk-3-dev libcairo2-dev

clean:
	rm -rf build $(BIN)

.PHONY: all clean prereqs
