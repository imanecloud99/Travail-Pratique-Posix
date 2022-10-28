#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
int val_retour;
//La fonction du thread
void* lire_entier(void *arg) {
  int un_entier;
  int val_arg = (int) arg;
  printf("Bienvenue chez le thread ayant comme argument %d\n", val_arg);
  printf("Priere de saisir un entier:");
  scanf("%d", &un_entier);
  pthread_exit((void *) un_entier); 
}//fin lire_entier

void* convertir(void * argm) {
  int time2=0;
  int time3=0;
  int val = (int) argm;
  if(val==0){
  printf("Le nombre que vous avez saisi en hexadecimal: %x\n",val_retour);
  time2=clock();
  printf("le temps pour l'afficher en hexa=%d ms\n",time2);
  }
  if(val==1){
  printf("Le nombre que vous avez saisi en octal: %o\n", val_retour);
  time3=clock();
  printf("le temps pour l'afficher en octal=%d ms\n",time3);
  }
}

int main(void) {
  int currentime=0;
  int time1=0;
  int i;
  pthread_t thread1;
  pthread_t tid0;
  pthread_t tid1;
  pthread_t * pthreads[] = {&tid0,&tid1};
  srand(time(NULL));
  i = 1 + rand() % 100;
  int succes = pthread_create(&thread1, NULL, lire_entier, (void *) i);
  if (succes != 0) {
    fprintf(stderr, "Erreur de creation du thread ...");
    exit(0);
  }//fin if (succes != 0)
  //Attente du thread de lecture
  pthread_join(thread1, (void *)&val_retour);
  printf("Le nombre que vous avez saisi en decimal est %d\n", val_retour);
  time1=clock();
  printf("Le temps de lecture = %d ms\n",time1);
  for (int j = 0; j < 2; j++){

      pthread_create(pthreads[j],NULL,convertir,(void*)j);
  } 
  pthread_join(tid0, NULL);
  pthread_join(tid1, NULL);
  currentime=clock();
  printf("le temps d'execution de tous le programme =%d ms",currentime);
  return 0;}
