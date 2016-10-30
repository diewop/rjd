# Proyecto 1 de Redes I Sección 1

## Autores
	* Diego Pedroza 12-11281
	* Jean Alexander 12-10848


## Archivos que lo componen:
	* cliente.c 
	* servidor.c
	* Makefile
	* pruebas.zip


### Argumentos del cliente
	* -p port_bsb_svr (puerto del servidor)
	* -d ip_server	(IP del servidor, en forma IPv4)
	* -c op monto   (Tipo de operación **d** o **r** seguido del monto)
	* -i codigoUsuario (Código del usuario)
	* -n nombreCajero (Nombre del Cajero)

### Argumentos del servidor
	* -l port_bsb_svr (puerto del servidor)
	* -i bitacora_deposito (Ruta del archivo donde estarán los depositos)
	* -o bitacora_retiro (Ruta del archivo donde estarán los retiros)


#### Impresión del Cajero
	fecha hora Op codigoUsuario

#### Impresión del Central
	fecha hora codigoUsuario cajero Op monto TotalDisponible

### Decisiones
	1. Si el total disponible del cajero es menor o igual a 5000, entonces
	dirá al usuario: "Dinero No Disponible" y le envía al servidor
	"Necesito recarga". Para la siguiente vez que se llame a ese cajero estará
	recargado.


