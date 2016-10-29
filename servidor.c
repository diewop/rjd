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

#define MAX_CLIENTES 4  // Máximo números de clientes
#define MAXDATASIZE 100	/* El número máximo de datos en bytes */
#define TOTAL_INICIO 80000 // TotalDisponible Inicial.

//Estructura del Cajero
struct cajero {
	char		  id[9];
	unsigned	  totalDisponible;
};

// Estructura del registro
struct registro {
	char fecha[24];
	char hora[24];
	int	usuario; 
	struct cajero atm;
	int TotalDisponible;
	char exitoso[2];
	char evento[2];
};

// Estructura para la lista de transacciones
struct lista_transacciones {
	struct registro  trans;
	struct lista_transacciones* next;
};

struct usuario {
	int codigo;
	int intentos;
};

struct lista_usuarios {
	struct usuario  user;
	struct lista_usuarios * next;
};

int main(int argc, char *argv[]){
	//Suponiendo que todos los argumentos válidos
	int i = 1;
	int port_bsb_svr;
	char * bitacora_deposito;
	char * bitacora_retiro;
	char cajeroV = 0;
	int totalDisponible = 0;
	char * str=malloc(sizeof(char)*400);
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
			//printf("\nBitacora Dep %s\n",bitacora_deposito);
			}else{
				if(!strcmp("-o",argv[i])){
					bitacora_retiro = malloc(sizeof(char)*strlen(argv[i+1]));
					bitacora_retiro = argv[i+1];
					//printf("\nBitacora Ret %s\n",bitacora_retiro);
				}
			}
		
		}
		i = i + 1;
	}// end while


	
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

	
	struct cajero * aux;
	

	
	int fd, fd2; /* los ficheros descriptores */

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
	char quien = 0; // Para guardar si paso por EME(3), CB1(1), CB2(2)
	char caj [3]; //Para guardar el nombre de la caja
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
					aux = &CB2; // Basico II
				}
			}else{
				aux = &CB1; // Basico I
			}
		}else{
			aux = &EME; // EME
		} 
		
		cajeroV = 1;
		buf[numbytes] = '\0';

		printf("%s\n",buf);
		if(cajeroV){
			//Envío el totalDisponible que tenía
			sprintf(str,"%u",aux->totalDisponible);
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
			
			buf[numbytes]='\0';
			printf("Nombre del Usuario %s\n",buf);
			
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
			
			
			//Envio de Confirmación
			if(send(fd2,"OK\n", 25, 0) == -1){
			printf("Erro\n");
				if(send(fd2,"OK\n", 25, 0) == -1){
								printf("Erro2\n");
					if(send(fd2,"OK\n", 25, 0) == -1){
									printf("Erro3\n");
						perror(" error en send. Cliente perdido \n");
						close(fd2);
					}
				}
			}
			printf("Envié OK\n");
			
			
			//Recibo Monto
			if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
				/* llamada a recv() */
				perror(" error en recv \n");
				exit(-1);
			}
			
			buf[numbytes]='\0';
			printf("Monto %s\n",buf);
			
			//Envio Confirmación de 
			if(send(fd2,"Descontar Monto\n", 25, 0) == -1){
				if(send(fd2,"Descontar Monto\n", 25, 0) == -1){
					if(send(fd2,"Descontar Monto\n", 25, 0) == -1){
						perror(" error en send. Cliente perdido \n");
						close(fd2);
					}
				}
			}
			printf("Envié Monto\n");
			
			//Recibo Estado
			if((numbytes=recv(fd2, buf, MAXDATASIZE, 0)) == -1){
				/* llamada a recv() */
				perror(" error en recv \n");
				exit(-1);
			}
			buf[numbytes]='\0';
			printf("Estado %s\n",buf);
			
			//MENSAJE DE ÉXITO
			if(send(fd2,"Mensaje Éxito\n", 25, 0) == -1){
				if(send(fd2,"Mensaje Éxito\n", 25, 0) == -1){
					if(send(fd2,"Mensaje Éxito\n", 25, 0) == -1){
						perror(" error en send. Cliente perdido \n");
						close(fd2);
					}
				}
			}
			

			printf("\nLLEGUÉ AL FINAL De Envíos\n");
			
		}//Cierre de CajeroV
		
			
		
		printf("\nLLEGUÉ AL FINAL\n");
		sleep(2);
		cajeroV = 0;
	}//While
		
		
	return 0;

}

