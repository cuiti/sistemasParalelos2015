#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

//Dimension por defecto de las matrices
	 int N=100;  
//int N=3;   <-- para testear, la cambiamos por 3

//Este metodo devuelve el max-min en result[0] y el promedio en result[1]  				
void getValues(double *matriz, double *resultados) {
	 int i=0;
	 double min=999999;
	 double max=-999999;
	 double total=0;



	#pragma omp parallel for shared (matriz,N,max,min) reduction(+:total)
	 for(i=0;i<(N*N);i++) {
	  if(matriz[i]>max) {
	   max=matriz[i];
	  }
	  if(matriz[i]<min) {
	   min=matriz[i];
	  }
	  total+=matriz[i];
	 }

	 resultados[0]=max-min;
	 resultados[1]=total/(N*N);



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

  //Asignamos un numero fijo de threads, también podrían llegar por parámetro
  int numThreads=4;
  omp_set_num_threads(numThreads);

 double *A,*B,*B2,*C,*D,*r1,*r2, *aux, *C2;
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
    r1=(double*)malloc(sizeof(double)*2);
    r2=(double*)malloc(sizeof(double)*2);

    //A=(double*)malloc(sizeof(double)*9);
    //B=(double*)malloc(sizeof(double)*9);
   // C=(double*)malloc(sizeof(double)*9);
    //B2=(double*)malloc(sizeof(double)*9);
   // D=(double*)malloc(sizeof(double)*3);
	
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

//	D[0]=4;
//	D[1]=2;
//	D[2]=3;
	
	
 //Punto A

  timetick = dwalltime();

    #pragma omp parallel for private(j,k) shared (A,B,B2,N)
	  for(i=0;i<N;i++){
	   for(j=0;j<N;j++){
		B2[i*N+j]=0;
		for(k=0;k<N;k++){
		 B2[i*N+j]=(B2[i*N+j] + A[i*N+k]*B[k+j*N]);
		}
	   }
	  }   

	#pragma omp parallel for private(j) shared (C,B2,D,N)
	  for(i=0;i<N;i++){
	   for(j=0;j<N;j++){
		C[i*N+j]=(B2[i*N+j]*D[j]);
	   }
	  }

  free(B2); 

 //VERIFICACION
 //	printf("\n");
 //	printf("Matriz resultante del punto 3:\n");
//	printf(" %f",C[0]);printf("  %f",C[1]);printf("  %f\n",C[2]);
//	printf(" %f",C[3]);printf("  %f",C[4]);printf("  %f\n",C[5]);
//	printf(" %f",C[6]);printf("  %f",C[7]);printf("  %f\n",C[8]);
	

 //Punto B

 double result;
 int l;

 getValues(A,r1);
 getValues(B,r2);
 result=((r1[0]*r1[0])/r1[1])*((r2[0]*r2[0])/r2[1]);

	#pragma omp parallel for shared(C,N)
	 for(l=0;l<(N*N);l++) {
	  C[l]=C[l]*result;
	 }

 //VERIFICACION
 //	printf("\n");
 //	printf("Matriz resultante del punto 3:\n");
//	printf(" %f",C[0]);printf("  %f",C[1]);printf("  %f\n",C[2]);
//	printf(" %f",C[3]);printf("  %f",C[4]);printf("  %f\n",C[5]);
//	printf(" %f",C[6]);printf("  %f",C[7]);printf("  %f\n",C[8]);
 
 
 free(r1);
 free(r2);

 //Punto C

 struct celda {
  double pos;
  double value;
 } valor, *column, auxCelda;

 C2=(double*)malloc(sizeof(double)*N*N);
 column=(struct celda*)malloc((sizeof(double)+sizeof(double))*N);

// C2=(double*)malloc(sizeof(double)*9);
// column=(struct celda*)malloc((sizeof(int)+sizeof(double))*3);


 for(i=0;i<N;i++) {
  
  for(k=0;k<N;k++) {
   valor.pos=k;
   valor.value=C[i+N*k];
   column[k]=valor;
  }

  for(j=1;j<N;j++) {
   for(k=0;k<N-j;k++) {
    if(column[k].value<column[k+1].value) {
     auxCelda=column[k];
     column[k]=column[k+1];
     column[k+1]=auxCelda;
    }
   }
  }
  for(j=0;j<N;j++){
   valor=column[j];
   for(k=0;k<N-i;k++){
    C2[(j*N)+k]=C[((int)valor.pos*N)+i+k];
   }
  }  

  for(j=0;j<N;j++){
   for(k=0;k<N-i;k++){
    C[(j*N)+k+i]=C2[(j*N)+k];
   }
  }

 }
 free(C2);

 //VERIFICACION
 //	printf("\n");
 //	printf("Matriz resultante del punto 3:\n");
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
