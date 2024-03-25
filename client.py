import socket
import time

# Lista para almacenar el caché de consultas
cache = []

def query_dns(server, domain, qtype='A'):
    # Crear un socket UDP
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # Establecer el tiempo de espera para recibir una respuesta (en segundos)
    client_socket.settimeout(5)
    
    try:
        # Enviar la consulta al servidor DNS
        query = create_dns_query(domain, qtype)
        client_socket.sendto(query, (server, 53))
        
        # Recibir la respuesta del servidor
        response, _ = client_socket.recvfrom(1024)
        
        # Analizar y mostrar la respuesta
        log_message = parse_dns_response(response, domain, qtype)
        print(log_message)
        
        # Registrar la petición en el caché
        update_cache(log_message)
        
        # Registrar la petición en un archivo de registro
        log_request(log_message)
        
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
    # Devolver el código de tipo de consulta según el tipo especificado
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

def parse_dns_response(response, domain, qtype):
    # Analizar la respuesta DNS
    # Aquí puedes implementar la lógica para analizar el formato de respuesta DNS
    # Este es un ejemplo básico que simplemente devuelve la respuesta sin procesar
    return f"{time.strftime('%Y-%m-%d %H:%M:%S')} <clientIP> {domain} {qtype} <responseIP>"

def log_request(message):
    # Guardar el mensaje en un archivo de registro
    with open("dns_log.txt", "a") as logfile:
        logfile.write(message + "\n")

def update_cache(message):
    # Actualizar el caché con la última consulta
    if len(cache) >= 10:
        cache.pop(0)  # Eliminar la consulta más antigua si el caché está lleno
    cache.append(message)

def flush_cache():
    # Borrar el caché
    cache.clear()
    print("El caché ha sido borrado.")

# Ejemplo de uso
if __name__ == "__main__":
    while True:
        user_input = input("Ingrese la información en el formato 'SERVER <ip address> TYPE <recurso de registro> DOMAIN <midominio.com>', o 'flush' para borrar el caché: ")
        parts = user_input.split()
        
        if len(parts) == 1 and parts[0].lower() == 'flush':
            flush_cache()
            continue
        
        if len(parts) != 7 or parts[0].upper() != 'SERVER' or parts[2].upper() != 'TYPE' or parts[4].upper() != 'DOMAIN':
            print("Formato incorrecto. Por favor, asegúrese de ingresar la información correctamente.")
            continue
        
        server = parts[1]
        qtype = parts[3]
        domain = parts[5]
        
        try:
            query_dns(server, domain, qtype)
        except ValueError as e:
            print(e)