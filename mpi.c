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
	int a[10],b[10],identificador, cantidad, N; 			//identificador es el numero de proceso, cantidad es la cant. de procesos
	MPI_Init(&argc, &argv );
	MPI_Status estado;
	MPI_Comm_size(MPI_COMM_WORLD, &cantidad ); 
	MPI_Comm_rank(MPI_COMM_WORLD, &identificador ); 
	int i,j,k;
	double timetick;
	
	
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s n\n  n: Tamaño de la porcion que reciben los slaves\n", argv[0]);
		exit(1);
	}
	
	
	if (identificador == 0){
		//Soy el master, aloco memoria y esas cosas, y me pongo a esperar que los slaves me pidan trabajo, con un Receive
	}
	else{ 
		//Soy un slave, envío solicitud de trabajo al master, hago el ordenamiento y pido de nuevo
	}
			
	MPI_Finalize()
}