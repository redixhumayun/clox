all:	test	main

test:
	cc ./test/test.c ./test/unity.c ./table.c ./object.c ./memory.c ./value.c ./vm.c ./chunk.c ./compiler.c ./scanner.c ./debug.c -o ./out

main:	
	cc -fdiagnostics-color=always -g ./$(wildcard *.c) -o ./project.a

wasm:
	emcc -fdiagnostics-color=always -g ./$(wildcard *.c) -o ./project.html

chunk.o:	memory.o
	/usr/bin/clang -fdiagnostics-color=always -Weverything -g memory.o -o ./memory.o

compiler.o:	scanner.o	chunk.o	debug.o	object.o
	/usr/bin/clang -fdiagnostics-color=always -Weverything -g scanner.o chunk.o debug.o object.o ./compiler.o -o ./compiler.o

debug.o:	
	/usr/bin/clang -fdiagnostics-color-always -Weverything -g -o ./debug.o

.PHONY:	test

clean:
	rm -f project.html
	rm -f project.js
	rm -f project.wasm
	