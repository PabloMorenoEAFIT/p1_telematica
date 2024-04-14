import socket
import time
import sys
import requests
import webbrowser

# Comprobar si se proporcionó la ruta del archivo de registro como argumento de la línea de comandos
if len(sys.argv) != 2:
    print("Uso: ./client.py </path/log.log>")
    sys.exit(1)

log_file_path = sys.argv[1]

def query_dns(server, domain, qtype='A'):
    # Crear un socket UDP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # tiempo de espera 
    client_socket.settimeout(5)
    
    try:
        # Construir la consulta DNS
        query = create_dns_query(domain, qtype)
        client_socket.sendto(query, (server, 53))
        
        # Recibir la respuesta 
        response, _ = client_socket.recvfrom(1024)
        
        # Obtener la dirección IP de la máquina cliente
        client_ip = socket.gethostbyname(socket.gethostname())
        
        # Analizar y mostrar la respuesta
        log_message = parse_dns_response(response, domain, qtype, client_ip)
        print(log_message)
        
        partes = log_message.split()
        http_ip = partes[-1]
        acceder_ip(http_ip)

        # Registrar la petición en un archivo de registro
        log_request(log_message, log_file_path)
        
    except socket.timeout:
        print("Timeout: No se recibió ninguna respuesta del servidor DNS.")
    finally:
        # Cerrar el socket
        client_socket.close()

def create_dns_query(domain, qtype='A'):
    # Estructura básica de una consulta DNS
    # Cabecera de 12 bytes
    # Consulta de dominio
    # Tipo de registro
    # Clase (IN para Internet)
    query_id = 1234  # ID de consulta arbitrario
    header = b'\x00\x00' + b'\x01\x00' + b'\x00\x01' + b'\x00\x00' + b'\x00\x00' + b'\x00\x00'
    question = b''
    for part in domain.split('.'):
        question += bytes([len(part)]) + part.encode()
    question += b'\x00'  # Terminador de cadena
    
    qtype_code = get_qtype_code(qtype)  # Código de tipo de consulta
    qclass = b'\x00\x01'  # Clase de consulta (IN)
    
    return header + question + qtype_code + qclass

def get_qtype_code(qtype):
    # Devolver el tipo de consulta 
    if qtype.upper() == 'A':
        return b'\x00\x01'
    elif qtype.upper() == 'NS':
        return b'\x00\x02'
    elif qtype.upper() == 'CNAME':
        return b'\x00\x05'
    elif qtype.upper() == 'SOA':
        return b'\x00\x06'
    elif qtype.upper() == 'MX':
        return b'\x00\x0f'
    else:
        raise ValueError("Tipo de registro no válido. Los tipos válidos son: A, NS, CNAME, SOA, MX.")

def parse_dns_response(response, domain, qtype, client_ip):
    # Analizar la respuesta DNS
    # La respuesta DNS es un mensaje binario que debe interpretarse según el protocolo DNS
    # Aquí, se analizará la respuesta y formateará como un archivo de zona DNS
    lines = []
    # Obtener la dirección IP de respuesta almacenada en dns.txt
    response_ip = get_response_ip(domain)
    # Obtener la fecha y hora actuales
    current_time = time.strftime('%Y-%m-%d %H:%M:%S')
    # Formatear la respuesta como se requiere
    response_message = f"{current_time} {client_ip} {domain} {qtype} {response_ip}"
    
    return response_message

def get_response_ip(domain):
    # Obtener la dirección IP de respuesta almacenada en dns.txt
    with open("dns.txt") as f:
        for line in f:
            # Eliminar los caracteres especiales y dividir la línea en nombre de dominio y dirección IP
            dns_entry = line.strip().strip('"').split(":")
            if dns_entry[0] == domain:
                return dns_entry[1]  # Devolver la dirección IP sin modificar

    return "Dirección IP no encontrada"

def acceder_ip(response_ip):
    try:
        # Abrir el navegador web y acceder a la URL correspondiente a la dirección IP
        webbrowser.open(f"http://{response_ip}")
        return f"Se abrió el navegador y se accedió a la página web asociada a la dirección IP {response_ip}"
    except Exception as e:
        return f"Error al intentar abrir el navegador y acceder a la página web: {str(e)}"

def log_request(message, log_file_path):
    # Guardar el mensaje en un archivo de registro
    with open(log_file_path, "a") as logfile:
        logfile.write(message + "\n")

if __name__ == "__main__":
    while True:
        user_input = input("Ingrese la información en el formato 'SERVER <ip address> TYPE <recurso de registro> DOMAIN <midominio.com>': ")
        parts = user_input.split()
        
        if len(parts) != 6 or parts[0].upper() != 'SERVER' or parts[2].upper() != 'TYPE' or parts[4].upper() != 'DOMAIN':
            print("Formato incorrecto. Por favor, asegúrese de ingresar la información correctamente.")
            continue
        
        server = parts[1]
        qtype = parts[3]
        domain = parts[5]
        
        try:
            query_dns(server, domain, qtype)
        except ValueError as e:
            print("Error:", e)
        except Exception as e:
            print("Se produjo un error inesperado:", e)
