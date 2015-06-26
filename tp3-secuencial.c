#include<stdio.h>
#include<stdlib.h>

//Dimension por defecto
double N=1048576; //<-- para testear
double block;

//Para calcular tiempo
double dwalltime() {
 double sec;
 struct timeval tv;
 gettimeofday(&tv,NULL);
 sec = tv.tv_sec + tv.tv_usec/1000000.0;
 return sec;
}

int main(int argc,char*argv[]) {
 double *A,*merge,aux;
 int i,j,k,l;
 double timetick;
 int sort=0;

 if((argc!=2)||((block=atoi(argv[1]))<=0)) {
  printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
  exit(1);
 }
  
	
 A=(double*)malloc(sizeof(double)*N);
 merge=(double*)malloc(sizeof(double));

  //TEST: Inicializa todo en 1
 for(i=0;i<N;i++) {
  A[i]=N-i;
 }   

 timetick = dwalltime();

 while(block<=N) {
  if(sort==0) {
   for(k=0;k<N;k=k+block) {
    for(i=1;i<block;i++) {
     for(j=k;j<k+block-i;j++) {
	  if(A[j]>=A[j+1]) {
	   aux=A[j];
	   A[j]=A[j+1];
	   A[j+1]=aux;
	  }
	 }
	}
   }
   sort++;
  }else{
   merge=(double*)realloc(merge,sizeof(double)*block);
   for(i=0;i<N;i=i+block) {
    j=i;
    k=i+block/2;
    l=0;
    while(j<i+block/2 && k<i+block) {
     if(A[j]<=A[k]){
      merge[l]=A[j];
      j++;
     }else{
      merge[l]=A[k];
      k++;
     }
     l++;
    }
    while(j<i+block/2) {
     merge[l]=A[j];
     j++;
     l++;
    }
    while(k<i+block) {
     merge[l]=A[k];
     k++;
     l++;
    }
    for(j=0;j<block;j++){
     A[i+j]=merge[j];
    }
   }
  }
  block=block*2;
 }
	
 printf("Tiempo en segundos %f\n\n", dwalltime()-timetick);

 free(A);

 return 0;
}
