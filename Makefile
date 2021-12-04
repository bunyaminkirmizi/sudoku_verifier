compile:
	gcc -pthread solver.c -o sudoku.out
run: compile
	./sudoku.out