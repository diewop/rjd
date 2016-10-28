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

#define PORT 21281 		/* El puerto que será abierto */

//Estructura del Cajero
struct cajero {
	char		  id[9];
	unsigned	  TotalDisponible;
	unsigned long ip;
};

// Estructura del registro
struct registro {
	char * fecha;
	char * hora;
	int	usuario;
	struct cajero atm;
	int TotalDisponible;
	char exitoso[2];
	char evento[2];
};



int main(int argc, char *argv[]){
	//Suponiendo que todos los argumentos válidos
	int i = 1;
	int port_bsb_svr;
	char * bitacora_deposito;
	char * bitacora_retiro;
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
	

	
	
	
	return 0;

}
