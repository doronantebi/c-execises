CC = gcc
OBJS = main.o MainAux.o Game.o Parser.o solver.o
EXEC = sudoku
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
main.o: main.c MainAux.c MainAux.h Game.c Game.h Parser.c Parser.h solver.c solver.h
	$(CC) $(COMP_FLAG) -c $*.c
MainAux.o: MainAux.c MainAux.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h solver.c solver.h Parser.c Parser.h MainAux.c MainAux.h
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h Game.c Game.h
	$(CC) $(COMP_FLAG) -c $*.c
solver.o: solver.c solver.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
