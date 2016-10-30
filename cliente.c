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


#define PORT 3550         
/* El Puerto Abierto del nodo remoto */

#define MAXDATASIZE 100   
/* El número máximo de datos en bytes */

int main(int argc, char *argv[]){
	int i = 1;
	int port_bsb_svr; //puerto del servidor
	char * ip_server; // IP del servidor
	char * op;		  // Tipo de operación
	int codigoUsuario; // Código del Usuario
	int monto;			// Monto a retirar
	char * nombreCajero;
	char * str=malloc(sizeof(char)*400);
	char * it=malloc(sizeof(char)*400);
	int totalDisponible = 80000;
	/*
	* Extrayendo argumentos:
	* -p port_bsb_svr (puerto del servidor)
	* -d ip_server	(IP del servidor, en forma IPv4)
	* -c op monto   (Tipo de operación["d" o "r"] seguido del monto)
	* -i codigoUsuario (Código del usuario)
	* -n nombreCajero (Nombre del Cajero)
	*/
	while(i<argc){
		if (!strcmp("-p",argv[i])){
			port_bsb_svr = atoi(argv[i+1]);
			//printf("\nPort %d\n",port_bsb_svr);
		}else{
			if(!strcmp("-d",argv[i])){
			ip_server = malloc(sizeof(char)*strlen(argv[i+1]));
			ip_server = argv[i+1];
			//printf("\nBitacora Dep %s\n",bitacora_deposito);
			}else{
				if(!strcmp("-c",argv[i])){
					op = malloc(sizeof(char)*strlen(argv[i+1]));
					op = argv[i+1];
					monto = atoi(argv[i+2]);
					//printf("\nBitacora Ret %s\n",bitacora_retiro);
				}else{
					if(!strcmp("-i",argv[i])){
						codigoUsuario = atoi(argv[i+1]);
						//printf("\nBitacora Ret %s\n",bitacora_retiro);
					}else{
						if(!strcmp("-n",argv[i])){
							nombreCajero = malloc(sizeof(char)*strlen(argv[i+1]));
							nombreCajero = argv[i+1];
						}
					
					}
				}
			}
		
		}
		i = i + 1;
	}// end while


	int fd, numbytes;       
	/* ficheros descriptores */

	char buf[MAXDATASIZE];  
	/* en donde es almacenará el texto recibido */

	struct hostent *he;         
	/* estructura que recibirá información sobre el nodo remoto */

	struct sockaddr_in server;  
	/* información sobre la dirección del servidor */
	
	
	//Obteniendo la IP del servidor(en caso de ser dns)
	if ((he=gethostbyname(ip_server))==NULL){       
		/* llamada a gethostbyname() */
		perror("error en gethostbyname()\n");
		exit(-1);
	}

	//Abrimos el Socket
	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){
		/* llamada a socket() */
		perror("error en socket()\n");
		exit(-1);
	}

	//Configurando el servidor
	server.sin_family = AF_INET;
	server.sin_port = htons(port_bsb_svr);
	server.sin_addr = *((struct in_addr *)he->h_addr);  
	bzero(&(server.sin_zero),8);


	//Conectando al servidor
	if(connect(fd, (struct sockaddr *)&server,
		sizeof(struct sockaddr))==-1){ 
			if(connect(fd, (struct sockaddr *)&server,
				sizeof(struct sockaddr))==-1){ 
					if(connect(fd, (struct sockaddr *)&server,
						sizeof(struct sockaddr))==-1){ 
							/* llamada a connect() */
							perror("El servidor está caído\n");
							exit(-1);
					}
			}
	}
	
	//Envío nombre del Cajero
	strcpy(str,nombreCajero);
	if(send(fd, str, 25, 0) == -1){
		if(send(fd, str, 25, 0) == -1){
			if(send(fd, str, 25, 0) == -1){
				// llamada a send
				perror(" error en send \n");
				exit(-1);
			}
		}
	}
	
	//Recibo el totalDisponible
	if ((numbytes=recv(fd, buf, MAXDATASIZE, 0)) == -1){  
		/* llamada a recv() */
		perror(" error recv \n");
		exit(-1);
	}
	
	if(!strcmp(buf,"Cajero Inválido\n")){
		perror(" Error cajero inválido ");
		exit(-1);
	
	}
	totalDisponible = atoi(buf);
	if(totalDisponible>5000 || (!strcmp(op,"d"))){
		//Bienvenido
		printf("Cajero Total Disponible: %s\n",buf);
	
		//Envio Codigo Usuario
		sprintf(it, "%d", codigoUsuario);
		strcpy(str,it);
		strcat(str,"\0");

		if(send(fd, str, 25, 0) == -1){
			if(send(fd, str, 25, 0) == -1){
				if(send(fd, str, 25, 0) == -1){
					// llamada a send
					perror(" error en send \n");
					exit(-1);
				}
			}
		}
	
		printf("Codigo Usuario\n");

		//Recibo Validez Usuario
		if ((numbytes=recv(fd, buf, MAXDATASIZE, 0)) == -1){  
			/* llamada a recv() */
			perror(" error recv \n");
			exit(-1);
		}
		if(strcmp(buf,"Usuario Válido\n")){
			perror(" Error Usuario Inválido ");
			exit(-1);
	
		}
		buf[numbytes]='\0';
		printf("%s\n",buf);
	
		//Envio Operación
		strcpy(str,op);
		strcat(str,"\0");
	
		if(send(fd, str, 25, 0) == -1){
			if(send(fd, str, 25, 0) == -1){
				if(send(fd, str, 25, 0) == -1){
					// llamada a send
					perror(" error en send \n");
					exit(-1);
				}
			}
		}
	
		//Recibo Validez de la Operación
		if ((numbytes=recv(fd, buf, MAXDATASIZE, 0)) == -1){  
			/* llamada a recv() */
			perror(" error recv \n");
			exit(-1);
		}
		if(strcmp(buf,"OK\n")){
			perror(" Error Operación Inválida ");
			exit(-1);
	
		}
		buf[numbytes]='\0';
		printf("%s",buf);
	
		//Envío Monto
		sprintf(it, "%d", monto);
		strcpy(str,it);
		strcat(str,"\0");
	
		if(send(fd, str, 25, 0) == -1){
			if(send(fd, str, 25, 0) == -1){
				if(send(fd, str, 25, 0) == -1){
					// llamada a send
					perror(" error en send \n");
					exit(-1);
				}
			}
		}
	
		//Recibo Validez del Monto
		if ((numbytes=recv(fd, buf, MAXDATASIZE, 0)) == -1){  
		/* llamada a recv() */
			perror(" error recv \n");
			exit(-1);
		}

		if(strcmp(buf,"Hecho?\n")){
			perror(" Error Monto Inválido ");
			exit(-1);
	
		}
	
		buf[numbytes]='\0';
		printf("%s\n",buf);
	
		//Envio Confirmación
		strcpy(str,"He realizado la operación");
		strcat(str,"\0");
	
		if(send(fd, str, 26, 0) == -1){
			if(send(fd, str, 26, 0) == -1){
				if(send(fd, str, 26, 0) == -1){
					// llamada a send
					perror(" error en send \n");
					exit(-1);
				}
			}
		}

		//Recibo Confirmación final
		//OJO REVISAR BLOQUEANTE
		if ((numbytes=recv(fd, buf, MAXDATASIZE, 0)) == -1){  
		/* llamada a recv() */
			printf("No recibí confirmación\n");
		}else{
			//Imprimo mensaje de éxito
			buf[numbytes]='\0';
			printf("\n%s\n",buf);
		}
	
	}else{
		printf("\nDinero no Disponible\n");
		strcpy(str,"Necesito Recarga");
		if(send(fd, str, 25, 0) == -1){
			if(send(fd, str, 25, 0) == -1){
				if(send(fd, str, 25, 0) == -1){
					// llamada a send
					perror(" error en send \n");
					exit(-1);
				}
			}
		}
	
	}
	close(fd);   /* cerramos fd =) */

	return 0;
}
