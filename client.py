import socket
import subprocess
import sys
import os
from datetime import datetime

BUFFER_SIZE = 256
TIMEOUT = 10  # Tiempo límite en segundos

def writeToLog(log_file, client_ip, query, response_ip):
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_entry = f"{current_time} {client_ip} {query} {response_ip}"
    with open(log_file, 'a') as f:
        f.write(log_entry + '\n')

def clearLogFile(log_file):
    # Abre el archivo en modo escritura, borrando su contenido
    with open(log_file, 'w'):
        pass

def dns_query(server_ip, server_port, query, log_file):
    # Crear un socket UDP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.settimeout(TIMEOUT)  # Configurar el tiempo de espera

    try:
        # Enviar la consulta DNS al servidor
        client_socket.sendto(query.encode(), (server_ip, server_port))

        # Recibir la respuesta del servidor DNS
        response, server_address = client_socket.recvfrom(BUFFER_SIZE)
        server_ip, server_port = server_address

        # Mostrar la respuesta recibida
        print("Respuesta recibida:", response.decode())

        # Cerrar el socket
        client_socket.close()

        # Verificar que la respuesta sea una dirección IP válida
        ip_address = response.decode()

        # Si la respuesta es una dirección IP válida, realizar una búsqueda en el navegador
        if ip_address.count('.') == 3:
            if sys.platform.startswith('win'):
                subprocess.run(["start", "http://" + ip_address], shell=True)
            else:
                subprocess.run(["xdg-open", "http://" + ip_address], check=True)
        else:
            print("La respuesta recibida no es una dirección IP válida.")

        # Escribir la consulta en el archivo de registro
        writeToLog(log_file, server_ip, query, ip_address)

    except socket.timeout:
        print("La solicitud se ha cancelado porque la respuesta del servidor DNS tardó demasiado.")
    except Exception as e:
        print("Se produjo un error:", e)

def main():
    if len(sys.argv) != 2:
        print("Uso:", sys.argv[0], "<archivo_log>")
        sys.exit(1)

    log_file = sys.argv[1]

    while True:
        user_input = input("Ingrese la consulta (SERVER <ip address> TYPE <recurso de registro> DOMAIN <midominio.com>) o 'clear' para borrar el archivo de registro: ")

        if user_input.strip().lower() == 'clear':
            clearLogFile(log_file)
            print(f"Contenido de {log_file} borrado.")
        else:
            parts = user_input.split()
            if len(parts) != 6 or parts[0] != 'SERVER' or parts[2] != 'TYPE' or parts[4] != 'DOMAIN':
                print("Formato de consulta incorrecto. Debe ser en el formato: SERVER <ip address> TYPE <recurso de registro> DOMAIN <midominio.com>")
                continue

            server_ip = parts[1]
            query = parts[5]
            server_port = 53  # Puerto por defecto para DNS

            # Enviar la consulta DNS y recibir la respuesta
            dns_query(server_ip, server_port, query, log_file)

if __name__ == "__main__":
    main()
