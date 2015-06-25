#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

double dwalltime(){		//Para calcular tiempo
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}


int main(int argc,char*argv[]){
	int a[10],b[10],identificador, cantidadProcesos, N; 			//identificador es el numero de proceso
	MPI_Init(&argc, &argv );
	MPI_Status *estado;
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadProcesos ); 
	MPI_Comm_rank(MPI_COMM_WORLD, &identificador ); 
	int i,j,k;
	double timetick, *arreglo, *aux, dimension;
	
	
		
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s n\n  n: Tamaño de la porcion que reciben los slaves\n", argv[0]);
		exit(1);
	}
	
	dimension = 1024;  //dimension = 1048576;	//la dimension del arreglo a ordenar es 2^20
	int cantBloques = dimension/N;
	
	if (identificador == 0){  //Este es el master
		

		arreglo=(double*)malloc(sizeof(double)*dimension);
		
		
		srand((unsigned int)time(NULL));
		for(i=0;i<dimension-1;i++){		
			//arreglo[i]=rand();		//Inicializa el arreglo con numeros random
			arreglo[i]=2;
		}
	
		
		aux=(double*)malloc(sizeof(double)*cantBloques);
		
		for(i=0; i<cantBloques; i++){
		
		  int k=0;
		  for (j=i*N; j<(i+1)*N; j++){
		    aux[k]= j;
		    k++;
		  }
		  
		  int workerNumber;
		  MPI_Recv(workerNumber, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, estado);   
		  
		  MPI_Send(aux,N,MPI_DOUBLE,workerNumber,1,MPI_COMM_WORLD);			//TAG "1" hace sort, tag "2" merge
		  
		}	
	
	}
	else{     //Soy un worker
		double *buf;
		buf=(double*)malloc(sizeof(double)*cantBloques);
		
		MPI_Send(identificador,1,MPI_INT,0,1,MPI_COMM_WORLD); 
		
		MPI_Recv(buf, N, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, estado);   
		
		printf("  %d\n",buf[0]);
		
	}
			
	MPI_Finalize();
}
