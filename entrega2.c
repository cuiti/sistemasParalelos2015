#include<stdio.h>
#include<stdlib.h>

//Dimension por defecto de las matrices
int N=100;

//Este metodo devuelve el max-min en result[0] y el promedio en result[1]
void getValues(double *matriz, double *resultados) {
 int i=0;
 double min=999999;
 double max=-999999;
 double total=0;

 for(i;i<N;i++) {
  if(matriz[i]>max) {
   max=matriz[i];
  }
  if(matriz[i]<min) {
   min=matriz[i];
  }
  total+=matriz[i];
 }
 resultados[0]=max-min;
 resultados[1]=total/N;
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

 //Aloca memoria para las matrices
 A=(double*)malloc(sizeof(double)*N*N);
 B=(double*)malloc(sizeof(double)*N*N);
 C=(double*)malloc(sizeof(double)*N*N);
 B2=(double*)malloc(sizeof(double)*N*N);
 D=(double*)malloc(sizeof(double)*N);
 r1=(double*)malloc(sizeof(double)*2);
 r2=(double*)malloc(sizeof(double)*2);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
    A[i*N+j]=1;
    B[i+j*N]=1;
   }
   D[i]=1;
  }   


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
    C[i*N+j]=0;
 	C[i*N+j]=(C[i*N+j] + B2[i*N+(N*j)]*D[N*j]);
   }
  }

  free(B2); 

 //VERIFICAR A
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(C[i*N+j])==N;
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 //Punto B

 double result;
 int l;

 getValues(A,r1);
 getValues(B,r2);
 result=((r1[0]*r1[0])/r1[1])*((r2[0]*r2[0])/r2[1]);
 for(l=0;l<(N*N);l++) {
  C[l]=C[l]*result;
 }

 //VERIFICAR B

 free(r1);
 free(r2);

 //Punto C

 aux=(double*)malloc(sizeof(double)*N);
 for(i=0;i<N;i++) {

  for(j=1;j<=N;j++) {
   for(k=0;k<N-j;k++) {
    if(C[i+N*k]>C[i+N*(k+1)]) {
     for(l=i;l<N;l++) {
      aux[l]=C[(i+l)*N+k];
     }
     for(l=i;l<N;l++) {
      C[(i+l)*N+k]=C[(i+l)*N+k+1];
     }
     for(l=i;l<N;l++) {
      C[(i+l)*N+k+1]=aux[l];
     }
    }
   }
  }

 }
 free(aux);

 //VERIFICAR C

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 free(A);
 free(B);
 free(C);
 free(D);

 return 0;
}
