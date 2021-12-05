#include<time.h>
#include<unistd.h>
#include<stdio.h>
#include<unistd.h>
int main(){
	printf("test");
	char *args[2]={"./sudoku.out",NULL};
    int start = clock();
    execvp(args[0],args);
    int end = clock();
    double cpu_time_used = ((double) (end - start))/CLOCKS_PER_SEC;
    printf("TIME USED:%f\n",cpu_time_used);
	fflush(stdout);
}
