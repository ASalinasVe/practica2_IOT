
import socket
import threading
import queue
import time

servidor_ip = '192.168.100.3'
puerto = 1234

distancia_queue = queue.Queue()

# Variables globales para conexiones
conn_sensor = None
conn_actuador = None

lock = threading.Lock()

tiempo_inicio = time.time()

def esperar_conexiones():
    global conn_sensor, conn_actuador
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((servidor_ip, puerto))
    server_socket.listen(5)
    print("[Servidor] Esperando conexiones...")

    while True:
        conn, addr = server_socket.accept()
        print(f"[Servidor] Conectado desde: {addr}")

        try:
            tipo = conn.recv(1024).decode().strip().lower()
            
            print(f"[Servidor] Tipo de dispositivo: {tipo}")
            tiempo_conexion = time.time() - tiempo_inicio  #tiempo transcurrido
            print(f"[Servidor] Conectado con: {addr} en {tiempo_conexion:.2f} segundos")
            
            if tipo == 'sensor':
                with lock:
                    if conn_sensor:
                        conn_sensor.close()
                    conn_sensor = conn
                threading.Thread(target=manejar_sensor, args=(conn,)).start()

            elif tipo == 'actuador':
                with lock:
                    if conn_actuador:
                        conn_actuador.close()
                    conn_actuador = conn
                threading.Thread(target=manejar_actuador, args=(conn,)).start()
            else:
                print("[Servidor] Tipo no reconocido. Cerrando conexión.")
                conn.close()
        except Exception as e:
            print("[Servidor] Error en identificación:", e)
            conn.close()


def manejar_sensor(conn):
    print("[Sensor] Escuchando...")
    buffer = ""  # Acumulador de datos
    while True:
        try:
            data = conn.recv(1024).decode()
            if not data:
                print("[Sensor] Sensor desconectado.")
                break
            buffer += data  # Agrega nuevos datos al buffer

            while '\n' in buffer:
                linea, buffer = buffer.split('\n', 1)  # Divide en línea y lo que queda
                mensaje = linea.strip()
                print(f"[Sensor] Distancia recibida: {mensaje}")
                distancia_queue.put(mensaje)

        except Exception as e:
            print("[Sensor] Desconectado o error:", e)
            with lock:
                global conn_sensor
                conn_sensor = None
            break
        

def manejar_actuador(conn):
    print("[Actuador] Enviando datos...")
    while True:
        try:
            mensaje = distancia_queue.get()
            conn.sendall((mensaje + '\n').encode())
            print(f"[Actuador] Distancia enviada: {mensaje}")
        except Exception as e:
            print("[Actuador] Desconectado o error:", e)
            with lock:
                global conn_actuador
                conn_actuador = None
            break

if __name__ == '__main__':
    hilo_aceptar = threading.Thread(target=esperar_conexiones)
    hilo_aceptar.start()

