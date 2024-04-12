# Proyecto 1 Telemática 2024-1

## Introducción

Mediante el presente se pretende crear un servidor DNS (Domain Name System) que basa su comunicación mediante el protocolo de comunicación UDP.
El proposito se basa en lograr una conexión estable en donde un usuario pueda acceder a la dirección IP de algún dominio seleccionado mediante un servidor alojado en AWS.

## Desarrollo

### Momento 1
En un inicio se realizó un análisis a profundidad del proyecto, buscando entender los objetivos y necesidades que se planteaban. 
Durante este mismo período de tiempo se crea así el archivo **server.c** y **client.py** donde se determinaron cuales serían las principales funciones que debía contener cada aplicación.

### Momento 2
Al determinar las funciones y cada necesidad de las aplicaciones se empezó la codificación del cliente donde se determinaba la estructura de los archivos de **log.txt** para cada tipo de registro al igual que la estrucutra general de como se harían las peticiones al servidor.

### Momento 3
Se crea la aplicación del servidor y se hace especial enfásis en crear las funciones del manejo de las peticiones y como se maneja la petición mediante un protocolo UDP.

### Momento 4
Se hace especial enfásis en el cómo ambas aplicaciones se comunican entre sí y que puedan generar los resultados esperados.


## Aspectos logrados y no logrados

### aspectos logrados
- Existe una comunicación entre **server.c** y **client.py**.
- La petición se recibe de manera correcta y el servidor entiende como debe resolverla.
- **server.c** se desplega correctamente en el servicio de AWS mediante una instancia EC2.

### aspectos no logrados

### observaciones generales

- **gcc -o dnserver server.c -lws2_32**
  :Es necesario correr el siguiente comando para vincular las librerias nativas de C con el programa para su normal ejecución
- **dnserver.exe <-ipconfig> 53 C:\Users\pamor\Desktop\universidad\S5\telematica\p1\p1_telematica\dns.txt C:\Users\pamor\Desktop\universidad\S5\telematica\p1\p1_telematica\log.txt** : para ejecutar el servidor
- **python client.py C:\Users\pamor\Desktop\universidad\S5\telematica\p1\p1_telematica** : **SERVER <-ipconfig> TYPE A DOMAIN avianca.com** : para ejecutar client.py
- **netstat -ano | findstr :53** : para revisar si el puerto 53 esta en uso

## Conclusiones 

## Referencias
