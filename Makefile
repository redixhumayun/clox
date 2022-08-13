all:	test	main

main:	chunk.o	compiler.o	debug.o	memory.o	object.o	scanner.o	table.o	value.o	vm.o
	/usr/bin/clang -fdiagnostics-color=always -Weverything -g chunk.o debug.o memory.o object.o scanner.o table.o value.o vm.o -o ./a.out

chunk.o:	memory.o
	/usr/bin/clang -fdiagnostics-color=always -Weverything -g memory.o -o ./memory.o

compiler.o:	scanner.o	chunk.o	debug.o	object.o
	/usr/bin/clang -fdiagnostics-color=always -Weverything -g scanner.o chunk.o debug.o object.o ./compiler.o -o ./compiler.o

debug.o:	
	/usr/bin/clang -fdiagnostics-color-always -Weverything -g -o ./debug.o