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
	int a[10],b[10],identificador, cantidadProcesos, N, activeWorkers,flag,workerNumber, size; 			//identificador es el numero de proceso
	MPI_Init(&argc, &argv );
	MPI_Status *estado;
	MPI_Comm_size(MPI_COMM_WORLD, &cantidadProcesos ); 
	MPI_Comm_rank(MPI_COMM_WORLD, &identificador ); 
	int i,j,k,l,finish=0;
	double timetick, *arreglo, aux, dimension, *paquete,*merge;
	
	
	
		
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s n\n  n: Tamaño de la porcion que reciben los slaves\n", argv[0]);	//En N queda el tamaño de las partes
		exit(1);
	}
	
	dimension=1024; //dimension = 1048576;	//la dimension del arreglo a ordenar es 2^20
	int cantBloques = dimension/N;
	
	activeWorkers=0;

	merge=(double*)malloc(sizeof(double));

//TAG "1" hace sort
//tag "2" merge
//TAG "3" significa que se completó el programa
//tag "4" para devolver arreglos



	if (identificador == 0){  //Este es el master
		

		arreglo=(double*)malloc(sizeof(double)*dimension);
		
		
		srand((unsigned int)time(NULL));
		for(i=0;i<dimension;i++){		//Inicializa el arreglo con numeros random
			//arreglo[i]=rand();
		  arreglo[i]=1;
		}
		
		
		
		paquete=(double*)malloc(sizeof(double)*N+1);
		
		timetick = dwalltime();
		for(i=0; i<cantBloques; i++){				//Copia las porciones y las envia a los workers
		
			int k=0;
			for (j=i*N; j<(i+1)*N; j++){
				paquete[k]= arreglo[j];
				k++;
			}
			paquete[N]=i*N;	//la posicion de la porcion queda en el ultimo lugar del arreglo

			
			MPI_Recv(workerNumber, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, estado);   
		  		
			MPI_Send(paquete,1,MPI_DOUBLE,workerNumber,1,MPI_COMM_WORLD); 				//Chequea si hay workers que ya terminaron					 
			activeWorkers++;
  
			if (MPI_Iprobe(MPI_ANY_SOURCE,4, MPI_COMM_WORLD, flag, estado)){						
				MPI_Recv(paquete, N+1, MPI_DOUBLE, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, estado);
				activeWorkers--;
				for(j=0; j<N; j++){
					arreglo[(int)paquete[N]]=paquete[j];
					paquete[N]++;
				}

			}
		}
		while (activeWorkers>0){
			MPI_Recv(paquete, N+1, MPI_DOUBLE, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, estado);
			activeWorkers--;
			for(j=0; j<N; j++){
				arreglo[(int)paquete[N]]=paquete[j];
				paquete[N]++;
			}
		
		}

		N=N*2;
		while (N<=dimension){
			cantBloques=cantBloques/2;
			for(i=0; i<cantBloques; i++){
				int k=0;
				for (j=i*N; j<(i+1)*N; j++){
					paquete[k]= j;
		    		k++;
		 		}	
			}
			paquete[N]=i*N;	//el inicio de la porcion queda en la ultima posicion del arreglo
			
			
			MPI_Recv(workerNumber, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, estado); 
			MPI_Send(N,1,MPI_INT,workerNumber,2,MPI_COMM_WORLD);
			MPI_Send(paquete,1,MPI_DOUBLE,workerNumber,2,MPI_COMM_WORLD);	//Envía primero el tamaño de bloque y después el paquete		

			if (MPI_Iprobe(MPI_ANY_SOURCE,4, MPI_COMM_WORLD,flag, estado)){						
				MPI_Recv(paquete, N+1, MPI_DOUBLE, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, estado);
				for(j=0; j<N; j++){
					arreglo[(int)paquete[N]]=paquete[j];
					paquete[N]++;
				}

			}
			
			N=N*2;
		}
		printf("Tiempo en segundos %f\n", dwalltime() - timetick);


		MPI_Send(N,1,MPI_DOUBLE,workerNumber,3,MPI_COMM_WORLD); //send para halt
			

	}

	else{     //Soy un worker
		while(finish==0) {
			if(MPI_Probe(0,3, MPI_COMM_WORLD, estado)){
				finish++;
			}else{
				MPI_Send(identificador,1,MPI_INT,0,0,MPI_COMM_WORLD); 
		
				if (MPI_Probe(0,1, MPI_COMM_WORLD, estado)){	//Hay mensaje con flag 1, entonces pasa a ordenar
	
					paquete=(double*)malloc(sizeof(double)*N+1);
		
					MPI_Recv(paquete, N+1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, estado);

					//aca ordena
					for(i=1;i<N;i++) {
			     		for(j=0;j<N-i;j++) {
				  			if(paquete[j]>=paquete[j+1]) {
				   				aux=paquete[j];
				   				paquete[j]=paquete[j+1];
				   				paquete[j+1]=aux;
				  			}
				 		}
					}
					MPI_Send(paquete,N+1,MPI_DOUBLE,0,4,MPI_COMM_WORLD);
			
				}
				else if(MPI_Probe(0,2, MPI_COMM_WORLD, estado)){						//Hay mensaje con flag 2, entonces pasa al merge
					MPI_Recv(size, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, estado);
					paquete=(double*)realloc(paquete,sizeof(double)*size+1);
					MPI_Recv(paquete, size+1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, estado);
					merge=(double*)realloc(merge,sizeof(double)*size);
		    		j=0;
		    		k=size/2;
		    		l=0;
		    		while(j<size/2 && k<size) {
		     			if(paquete[j]<=paquete[k]){
		      				merge[l]=paquete[j];
		      				j++;
		     			}else{
		      				merge[l]=paquete[k];
		      				k++;
		     			}
		     			l++;
		    		}
		    		while(j<size/2) {
		     			merge[l]=paquete[j];
		     			j++;
		     			l++;
		    		}
		    		while(k<size) {
		     			merge[l]=paquete[k];
		     			k++;
		     			l++;
		    		}
		    		for(j=0;j<size;j++){
		     			paquete[j]=merge[j];
		    		}
		   	}
				MPI_Send(paquete,size,MPI_DOUBLE,0,4,MPI_COMM_WORLD);
			}
		}			
	}
	MPI_Finalize();	
}
