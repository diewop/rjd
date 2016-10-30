#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>  
#include <string.h>  
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>   //fecha y hora

#define MAX_CLIENTES 4  // Máximo números de clientes
#define MAXDATASIZE 100	// El número máximo de datos en bytes
#define TOTAL_INICIO 80000 // TotalDisponible Inicial.
#define MAX_INTENTOS 3 //Máximo retiros permitidos por usuario

struct cajero {
	char		  id[9];
	long long	  totalDisponible;
};

/*// Estructura del registro
struct registro {
	char fecha[24];
	char hora[24];
	int	usuario; 
	struct cajero atm;
	long long totalDisponible;
	char exitoso[2];
	char evento[2];
	struct registro * next;
};
*/


struct usuario {
	int codigo;
	char intentos;
	struct usuario *next;
};

struct usuario * searchLista(struct usuario * lista, char * id){
	struct usuario * userAux =  malloc(sizeof(struct usuario));
	char * str =  malloc(sizeof(char)*400);
	userAux=lista;
	sprintf(str,"%d",lista->codigo);
	if(!strcmp(str,id)){
		userAux = lista;
		return userAux;	
	}else{
		while(userAux->next!=NULL){
			userAux = userAux->next;
			sprintf(str,"%d",userAux->codigo);
			if(!strcmp(str,id)){
				return userAux;
			}
		}
		userAux->next = malloc(sizeof(struct usuario));
		userAux = userAux->next;
		userAux->codigo = atoi(id);
		userAux->next = NULL;
		userAux->intentos = 1;
		return userAux;
			
	}
};


int main(int argc, char *argv[]){
	//Suponiendo que todos los argumentos válidos
	int i = 1;
	int port_bsb_svr;
	char * bitacora_deposito;
	char * bitacora_retiro;
	char cajeroV = 0;
	long long totalDisponible = 0;
	char * str=malloc(sizeof(char)*400);
	int monto =0 ;
	char exitoso[4];
	strcpy(exitoso,"NO");
	FILE * fdR, * fdD;//Descriptores para (..R)retiros y (..D)depositos
	/*
	* -l port_bsb_svr (puerto del servidor)
	* -i bitacora_deposito (Ruta del archivo donde estarán los depositos)
	* -o bitacora_retiro (Ruta del archivo donde estarán los retiros)
	*/
	while(i<argc){
		if (!strcmp("-l",argv[i])){
			port_bsb_svr = atoi(argv[i+1]);
			//printf("\nPort %d\n",port_bsb_svr);
		}else{
			if(!strcmp("-i",argv[i])){
			bitacora_deposito = malloc(sizeof(char)*strlen(argv[i+1]));
			bitacora_deposito = argv[i+1];
			printf("\nBitacora Dep %s\n",bitacora_deposito);
			}else{
				if(!strcmp("-o",argv[i])){
					bitacora_retiro = malloc(sizeof(char)*strlen(argv[i+1]));
					bitacora_retiro = argv[i+1];
					printf("\nBitacora Ret %s\n",bitacora_retiro);
				}
			}
		
		}
		i = i + 1;
	}// end while

	//Asignando los descriptores correspondientes
	if((fdR = fopen ( bitacora_retiro, "w+" ))==NULL){
		perror("Error de Entrada y Salida");
		exit(-1);
	};
	
	if((fdD = fopen ( bitacora_deposito, "w+" ))==NULL){
		perror("Error de Entrada y Salida");
		exit(-1);
	};
	
	
	if(fprintf(fdD,"%s","Fecha\t\tHora\t\tcódigo\tcajero\top\tmonto\tTotalDisponible\t¿Fue Exitoso?\n")==-1){
		perror("What:? );");
		exit(-1);
	}
	fflush(fdD);  //Prints to a file
	if(fprintf(fdR,"%s","Fecha\t\tHora\t\tcódigo\tcajero\top\tmonto\tTotalDisponible\t¿Fue Exitoso?\n")==-1){
		perror("What:? );");
		exit(-1);
	}
	fflush(fdR);
	
	/*Tres Cajeros, EME, CB1 CB2*/
	struct cajero CB1;
	strcpy(CB1.id,"CB1");
	CB1.totalDisponible = TOTAL_INICIO;

	struct cajero CB2;
	strcpy(CB2.id,"CB2");
	CB2.totalDisponible = TOTAL_INICIO;

	struct cajero EME;
	strcpy(EME.id,"EME");
	EME.totalDisponible = TOTAL_INICIO;

	//Cajero Auxiliar, me servirá para acceder a la información de los cajeros.
	struct cajero * aux; 
	
	// Lista de Usuarios
	struct usuario * lista = malloc(sizeof(struct usuario));
	// Apuntador a Usuario Auxiliar
	struct usuario * userAux = malloc(sizeof(struct usuario));


	
	int fd, fd2; //Descriptores de Archivos para los sockets*/


	struct sockaddr_in server; 
	/* para la información de la dirección del servidor */

	struct sockaddr_in client; 
	/* para la información de la dirección del cliente */

	int sin_size;

	char buf[MAXDATASIZE];
	/* en donde es almacenará el texto recibido */
	int numbytes;
	/* cantidad de bytes en el buffer */	
	
	
	// Abrir un socket, de tipo IPv4 y tipo TCP
	if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
		perror("error en socket()\n");
		exit(-1);
	}
	
	//Preparando el servidor
	server.sin_family = AF_INET;         //Familia de Protocolo IPv4     
	server.sin_addr.s_addr = INADDR_ANY; //Coloca la dirección IP automáticamente
	server.sin_port = htons(port_bsb_svr); //Puerto del servidor
	bzero(&(server.sin_zero),8); 
	
	/* A continuación la llamada a bind() para asignar el socket al puerto
	 *  definido 
	*/
	if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1) {
		perror("error en bind() \n");
		exit(-1);
	}
	//Colocará el socket en modo escucha
	if(listen(fd,MAX_CLIENTES) == -1) { 
		perror("error en listen()\n");
		exit(-1);
	}
	
	while(1){
		sin_size=sizeof(struct sockaddr_in);
		//Accept permitirá que algún cliente se conecte tras utilizar la función connect()
		if ((fd2 = accept(fd,(struct sockaddr *)&client, &sin_size))==-1){
			perror("error en accept()\n");
			exit(-1);
		
		}
		printf("Connection from %s accepted \n", inet_ntoa(client.sin_addr) ); 
		/* que mostrará la IP del cliente */
			 
		if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
			/* llamada a recv() */
			perror(" error en recv \n");
			exit(-1);
		}

		if(strcmp(buf,"EME")){
			if(strcmp(buf,"CB1")){
				if(strcmp(buf,"CB2")){
					if(send(fd2,"Cajero Inválido\n", 25, 0) == -1){
						perror("error de envío, cliente perdido");
						close(fd2);
					}
					printf("Cajero Inválido: %s\n",buf);
				}else{
					cajeroV = 1;
					aux = &CB2; // Basico II
				}
			}else{
				cajeroV = 1;
				aux = &CB1; // Basico I
			}
		}else{
			cajeroV = 1;
			aux = &EME; // EME
		} 
		
		
		buf[numbytes] = '\0';

		printf("%s\n",buf);
		if(cajeroV){
			//Envío el totalDisponible que tenía
			sprintf(str,"%lli",aux->totalDisponible);
			if(send(fd2,str, 25, 0) == -1){
				if(send(fd2,str, 25, 0) == -1){
					if(send(fd2,str, 25, 0) == -1){
						perror(" error en send. Cliente perdido \n");
						close(fd2);
					}
				}
			}		
			
			
			//Recibo codigo del Usuario
			if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
				/* llamada a recv() */
				perror(" error en recv \n");
				exit(-1);
			}
			
			if(!strcmp(buf,"Necesito Recarga")){
				aux->totalDisponible = TOTAL_INICIO;
			}
			else{
				if(!strcmp(buf,"Monto Por Encima")){
					buf[numbytes]='\0';
					printf("Monto inválido %s\n",buf);
				}else{
					userAux = searchLista(lista,buf);
					
					//Envio de Usuario Válido			
					if(send(fd2,"Usuario Válido\n", 25, 0) == -1){
						if(send(fd2,"Usuario Válido\n", 25, 0) == -1){
							if(send(fd2,"Usuario Válido\n", 25, 0) == -1){
								perror(" error en send. Cliente perdido \n");
								close(fd2);
							}
						}
					}
					printf("Envié Usuario Válido\n");
					//Recibo Operación
					if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
						/* llamada a recv() */
						perror(" error en recv \n");
						exit(-1);
					}
					buf[numbytes]='\0';
					printf("Tipo de operación %s\n",buf);
					
					//Número de intentos máximos
					//printf("\n\nNúmero de Intentos: %d**\n\n",userAux->intentos);
					if((userAux->intentos > MAX_INTENTOS) & (!strcmp(buf,"r"))){
						printf("Usuario ha alcanzado máximo Número de retiros\n");
						if(send(fd2,"MAX_ALC\n", 25, 0) == -1){
							if(send(fd2,"OK\n", 25, 0) == -1){
								if(send(fd2,"MAX_ALC\n", 25, 0) == -1){
									perror(" error en send. Cliente perdido \n");
									close(fd2);
								}
							}
						}				
					}else{
			
						//Envio de Confirmación
						if(send(fd2,"OK\n", 25, 0) == -1){
							if(send(fd2,"OK\n", 25, 0) == -1){
								if(send(fd2,"OK\n", 25, 0) == -1){
									perror(" error en send. Cliente perdido \n");
									close(fd2);
								}
							}
						}
						printf("Envié OK\n");
			
						strcpy(str,buf); //Tipo de Operación;
					
					
					
						//Recibo Monto
						if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
							/* llamada a recv() */
							perror(" error en recv \n");
							exit(-1);
						}
			
						buf[numbytes]='\0';
						printf("Monto %s\n",buf);
						monto = atoi(buf);
						if(!strcmp(str,"r")){
							if(aux->totalDisponible<=5000){
								//No debería pasar estp
								if(send(fd2,"No suficiente\n", 25, 0) == -1){
									perror(" error en send. Cliente perdido \n");
									close(fd2);
								}
							}else{
								aux->totalDisponible=aux->totalDisponible-monto;
								//Envio Confirmación de confirmación
								if(send(fd2,"Hecho?\n", 25, 0) == -1){
									if(send(fd2,"Hecho?\n", 25, 0) == -1){
										if(send(fd2,"Hecho?\n", 25, 0) == -1){
											perror(" error en send. Cliente perdido \n");
											close(fd2);
										}
				
									}
								}
							}
						}//cierre de retiro
						if(!strcmp(str,"d")){
							aux->totalDisponible = aux->totalDisponible+monto;
							if(send(fd2,"Hecho?\n", 25, 0) == -1){
									if(send(fd2,"Hecho?\n", 25, 0) == -1){
										if(send(fd2,"Hecho?\n", 25, 0) == -1){
											perror(" error en send. Cliente perdido \n");
											close(fd2);
										}
				
									}
								}
						}
			
			
			
						//Recibo Estado
						if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
							/* llamada a recv() */
							if(!strcmp(str,"d")){
								strcpy(exitoso,"NO");
							}else{
								strcpy(exitoso,"NO");
							}
							perror(" error en recv \n");
							exit(-1);
						}
						buf[numbytes]='\0';
						printf("Estado %s\n",buf);
						strcpy(exitoso,"SÍ");
			
	
						userAux->intentos += 1;
						
						time_t tiempo = time(0);
						struct tm * tlocal = localtime(&tiempo);
						char output[128];
						strftime(output,128,"%d/%m/%y\t%H:%M:%S",tlocal);
						if(!strcmp(str,"r")){
							fprintf(fdR," %s ",output);
							fprintf(fdR,"\t%d",userAux->codigo);
							fprintf(fdR,"\t%s ",aux->id);
							fprintf(fdR,"\t%s ",str);
							fprintf(fdR,"\t%d ",monto);
							fprintf(fdR,"\t%lld",aux->totalDisponible);
							fprintf(fdR,"\t\t\t%s\n",exitoso);
							fflush(fdR); 
						}else{
							fprintf(fdD," %s ",output);
							fprintf(fdD,"\t%d",userAux->codigo);
							fprintf(fdD,"\t%s ",aux->id);
							fprintf(fdD,"\t%s ",str);
							fprintf(fdD,"\t%d ",monto);
							fprintf(fdD,"\t%lld",aux->totalDisponible);
							fprintf(fdD,"\t\t\t%s\n",exitoso);
							fflush(fdD); 
						
						}
			
						//MENSAJE DE ÉXITO
						if(send(fd2,output, 25, 0) == -1){
							if(send(fd2,output, 25, 0) == -1){
								if(send(fd2,output, 25, 0) == -1){
									perror(" error en send. Cliente perdido \n");
									close(fd2);
								}
							}
						}
						
						
					}
				}//Número de intentos máximos alcanzados
			}//En caso que necesita recarga o sobrepase
		}//Cierre de CajeroV
		printf("\n\n\t\tMensaje Final\t\t\n");
		cajeroV = 0;
	}//While
	fclose(fdR);
	fclose(fdD);
		
	return 0;

}

