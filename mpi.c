#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>


//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}


int main(int argc,char*argv[]){
	int a[10],b[10],identificador, cantidad; 
	MPI_Init(&argc, &argv );
	MPI_Status estado;
	MPI_Comm_size(MPI_COMM_WORLD, &cantidad ); 
	MPI_Comm_rank(MPI_COMM_WORLD, &identificador ); 
	int i,j,k;
	
	double timetick;
	
	//HACE COSAS LOCAS PARALELAS
	
	MPI_Finalize()
}