LLVM_DIR := /Users/bialy/usr/bin
WASI_LIBC := /Users/bialy/usr/share/wasi-sysroot
CC := $(LLVM_DIR)/clang
CC_SRC := $(wildcard src/*.c)
TS_SRC := $(wildcard src/*.ts)
CC_TARGET := dist/backend.wasm
CC_ARGS := --target=wasm32-wasi \
		--sysroot $(WASI_LIBC) \
		-nostartfiles \
		-O3 \
		-flto \
		-fvisibility=hidden \
		-Iinclude \
		-Wl,--no-entry,--lto-O3,--export=malloc,--export=free,--export-dynamic,-allow-undefined-file=src/symbols


export PATH := $(LLVM_DIR):$(PATH)


all: $(TS_SRC) $(CC_SRC)
	mkdir -p dist 	
	tsc
	make wasm 
	browserify -o dist/script.js -e tmp/main.js 
	uglifyjs dist/script.js -o dist/script.min.js


wasm: $(CC_SRC)
	export PATH
	$(CC) $(CC_ARGS) -o $(CC_TARGET) $^


test: $(CC_SRC) test/main.c 
	gcc -O3 -Iinclude -o test/test $^


benchmark: $(CC_SRC) benchmark/benchmark.c 
	gcc -O3 -Iinclude -o benchmark/benchmark $^ 
	