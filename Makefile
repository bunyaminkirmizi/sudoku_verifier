compile:
	gcc -pthread sudoku_verifier.c -o sudoku.out
run: compile
	./sudoku.out
timer: compile
	time ./sudoku.out