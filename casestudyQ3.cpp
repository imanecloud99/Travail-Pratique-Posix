#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define MAX_THREADS 256
#define ROWS 20 /*10000*/
#define COLS 20 /*10000*/

/* Arguments exchanged with threads */

struct argument {
int startRow ;
int nRows ;
pthread_mutex_t* mutex;
//long partialSum; //On n'aura plus besoin de la somme partielle
} threadArgs[MAX_THREADS];

long totalSum = 0; //Variable globale pour la somme

/* Matrix pointer: it will be dynamically allocated */
long *bigMatrix;
/* Thread routine : make the summation of all the elements of the assigned matrix rows */
static void *thread_routine(void *arg) {
	int i,j;
	/* Typecast passed pointer to expected structure containing the start row,the number of rows to be summed and the return sum argument */
	struct argument *currArg = (struct argument *) arg;
	long sum=0;
	for (i=0;i<currArg->nRows;i++){
	  for (j=0;j<COLS;j++){
	  	pthread_mutex_lock(currArg->mutex);
	  	totalSum += bigMatrix[(currArg->startRow+i)*COLS+j];
	  	pthread_mutex_unlock(currArg->mutex);
	  }
	}
	return NULL ;
}//fin thread_routine()

int main (int argc, char *args[]) {
	/* Time measurment */
	clock_t start,end;
	start = clock();
	/* Array of thread identifiers */
	pthread_t threads[MAX_THREADS];
	int i,j,nThreads,rowsPerThread,lastThreadRows;
	/* Get the number of threads from command parameters */
	if (argc != 2) {
	  printf("Usage: %s <numThreads>\n",args[0]);
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
	  
	/* Declaration et initialisation du mutex */
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	/* Prepare arguments for threads */
	for (i=0;i<nThreads;i++) {
	/* Prepare Thread arguments */
	  threadArgs[i].mutex = &mutex;
	  threadArgs[i].startRow=i*rowsPerThread;
	  if (i==nThreads-1)
	    threadArgs[i].nRows=lastThreadRows;
	  else
	    threadArgs[i].nRows=rowsPerThread;
	}//fin for
	
	
	/* Start the threads using default thread attributes */
	for (i=0;i<nThreads;i++)
	  pthread_create(&threads[i],NULL,thread_routine,&threadArgs[i]);
	/* Display the values of the matrix */
	  /*printf("Your matrix is as follows:\n");
	  for (i=0;i<ROWS;i++) {
	  printf("[");
	  for (j=0;j<COLS;j++) 
	    printf("%ld:",bigMatrix[i*COLS+j]);
	  printf("]\n");
	}//fin for*/
	printf("Please hit enter to continue ...");
	getchar();
	/* Wait thread termination and use the corresponding sum value for the final summation */
	for (i=0;i<nThreads;i++) {
	  pthread_join(threads[i],NULL);
	}//fin for

	/* Get clock at end of execution */

	end = clock();

	/* Display the total sum calculated */
	printf("The total sum calculated by all the threads is %ld\n",totalSum);
	printf("Time elapsed: %f", (float) (end-start)/CLOCKS_PER_SEC);
	pthread_exit(NULL);
}//fin main
