#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//Dimension por defecto de las matrices
	// int N=3;  <-- para testear la cambiamos por 3
int N=3;
int threads=4;
double *A,*B,*B2,*C,*D, result;
double minA=9999999;
double maxA=0;
double minB=9999999;
double maxB=0;
double totalA=0;
double totalB=0;
pthread_mutex_t seccion_critica_maxA; 
pthread_mutex_t seccion_critica_maxB;
pthread_mutex_t seccion_critica_totalA;
pthread_mutex_t seccion_critica_totalB;  
pthread_mutex_t seccion_critica_minA; 
pthread_mutex_t seccion_critica_minB;


void* multiply(void* slice) {
  int s=(int)slice;   // retrive the slice info
  int from=(s*N)/threads; // note that this 'slicing' works fine
  int to=((s+1)*N)/threads; // even if SIZE is not divisible by num_thrd
  int i,j,k;

  printf("Soy un thread!!");
 
  for (i=from; i<to; i++) {  
    for (j=0; j<N; j++) {
      B2[i*N+j]=0;
      for (k=0; k<N; k++) {
        B2[i*N+j]=(B2[i*N+j] + A[i*N+k]*B[k+j*N]);
      }
    }
  }
  pthread_exit(0);
}


void* multiply_diag(void* slice) {
  int s=(int)slice;   // retrive the slice info
  int from=(s*N)/threads; // note that this 'slicing' works fine
  int to=((s+1)*N)/threads; // even if SIZE is not divisible by num_thrd
  int i,j;

  printf("Soy un thread!!");
 
  for(i=from;i<to;i++){
   for(j=0;j<N;j++){
    C[i*N+j]=(B2[i*N+j]*D[j]);
   }
  }
  pthread_exit(0);
}


void* min_max_total(void* slice) {
  int s=(int)slice;   // retrive the slice info
  int from=(s*(N*N))/threads; // note that this 'slicing' works fine
  int to=((s+1)*(N*N))/threads; // even if SIZE is not divisible by num_thrd
  int i;
  double local_min_A=999999;
  double local_max_A=0;
  double local_total_A=0;
  double local_min_B=999999;
  double local_max_B=0;
  double local_total_B=0;

  printf("Soy un thread!!");
 
  for (i=from; i<to; i++){  
    if(A[i]<local_min_A){
      local_min_A=A[i];
    }
    if(A[i]>local_max_A){
      local_max_A=A[i];
    }
  }
  pthread_mutex_lock(&seccion_critica_minA); 
  if(local_min_A<minA) {
    minA=local_min_A;
  }
  pthread_mutex_unlock(&seccion_critica_minA);
  pthread_mutex_lock(&seccion_critica_maxA); 
  if(local_max_A>maxA) {
    maxA=local_max_A;
  }
  pthread_mutex_unlock(&seccion_critica_maxA);
  pthread_mutex_lock(&seccion_critica_totalA); 
  totalA+=local_total_A;
  pthread_mutex_unlock(&seccion_critica_totalA);

  for (i=from; i<to; i++){  
    if(B[i]<local_min_B){
      local_min_B=B[i];
    }
    if(B[i]>local_max_B){
      local_max_B=B[i];
    }
  }
  pthread_mutex_lock(&seccion_critica_minB); 
  if(local_min_B<minB) {
    minB=local_min_B;
  }
  pthread_mutex_unlock(&seccion_critica_minB);
  pthread_mutex_lock(&seccion_critica_maxB); 
  if(local_max_B>maxB) {
    maxB=local_max_B;
  }
  pthread_mutex_unlock(&seccion_critica_maxB);
  pthread_mutex_lock(&seccion_critica_totalB); 
  totalB+=local_total_B;
  pthread_mutex_unlock(&seccion_critica_totalB);

  pthread_exit(0);
}


void* multiply_ident(void* slice) {
  int s=(int)slice;   // retrive the slice info
  int from=(s*N)/threads; // note that this 'slicing' works fine
  int to=((s+1)*N)/threads; // even if SIZE is not divisible by num_thrd
  int i,j;

  printf("Soy un thread!!");
 
  for(i=from;i<to;i++){
   C[i]=C[i]*result;
  }
  pthread_exit(0);
}


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]) {
 double *aux;
 int i,j,k;
 double timetick;
 int check=1;

 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

      //Aloca memoria para las matrices
	
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    B2=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N);

    //A=(double*)malloc(sizeof(double)*9);
    //B=(double*)malloc(sizeof(double)*9);
    //C=(double*)malloc(sizeof(double)*9);
    //B2=(double*)malloc(sizeof(double)*9);
    //D=(double*)malloc(sizeof(double)*3);
	
	//Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
	  for(i=0;i<N;i++){
	   for(j=0;j<N;j++){
		A[i*N+j]=1;
		B[i+j*N]=1;
	   }
	   D[i]=1;
	  }   


//Inicializa matrices con los valores de prueba
//	A[0]=1;
//	A[1]=2;
//	A[2]=6;
//	A[3]=5;
//	A[4]=9;
//	A[5]=3;
//	A[6]=4;
//	A[7]=8;
//	A[8]=7;
	
//	B[0]=3;
//	B[1]=3;
//	B[2]=1;
//	B[3]=2;
//	B[4]=5;
//	B[5]=7;
//	B[6]=1;
//	B[7]=3;
//	B[8]=2;

//  D[0]=4;
//	D[1]=2;
//	D[2]=3;
	
  pthread_t thread[threads];
  int param[threads];
	
 //Punto A

  timetick = dwalltime();

  for(i=0; i<threads; i++){
    param[i]=i;
    pthread_create (&thread[i], NULL, multiply, (void*)param[i]);
  }

  for(i=0; i< threads; i++) { 
    pthread_join(thread[i], NULL); 
  }

  for(i=0; i<threads; i++){
    param[i]=i;
    pthread_create (&thread[i], NULL, multiply_diag, (void*)param[i]);
  }

  for(i=0; i< threads; i++) { 
    pthread_join(thread[i], NULL); 
  }  

  free(B2); 

 //VERIFICA PUNTO A 	
			
//	check=check&&(C[0])==60;
//	check=check&&(C[1])==108;
//	check=check&&(C[2])==57;
//	check=check&&(C[3])==180;
//	check=check&&(C[4])==152;
//	check=check&&(C[5])==114;
//	check=check&&(C[6])==172;
//	check=check&&(C[7])==194;
//	check=check&&(C[8])==126;

//	printf("Matriz A.B.C resultante del punto 1:\n");
//	printf(" %f",C[0]);printf("  %f",C[1]);printf("  %f\n",C[2]);
//	printf(" %f",C[3]);printf("  %f",C[4]);printf("  %f\n",C[5]);
//	printf(" %f",C[6]);printf("  %f",C[7]);printf("  %f\n",C[8]);
	
//  if(check){
//   printf("Multiplicacion de matrices A.B.C resultado correcto\n");
//  }else{
//   printf("Multiplicacion de matrices A.B.C resultado erroneo\n");
//  }

 //Punto B

 int l;

 for(i=0; i<threads; i++){
   param[i]=i;
   pthread_create (&thread[i], NULL, min_max_total, (void*)param[i]);
 }
 for(i=0; i<threads; i++) { 
   pthread_join(thread[i], NULL);
 }
 result=(((maxA-minA)*(maxA-minA))/(totalA/(N*N)))*(((maxB-minB)*(maxB-minB))/(totalB/(N*N)));
 for(i=0; i<threads; i++){
   param[i]=i;
   pthread_create (&thread[i], NULL, multiply_ident, (void*)param[i]);
 }
 for(i=0; i<threads; i++) { 
   pthread_join(thread[i], NULL);
 }


 //VERIFICA PUNTO B
//	check=check&&(C[0])==9216;
//	check=check&&(C[1])==41472;
//	check=check&&(C[2])==8755.2;
//	check=check&&(C[3])==27648;
//	check=check&&(C[4])==58368;
//	check=check&&(C[5])==17510.4;
//	check=check&&(C[6])==26419.2;
//	check=check&&(C[7])==74496;
//	check=check&&(C[8])==19353.6;
//	
//   if(check){
//     printf("Etapa 2 resultado correcto\n");
//    }else{
//     printf("Etapa 2 resultado erroneo\n");
//    }
// 	printf("\n");
// 	printf("Matriz resultante del punto 2:\n");
//	printf(" %f",C[0]);printf("  %f",C[1]);printf("  %f\n",C[2]);
//	printf(" %f",C[3]);printf("  %f",C[4]);printf("  %f\n",C[5]);
//	printf(" %f",C[6]);printf("  %f",C[7]);printf("  %f\n",C[8]);


 //Punto C

 aux=(double*)malloc(sizeof(double)*N);
 for(i=0;i<N;i++) {

  for(j=1;j<N;j++) {
   for(k=0;k<N-j;k++) {
    if(C[i+N*k]<C[i+N*(k+1)]) {
     for(l=0;l<N-i;l++) {
      aux[l]=C[i+l+N*k];
     }
     for(l=0;l<N-i;l++) {
      C[i+l+N*k]=C[i+l+N*(k+1)];
     }
     for(l=0;l<N-i;l++) {
      C[i+l+N*(k+1)]=aux[l];
     }
    }
   }
  }

 }
 free(aux);

 //VERIFICA PUNTO C

	//check=check&&(C[0])==27648;
	//check=check&&(C[1])==74496;
	//check=check&&(C[2])==19353.6;
	//check=check&&(C[3])==26419.2;
	//check=check&&(C[4])==58368;
	//check=check&&(C[5])==17510.4;
	//check=check&&(C[6])==9216;
	//check=check&&(C[7])==41472;
	//check=check&&(C[8])==8755.2;
	
    //if(check){
     //printf("Etapa 3 resultado correcto\n");
    //}else{
     //printf("Etapa 3 resultado erroneo\n");
    //}

// 	printf("\n");
// 	printf("Matriz resultante del punto 3:\n");
//	printf(" %f",C[0]);printf("  %f",C[1]);printf("  %f\n",C[2]);
//	printf(" %f",C[3]);printf("  %f",C[4]);printf("  %f\n",C[5]);
//	printf(" %f",C[6]);printf("  %f",C[7]);printf("  %f\n",C[8]);	
	
 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 free(A);
 free(B);
 free(C);
 free(D);

 return 0;
}
