# SDL FLAGS
SDLCFLAGS = `sdl2-config --cflags`
SDLLIBS = `sdl2-config --libs`
# NORMAL COMPILE
STANDARD = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -g3
# MEMORY COMPILE
MEMORY = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -fsanitize=undefined -fsanitize=address -g3
# OPTIMISE COMPILE
OPTIMIZE = -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -O3

GENERAL = Neill_files/general.h Neill_files/general.c
NEILLSIMPLESCREEN = Neill_files/neillsimplescreen.c Neill_files/neillsimplescreen.h
NEILLSDL = Neill_files/neillsdl2.c Neill_files/neillsdl2.h
STACK = Stack/stack.c Stack/stack.h Stack/specific.h

LDLIBS =  -lm
CC = gcc
#CC = clang

parse: Parse/parse.h Parse/parse.c $(GENERAL) $(NEILLSIMPLESCREEN)
	$(CC) Parse/parse.c Neill_files/general.c Neill_files/neillsimplescreen.c -o parse_s $(STANDARD) $(LDLIBS)

parse_m: Parse/parse.h Parse/parse.c $(GENERAL) $(NEILLSIMPLESCREEN)
	$(CC) Parse/parse.c Neill_files/general.c Neill_files/neillsimplescreen.c -o parse_m $(MEMORY) $(LDLIBS)

parse_o: Parse/parse.h Parse/parse.c $(GENERAL) $(NEILLSIMPLESCREEN)
	$(CC) Parse/parse.c Neill_files/general.c Neill_files/neillsimplescreen.c -o parse_o $(OPTIMIZE) $(LDLIBS)


interp: Interp/interp.h Interp/interp.c $(GENERAL) $(NEILLSIMPLESCREEN) $(NEILLSDL) $(STACK)
	$(CC) Interp/interp.c Neill_files/general.c Neill_files/neillsimplescreen.c Neill_files/neillsdl2.c Stack/stack.c -o interp_s $(STANDARD) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)

interp_m: Interp/interp.h Interp/interp.c $(GENERAL) $(NEILLSIMPLESCREEN) $(NEILLSDL) $(STACK)
	$(CC) Interp/interp.c Neill_files/general.c Neill_files/neillsimplescreen.c Neill_files/neillsdl2.c Stack/stack.c -o interp_m $(MEMORY) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)

interp_o: Interp/interp.h Interp/interp.c $(GENERAL) $(NEILLSIMPLESCREEN) $(NEILLSDL) $(STACK)
	$(CC) Interp/interp.c Neill_files/general.c Neill_files/neillsimplescreen.c Neill_files/neillsdl2.c Stack/stack.c -o interp_o $(OPTIMIZE) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)


extension: Extension/extension.h Extension/extension.c $(GENERAL) $(NEILLSDL)
	$(CC) Extension/extension.c Neill_files/general.c Neill_files/neillsdl2.c -o extension_s $(STANDARD) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)

extension_m: Extension/extension.h Extension/extension.c $(GENERAL) $(NEILLSDL)
	$(CC) Extension/extension.c Neill_files/general.c Neill_files/neillsdl2.c -o extension_m $(MEMORY) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)

extension_o: Extension/extension.h Extension/extension.c $(GENERAL) $(NEILLSDL)
	$(CC) Extension/extension.c Neill_files/general.c Neill_files/neillsdl2.c -o extension_o $(OPTIMIZE) $(LDLIBS) $(SDLCFLAGS) $(SDLLIBS)

all:
	make parse
	make parse_m
	make parse_o
	make interp
	make interp_m
	make interp_o
	make extension
	make extension_m
	make extension_o

# Runs all with valid .ttl file
run:
	./parse_s test_files/valid_1.ttl
	./parse_m test_files/valid_1.ttl
	./parse_o test_files/valid_1.ttl
	valgrind ./parse_s test_files/valid_1.ttl

	# SDL files not executed with memory checks
	# as memory leaks are to be expected
	./interp_s test_files/valid_1.ttl
	./interp_o test_files/valid_1.ttl

# Run extension
run_e:
	./extension_s

clean:
	rm parse_s parse_m parse_o interp_s interp_m interp_o extension_s extension_m extension_o
