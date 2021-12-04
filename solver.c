#include<fcntl.h> 
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
// #include "sudoku_structs.h"
#include<time.h>

clock_t start,end;
// ascii eof:0x05
// tab 09
unsigned int TAB = '\t';
unsigned int LINEFEED = '\n';

typedef struct sudoku{
    int*sudoku_mem;
    int row;
    int rowsize;
    int isvalid;
}sudoku;

typedef struct thread_args{
    sudoku* sudoku;
    int index;
}thread_args;

void *check_row(void *s){

    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }

    // int row = args->index;
    // int rowsize = *(args->sudoku->sudoku_mem) * *(args->sudoku->sudoku_mem);
    int* rowstart = 1+args->index*args->sudoku->rowsize+args->sudoku->sudoku_mem;
    int flag[args->sudoku->rowsize];
    
    for(int i =0;i<args->sudoku->rowsize;i++){
        flag[i]= 1;
    }

    for(int i = 0;i<args->sudoku->rowsize;i++){
		int flagindex = *(rowstart + i) - 1;
		if(flagindex<args->sudoku->rowsize && flagindex >= 0){
        	flag[flagindex]--;
			if(flag[flagindex] < 0){
                args->sudoku->isvalid = 0;
                // printf("row %d is NOT valid\n",args->index);

                pthread_exit(0);
			}
		}else{

			args->sudoku->isvalid = 0;
            // printf("row %d is NOT valid\n",args->index);

            pthread_exit(0);
		}
	}
    // printf("row %d is valid\n",args->index);
    pthread_exit(0);
}
void *check_col(void *s){
    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }
    // int col = args->index;
    // int colsize = (args->sudoku->rowsize);
    int* colstart = 1+args->index+args->sudoku->sudoku_mem;
    // printf("colstart: %d\n",*colstart);
    int flag[args->sudoku->rowsize];
    for(int i =0;i<args->sudoku->rowsize;i++){
        flag[i]= 1;
    }
    for(int i =0; i<args->sudoku->rowsize;i++){
        int flagindex = *(colstart + i * args->sudoku->rowsize) - 1;
        if(flagindex<args->sudoku->rowsize && flagindex >= 0){
			flag[flagindex]--;
            // printf("=>[%d].%d -->%d-->\n",flagindex,i,*(colstart + i));

			if(flag[flagindex] < 0){
                // printf("[%d].%d -->%d-->\n",flagindex,i,*(colstart + i));
                // printf("col %d is NOT valid\n",args->index);
                args->sudoku->isvalid = 0;
                pthread_exit(0);
			}
		}else{
            // printf("-[%d].%d -->%d-->\n",flagindex,i,*(colstart ));
            // printf("col %d is NOT valid\n",args->index);
			args->sudoku->isvalid = 0;
            pthread_exit(0);
		}
    }
    // printf("col %d is valid\n",args->index);
    pthread_exit(0);
}
void *check_sqr(void *s){
    // printf("check sqr joined");
    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }
    // int sqr = args->index;
    // int sqrsize = (args->sudoku->rowsize);
    // printf("sqrsize %d--\n",sqrsize);
    // printf("::%d/%d::\n",args->index , args->sudoku->row);
    int* sqrstart = args->sudoku->sudoku_mem + 1+ args->sudoku->rowsize*(args->sudoku->row -1)*((args->index/args->sudoku->row)) + args->index*args->sudoku->row;
    // printf("sqrstartindex:%d:\n", 1+ sqrsize*(args->sudoku->row -1)*((args->index/args->sudoku->row)) + args->index*args->sudoku->row);
    // printf("sqrstart: %d\n",*sqrstart);
    int flag[args->sudoku->rowsize];

    for(int i =0;i<args->sudoku->rowsize;i++){
        flag[i]= 1;
    }

    for(int i =0; i<args->sudoku->row;i++){
        for(int j =0; j<args->sudoku->row;j++){
            int flagindex = *(sqrstart + i * args->sudoku->rowsize + j) - 1;
            // printf("i%dj%dflagindex%d\n",i,j,flagindex);
            if(flagindex<args->sudoku->rowsize && flagindex >= 0){
                flag[flagindex]--;
                // printf("=>[%d].%d -->%d-->\n",flagindex,i,*(sqrstart + i));

                if(flag[flagindex] < 0){
                    // printf("[%d].%d -->%d-->\n",flagindex,i,*(sqrstart + i));

                    args->sudoku->isvalid = 0;
                    // printf("sqr %d is NOT valid\n",args->index);
                    pthread_exit(0);
                }
            }else{
                // printf("-[%d].%d -->%d-->\n",flagindex,i,*(sqrstart ));

                args->sudoku->isvalid = 0;
                // printf("th sqr %d is NOT valid\n",args->index);
                pthread_exit(0);
		    }
        }
    }
    // printf("th sqr %d is valid\n",args->index);
    pthread_exit(0);
    
}

sudoku* sudoku_read(const char* file_path){
    int fd = open(file_path,O_RDONLY);
    char size[30];
    int size_index = 0;
    int sudokusize = 0;
    char one_by_one;

    while(read(fd, &one_by_one, 1)>0){
        size[size_index++] = one_by_one;
        if(one_by_one == LINEFEED || one_by_one == TAB || one_by_one == 38){
            sudokusize = atoi(size);
            size[0] = '\n';
            size_index = 0;
            break;
        };
    }
    int tablesize = sudokusize*sudokusize*sudokusize*sudokusize;
    int arr_size = tablesize+1;
    int* mem_sudoku = malloc(sizeof(int)*arr_size);
    int mem_index = 0;
    *(mem_sudoku + mem_index++) = sudokusize;
    int i = 0;
    while(read(fd, &one_by_one, 1)>0){
        size[size_index++] = one_by_one;
        
        if(one_by_one == LINEFEED || one_by_one == TAB || one_by_one == 38){
            *(mem_sudoku + mem_index++) = atoi(size);
            size[0] = '\n';
            size_index = 0;
        }
    }
    *(mem_sudoku + mem_index++) = atoi(size);
    sudoku* newsudoku = (sudoku*) malloc(sizeof(sudoku));
    newsudoku->sudoku_mem = mem_sudoku;
    newsudoku->row = *(mem_sudoku);
    newsudoku->rowsize= *(mem_sudoku) * *(mem_sudoku);
    newsudoku->isvalid = 1;
    return newsudoku;
}

int main(){
    start = clock();
    const int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    const char file_path[100] = "./sudoku_problems/sudoku1.txt";
    sudoku* s1 = sudoku_read(file_path);
    thread_args args[s1->rowsize];
    pthread_t sudoku_validation_threads[s1->rowsize*s1->row];
    pthread_t* thread_row = sudoku_validation_threads;
    pthread_t* thread_col = sudoku_validation_threads+s1->rowsize;
    pthread_t* thread_sqr = sudoku_validation_threads+2*s1->rowsize;
    // printf("%d",s1->rowsize);
    for(int i = 0 ; i<s1->rowsize;i++){
        args[i].index = i;
        args[i].sudoku = s1;
        int d =4;
        pthread_create( thread_row + i, NULL, check_row, (void*) &args[i]);
        pthread_create( thread_col + i, NULL, check_col, (void*) &args[i]);
        pthread_create( thread_sqr + i, NULL, check_sqr, (void*) &args[i]);
    }
    int threadnum = s1->rowsize*s1->row;
    for(int i = 0; i<threadnum;i+=numCPU){
        for (int j = i; j < numCPU ; j++)
        {
            if(j>threadnum){
                break;
            }
            pthread_join( sudoku_validation_threads[j], NULL);
        }
    }
    // for (int i = 0; i < threadnum; i++)
    // {
    //     pthread_join( sudoku_validation_threads[i], NULL);
    // }
    
    printf("is valid: %d\n",s1->isvalid);
    end = clock();
    double cpu_time_used = ((double) (end - start))/CLOCKS_PER_SEC;
    printf("%f",cpu_time_used);
    return 0;
}