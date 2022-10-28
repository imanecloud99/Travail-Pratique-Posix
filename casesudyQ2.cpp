#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define MAX_THREADS 256
#define ROWS 100 /*10000*/
#define COLS 100 /*10000*/

/* Arguments exchanged with threads */

struct argument {
int startRow ;
int nRows ;
long partialSum;
} threadArgs[MAX_THREADS];

/* Matrix pointer: it will be dynamically allocated */
long *bigMatrix;

int main (int argc, char *args[]) {
/* Array of thread identifiers */
pthread_t threads[MAX_THREADS];
long totalSum ;
int i,j,nThreads,rowsPerThread,lastThreadRows;
int k;

/* Get the number of threads from command parameters */
if (argc != 2) {
  printf("Usage: %s <numProcesses>\n",args[0]);
  exit (0);
}//fin if
sscanf(args[1],"%d",&nThreads);

/* Allocate the matrix M */
bigMatrix = malloc(ROWS*COLS*sizeof(long));
/* Fill the matrix with some values */
for (i=0;i<ROWS;i++)
  for (j=0;j<COLS;j++)
     bigMatrix[i*COLS+j]=i+COLS*j;

/* If the number of rows cannot be divided exactly by the number of threads,let the last thread handle also the remaining rows */
rowsPerThread=ROWS/nThreads;
if (ROWS%nThreads==0)
  lastThreadRows=rowsPerThread;
else
  lastThreadRows=rowsPerThread+ROWS%nThreads;
/* Prepare arguments for threads */
for (i=0;i<nThreads;i++) {
/* Prepare Thread arguments */
  threadArgs[i].startRow=i*rowsPerThread;
  if (i==nThreads-1)
    threadArgs[i].nRows=lastThreadRows;
  else
    threadArgs[i].nRows=rowsPerThread;
}//fin for

printf("Please hit enter to continue ...");
getchar();

/* Start the processes using default thread attributes */
struct pipes {
  int pipefd[2]
} pipeList[nThreads];

int currentRow = 0;
pid_t pids[nThreads];

for(k = 0;k<nThreads;k++){
	pipe(pipeList[k].pipefd);
	pids[k]=fork();
	if(pids[k] > 0){
		close(pipeList[k].pipefd[1]);
	}else if(pids[k]<0){
		printf("failed to create child process!");
	}else{
		/* CALCULER SOMME PARTIELLE A PARTIR DE startRow */
		long buf = 0;
		close(pipeList[k].pipefd[0]);
		struct argument currArg = threadArgs[k];
		for (i=0;i<currArg.nRows;i++)
		  for (j=0;j<COLS;j++)
		    buf += bigMatrix[(currArg.startRow+i)*COLS+j];
		if(write(pipeList[k].pipefd[1],&buf,9)<0)
		  printf("Process k couldn't write\n");
		close(pipeList[k].pipefd[1]);
		printf("Process %d just terminated with its partial sum %ld\n",k, buf);
		return 0;
	}
}

/* Read piped values and close pipes */
totalSum=0;
long buf = 0;
int status;
for (i=0;i<nThreads;i++){
  waitpid(pids[i],&status,0);
}
for (i=0;i<nThreads;i++) {
  if(read(pipeList[i].pipefd[0],&buf,9)<0)
    printf("Couldn't read from process %d\n",i);
  totalSum += buf;
}//fin for

close(pipeList[k].pipefd[0]);

/* Display the total sum calculated */
printf("The total sum calculated by all the processes is %ld\n",totalSum);
pthread_exit(NULL);
}//fin main
