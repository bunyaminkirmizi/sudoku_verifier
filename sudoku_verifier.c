#include<fcntl.h> 
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
    unsigned int index;
}thread_args;

void *check_row(void *s){
    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }
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
                pthread_exit(0);
			}
		}else{

			args->sudoku->isvalid = 0;
            pthread_exit(0);
		}
	}
    pthread_exit(0);
}
void *check_col(void *s){
    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }
    int* colstart = 1+args->index+args->sudoku->sudoku_mem;
    int flag[args->sudoku->rowsize];
    for(int i =0;i<args->sudoku->rowsize;i++){
        flag[i]= 1;
    }
    for(int i =0; i<args->sudoku->rowsize;i++){
        int flagindex = *(colstart + i * args->sudoku->rowsize) - 1;
        if(flagindex<args->sudoku->rowsize && flagindex >= 0){
			flag[flagindex]--;
			if(flag[flagindex] < 0){
                args->sudoku->isvalid = 0;
                pthread_exit(0);
			}
		}else{
			args->sudoku->isvalid = 0;
            pthread_exit(0);
		}
    }
    pthread_exit(0);
}
void *check_sqr(void *s){
    thread_args* args;
    args = (thread_args*) s;
    if(args->sudoku->isvalid  == 0){
        pthread_exit(0);
    }
    int* sqrstart = args->sudoku->sudoku_mem + 1+ args->sudoku->rowsize*(args->sudoku->row -1)*((args->index/args->sudoku->row)) + args->index*args->sudoku->row;
    int flag[args->sudoku->rowsize];

    for(int i =0;i<args->sudoku->rowsize;i++){
        flag[i]= 1;
    }

    for(int i =0; i<args->sudoku->row;i++){
        for(int j =0; j<args->sudoku->row;j++){
            int flagindex = *(sqrstart + i * args->sudoku->rowsize + j) - 1;
            if(flagindex<args->sudoku->rowsize && flagindex >= 0){
                flag[flagindex]--;
                if(flag[flagindex] < 0){
                    args->sudoku->isvalid = 0;
                    pthread_exit(0);
                }
            }else{
                args->sudoku->isvalid = 0;
                pthread_exit(0);
		    }
        }
    }
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
        fflush(stdout);
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

    for(int fn = 0;fn<5000;fn++){
	char file_path[100] = "./sudoku_problems/sudoku";
    int sudokunum = fn;

    char *s_sudoku_num;
    int length = snprintf( NULL, 0, "%d", sudokunum );
    s_sudoku_num = malloc( length + 1 );
    snprintf( s_sudoku_num, length + 1, "%d", sudokunum);

    strcat(file_path,s_sudoku_num);
    strcat(file_path,".txt");
	    if( access( file_path, F_OK ) != 0 ) {
    printf("%s not found\n",file_path);
    continue;
}
    sudoku* s1 = sudoku_read(file_path);
    thread_args args[s1->rowsize];
    int threadnum = s1->rowsize*3;
    pthread_t sudoku_validation_threads[threadnum];
    pthread_t* thread_row = sudoku_validation_threads;
    pthread_t* thread_col = sudoku_validation_threads;
    pthread_t* thread_sqr = sudoku_validation_threads;
    thread_col = thread_col + s1->rowsize;
    thread_sqr = thread_sqr + 2 * s1->rowsize;
    for(int i = 0 ; i<s1->rowsize;i++){
        args[i].index = i;
        args[i].sudoku = s1;
        pthread_create( thread_row + i, NULL, check_row, (void*) &args[i]);
        pthread_create( thread_col + i, NULL, check_col, (void*) &args[i]);
        pthread_create( thread_sqr + i, NULL, check_sqr, (void*) &args[i]);
    }

    
    for (int i = 0; i < threadnum; i++)
    {
        pthread_join( sudoku_validation_threads[i], NULL);
    }

    printf("%s is validation: %d\n",file_path,s1->isvalid);
    free(s1->sudoku_mem);
    free(s1);
    free(s_sudoku_num);
    }

    return 0;
}