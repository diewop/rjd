# Proyecto 1 de Redes I Sección 1

## Autores
	* Diego Pedroza 12-11281
	* Jean Alexander 12-10848


## Archivos que lo componen:
	* cliente.c    (programa del cliente)
	* servidor.c   (programa del servidor)
	* Makefile
	* Leeme.txt    (README.md pero en formato txt)
	* README.md    (Explicación de los programas y sus condiciones asociadas)     


##Descripción ¿Qué hace el programa?

### Cliente.c
	Es el programa que representa a los cajeros de tipo Full. 
	Permite que un usuario retire o deposite dinero. 
	
#### ¿Cómo se ejecuta?
	bsb_cli -d ip_server  -p port_bsb_svr -c op monto -i codigoUsuario 
-n nombreCajero
	
#### Argumentos del cliente
	**No son opcionales**
	* -p port_bsb_svr (puerto del servidor)
	* -d ip_server	(IP del servidor o nombre en forma IPv4)
	* -c op monto   (Tipo de operación **d** o **r** seguido del monto)
	* -i codigoUsuario (Código del usuario)
	* -n nombreCajero (Nombre del Cajero)
	
#### Impresión del Cliente
	fecha hora Op codigoUsuario monto

### Servidor.c
	Es el programa servidor, representa a una Computadora Central. 
	En esta computadora están los usuarios válidos, el Total Disponible por 
	cajero, y las transacciones que han sido realizadas. Es el encargado de 
	mantener registro sobre las transacciones.
	
#### ¿Cómo se ejecuta?
	bsb_svr -l port_bsb_svr - i bitacora_deposito -o bitacora_retiro

#### Argumentos del servidor
	**No son opcionales**
	* -l port_bsb_svr (puerto del servidor)
	* -i bitacora_deposito (Ruta del archivo donde estarán los depositos)
	* -o bitacora_retiro (Ruta del archivo donde estarán los retiros)

#### Impresión del Servidor
	fecha hora codigoUsuario cajero Op monto TotalDisponible ¿Fue Exitoso?





### Decisiones
	1. Si el total disponible del cajero es menor o igual a 5000, entonces
	dirá al usuario: "Dinero No Disponible" y le envía al servidor
	"Necesito recarga". Para la siguiente vez que se llame a ese cajero estará
	recargado.
	2. Se añadió en el argumento "-c" op, el monto, para que representará una 
	operación de cajero. Asimismo, se añadió  el argumento -n nombre del cajero, 
	para perservar los distintos cajeros(CB1,CB2,EME). Solo aceptará esos cajeros.
	3. Se añadió en la impresión del cajero, el monto.
	4. Se añadió en la impresión del CC el monto y si fue exitoso o no.
	5. El código del Usuario es un número entero, por lo tanto 0123 es igual a 123.
	6. Tanto el monto como el código no pueden ser letras, el número cero no es 
	un código válido. (Usamos una librería que devuelve cero si no es un número, 
	por ello no podemos utilizar el cero).


