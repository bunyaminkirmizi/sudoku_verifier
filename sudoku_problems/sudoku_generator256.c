#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <string.h>

#define SUDOKU_SIZE 256
#define SIZE_ROW 16
FILE *fp;
int sudoku[SUDOKU_SIZE][SUDOKU_SIZE];

void sudoku_row_changer(int row_x,int row_y){
	int temp;
	for(int i = 0; i<SUDOKU_SIZE;i++){
		temp = sudoku[row_x][i];
		sudoku[row_x][i] = sudoku[row_y][i];
		sudoku[row_y][i] = temp;
	}
	return;
}

void sudoku_col_changer(int col_x,int col_y){
	int temp;
	for(int i = 0; i<SUDOKU_SIZE;i++){
		temp = sudoku[i][col_x];
		sudoku[i][col_x] = sudoku[i][col_y];
		sudoku[i][col_y] = temp;
	}
	return;
}

void swapNumbers(int n1, int n2) {
    for (int y = 0; y<9; y++) {
        for (int x = 0; x<9; x++) {
            if (sudoku[x][y] == n1) {
                sudoku[x][y] = n2;
            } else if (sudoku[x][y] == n2) {
                sudoku[x][y] = n1;
            }
        }
    }
}
void printf_sudoku_to_file(int sudoku_number){
		char filename[1000];
	
    char *s_sudoku_num;
    int length = snprintf( NULL, 0, "%d", sudoku_number );
    s_sudoku_num = malloc( length + 1 );
    snprintf( s_sudoku_num, length + 1, "%d", sudoku_number);

	strcpy(filename, "./sudoku_problems/sudoku");
	strcat(filename, s_sudoku_num);
	strcat(filename,".txt");

	fp = fopen (filename, "w");

	fprintf(fp,"%d\n",SIZE_ROW);
	for(int i = 0;i<SUDOKU_SIZE;i++){
		for(int j = 0;j<SUDOKU_SIZE;j++){
			fprintf(fp,"%d",sudoku[i][j]);
			if(j != SUDOKU_SIZE - 1){
				fprintf(fp,"\t");
			}
		}
		if(i != SUDOKU_SIZE - 1){
				fprintf(fp,"\n");
			}
		
	}
	pclose(fp);
}
void shift_right(int x){
	sudoku_col_changer(x,x+1);
}
void generate_table(){
	for(int i = 0;i<SIZE_ROW;i++){
		for(int j = 0;i<SIZE_ROW;i++){
			if(i == 0){
				sudoku[i][j] = j + 1;
			}
		}
	}
}
int main(){
	srand(time(NULL));

	int col_change_parameters[3][2] = {
		{0,1},
		{1,2},
		{2,0}
		};
	// printf_sudoku();
	for(int i = 0 ;i<1;i++){
		for(int j = 0;j<15;j++){
			
			int randparameter = rand() % 3;
			int randomsection = rand() % 3;
			int x = col_change_parameters[randparameter][0]+randomsection*3;
			int y = col_change_parameters[randparameter][1]+randomsection*3;
			sudoku_col_changer(x,y);
			randparameter = rand() % 3;
			randomsection = rand() % 3;
			x = col_change_parameters[randparameter][0]+randomsection*3;
			y = col_change_parameters[randparameter][1]+randomsection*3;
			sudoku_row_changer(x,y);

			int swap1= rand() %SUDOKU_SIZE +1;
			int swap2= rand() %SUDOKU_SIZE +1;
			while(swap1 == swap2){
				
				swap2= rand() %SUDOKU_SIZE +1;
			}
			swapNumbers(swap1,swap2);
		}
		printf_sudoku_to_file(i);
	}
	return 0;
}