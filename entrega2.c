#include<stdio.h>
#include<stdlib.h>

//Dimension por defecto de las matrices
	// int N=100;  <-- para testear, la cambiamos por 3
int N=3;

//Este metodo devuelve el max-min en result[0] y el promedio en result[1]
void getValues(double *matriz, double *resultados) {
 int i=0;
 double min=999999;
 double max=-999999;
 double total=0;

 for(i;i<(N*N);i++) {
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
 double *A,*B,*B2,*C,*D,*r1,*r2, *aux;
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

//    A=(double*)malloc(sizeof(double)*9);
//    B=(double*)malloc(sizeof(double)*9);
//    C=(double*)malloc(sizeof(double)*9);
//    B2=(double*)malloc(sizeof(double)*9);
//    D=(double*)malloc(sizeof(double)*3);
	
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

  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    B2[i*N+j]=0;
    for(k=0;k<N;k++){
     B2[i*N+j]=(B2[i*N+j] + A[i*N+k]*B[k+j*N]);
    }
   }
  }   

  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    C[i*N+j]=(B2[i*N+j]*D[j]);
   }
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

 double result;
 int l;

 getValues(A,r1);
 getValues(B,r2);
 result=((r1[0]*r1[0])/r1[1])*((r2[0]*r2[0])/r2[1]);
 for(l=0;l<(N*N);l++) {
  C[l]=C[l]*result;
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
 
 
 free(r1);
 free(r2);

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
