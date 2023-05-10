CC=gcc
CK = -Werror -Wall
MAKE = make --no-print-directory

.PHONY: ALL
ALL: pid_node.o display.o centre.o main
.PHONY: r-all
r-all: clean ALL

.PHONY: clean
clean: clean_out
	rm -rf *.o
	rm -rf main

.PHONY: clean_out
clean_out:
	rm -rf compositeTable.bin
	rm -rf compositeTable.txt

pid_node.o: pid_node.c pid_node.h
	$(CC) $(CK) -c $< -o $@

display.o: display.c pid_node.o display.h
	$(CC) $(CK) -c $< -o $@
centre.o: centre.c display.o centre.h
	$(CC) $(CK) -c $< -o $@

main: a2.c centre.o display.o pid_node.o
	$(CC) $(CK) -o $@ $^
