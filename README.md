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
- Se accede correctamente a las direcciones IP asociadas a los dominios descritos.
- el archivo **log.txt** almacena toda la informacion relacionada a la actividad del usuario.

### aspectos no logrados

- El formato de almacenamiento de los recursos de registros es bastante simple, y que no contiene toda la información disponible relacionada a un dominio.
- El uso de algunas librerías fue necesario para poder cumplir con los retos propuestos.

### observaciones generales

- **gcc -o dnserver server.c -lws2_32**
  :Es necesario correr el siguiente comando para vincular las librerias nativas de C con el programa para su normal ejecución a la par que para crear el archivo ejecutable del servidor
- **dnserver.exe <-ipconfig> 53 ./dns.txt ./log.txt** : para ejecutar el servidor
- **python client.py ./log.txt** : **SERVER <-ipconfig> TYPE A DOMAIN <midominio.com>** : para ejecutar client.py
- **netstat -ano | findstr :53** : para revisar si el puerto 53 esta en uso
- **Para acceder intstancia EC2**: ssh -i /path/to/your/key.pem ec2-user@<ip_privada>
- para ejecutar server.c : **sudo ./dnserver <ip_privada> 53 ./dns.txt ./log.txt**


## Conclusiones 

Durante este proceso, hemos explorado la implementación de un servidor DNS básico utilizando el protocolo UDP. Aquí hay algunas conclusiones clave:

**Entendimiento de los protocolos UDP:** El protocolo UDP (User Datagram Protocol) es un protocolo de comunicación ligero y sin conexión que se utiliza para la transmisión de datos en redes. A diferencia de TCP, UDP no garantiza la entrega de datos ni el orden en el que se reciben.

**Conexiones DNS:** El Sistema de Nombres de Dominio (DNS) es un sistema distribuido para traducir nombres de dominio legibles por humanos en direcciones IP numéricas y viceversa. Los servidores DNS utilizan protocolos como UDP para responder a las consultas de resolución de nombres de dominio.

**Uso de sockets:** Los sockets son una abstracción de programación que permite la comunicación entre procesos a través de una red. En este caso, hemos utilizado sockets para crear un servidor DNS que escucha las solicitudes entrantes en un puerto específico y responde a esas solicitudes.

**Implementación del servidor DNS:** Hemos creado un servidor DNS básico que utiliza el protocolo UDP para recibir y responder a las consultas de resolución de nombres de dominio. El servidor carga los registros DNS desde un archivo de configuración, maneja las solicitudes entrantes y envía respuestas adecuadas a los clientes.

Este proceso nos ha permitido comprender cómo funciona la comunicación DNS a nivel de protocolo y cómo implementar un servidor DNS simple utilizando sockets y el protocolo UDP.

## Referencias

- https://learn.microsoft.com/es-es/windows/win32/dns/resource-records
- https://www.ibm.com/docs/en/zos/2.4.0?topic=programming-c-socket-call-guidance
- https://www.ibm.com/docs/es/i/7.3?topic=concepts-dns-resource-records
- https://docs.aws.amazon.com/codedeploy/latest/userguide/deployment-steps-server.html
