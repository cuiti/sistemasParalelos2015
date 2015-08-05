#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

double dwalltime(){	//Para calcular tiempo
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int main(int argc,char*argv[]){
	int a[10],b[10],identificador,N,workers,flag,size,total,i,j,k,l,workerNumber;	//identificador es el numero de proceso
	MPI_Init(&argc,&argv);
	MPI_Status estado;
	MPI_Comm_rank(MPI_COMM_WORLD,&identificador); 
	int finish=0;
	int sort=0;
	int index=0;
	int recieve=0;
	double timetick,tiempoComunicaciones,porcionTiempo,*arreglo,dimension,*paquete,aux,*merge,*paqMerge;
	tiempoComunicaciones =0;

	if ((argc != 2) || ((N = atoi(argv[1])) <= 0)){
		printf("\nUsar: %s n\n  n: Tamaño de la porcion que reciben los slaves\n", argv[0]);	//En N queda el tamaño de las partes
		exit(1);
	}
	dimension = 1024;//268435456;	//la dimension del arreglo a ordenar es 2^20

//TAG "0" para pedir trabajo
//TAG "1" hace sort
//TAG "2" merge
//TAG "3" significa que se completó el programa
//TAG "4" para devolver arreglos


	if(identificador==0){  //Este es el master
		paquete=(double*)malloc(sizeof(double)*N+1);
		arreglo=(double*)malloc(sizeof(double)*dimension);
		srand((unsigned int)time(NULL));
		for(i=0;i<dimension;i++){	
			arreglo[i]=(rand()/10000); //Inicializa el arreglo con numeros random
			//arreglo[i]=dimension-i; //Inicializa con el peor de los casos
			printf(" %i", (int)arreglo[i]);
		}
		timetick = dwalltime();
		MPI_Comm_size(MPI_COMM_WORLD, &workers);
		MPI_Comm_size(MPI_COMM_WORLD, &total);
		paquete=(double*)malloc(sizeof(double)*N+1);
		while((N<dimension)||(workers!=total)){
			MPI_Iprobe(MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&flag,&estado);
			if((flag)&(N<dimension)){
				porcionTiempo=dwalltime();
				MPI_Recv(&workerNumber,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&estado);
				tiempoComunicaciones+=dwalltime()-porcionTiempo;
				k=0;
				for(j=index;j<index+N;j++){
					paquete[k]=arreglo[j];
					k++;
				}
				paquete[N]=(double)index;					//la posicion de la porcion queda en el ultimo lugar del arreglo
				porcionTiempo=dwalltime();
				if(sort==0){
					MPI_Send(paquete,N+1,MPI_DOUBLE,workerNumber,1,MPI_COMM_WORLD);	
				}else{
					MPI_Send(&N,1,MPI_INT,workerNumber,2,MPI_COMM_WORLD);				//Envía primero el tamaño de bloque y después el paquete
					MPI_Send(paquete,N+1,MPI_DOUBLE,workerNumber,2,MPI_COMM_WORLD);
				}
				tiempoComunicaciones+=dwalltime()-porcionTiempo;
				workers--;
				index=index+N;
				if(index==dimension){
					while(workers!=total){
						MPI_Iprobe(MPI_ANY_SOURCE,4,MPI_COMM_WORLD,&flag,&estado);
						if(flag){
							porcionTiempo=dwalltime();
							MPI_Recv(paquete,N+1,MPI_DOUBLE,MPI_ANY_SOURCE,4,MPI_COMM_WORLD,&estado);
							tiempoComunicaciones+=dwalltime()-porcionTiempo;
							for(j=0;j<N;j++){
								arreglo[(int)paquete[N]]=paquete[j];
								paquete[N]++;
							}
							workers++;
						}
					}
					N=N*2;
    				paquete=(double*)realloc(paquete,sizeof(double)*N+1);			
					index=0;
					if(sort==0){
						sort++;
					}
				}
			
			}else if(flag){
				porcionTiempo=dwalltime();
			    MPI_Recv(&workerNumber,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&estado);
				tiempoComunicaciones+=dwalltime()-porcionTiempo;
			}else{
				MPI_Iprobe(MPI_ANY_SOURCE,4,MPI_COMM_WORLD,&flag,&estado);
				if(flag){
					porcionTiempo=dwalltime();
					MPI_Recv(paquete,N+1,MPI_DOUBLE,MPI_ANY_SOURCE,4,MPI_COMM_WORLD,&estado);
					tiempoComunicaciones+=dwalltime()-porcionTiempo;
					for(j=0;j<N;j++){
						arreglo[(int)paquete[N]]=paquete[j];
						paquete[N]++;
					}
					workers++;
				}
			}

		}
		for(i=1;i<total;i++){
			MPI_Send(&N,1,MPI_INT,i,3,MPI_COMM_WORLD);	//send para halt
		}

		  paquete=(double*)malloc(sizeof(double)*dimension);
	
		j=0;
		k=dimension/2;
		l=0;
		//printf("\n");printf("Master comienza el ultimo  merge \n");fflush(stdout);
		while(j<dimension/2 && k<dimension) {
			if(arreglo[j]<=arreglo[k]){
				paquete[l]=arreglo[j];
				j++;
			}else{
				paquete[l]=arreglo[k];
				k++;
			}
			l++;
		}
		while(j<dimension/2) {
			paquete[l]=arreglo[j];
			j++;
			l++;
		}
		while(k<dimension) {
			paquete[l]=arreglo[k];
			k++;
			l++;
		}
		for(j=0;j<dimension;j++){
			arreglo[j]=paquete[j];
		}
		printf("Master finaliza el ultimo  merge \n");
		printf("Tiempo en segundos %f\n", dwalltime() - timetick);
		printf("Tiempo de comunicaciones en el master: %f\n", tiempoComunicaciones);

		for(i=0;i<dimension;i++){	printf(" %i", (int)arreglo[i]);}


	}else{     									//Soy un worker
		MPI_Iprobe(MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&flag,&estado);
		if(flag){
			finish++;
		}else{
			porcionTiempo=dwalltime();
			MPI_Send(&identificador,1,MPI_INT,0,0,MPI_COMM_WORLD);
			tiempoComunicaciones+=dwalltime()-porcionTiempo;
			paquete=(double*)malloc(sizeof(double)*N+1);
		}
		while(finish==0) {
			MPI_Iprobe(MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&flag,&estado);
			if(flag){
				finish++;
			}else{
				MPI_Iprobe(0,1,MPI_COMM_WORLD,&flag,&estado);
				if(flag){						//Hay mensaje con flag 1, entonces pasa a ordenar
					//printf("Entra a ordenar ");
					porcionTiempo=dwalltime();			
					MPI_Recv(paquete,N+1,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&estado);
					tiempoComunicaciones+=dwalltime()-porcionTiempo;					
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
				    recieve++;
				}else{
					MPI_Iprobe(0,2,MPI_COMM_WORLD,&flag,&estado);
					if(flag){					//Hay mensaje con flag 2, entonces pasa al merge
						porcionTiempo=dwalltime();					
						MPI_Recv(&N,1,MPI_INT,0,2,MPI_COMM_WORLD,&estado);
						tiempoComunicaciones+=dwalltime()-porcionTiempo;

					    paquete=(double*)realloc(paquete,sizeof(double)*N+1);
				
						porcionTiempo=dwalltime();
						MPI_Recv(paquete,N+1,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&estado);
						tiempoComunicaciones+=dwalltime()-porcionTiempo;
						merge=(double*)malloc(sizeof(double)*N);
			       		j=0;
			        	k=N/2;
			        	l=0;
			        	while(j<N/2 && k<N) {
			         		if(paquete[j]<=paquete[k]){
			          		merge[l]=paquete[j];
			          		j++;
			         		}else{
			          		merge[l]=paquete[k];
			          		k++;
			         		}
			         		l++;
			        	}
			        	while(j<N/2) {
			         		merge[l]=paquete[j];
			         		j++;
			         		l++;
			        	}
			        	while(k<N) {
			         		merge[l]=paquete[k];
			         		k++;
			         		l++;
			        	}
			        	for(j=0;j<N;j++){
			         		paquete[j]=merge[j];
			        	}
					    recieve++;
					    free(merge);
			        }
		   	    }
				
				if(recieve!=0){
					porcionTiempo=dwalltime();
					MPI_Send(paquete,N+1,MPI_DOUBLE,0,4,MPI_COMM_WORLD);	//Envía el arreglo procesado
					MPI_Send(&identificador,1,MPI_INT,0,0,MPI_COMM_WORLD);	//Pide trabajo de nuevo
					tiempoComunicaciones+=dwalltime()-porcionTiempo;
					recieve = 0;
				}
			}
		}
		printf("\n Finalizo el worker %d", identificador);
		printf(" con tiempo de comunicaciones %f\n", tiempoComunicaciones);
	}
	MPI_Finalize();	
}
