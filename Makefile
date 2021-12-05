compile:
	gcc -pthread sudoku_verifier.c -o sudoku.out
run: compile
	./sudoku.out
timer: compile
	time ./sudoku.out
test: compile 
	gcc ./sudoku_problems/sudoku_generator.c -o ./sudoku_problems/sudoku_generator.out
	./sudoku_problems/sudoku_generator.out
	make run